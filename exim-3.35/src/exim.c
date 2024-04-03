/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* The main function: entry point, initialization, and high-level control. */


#include "exim.h"



/*************************************************
*      Function interface to store functions     *
*************************************************/

/* We need some real functions to pass to the PCRE regular expression library
for store allocation via Exim's store manager. The normal calls are actually
macros that pass over location information to make tracing easier. These
functions just interface to the standard macro calls. A good compiler will
optimize out the tail recursion and so not make them too expensive. There
are two sets of functions; one for use when we want to retain the compiled
regular expression for a long time; the other for short-term use. */

static void *
function_store_get(size_t size)
{
return store_get((int)size);
}

static void
function_dummy_free(void *block) { block = block; }

static void *
function_store_malloc(size_t size)
{
return store_malloc((int)size);
}

static void
function_store_free(void *block)
{
store_free(block);
}




/*************************************************
*  Compile regular expression and panic on fail  *
*************************************************/

/* This function is called when failure to compile a regular expression leads
to a panic exit. In other cases, pcre_compile() is called directly. In many
cases where this function is used, the results of the compilation are to be
placed in long-lived store, so we temporarily reset the store management
functions that PCRE uses if the use_malloc flag is set.

Argument:
  pattern     the pattern to compile
  caseless    TRUE if caseless matching is required
  use_malloc  TRUE if compile into malloc store

Returns:      pointer to the compiled pattern
*/

pcre *
regex_must_compile(char *pattern, BOOL caseless, BOOL use_malloc)
{
int offset;
int options = PCRE_COPT;
pcre *yield;
const char *error;
if (use_malloc)
  {
  pcre_malloc = function_store_malloc;
  pcre_free = function_store_free;
  }
if (caseless) options |= PCRE_CASELESS;
yield = pcre_compile(pattern, options, &error, &offset, NULL);
pcre_malloc = function_store_get;
pcre_free = function_dummy_free;
if (yield == NULL)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "regular expression error: "
    "%s at offset %d while compiling %s", error, offset, pattern);
return yield;
}




/*************************************************
*   Execute regular expression and set strings   *
*************************************************/

/* This function runs a regular expression match, and sets up the pointers to
the matched substrings.

Arguments:
  re          the compiled expression
  subject     the subject string
  options     additional PCRE options
  setup       if < 0 do full setup
              if >= 0 setup from setup+1 onwards,
                excluding the full matched string

Returns:      TRUE or FALSE
*/

BOOL
regex_match_and_setup(pcre *re, char *subject, int options, int setup)
{
int ovector[3*(EXPAND_MAXN+1)];
int n = pcre_exec(re, NULL, subject, (int)strlen(subject), 0,
  PCRE_EOPT | options, ovector, sizeof(ovector)/sizeof(int));
BOOL yield = n >= 0;
if (n == 0) n = EXPAND_MAXN + 1;
if (yield)
  {
  int nn;
  expand_nmax = (setup < 0)? 0 : setup + 1;
  for (nn = (setup < 0)? 0 : 2; nn < n*2; nn += 2)
    {
    expand_nstring[expand_nmax] = subject + ovector[nn];
    expand_nlength[expand_nmax++] = ovector[nn+1] - ovector[nn];
    }
  expand_nmax--;
  }
return yield;
}




/*************************************************
*             Timeout handler                    *
*************************************************/

/* This is called from various parts of the code when a timeout is required
that must just set a flag on expiry. This is typically used to place a time
limit on a system call of some kind. There are some other SIGALRM handlers that
are used in special cases when more than just a flag setting is required; they
are normally in the code module that uses them.

Argument: the signal value (SIGALRM)
Returns:  nothing
*/

void
sigalrm_handler(int sig)
{
sig = sig;      /* Keep picky compilers happy */
sigalrm_seen = TRUE;
}



/*************************************************
*             Handler for SIGUSR1                *
*************************************************/

/* SIGUSR1 causes any exim process to write to the process log details of
what it is currently doing. It will only be used if the OS is capable of
setting up a handler that causes automatic restarting of any system call
that is in progress at the time.

Argument: the signal number (SIGUSR1)
Returns:  nothing
*/

static void
usr1_handler(int sig)
{
sig = sig;      /* Keep picky compilers happy */
log_write(0, LOG_PROCESS, "%s", process_info);
log_close_all();
os_restarting_signal(SIGUSR1, usr1_handler);
}



/*************************************************
*            Set up processing details           *
*************************************************/

/* Save a text string for dumping when SIGUSR1 is received.
Do checks for overruns.

Arguments: format and arguments, as for printf()
Returns:   nothing
*/

void
set_process_info(char *format, ...)
{
int len;
va_list ap;
sprintf(process_info, "%5d ", (int)getpid());
len = strlen(process_info);
va_start(ap, format);
if (!string_vformat(process_info + len, PROCESS_INFO_SIZE - len, format, ap))
  strcpy(process_info + len, "**** string overflowed buffer ****");
DEBUG(2) debug_printf("set_process_info: %s\n", process_info);
va_end(ap);
}





/*************************************************
*   Close unwanted file descriptors for delivery *
*************************************************/

/* This function is called from a new process that has been forked to deliver
an incoming message, either directly, or using exec.

We want any smtp input streams to be closed in this new process. However, it
has been observed that using fclose() here causes trouble. When reading in -bS
input, duplicate copies of messages have been seen. The files will be sharing a
file pointer with the parent process, and it seems that fclose() (at least on
some systems - I saw this on Solaris 2.5.1) messes with that file pointer, at
least sometimes. Hence we go for closing the underlying file descriptors.

For delivery of a non-SMTP message, we want to close stdin and stdout (and
stderr unless debugging) because the calling process might have set them up as
pipes and be waiting for them to close before it waits for the submission
process to terminate. If they aren't closed, they hold up the calling process
until the initial delivery process finishes, which is not what we want.

Arguments:   None
Returns:     Nothing
*/

static void
close_unwanted(void)
{
if (smtp_input)
  {
  close(fileno(smtp_in));
  close(fileno(smtp_out));
  smtp_in = NULL;
  }
else
  {
  close(0);                         /* stdin */
  close(1);                         /* stdout */
  if (debug_level <= 0)             /* stderr */
    {
    close(2);
    log_stderr = NULL;
    }
  }
}

/****************************************************
*   Check for access permissions to the config file *
****************************************************/
static int
check_permissions(const char *cfgfile, uid_t id, uid_t gid)
{
   int ret;
   uid_t oldid = getuid();
   uid_t oldgid = getgid();
   struct passwd *pw;

   if(setreuid(id, -1) != 0) {
     fprintf(stderr, "exim: failed to restore uid: %s\n", strerror(errno));
     exit(EXIT_FAILURE);
   }

   if(setregid(gid, -1) != 0) {
     fprintf(stderr, "exim: failed to restore gid: %s\n", strerror(errno));
     exit(EXIT_FAILURE);
   }

   pw = getpwuid(id);

  if (pw!=NULL)
    initgroups(pw->pw_name, pw->pw_gid);

   ret = access(cfgfile, R_OK);

   if (setreuid(oldid, -1) != 0)
     perror("setreuid");
     
   if (setregid(oldgid, -1) != 0)
     perror("setregid");
     
   return ret;
}


/*************************************************
*          Set uid and gid                       *
*************************************************/

/* This function sets a new uid and gid permanently. It is not expected to
fail, but if it does, it normally bombs out. There is one special case: if the
original euid was not root, we are running in an unprivileged state (e.g.
setuid=exim and not called by root). In this case, if the uid/gid we are trying
to set is actually the current effective uid/gid, ignore errors.

Arguments:
  uid        the uid
  gid        the gid
  msg        text to use in a failure log

Returns:     nothing
*/

void
exim_setugid(uid_t uid, gid_t gid, char *msg)
{
if (setgid(gid) < 0 || setuid(uid) < 0)
  {
  if (original_euid == root_uid || uid != geteuid() || gid != getegid())
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "unable to set uid=%ld or gid=%ld "
      "for %s (euid=%ld)", (long int)uid, (long int)gid, msg,
      (long int)geteuid());
  }
}




/*************************************************
*               Exit point                       *
*************************************************/

/* Exim exits via this function so that it always clears up any open
databases.

Arguments:
  rc         return code

Returns:     does not return
*/

void
exim_exit(int rc)
{
search_tidyup();
exit(rc);
}




/*************************************************
*          Entry point and high-level code       *
*************************************************/

/* Entry point for the Exim mailer. Analyse the arguments and arrange to take
the appropriate action. All the necessary functions are present in the one
binary. I originally thought one should split it up, but it turns out that so
much of the apparatus is needed in each chunk that one might as well just have
it all available all the time, which then makes the coding easier as well.

Arguments:
  argc      count of entries in argv
  argv      argument strings, with argv[0] being the program name

Returns:    EXIT_SUCCESS if terminated successfully
            EXIT_FAILURE otherwise, except when a message has been sent
              to the sender, and -oee was given
*/

int
main(int argc, char **argv)
{
int  arg_accept_timeout = -1;
int  arg_error_handling = error_handling;
int  filter_fd = -1;
int  group_count;
int  i;
int  list_queue_option = 0;
int  message_count = 0;
int  msg_action = 0;
int  msg_action_arg = -1;
int  namelen = (argv[0] == NULL)? 0 : (int)strlen(argv[0]);
int  queue_only_reason = 0;
#ifdef EXIM_PERL
int  perl_start_option = 0;
#endif
int  recipients_arg = argc;
int  sender_address_domain = 0;
int  test_retry_arg = -1;
int  test_rewrite_arg = -1;
BOOL arg_queue_only = FALSE;
BOOL bi_option = FALSE;
BOOL checking = FALSE;
BOOL count_queue = FALSE;
BOOL expansion_test = FALSE;
BOOL extract_recipients = FALSE;
BOOL forced_delivery = FALSE;
BOOL deliver_give_up = FALSE;
BOOL is_inetd = FALSE;
BOOL list_queue = FALSE;
BOOL list_variables = FALSE;
BOOL local_queue_only;
BOOL more = TRUE;
BOOL one_msg_action = FALSE;
BOOL queue_only_set = FALSE;
BOOL synchronous_delivery = FALSE;
BOOL unprivileged;
BOOL removed_privilege = FALSE;
BOOL verify_as_sender = FALSE;
BOOL version_printed = FALSE;
char *alias_arg = NULL;
char *called_as = "";
char *start_queue_run_id = NULL;
char *stop_queue_run_id = NULL;
char *ftest_domain = NULL;
char *ftest_localpart = NULL;
char *ftest_prefix = NULL;
char *ftest_suffix = NULL;
char *real_sender_address;
char *originator_home = "/";
BOOL ftest_system = FALSE;
transport_instance *ti;
void *reset_point;

struct passwd *pw;
struct stat statbuf;
pid_t passed_qr_pid = (pid_t)0;
int passed_qr_pipe = -1;
gid_t group_list[NGROUPS_MAX];

#if HAVE_IPV6
struct sockaddr_in6 inetd_sock;
#else
struct sockaddr inetd_sock;
#endif

/* Possible options for -R and -S */

static char *rsopts[] = { "f", "ff", "r", "rf", "rff" };

/* Need to define this in case we need to change the environment in order
to get rid of a bogus time zone. */

extern char **environ;

/* Check a field which is patched when we are running Exim within its
testing harness; do a fast initial check, and then the whole thing. */

running_in_test_harness =
  *running_status == '<' && strcmp(running_status, "<<<testing>>>") == 0;

/* If we are running in the test harness, and -MC is in use, delay for a second
here, to let the process that set this one up complete before we run the new
Exim. This makes for repeatability of the logging, etc. output. */

if (running_in_test_harness)
  for (i = 1; i < argc; i++) if (strcmp(argv[i], "-MC") == 0) sleep(1);

/* Ensure we have a buffer for constructing log entries. Use malloc directly,
because store_malloc writes a log entry on failure. */

log_buffer = (char *)malloc(LOG_BUFFER_SIZE);
if (log_buffer == NULL)
  {
  fprintf(stderr, "exim: failed to get store for log buffer\n");
  exit(EXIT_FAILURE);
  }

/* Set log_stderr to stderr, provided that stderr exists. This gets reset to
NULL when the daemon is run and the file is closed. We have to use this
indirection, because some systems don't allow writing to the variable "stderr".
*/

if (fstat(fileno(stderr), &statbuf) >= 0) log_stderr = stderr;

/* Arrange for the PCRE regex library to use our store functions. Note that
the normal calls are actually macros that add additional arguments for
debugging purposes so we have to assign specially constructed functions here.
The default is to use store in the stacking pool, but this is overridden in the
regex_must_compile() function. */

pcre_malloc = function_store_get;
pcre_free = function_dummy_free;

/* Ensure there is a big buffer for temporary use in several places. It is put
in malloc store so that it can be freed for enlargement if necessary. */

big_buffer = store_malloc(big_buffer_size);

/* Set up the handler for the data request signal, and set the initial
descriptive text. */

set_process_info("initializing");
os_restarting_signal(SIGUSR1, usr1_handler);

/* SIGHUP is used to get the daemon to reconfigure. It gets set as appropriate
in the daemon code. For the rest of exim's uses, we ignore it. */

signal(SIGHUP, SIG_IGN);

/* We don't want to die on pipe errors as the code is written to handle
the write error instead. */

signal(SIGPIPE, SIG_IGN);

/* Under some circumstance on some OS, Exim can get called with SIGCHLD
set to SIG_IGN. This causes subprocesses that complete before the parent
process waits for them not to hang around, so when Exim calls wait(), nothing
is there. The wait() code has been made robust against this, but let's ensure
that SIGCHLD is set to SIG_DFL, because it's tidier to wait and get a process
ending status. */

signal(SIGCHLD, SIG_DFL);

/* Save the arguments for use if we re-exec exim as a daemon after receiving
SIGHUP. */

sighup_argv = argv;

/* Set up the version number. Set up the leading 'E' for the external form of
message ids, set the pointer to the internal form, and initialize it to
indicate no message being processed. */

version_init();
message_id_option[0] = '-';
message_id_external = message_id_option + 1;
message_id_external[0] = 'E';
message_id = message_id_external + 1;
message_id[0] = 0;

/* Set the umask to zero so that any files that Exim creates are created
with the modes that it specifies. */

umask(0);

/* Precompile the regular expression for matching a message id. Keep this in
step with the code that generates ids in the accept.c module. We need to do
this here, because the -M options check their arguments for syntactic validity
using mac_ismsgid, which uses this. */

regex_ismsgid =
  regex_must_compile("^(?:[^\\W_]{6}-){2}[^\\W_]{2}$", FALSE, TRUE);

/* If the program is called as "mailq" treat it as equivalent to "exim -bp";
this seems to be a generally accepted convention, since one finds symbolic
links called "mailq" in standard OS configurations. */

if ((namelen == 5 && strcmp(argv[0], "mailq") == 0) ||
    (namelen  > 5 && strncmp(argv[0] + namelen - 6, "/mailq", 6) == 0))
  {
  list_queue = TRUE;
  called_as = "-mailq";
  }

/* If the program is called as "rmail" treat it as equivalent to
"exim -i -oee", thus allowing UUCP messages to be input using non-SMTP mode,
i.e. preventing a single dot on a line from terminating the message, and
returning with zero return code, even in cases of error (provided an error
message has been sent). */

if ((namelen == 5 && strcmp(argv[0], "rmail") == 0) ||
    (namelen  > 5 && strncmp(argv[0] + namelen - 6, "/rmail", 6) == 0))
  {
  dot_ends = FALSE;
  called_as = "-rmail";
  errors_sender_rc = EXIT_SUCCESS;
  }

/* If the program is called as "rsmtp" treat it as equivalent to "exim -bS";
this is a smail convention. */

if ((namelen == 5 && strcmp(argv[0], "rsmtp") == 0) ||
    (namelen  > 5 && strncmp(argv[0] + namelen - 6, "/rsmtp", 6) == 0))
  {
  smtp_input = smtp_batched_input = TRUE;
  called_as = "-rsmtp";
  }

/* If the program is called as "runq" treat it as equivalent to "exim -q";
this is a smail convention. */

if ((namelen == 4 && strcmp(argv[0], "runq") == 0) ||
    (namelen  > 4 && strncmp(argv[0] + namelen - 5, "/runq", 5) == 0))
  {
  queue_interval = 0;
  called_as = "-runq";
  }

/* If the program is called as "newaliases" treat it as equivalent to
"exim -bi"; this is a sendmail convention. */

if ((namelen == 10 && strcmp(argv[0], "newaliases") == 0) ||
    (namelen  > 10 && strncmp(argv[0] + namelen - 11, "/newaliases", 11) == 0))
  {
  bi_option = TRUE;
  called_as = "-newaliases";
  }

/* Save the original effective uid for a couple of uses later. It should
normally be root, but in some esoteric environments it may not be. */

original_euid = geteuid();

/* Get the real uid and gid. If the caller is root, force the effective
uid/gid to be the same as the real ones. This makes a difference only if Exim
is setuid (or setgid) to something other than root, which could be the case in
some special configurations. */

real_uid = getuid();
real_gid = getgid();

if (real_uid == root_uid)
  {
  mac_seteuid(real_uid);
  mac_setegid(real_gid);
  }

/* If the real_uid is root, or the original effective uid was root, Exim is
running in a privileged state. Otherwise it is running unprivileged. */

unprivileged = (real_uid != root_uid && original_euid != root_uid);

/* Scan the program's arguments. Some can be dealt with right away; others are
simply recorded for checking and handling afterwards. Do a high-level switch
on the second character (the one after '-'), to save some effort. */

for (i = 1; i < argc; i++)
  {
  BOOL badarg = FALSE;
  char *arg = argv[i];
  char *argrest;
  int switchchar;

  /* An argument not starting with '-' is the start of a recipients list;
  break out of the options-scanning loop. */

  if (arg[0] != '-')
    {
    recipients_arg = i;
    break;
    }

  /* An option consistion of -- terminates the options */

  if (strcmp(arg, "--") == 0)
    {
    recipients_arg = i + 1;
    break;
    }

  /* Handle flagged options */

  switchchar = arg[1];
  argrest = arg+2;

  /* Make all -ex options synonymous with -oex arguments, since that
  is assumed by various callers. Also make -qR options synonymous with -R
  options, as that seems to be required as well. Allow for -qqR too, and
  the same for -S options. */

  if (strncmp(arg+1, "oe", 2) == 0 ||
      strncmp(arg+1, "qR", 2) == 0 ||
      strncmp(arg+1, "qS", 2) == 0)
    {
    switchchar = arg[2];
    argrest++;
    }
  else if (strncmp(arg+1, "qqR", 3) == 0 || strncmp(arg+1, "qqS", 3) == 0)
    {
    switchchar = arg[3];
    argrest += 2;
    queue_2stage = TRUE;
    }

  /* Make -r synonymous with -f, since it is a documented alias */

  else if (arg[1] == 'r') switchchar = 'f';

  /* Make -ov synonymous with -v */

  else if (strcmp(arg, "-ov") == 0)
    {
    switchchar = 'v';
    argrest++;
    }

  /* High-level switch on active initial letter */

  switch(switchchar)
    {
    /* -Btype is a sendmail option for 7bit/8bit setting. Exim is 8-bit clean
    so has no need of it. */

    case 'B':
    if (*argrest == 0) i++;   /* Skip over the type */
    break;


    case 'b':

    /* -bd: Run in daemon mode, awaiting SMTP connections. */

    if (strcmp(argrest, "d") == 0) daemon_listen = TRUE;

    /* -be: Run in expansion test mode */

    else if (*argrest == 'e')
      expansion_test = checking = TRUE;

    /* -bf:  Run in mail filter testing mode
       -bF:  Ditto, but for system filters
       -bfd: Set domain for filter testing
       -bfl: Set local part for filter testing
       -bfp: Set prefix for filter testing
       -bfs: Set suffix for filter testing
    */

    else if (*argrest == 'f' || *argrest == 'F')
      {
      ftest_system = *argrest++ == 'F';
      if (*argrest == 0)
        {
        if(++i < argc) filter_test = argv[i]; else
          {
          fprintf(stderr, "exim: file name expected after %s\n", argv[i-1]);
          exit(EXIT_FAILURE);
          }
        }
      else
        {
        if (++i >= argc)
          {
          fprintf(stderr, "exim: string expected after %s\n", arg);
          exit(EXIT_FAILURE);
          }
        if (strcmp(argrest, "d") == 0) ftest_domain = argv[i];
        else if (strcmp(argrest, "l") == 0) ftest_localpart = argv[i];
        else if (strcmp(argrest, "p") == 0) ftest_prefix = argv[i];
        else if (strcmp(argrest, "s") == 0) ftest_suffix = argv[i];
        else { badarg = TRUE; break; }
        }
      }

    /* -bh: Host checking - an IP address must follow. */

    else if (strcmp(argrest, "h") == 0)
      {
      if (++i >= argc) { badarg = TRUE; break; }
      sender_host_address = argv[i];
      host_checking = checking = TRUE;
      }

    /* -bi: This option is used by sendmail to initialise *the* alias file,
    though it has the -oA option to specify a different file. Exim has no
    concept of *the* alias file, but since Sun's YP make script calls
    sendmail this way, some support must be provided. */

    else if (strcmp(argrest, "i") == 0) bi_option = TRUE;

    /* -bm: Accept and deliver message - the default option. */

    else if (strcmp(argrest, "m") == 0) continue;

    /* -bpxx: List the contents of the mail queue, in various forms. If
    the option is -bpc, just a queue count is needed. Otherwise, if the
    first letter after p is r, then order is random. */

    else if (*argrest == 'p')
      {
      if (*(++argrest) == 'c')
        {
        count_queue = TRUE;
        if (*(++argrest) != 0) badarg = TRUE;
        break;
        }

      if (*argrest == 'r')
        {
        list_queue_option = 8;
        argrest++;
        }
      else list_queue_option = 0;

      /* -bp: List the contents of the mail queue, top-level only */

      if (*argrest == 0) list_queue = TRUE;

      /* -bpu: List the contents of the mail queue, top-level undelivered */

      else if (strcmp(argrest, "u") == 0)
        {
        list_queue = TRUE;
        list_queue_option += 1;
        }

      /* -bpa: List the contents of the mail queue, including all delivered */

      else if (strcmp(argrest, "a") == 0)
        {
        list_queue = TRUE;
        list_queue_option += 2;
        }

      /* Unknown after -bp[r] */

      else
        {
        badarg = TRUE;
        break;
        }
      }


    /* -bP: List the configuration variables given as the address list.
    Force -v, so configuration errors get displayed. */

    else if (strcmp(argrest, "P") == 0)
      {
      list_variables = TRUE;
      if (debug_level <= 0) debug_level = 1;
      debug_file = stderr;
      }

    /* -brt: Test retry configuration lookup */

    else if (strcmp(argrest, "rt") == 0)
      {
      test_retry_arg = i + 1;
      goto END_ARG;
      }

    /* -brw: Test rewrite configuration */

    else if (strcmp(argrest, "rw") == 0)
      {
      test_rewrite_arg = i + 1;
      goto END_ARG;
      }

    /* -bS: Read SMTP commands on standard input, but produce no replies -
    all errors are reported by sending messages. */

    else if (strcmp(argrest, "S") == 0)
      smtp_input = smtp_batched_input = TRUE;

    /* -bs: Read SMTP commands on standard input and produce SMTP replies
    on standard output. */

    else if (strcmp(argrest, "s") == 0) smtp_input = TRUE;

    /* -bt: address testing mode */

    else if (strcmp(argrest, "t") == 0)
      address_test_mode = checking = TRUE;

    /* -bv: verify addresses */

    else if (strcmp(argrest, "v") == 0)
      verify_only = checking = TRUE;

    /* -bvs: verify sender addresses */

    else if (strcmp(argrest, "vs") == 0)
      {
      verify_only = checking = TRUE;
      verify_as_sender = TRUE;
      }

    /* -bV: Print version string */

    else if (strcmp(argrest, "V") == 0)
      {
      printf("Exim version %s #%s built %s\n", version_string,
        version_cnumber, version_date);
      printf("%s\n", version_copyright);
      version_printed = TRUE;
      }

    else badarg = TRUE;
    break;


    /* -C: change configuration file; ignore if it isn't really
    a change! */

    case 'C':
    if (*argrest == 0)
      {
      if(++i < argc) argrest = argv[i]; else
        { badarg = TRUE; break; }
      }
    if (strcmp(config_filename, argrest) != 0)
      {
      config_filename = argrest;
      config_changed = TRUE;
      }
    break;


    /* -D: set up a macro definition */

    case 'D':
      {
      int ptr = 0;
      macro_item *mlast = NULL;
      macro_item *m;
      char name[24];
      char *s = argrest;

      if (*s == 0)
        {
        if(++i < argc) s = argv[i]; else
          { badarg = TRUE; break; }
        }
      while (isalnum((uschar)*s) || *s == '_')
        {
        if (ptr < sizeof(name)-1) name[ptr++] = *s;
        s++;
        }
      name[ptr] = 0;
      while (isspace((uschar)*s)) s++;
      if (*s == 0)
        {
        if(++i < argc) s = argv[i]; else
          { badarg = TRUE; break; }
        }
      if (*s++ != '=') { badarg = TRUE; break; }
      if (*s == 0)
        {
        if(++i < argc) s = argv[i]; else
          { badarg = TRUE; break; }
        }

      for (m = macros; m != NULL; m = m->next)
        {
        if (strcmp(m->name, name) == 0)
          {
          fprintf(stderr, "exim: duplicated -D in command line\n");
          exit(EXIT_FAILURE);
          }
        mlast = m;
        }

      m = store_get(sizeof(macro_item) + (int)strlen(name));
      m->next = NULL;
      m->command_line = TRUE;
      if (mlast == NULL) macros = m; else mlast->next = m;
      strcpy(m->name, name);
      m->replacement = string_copy(s);

      if (clmacro_count >= MAX_CLMACROS)
        {
        fprintf(stderr, "exim: too many -D options on command line\n");
        exit(EXIT_FAILURE);
        }
      clmacros[clmacro_count++] = string_sprintf("-D%s=%s", m->name,
        m->replacement);
      }
    break;

    /* -d: Set debug level (see also -v below) or set memory tracing, or
    set up stderr to a file, or set the drop_cr option. */

    case 'd':
    if (strcmp(argrest, "m") == 0)
      {
      debug_trace_memory = running_in_test_harness? 1:2;
      if (debug_level < 1) debug_level = 1;
      }
    else if (strcmp(argrest, "ropcr") == 0) drop_cr = TRUE;
    else if (strcmp(argrest, "f") == 0)
      {
      if (*stderr_filename != 0) freopen(stderr_filename, "a", stderr);
      }
    else if (*argrest == 0 && (i+1 >= argc || !isdigit((uschar)(argv[i+1][0]))))
      debug_level = 1;
    else
      {
      char *s;
      int n = 0;
      s = (*argrest == 0)? argv[++i] : argrest;
      (void)sscanf(s, "%d%n", &debug_level, &n);
      if (n+2 < (int)strlen(arg)) badarg = TRUE;
      }
    if (debug_level > 0 && !badarg)
      {
      debug_file = stderr;
      debug_printf("Exim version %s debug level %d uid=%ld gid=%ld\n",
        version_string, debug_level, (long int)real_uid, (long int)real_gid);

      #ifdef DB_VERSION_STRING
      debug_printf("Berkeley DB: %s\n", DB_VERSION_STRING);
      #elif defined(BTREEVERSION) && defined(HASHVERSION)
        #ifdef USE_DB
        debug_printf("probably Berkeley DB version 1.8x (native mode)\n");
        #else
        debug_printf("probably Berkeley DB version 1.8x (compatibility mode)\n");
        #endif
      #elif defined(_DBM_RDONLY) || defined(dbm_dirfno)
      debug_printf("probably ndbm\n");
      #elif defined(USE_TDB)
      debug_printf("Using tdb\n");
      #else
      debug_printf("probably GDBM\n");
      #endif
      }
    break;


    /* -E: This is a local error message. This option is not intended for
    external use at all, but is not restricted to trusted callers because it
    does no harm (just suppresses certain error messages) and if exim is run
    not setuid root it won't always be trusted when it generates error
    messages using this option. If there is a message id following -E, point
    message_reference at it, for logging. */

    case 'E':
    local_error_message = TRUE;
    if (mac_ismsgid(argrest)) message_reference = argrest;
    break;


    /* -ex: The vacation program calls sendmail with the undocumented "-eq"
    option, so it looks as if historically the -oex options are also callable
    without the leading -o. So we have to accept them. Before the switch,
    anything starting -oe has been converted to -e. Exim does not support all
    of the sendmail error options. */

    case 'e':
    if (strcmp(argrest, "e") == 0)
      {
      arg_error_handling = ERRORS_SENDER;
      errors_sender_rc = EXIT_SUCCESS;
      }
    else if (strcmp(argrest, "m") == 0) arg_error_handling = ERRORS_SENDER;
    else if (strcmp(argrest, "p") == 0) arg_error_handling = ERRORS_STDERR;
    else if (strcmp(argrest, "q") == 0) arg_error_handling = ERRORS_STDERR;
    else if (strcmp(argrest, "w") == 0) arg_error_handling = ERRORS_SENDER;
    else badarg = TRUE;
    break;


    /* -F: Set sender's full name, used instead of the gecos entry from
    the password file. Since users can usually alter their gecos entries,
    there's no security involved in using this instead. The data can follow
    the -F or be in the next argument. */

    case 'F':
    if (*argrest == 0)
      {
      if(++i < argc) argrest = argv[i]; else
        { badarg = TRUE; break; }
      }
    originator_name = argrest;
    break;


    /* -f: Set sender's address - only actually used if run by a trusted
    user, or if untrusted_set_sender is set, except that the null address can
    always be set by any user. For an untrusted user, the actual sender is
    still put in Sender: if it doesn't match the From: header. The data can
    follow the -f or be in the next argument. The -r switch is an obsolete form
    of -f but since there appear to be programs out there that use anything
    that sendmail ever supported, better accept it - the synonymizing is done
    before the switch above. */

    case 'f':
      {
      int start, end;
      char *errmess;
      if (*argrest == 0)
        {
        if (i+1 < argc) argrest = argv[++i]; else
          { badarg = TRUE; break; }
        }
      sender_address =
        parse_extract_address(argrest, &errmess, &start, &end,
          &sender_address_domain, TRUE);
      if (sender_address == NULL)
        {
        fprintf(stderr, "exim: %s - bad address: %s\n", argrest, errmess);
        return EXIT_FAILURE;
        }
      sender_address_forced = TRUE;
      }
    break;


    /* -h: Set the hop count for an incoming message. Exim does not currently
    support this; it always computes it by counting the Received: headers.
    To put it in will require a change to the spool header file format. */

    case 'h':
    if (*argrest == 0)
      {
      if(++i < argc) argrest = argv[i]; else
        { badarg = TRUE; break; }
      }
    if (!isdigit((uschar)*argrest)) badarg = TRUE;
    break;


    /* -i: Set flag so dot doesn't end non-SMTP input (same as -oi, seems
    not to be documented for sendmail but mailx (at least) uses it) */

    case 'i':
    if (*argrest == 0) dot_ends = FALSE; else badarg = TRUE;
    break;


    case 'M':

    /* -MC:  continue delivery of another message via an existing open
    file descriptor. This option is used for an internal call by the
    smtp transport when there is a pending message waiting to go to an
    address to which it has got a connection. Four subsequent arguments are
    required: transport name, host name, sequence number, and message_id.
    Transports may decline to create new processes if the sequence number
    gets too big. The channel is stdin. This (-MC) must be the last argument.
    There's a subsequent check that the real-uid is privileged. */

    if (strcmp(argrest, "C") == 0)
      {
      if (argc != i + 5)
        {
        fprintf(stderr, "exim: too many or too few arguments after -MC\n");
        return EXIT_FAILURE;
        }

      if (msg_action_arg >= 0)
        {
        fprintf(stderr, "exim: incompatible arguments\n");
        return EXIT_FAILURE;
        }

      continue_transport = argv[++i];
      continue_hostname = argv[++i];
      continue_sequence = atoi(argv[++i]);
      msg_action = MSG_DELIVER;
      msg_action_arg = ++i;
      forced_delivery = TRUE;
      queue_run_pid = passed_qr_pid;
      queue_run_pipe = passed_qr_pipe;

      if (!mac_ismsgid(argv[i]))
        {
        fprintf(stderr, "exim: malformed message id %s after -MC option\n",
          argv[i]);
        return EXIT_FAILURE;
        }
      break;
      }

    /* -MCA: set the smtp_authenticated flag; this is useful only when it
    precedes -MC (see above). The flag indicates that the host to which
    Exim is connected has accepted an AUTH sequence. */

    #ifdef HAVE_AUTH
    else if (strcmp(argrest, "CA") == 0)
      {
      smtp_authenticated = TRUE;
      break;
      }
    #endif

    /* -MCD: set the smtp_use_dsn flag; this is useful only when it
    precedes -MC (see above) */

    #ifdef SUPPORT_DSN
    /* This partial attempt at doing DSN was abandoned. This obsolete code is
    left here just in case. Nothing is documented. */

    else if (strcmp(argrest, "CD") == 0)
      {
      smtp_use_dsn = TRUE;
      break;
      }
    #endif

    /* -MCP: set the smtp_use_pipelining flag; this is useful only when
    it preceded -MC (see above) */

    else if (strcmp(argrest, "CP") == 0)
      {
      smtp_use_pipelining = TRUE;
      break;
      }

    /* -MCQ: pass on the pid of the queue-running process that started
    this chain of deliveries and the fd of its synchronizing pipe; this
    is useful only when it precedes -MC (see above) */

    else if (strcmp(argrest, "CQ") == 0)
      {
      if(++i < argc) passed_qr_pid = (pid_t)(atol(argv[i]));
        else badarg = TRUE;
      if(++i < argc) passed_qr_pipe = (int)(atol(argv[i]));
        else badarg = TRUE;
      break;
      }

    /* -MCS: set the smtp_use_size flag; this is useful only when it
    precedes -MC (see above) */

    else if (strcmp(argrest, "CS") == 0)
      {
      smtp_use_size = TRUE;
      break;
      }

    /* -MCT: set the tls_offered flag; this is useful only when it
    precedes -MC (see above). The flag indicates that the host to which
    Exim is connected has offered TLS support. */

    #ifdef SUPPORT_TLS
    else if (strcmp(argrest, "CT") == 0)
      {
      tls_offered = TRUE;
      break;
      }
    #endif

    /* -M[x]: various operations on the following list of message ids:
       -M    deliver the messages, ignoring next retry times and thawing
       -Mc   deliver the messages, checking next retry times, no thawing
       -Mf   freeze the messages
       -Mg   give up on the messages
       -Mt   thaw the messages
       -Mrm  remove the messages
    In the above cases, this must be the last option. There are also the
    following options which are followed by a single message id, and which
    act on that message. Some of them use the "recipient" addresses as well.
       -Mar  add recipient(s)
       -Mmad mark all recipients delivered
       -Mmd  mark recipients(s) delivered
       -Mes  edit sender
       -Meb  edit body
       -Mvb  show body
       -Mvh  show header
       -Mvl  show log
    */

    else if (*argrest == 0)
      {
      msg_action = MSG_DELIVER;
      forced_delivery = deliver_force_thaw = TRUE;
      }
    else if (strcmp(argrest, "ar") == 0)
      {
      msg_action = MSG_ADD_RECIPIENT;
      one_msg_action = TRUE;
      }
    else if (strcmp(argrest, "c") == 0)  msg_action = MSG_DELIVER;
    else if (strcmp(argrest, "eb") == 0)
      {
      msg_action = MSG_EDIT_BODY;
      one_msg_action = TRUE;
      }
    else if (strcmp(argrest, "es") == 0)
      {
      msg_action = MSG_EDIT_SENDER;
      one_msg_action = TRUE;
      }
    else if (strcmp(argrest, "f") == 0)  msg_action = MSG_FREEZE;
    else if (strcmp(argrest, "g") == 0)
      {
      msg_action = MSG_DELIVER;
      deliver_give_up = TRUE;
      }
    else if (strcmp(argrest, "mad") == 0)
      {
      msg_action = MSG_MARK_ALL_DELIVERED;
      }
    else if (strcmp(argrest, "md") == 0)
      {
      msg_action = MSG_MARK_DELIVERED;
      one_msg_action = TRUE;
      }
    else if (strcmp(argrest, "rm") == 0) msg_action = MSG_REMOVE;
    else if (strcmp(argrest, "t") == 0)  msg_action = MSG_THAW;
    else if (strcmp(argrest, "vb") == 0)
      {
      msg_action = MSG_SHOW_BODY;
      one_msg_action = TRUE;
      }
    else if (strcmp(argrest, "vh") == 0)
      {
      msg_action = MSG_SHOW_HEADER;
      one_msg_action = TRUE;
      }
    else if (strcmp(argrest, "vl") == 0)
      {
      msg_action = MSG_SHOW_LOG;
      one_msg_action = TRUE;
      }
    else { badarg = TRUE; break; }

    /* All the -Mxx options require at least one message id. */

    msg_action_arg = i + 1;
    if (msg_action_arg >= argc)
      {
      fprintf(stderr, "exim: no message ids given after %s option\n", arg);
      return EXIT_FAILURE;
      }

    /* Some require only message ids to follow */

    if (!one_msg_action)
      {
      int j;
      for (j = msg_action_arg; j < argc; j++) if (!mac_ismsgid(argv[j]))
        {
        fprintf(stderr, "exim: malformed message id %s after %s option\n",
          argv[j], arg);
        return EXIT_FAILURE;
        }
      goto END_ARG;   /* Remaining args are ids */
      }

    /* Others require only one message id, possibly followed by addresses,
    which will be handled as normal arguments. */

    else
      {
      if (!mac_ismsgid(argv[msg_action_arg]))
        {
        fprintf(stderr, "exim: malformed message id %s after %s option\n",
          argv[msg_action_arg], arg);
        return EXIT_FAILURE;
        }
      i++;
      }
    break;


    /* Some programs seem to call the -om option without the leading o;
    for sendmail it askes for "me too". Exim always does this. */

    case 'm':
    if (*argrest != 0) badarg = TRUE;
    break;


    /* -N: don't do delivery - a debugging option that stops transports doing
    their thing. It implies debugging. */

    case 'N':
    if (*argrest == 0)
      {
      dont_deliver = TRUE;
      if (debug_level <= 0)
        {
        debug_level = 1;
        debug_file = stderr;
        }
      }
    else badarg = TRUE;
    break;


    /* -n: This means "don't alias" in sendmail, apparently. Just ignore
    it. */

    case 'n':
    break;


    case 'o':

    /* -oA: Set an argument for the bi command (sendmail's "alternate alias
    file" option). */

    if (*argrest == 'A')
      {
      alias_arg = argrest + 1;
      if (alias_arg[0] == 0)
        {
        if (i+1 < argc) alias_arg = argv[++i]; else
          {
          fprintf(stderr, "exim: string expected after -oA\n");
          exit(EXIT_FAILURE);
          }
        }
      }

    /* -oB: Set a batch max value for (at present only SMTP) remote
    deliveries */

    else if (*argrest == 'B')
      {
      char *p = argrest + 1;
      if (p[0] == 0)
        {
        if (i+1 < argc && isdigit((uschar)(argv[i+1][0]))) p = argv[++i]; else
          {
          batch_max = 1;
          p = NULL;
          }
        }

      if (p != NULL)
        {
        if (!isdigit((uschar)*p))
          {
          fprintf(stderr, "exim: number expected after -oB\n");
          exit(EXIT_FAILURE);
          }
        batch_max = atoi(p);
        }
      }

    /* -odb: background delivery */

    else if (strcmp(argrest, "db") == 0)
      {
      synchronous_delivery = FALSE;
      arg_queue_only = FALSE;
      queue_only_set = TRUE;
      }

    /* -odf: foreground delivery (smail-compatible option); same effect as
       -odi: interactive (synchronous) delivery (sendmail-compatible option)
    */

    else if (strcmp(argrest, "df") == 0 || strcmp(argrest, "di") == 0)
      {
      synchronous_delivery = TRUE;
      arg_queue_only = FALSE;
      queue_only_set = TRUE;
      }

    /* -odq: queue only */

    else if (strcmp(argrest, "dq") == 0)
      {
      synchronous_delivery = FALSE;
      arg_queue_only = TRUE;
      queue_only_set = TRUE;
      }

    /* -odqr: queue remote addresses only - do local deliveries */

    else if (strcmp(argrest, "dqr") == 0)
      {
      queue_remote = TRUE;
      }

    /* -odqs: queue SMTP only - do local deliveries and remote routing,
    but no remote delivery */

    else if (strcmp(argrest, "dqs") == 0)
      {
      queue_smtp = TRUE;
      }

    /* -oex: Sendmail error flags. As these are also accepted without the
    leading -o prefix, for compatibility with vacation and other callers,
    they are handled with -e above. */

    /* -oi:     Set flag so dot doesn't end non-SMTP input (same as -i)
       -oitrue: Another sendmail syntax for the same */

    else if (strcmp(argrest, "i") == 0 ||
             strcmp(argrest, "itrue") == 0)
      dot_ends = FALSE;

    /* -oM*: Set various characteristics for an incoming message; actually
    acted on for trusted callers only (or when testing things). */

    else if (*argrest == 'M')
      {
      if (i+1 >= argc)
        {
        fprintf(stderr, "exim: data expected after -o%s\n", argrest);
        exit(EXIT_FAILURE);
        }

      /* -oMa: Set sender host address */

      if (strcmp(argrest, "Ma") == 0) sender_host_address = argv[++i];

      /* -oMas: setting authenticated sender */

      else if (strcmp(argrest, "Mas") == 0) authenticated_sender = argv[++i];

      /* -oMai: setting authenticated id */

      else if (strcmp(argrest, "Mai") == 0) authenticated_id = argv[++i];

      /* -oMi: Set incoming interface address */

      else if (strcmp(argrest, "Mi") == 0) interface_address = argv[++i];

      /* -oMr: Received protocol (root or exim only) */

      else if (strcmp(argrest, "Mr") == 0) received_protocol = argv[++i];

      /* -oMs: Set sender host name */

      else if (strcmp(argrest, "Ms") == 0) sender_host_name = argv[++i];

      /* -oMt: Set sender ident */

      else if (strcmp(argrest, "Mt") == 0) sender_ident = argv[++i];

      /* Else a bad argument */

      else
        {
        badarg = TRUE;
        break;
        }
      }

    /* -om: Me-too flag for aliases. Exim always does this. Some programs
    seem to call this as -m (undocumented), so that is also accepted (see
    above). */

    else if (strcmp(argrest, "m") == 0) {}

    /* -oo: An ancient flag for old-style addresses which still seems to
    crop up in some calls (see in SCO). */

    else if (strcmp(argrest, "o") == 0) {}

    /* -or <n>: set timeout for non-SMTP acceptance */

    else if (*argrest == 'r')
      {
      if (argrest[1] == 0)
        {
        if (i+1 < argc)
          arg_accept_timeout = readconf_readtime(argv[++i], 0);
        }
      else arg_accept_timeout = readconf_readtime(argrest + 1, 0);
      if (arg_accept_timeout < 0)
        {
        fprintf(stderr, "exim: bad time value %s: abandoned\n", argv[i]);
        exit(EXIT_FAILURE);
        }
      }

    /* -oX <n>: Set SMTP listening port to <n> */

    else if (strcmp(argrest, "X") == 0)
      {
      char *end;
      smtp_port = strtol(argv[++i], &end, 10);
      while (isspace((uschar)*end)) end++;
      if (*end != 0)
        {
        fprintf(stderr, "exim: bad port number %s: abandoned\n", argv[i]);
        exit(EXIT_FAILURE);
        }
      }

    /* Unknown -o argument */

    else badarg = TRUE;
    break;


    /* -ps: force Perl startup; -pd force delayed Perl startup */

    #ifdef EXIM_PERL
    case 'p':
    if (*argrest == 's' && argrest[1] == 0) perl_start_option = 1;
    else
      if (*argrest == 'd' && argrest[1] == 0) perl_start_option = -1;
    else badarg = TRUE;
    break;
    #endif


    case 'q':

    /* -qq...: Do queue runs in a 2-stage manner */

    if (*argrest == 'q')
      {
      queue_2stage = TRUE;
      argrest++;
      }

    /* -qf...: Run the queue, forcing deliveries
       -qff..: Ditto, forcing thawing as well */

    if (*argrest == 'f')
      {
      queue_run_force = TRUE;
      if (*(++argrest) == 'f')
        {
        deliver_force_thaw = TRUE;
        argrest++;
        }
      }

    /* -q[f][f]l...: Run the queue only on local deliveries */

    if (*argrest == 'l')
      {
      queue_run_local = TRUE;
      argrest++;
      }

    /* -q[f][f][l]: Run the queue, optionally forced, optionally local only,
    optionally starting from a given message id. */

    if (*argrest == 0 &&
        (i + 1 >= argc || argv[i+1][0] == '-' || mac_ismsgid(argv[i+1])))
      {
      queue_interval = 0;
      if (i+1 < argc && mac_ismsgid(argv[i+1]))
        start_queue_run_id = argv[++i];
      if (i+1 < argc && mac_ismsgid(argv[i+1]))
        stop_queue_run_id = argv[++i];
      }

    /* -q[f][f][l]<n>: Run the queue at regular intervals, optionally forced,
    optionally local only. */

    else
      {
      if (*argrest != 0)
        queue_interval = readconf_readtime(argrest, 0);
      else
        queue_interval = readconf_readtime(argv[++i], 0);
      if (queue_interval <= 0)
        {
        fprintf(stderr, "exim: bad time value %s: abandoned\n", argv[i]);
        exit(EXIT_FAILURE);
        }
      }
    break;


    case 'R':   /* Synonymous with -qR... */

    /* -Rf:   As -R (below) but force all deliveries,
       -Rff:  Ditto, but also thaw all frozen messages,
       -Rr:   String is regex
       -Rrf:  Regex and force
       -Rrff: Regex and force and thaw

    in all cases provided there are no further characters in this
    argument. */

    if (*argrest != 0)
      {
      int i;
      for (i = 0; i < sizeof(rsopts)/sizeof(char *); i++)
        {
        if (strcmp(argrest, rsopts[i]) == 0)
          {
          if (i != 2) queue_run_force = TRUE;
          if (i >= 2) deliver_selectstring_regex = TRUE;
          if (i == 1 || i == 4) deliver_force_thaw = TRUE;
          argrest += (int)strlen(rsopts[i]);
          }
        }
      }

    /* -R: Set string to match in addresses for forced queue run to
    pick out particular messages. */

    if (*argrest == 0)
      {
      if (i+1 < argc) deliver_selectstring = argv[++i]; else
        {
        fprintf(stderr, "exim: string expected after -R\n");
        exit(EXIT_FAILURE);
        }
      }
    else deliver_selectstring = argrest;
    if (queue_interval < 0) queue_interval = 0;
    break;


    /* -r: an obsolete synonym for -f (see above) */


    /* -S: Like -R but works on sender. */

    case 'S':   /* Synonymous with -qR... */

    /* -Sf:   As -S (below) but force all deliveries,
       -Sff:  Ditto, but also thaw all frozen messages,
       -Sr:   String is regex
       -Srf:  Regex and force
       -Srff: Regex and force and thaw

    in all cases provided there are no further characters in this
    argument. */

    if (*argrest != 0)
      {
      int i;
      for (i = 0; i < sizeof(rsopts)/sizeof(char *); i++)
        {
        if (strcmp(argrest, rsopts[i]) == 0)
          {
          if (i != 2) queue_run_force = TRUE;
          if (i >= 2) deliver_selectstring_sender_regex = TRUE;
          if (i == 1 || i == 4) deliver_force_thaw = TRUE;
          argrest += (int)strlen(rsopts[i]);
          }
        }
      }

    /* -S: Set string to match in addresses for forced queue run to
    pick out particular messages. */

    if (*argrest == 0)
      {
      if (i+1 < argc) deliver_selectstring_sender = argv[++i]; else
        {
        fprintf(stderr, "exim: string expected after -S\n");
        exit(EXIT_FAILURE);
        }
      }
    else deliver_selectstring_sender = argrest;
    if (queue_interval < 0) queue_interval = 0;
    break;


    /* -t: Set flag to extract recipients from body of message. */

    case 't':
    if (*argrest == 0) extract_recipients = TRUE;
      else badarg = TRUE;
    break;


    /* -U: This means "initial user submission" in sendmail, apparently. The
    doc claims that in future sendmail may refuse syntactically invalid
    messages instead of fixing them. For the moment, we just ignore it. */

    case 'U':
    break;


    /* -v: verify things - this is the same as -d or -d1. */

    case 'v':
    if (*argrest == 0)
      {
      if (debug_level <= 0)
        {
        debug_level = 1;
        debug_file = stderr;
        }
      }
    else badarg = TRUE;
    break;


    /* -x: AIX uses this to indicate some fancy 8-bit character stuff:

      The -x flag tells the sendmail command that mail from a local
      mail program has National Language Support (NLS) extended characters
      in the body of the mail item. The sendmail command can send mail with
      extended NLS characters across networks that normally corrupts these
      8-bit characters.

    As Exim is 8-bit clean, it just ignores this flag. */

    case 'x':
    if (*argrest != 0) badarg = TRUE;
    break;

    /* All other initial characters are errors */

    default:
    badarg = TRUE;
    break;
    }         /* End of high-level switch statement */

  /* Failed to recognize the option, or syntax error */

  if (badarg)
    {
    fprintf(stderr, "exim abandoned: unknown, malformed, or incomplete "
      "option %s\n", arg);
    exit(EXIT_FAILURE);
    }
  }


/* Arguments have been processed. Check for incompatibilities. */

END_ARG:
if ((
    (smtp_input || extract_recipients || recipients_arg < argc) &&
    (daemon_listen || queue_interval >= 0 || bi_option ||
      test_retry_arg >= 0 || test_rewrite_arg >= 0 ||
      filter_test != NULL || (msg_action_arg > 0 && !one_msg_action))
    ) ||
    (
    msg_action_arg > 0 &&
    (daemon_listen || queue_interval >= 0 || list_variables || checking ||
     bi_option || test_retry_arg >= 0 || test_rewrite_arg >= 0)
    ) ||
    (
    (daemon_listen || queue_interval >= 0) &&
    (sender_address != NULL || list_variables || list_queue || checking ||
     bi_option)
    ) ||
    (
    daemon_listen && queue_interval == 0
    ) ||
    (
    list_variables &&
    (checking || smtp_input || extract_recipients ||
      filter_test != NULL || bi_option)
    ) ||
    (
    verify_only &&
    (address_test_mode || smtp_input || extract_recipients ||
      filter_test != NULL || bi_option)
    ) ||
    (
    address_test_mode && (smtp_input || extract_recipients ||
      filter_test != NULL || bi_option)
    ) ||
    (
    smtp_input && (sender_address != NULL || filter_test != NULL ||
      extract_recipients)
    ) ||
    (
    deliver_selectstring != NULL && queue_interval < 0
    )
   )
  {
  fprintf(stderr, "exim: incompatible command-line options or arguments\n");
  exit(EXIT_FAILURE);
  }

/* If debugging is set up, set the file descriptor to pass on to child
processes. It should, of course, be 2 for stderr. */

if (debug_file != NULL) debug_fd = fileno(debug_file);

if (list_variables)
  {
    if ( check_permissions(config_filename, real_uid, real_gid) != 0) 
      {
	printf("No permissions to read config file %s\n", config_filename);
	exit(EXIT_FAILURE);
      }
  }

/* Exim is normally entered as root (but some special configurations are
possible that don't do this). The security level controls how it uses
set{u,g}id and/or sete{g,u}id to reduce its privilege when not necessary.
However, it always spins off sub-processes that set their uid and gid as
required for local delivery. We don't want to pass on any extra groups that
root may belong to, so we want to get rid of them all at this point.

We need to obey setgroups() at this stage, before possibly giving up root
privilege for a changed configuration file, but later on we might need to
check on the additional groups for the admin user privilege - can't do that
till after reading the config, which might specify the exim gid. Therefore,
save the group list here first. */

group_count = getgroups(NGROUPS_MAX, group_list);

/* There is a fundamental difference in some BSD systems in the matter of
groups. FreeBSD and BSDI are known to be different; NetBSD and OpenBSD are
known not to be different. On the "different" systems there is a single group
list, and the first entry in it is the current group. On all other versions of
Unix there is a supplementary group list, which is in *addition* to the current
group. Consequently, to get rid of all extraneous groups on a "standard" system
you pass over 0 groups to setgroups(), while on a "different" system you pass
over a single group - the current group, which is always the first group in the
list. Calling setgroups() with zero groups on a "different" system results in
an error return. The following code should cope with both types of system.

However, if this process isn't running as root, setgroups() can't be used
since you have to be root to run it, even if throwing away groups. Not being
root here happens only in some unusual configurations. We just ignore the
error. */

if (setgroups(0, NULL) != 0)
  {
  if (setgroups(1, group_list) != 0 && !unprivileged)
    {
    fprintf(stderr, "exim: setgroups() failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
    }
  }

/* If the configuration file name has been altered by an argument on the
command line (either a new file name or a macro definition) and the caller is
not root or the exim user, or if this is a filter testing run, remove any
setuid privilege the program has, and run as the underlying user. Otherwise,
set the real ids to the effective values (should be root unless run from
inetd, which it can either be root or the exim uid, if one is configured).
When running with the security options set, root privilege will subsequently
be relinquished as soon as possible.

There is a private mechanism for bypassing some of this, in order to make it
possible to test lots of configurations automatically, without having either to
recompile each time, or to patch in an actual configuration file name and other
values (such as the path name). However, if running in the test harness,
pretend that configuration file changes and macro definitions haven't
happened. */

if ((                                            /* EITHER */
    (config_changed || macros != NULL) &&        /* Config changed, and */
    real_uid != root_uid &&                      /* Not root, and */
    (!exim_uid_set || real_uid != exim_uid) &&   /* Not exim, and */
    !running_in_test_harness                     /* Not fudged */
    ) ||                                         /*   OR   */
    expansion_test                               /* expansion testing */
    ||                                           /*   OR   */
    filter_test != NULL)                         /* Filter testing */
  {
  exim_setugid(real_uid, real_gid, "removing setuid privilege (1)");
  removed_privilege = TRUE;
  DEBUG(9) debug_print_ids("Removed setuid privilege:");

  /* In the normal case when Exim is called like this, stderr is available
  and should be used for any logging information because attempts to write
  to the log will usually fail. To arrange this, we unset really_exim. However,
  if no stderr is available there is no point - we might as well have a go
  at the log (if it fails, syslog will be written). */

  if (log_stderr != NULL) really_exim = FALSE;
  }

/* Privilege is to be retained; set the real uid to the effective so that
subsequent re-execs of Exim are done by a privileged user. */

else exim_setugid(geteuid(), getegid(), "forcing real = effective");

/* If testing a filter, open the file now, before wasting time doing other
setups and reading the message. */

if (filter_test != NULL)
  {
  filter_fd = open(filter_test, O_RDONLY);
  if (filter_fd < 0)
    {
    fprintf(stderr, "exim: failed to open %s: %s\n", filter_test,
      strerror(errno));
    return EXIT_FAILURE;
    }
  }

/* Read the main runtime configuration data; this gives up if there
is a failure. It leaves the configuration file open so that the subsequent
configuration data for delivery can be read if needed. */

readconf_main();

/* Get rid of any bogus timezone setting. This can arise when Exim is called by
a user who has set the TZ variable. This then affects the timestamps in log
files and in Received: headers, and any created Date: header lines. The
required timezone is settable in the configuration file, so nothing can be done
about this earlier - but hopefully nothing will normally be logged earlier than
this. We have to make a new environment if TZ is wrong, but don't bother if
timestamps_utc is set, because then all times are in UTC anyway. */

if (!timestamps_utc)
  {
  char *envtz = getenv("TZ");
  if ((envtz == NULL && timezone_string != NULL) ||
      (envtz != NULL &&
        (timezone_string == NULL ||
         strcmp(timezone_string, envtz) != 0)))
    {
    char **p = environ;
    char **new;
    char **newp;
    int count = 0;
    while (*p++ != NULL) count++;
    if (envtz == NULL) count++;
    newp = new = malloc(sizeof(char *) * (count + 1));
    for (p = environ; *p != NULL; p++)
      {
      if (strncmp(*p, "TZ=", 3) == 0) continue;
      *newp++ = *p;
      }
    if (timezone_string != NULL)
      {
      *newp = malloc((int)strlen(timezone_string) + 4);
      sprintf(*newp++, "TZ=%s", timezone_string);
      }
    *newp = NULL;
    environ = new;
    tzset();
    DEBUG(9) debug_printf("Reset TZ to %s: time is %s\n", timezone_string,
      tod_stamp(tod_log));
    }
  }

/* If we have removed the setuid privilege because of -C or -D, and it turns
out we were running as the exim user defined in the configuration file, log
an error, because this doesn't work. The exim user has to be built into the
binary for -C/-D to retain privilege. */

if (removed_privilege && (config_changed || macros != NULL) &&
    exim_uid_set && real_uid == exim_uid)
  log_write(0, LOG_MAIN|LOG_PANIC,
    "exim user (uid=%d) is defined only at runtime; privilege lost for %s",
    (int)exim_uid, config_changed? "-C" : "-D");

/* Start up Perl interpreter if Perl support is configured and there is a
perl_startup option, and the configuration or the command line specifies
initializing starting. Note that the global variables are actually called
opt_perl_xxx to avoid clashing with perl's namespace (perl_*). */

#ifdef EXIM_PERL
if (perl_start_option != 0)
  opt_perl_at_start = (perl_start_option > 0);
if (opt_perl_at_start && opt_perl_startup != NULL)
  {
  char *errstr;
  DEBUG(9) debug_printf("Starting Perl interpreter\n");
  errstr = init_perl(opt_perl_startup);
  if (errstr != NULL)
    {
    fprintf(stderr, "exim: error in perl_startup code: %s\n", errstr);
    return EXIT_FAILURE;
    }
  opt_perl_started = TRUE;
  }
#endif /* EXIM_PERL */

/* Log the arguments of the call if the configuration file said so. This is
a debugging feature for finding out what arguments certain MUAs actually use.
Don't attempt it if logging is disabled, or if listing variables or if
verifying/testing addresses or expansions. */

if (log_arguments && really_exim && !list_variables && !checking)
  {
  int i;
  char *p = big_buffer;
  sprintf(big_buffer, "%d args:", argc);
  while (*p) p++;
  for (i = 0; i < argc; i++)
    {
    int len = (int)strlen(argv[i]);
    if (p + len + 8 >= big_buffer + big_buffer_size)
      {
      strcpy(p, " ...");
      log_write(0, LOG_MAIN, "%s", big_buffer);
      strcpy(big_buffer, "...");
      p = big_buffer + 3;
      }
    sprintf(p, " %.*s", big_buffer_size - (p - big_buffer) - 2,
      string_printing(argv[i]));
    while (*p) p++;
    }
  log_write(0, LOG_MAIN, "%s", big_buffer);
  }

/* Handle calls with the -bi option. This is a sendmail option to rebuild *the*
alias file. Exim doesn't have such a concept, but this call is screwed into
Sun's YP makefiles. Handle this by calling a configured script, as the real
user who called Exim. The -oA option can be used to pass an argument to the
script. */

if (bi_option)
  {
  fclose(config_file);
  if (bi_command != NULL)
    {
    int i = 0;
    char *argv[3];
    argv[i++] = bi_command;
    if (alias_arg != NULL) argv[i++] = alias_arg;
    argv[i++] = NULL;

    exim_setugid(real_uid, real_gid, "running bi_command");

    DEBUG(2) debug_printf("exec %.256s %.256s\n", argv[0],
      (argv[1] == NULL)? "" : argv[1]);

    execv(argv[0], argv);
    fprintf(stderr, "exim: exec failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
    }
  else
    {
    DEBUG(2) debug_printf("-bi used but bi_command not set; exiting\n");
    exit(EXIT_SUCCESS);
    }
  }


/* Build the argument list for calling Exim to send an error message. */

mailer_argv = store_get((18 + MAX_CLMACROS) * (sizeof(char *)));
mailer_argv[0] = exim_path;
mailer_argv[1] = "-t";
mailer_argv[2] = "-oem";
mailer_argv[3] = "-oi";
mailer_argv[4] = "-f";
mailer_argv[5] = "<>";

/* Set up the -E option to give the current message id, if any. If debugging is
turned on, arrange to pass the setting when we re-exec exim for error messages,
etc. Also pass on -N if set (-d is always set if -N is.) The argument list has
several 0 entries at the end, all but the last of which can be overwritten. */

mailer_argv[6] = message_id_option;

i = 7;
if (debug_level > 0)
  {
  mailer_argv[i++] = string_sprintf("-d%d", debug_level);
  if (dont_deliver) mailer_argv[i++] = "-N";
  }

if (synchronous_delivery) mailer_argv[i++] = "-odi";

/* Ensure any alternate configuration file and macro settings are passed on. */

if (config_changed)
  {
  mailer_argv[i++] = "-C";
  mailer_argv[i++] = config_filename;
  }

if (clmacro_count > 0)
  {
  memcpy(mailer_argv + i, clmacros, clmacro_count * sizeof(char *));
  i += clmacro_count;
  }

/* Terminate the argument list */

mailer_argv[i] = (char *)0;

/* If an SMTP message is being received and the uid is root or exim, check to
see whether this is a call from inetd by looking for a peername. We do this
early, while still root, so that we can also call os_getloadavg() if required,
because some OS need the first call to os_getloadavg() to be done while root,
in order to open a kernel file. The socket information is kept available for
use later. The cast is necessary for the case of an IPv6 socket. */

if (smtp_input && (real_uid == root_uid ||
    (exim_uid_set && real_uid == exim_uid) ||
    (!exim_uid_set && real_uid == EXIM_UID)))
  {
  int size = sizeof(inetd_sock);
  if (getpeername(0, (struct sockaddr *)(&inetd_sock), &size) == 0)
    {
    int family = ((struct sockaddr *)(&inetd_sock))->sa_family;
    if (family == AF_INET || family == AF_INET6) is_inetd = TRUE;
    }
  }

/* If the load average is going to be needed while receiving a message,
get it now, because some OS require the first call to os_getloadavg() to
be done as root. What a pain. */

if ((is_inetd && smtp_load_reserve >= 0) ||
    (queue_only_load >= 0 &&
      (smtp_input || extract_recipients ||
        (recipients_arg < argc && !checking))))
  load_average = os_getloadavg();

/* If an action on specific messages is requested, or if a daemon or queue
runner is being started, we need to know if Exim was called by an admin user.
This is the case if the real user is root or exim, or if the real group is
exim, or if one of the supplementary groups is exim. We don't fail all message
actions immediately if not admin_user, since some actions can be performed by
non-admin users. Instead, set admin_user for later interrogation.

Another group of privileged users are the trusted users. These are root,
exim, and any caller matching trusted_users or trusted_groups. Trusted callers
are permitted to specify sender_addresses with -f on the command line, and
other message parameters as well. */

if (real_uid == root_uid ||
    (exim_uid_set && real_uid == exim_uid) ||
    (exim_gid_set && real_gid == exim_gid))
  {
  admin_user = trusted_caller = TRUE;
  }

else
  {
  int i, j;

  /* If one of the supplementary groups is exim, or a group listed in
  admin_groups, the caller is an admin user. */

  for (i = 0; i < group_count; i++)
    {
    if (exim_gid_set && group_list[i] == exim_gid) admin_user = TRUE;
    else if (admin_groups != NULL)
      {
      for (j = 1; j <= (int)(admin_groups[0]); j++)
        if (admin_groups[j] == group_list[i])
          { admin_user = TRUE; break; }
      }
    if (admin_user) break;
    }

  /* Scan trusted_users and trusted_groups */

  if (trusted_users != NULL)
    {
    for (i = 1; i <= (int)(trusted_users[0]); i++)
      if (trusted_users[i] == real_uid)
        { trusted_caller = TRUE; break; }
    }

  if (!trusted_caller && trusted_groups != NULL)
    {
    for (i = 1; i <= (int)(trusted_groups[0]); i++)
      {
      if (trusted_groups[i] == real_gid)
        trusted_caller = TRUE;
      else for (j = 0; j < group_count; j++)
        {
	if (trusted_groups[i] == group_list[j])
	  { trusted_caller = TRUE; break; }
        }
      if (trusted_caller) break;
      }
    }
  }

DEBUG(2)
  {
  if (admin_user) debug_printf("Caller is an admin user\n");
  if (trusted_caller) debug_printf("Caller is a trusted user\n");
  }

/* Only an admin user may start the daemon or force a queue run in the default
configuration, but the queue run restriction can be relaxed. Only an admin
user may request that a message be returned to its sender forthwith. Only an
admin user may specify a debug level greater than 1 (because it might show
passwords, etc. in lookup queries). Only an admin user may request a queue
count or queue list. */

if (!admin_user)
  {
  if (deliver_give_up || daemon_listen ||
     (count_queue && queue_list_requires_admin) ||
     (list_queue && queue_list_requires_admin) ||
     (queue_interval >= 0 && prod_requires_admin))
    {
    fprintf(stderr, "exim: permission denied\n");
    exit(EXIT_FAILURE);
    }
  if (debug_level > 1) debug_level = 1;
  }

/* Set the working directory to be the top-level spool directory. We don't rely
on this in the code, which always uses fully qualified names, but it's useful
for core dumps etc. Don't complain if it fails - the spool directory might not
be generally accessible and calls with the -C option (and others) have lost
privilege by now. */

if (chdir(spool_directory) != 0)
  {
  (void)directory_make(spool_directory, "", SPOOL_DIRECTORY_MODE, TRUE);
  (void)chdir(spool_directory);
  }


/* If the real user is not root or the exim uid, the argument for passing
in an open TCP/IP connection for another message is not permitted, nor is
running with the -N option for any delivery action, unless this call to exim is
one that supplied an input message, or we are using a patched exim for
regression testing. */

if (real_uid != root_uid && (!exim_uid_set || real_uid != exim_uid) &&
     (continue_hostname != NULL ||
       (dont_deliver &&
         (queue_interval >= 0 || daemon_listen || msg_action_arg > 0)
       )) && !running_in_test_harness)
  {
  fprintf(stderr, "exim: Permission denied\n");
  return EXIT_FAILURE;
  }

/* If the caller is not trusted, certain arguments are ignored when running for
real, but are permitted when checking things (-be, -bv, -bt, -bh, -bf). Note
that authority for performing certain actions on messages is tested in the
queue_action() function. */

if (!trusted_caller && !checking && filter_test == NULL)
  {
  sender_host_name = sender_host_address = interface_address =
    sender_ident = received_protocol = NULL;
  sender_host_port = 0;
  authenticated_sender = authenticated_id = NULL;
  }

/* If a sender host address is set, extract the optional port number off the
end of it. Then check its syntax. */

else if (sender_host_address != NULL)
  {
  sender_host_port = host_extract_port(sender_host_address);
  if(!string_is_ip_address(sender_host_address, NULL))
    {
    fprintf(stderr, "exim abandoned: \"%s\" is not an IP address\n",
      sender_host_address);
    exit(EXIT_FAILURE);
    }
  }

/* The queue_only configuration option can be overridden by -odx on the command
line, and the accept_timeout option can be overridden by -or. */

if (queue_only_set) queue_only = arg_queue_only;
if (arg_accept_timeout >= 0) accept_timeout = arg_accept_timeout;

/* If an exim uid is specified, handle setuid/seteuid setting according to the
security level, unless we have already removed privilege. The macros
mac_setegid and mac_seteuid are defined as -1 on systems which do not have
seteuid and setegid, so that this code will compile on all systems. However,
the security level won't be set to values that cause the use of the sete{u,g}id
functions on systems that don't have them (see readconf.c). */

if (exim_uid_set && !removed_privilege)
  {
  /* Level 1: Use seteuid to reduce privilege at all times it is not needed,
  but use setuid if we know that privilege is never going to be required again,
  typically for enquiry type calls. However, privilege might be needed for
  the forwardfile director, so can't lose it for verification. */

  if (security_level == 1)
    {
    if (queue_interval < 0 &&                  /* not running the queue */
        !daemon_listen &&                      /* and not starting the daemon */
        (msg_action_arg < 0 ||
          msg_action != MSG_DELIVER) &&        /* and not delivering */
        (!checking || host_checking) &&        /* and not checking address */
        (list_variables ||                     /* and either not receiving */
          (recipients_arg >= argc && !extract_recipients && !smtp_input) ||
          queue_only))                         /* or just queueing */
      {
      exim_setugid(exim_uid, exim_gid, "removing setuid privilege (2)");
      }

    /* Running the queue, starting daemon, delivering, checking address,
    or receiving with queue_only FALSE. */

    else
      {
      mac_setegid(exim_gid);
      mac_seteuid(exim_uid);
      }
    }

  /* Level 2: if neither starting the daemon nor delivering messages,
  verifying, or testing an address, always use setuid to remove all root
  privilege. Level 2 makes no use of the sete{g,u}id functions; subsequent
  re-execs are used to regain root privilege for delivering.

  Level 3: as level 2, but in addition use sete{g,u}id at other times when
  having root privilege is not needed.

  Level 4: use setuid to remove privilege in all circumstances except when
  starting the daemon (so that it can bind to port 25 first). This means we
  can deliver only as the Exim user. */

  else
    {
    if (!daemon_listen &&                   /* not starting the daemon */
	 (				    /*	     AND	       */
         security_level >= 4 ||             /* either at level 4       */
           (                                /*       OR                */
           queue_interval < 0 &&            /* not running the queue   */
           (msg_action_arg < 0 ||           /*       and               */
             msg_action != MSG_DELIVER) &&  /* not delivering and      */
           (!checking || host_checking)     /* not checking addresses  */
           )
         ))
      {
      exim_setugid(exim_uid, exim_gid, "removing setuid privilege (3)");
      }

    /* Level 3, or starting the daemon with level 4. */

    else if (security_level > 2)
      {
      mac_setegid(exim_gid);
      mac_seteuid(exim_uid);
      }
    }
  }


/* Handle a request to list the delivery queue */

if (list_queue)
  {
  set_process_info("listing the queue");
  queue_list(list_queue_option, argv + recipients_arg, argc - recipients_arg);
  exit(EXIT_SUCCESS);
  }

/* Handle a request to count the delivery queue */

if (count_queue)
  {
  set_process_info("counting the queue");
  queue_count();
  exit(EXIT_SUCCESS);
  }

/* Handle actions on specific messages, except for the force delivery action,
which is done below. Some actions take a whole list of message ids, which
are known to continue up to the end of the arguments. Others take a single
message id and then operate on the recipients list. */

if (msg_action_arg > 0 && msg_action != MSG_DELIVER)
  {
  int yield = EXIT_SUCCESS;
  set_process_info("acting on specified messages");

  if (!one_msg_action)
    {
    for (i = msg_action_arg; i < argc; i++)
      if (!queue_action(argv[i], msg_action, NULL, 0, 0))
        yield = EXIT_FAILURE;
    }

  else if (!queue_action(argv[msg_action_arg], msg_action, argv, argc,
    recipients_arg)) yield = EXIT_FAILURE;
  exit(yield);
  }


/* All the modes below here require the delivery configuration options
to be set up. Doing it here saves effort when several processes are
subsequently spun off. */

transport_init();
direct_init();
route_init();
readconf_retries();
readconf_rewrites();

/* If any authentication mechanisms are included, configure them. */

#ifdef HAVE_AUTH
  {
  auth_instance *au;
  readconf_driver_init("auth",
    (driver_instance **)(&auths),      /* chain anchor */
    (driver_info *)auths_available,    /* available drivers */
    sizeof(auth_info),                 /* size of info block */
    &auth_defaults,                    /* default values for generic options */
    sizeof(auth_instance),             /* size of instance block */
    optionlist_auths,                  /* generic options */
    optionlist_auths_size);

  for (au = auths; au != NULL; au = au->next)
    {
    if (au->public_name == NULL)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "no public name specified for "
        "the %s authenticator", au->name);
    }
  }
#endif

fclose(config_file);

/* Handle the -brt option. This is for checking out retry configurations.
The next three arguments are a domain name or a complete address, and
optionally two error numbers. All it does is to call the function that
scans the retry configuration data. */

if (test_retry_arg >= 0)
  {
  retry_config *yield;
  int basic_errno = 0;
  int more_errno = 0;
  char *s1, *s2;

  if (test_retry_arg >= argc)
    {
    printf("-brt needs a domain or address argument\n");
    exim_exit(EXIT_FAILURE);
    }
  s1 = argv[test_retry_arg++];
  s2 = NULL;

  /* If the first argument contains no @ and no . it might be a local user
  or it might be a single-component name. Treat as a domain. */

  if (strchr(s1, '@') == NULL && strchr(s1, '.') == NULL)
    {
    printf("Warning: \"%s\" contains no '@' and no '.' characters. It is "
      "being \ntreated as a one-component domain, not as a local part.\n\n",
      s1);
    }

  /* There may be an optional second domain arg. */

  if (test_retry_arg < argc && strchr(argv[test_retry_arg], '.') != NULL)
    s2 = argv[test_retry_arg++];

  /* The final arg is an error name */

  if (test_retry_arg < argc)
    {
    char *ss = argv[test_retry_arg];
    char *error =
      readconf_retry_error(ss, ss + (int)strlen(ss), &basic_errno, &more_errno);
    if (error != NULL)
      {
      printf("%s\n", error);
      return EXIT_FAILURE;
      }
    }

  yield = retry_find_config(s1, s2, basic_errno, more_errno);
  if (yield == NULL) printf("No retry information found\n"); else
    {
    retry_rule *r;
    more_errno = yield->more_errno;
    printf("Retry rule: %s  ", yield->destination);

    if (yield->basic_errno == ERRNO_EXIMQUOTA)
      {
      printf("quota%s%s  ",
        (more_errno > 0)? "_" : "",
        (more_errno > 0)? readconf_printtime(more_errno) : "");
      }
    else if (yield->basic_errno == ECONNREFUSED)
      {
      printf("refused%s%s  ",
        (more_errno > 0)? "_" : "",
        (more_errno == 'M')? "MX" :
        (more_errno == 'A')? "A" : "");
      }
    else if (yield->basic_errno == ETIMEDOUT)
      {
      printf("timeout%s%s  ",
        (more_errno > 0)? "_" : "",
        (more_errno =='D')? "DNS" :
        (more_errno == 'C')? "connect" : "");
      }

    for (r = yield->rules; r != NULL; r = r->next)
      {
      printf("%c,%s", r->rule, readconf_printtime(r->timeout)); /* Do not */
      printf(",%s", readconf_printtime(r->p1));                 /* amalgamate */
      if (r->rule == 'G')
        {
        int x = r->p2;
        int f = x % 1000;
        int d = 100;
        printf(",%d.", x/1000);
        do
          {
          printf("%d", f/d);
          f %= d;
          d /= 10;
          }
        while (f != 0);
        }
      printf("; ");
      }

    printf("\n");
    }
  exim_exit(EXIT_SUCCESS);
  }

/* Handle a request to list one or more configuration variables */

if (list_variables)
  {
  set_process_info("listing variables");
  if (recipients_arg >= argc) readconf_print("all", NULL);
    else for (i = recipients_arg; i < argc; i++)
      {
      if (i < argc - 1 &&
          (strcmp(argv[i], "director") == 0 ||
           strcmp(argv[i], "router") == 0 ||
           strcmp(argv[i], "transport") == 0 ||
           strcmp(argv[i], "authenticator") == 0))
        {
        readconf_print(argv[i+1], argv[i]);
        i++;
        }
      else readconf_print(argv[i], NULL);
      }
  exim_exit(EXIT_SUCCESS);
  }

/* Ensure the address of the mailmaster is fully qualified. Don't use
rewrite_address_qualify, as that frees the input store, and errors_address
may not be in dynamic store. */

if (strchr(errors_address, '@') == NULL)
  errors_address = string_sprintf("%s@%s", errors_address,
    qualify_domain_sender);

/* Search the configured transports for the ones that will be used for
generated pipe, file, and directory addresses and auto replies from the system
filter. If they are not found, do not complain now. A complaint will be
generated later if filtering address actually causes a pipe, file, directory or
auto-reply delivery to be generated. */

for (ti = transports; ti != NULL; ti = ti->next)
  {
  if (message_filter_pipe_transport != NULL &&
      strcmp(ti->name, message_filter_pipe_transport) == 0)
    transport_message_filter_pipe = ti;
  else if (message_filter_file_transport != NULL &&
           strcmp(ti->name, message_filter_file_transport) == 0)
    transport_message_filter_file = ti;
  else if (message_filter_reply_transport != NULL &&
           strcmp(ti->name, message_filter_reply_transport) == 0)
    transport_message_filter_reply = ti;
  else if (message_filter_directory_transport != NULL &&
           strcmp(ti->name, message_filter_directory_transport) == 0)
    transport_message_filter_directory = ti;
  else if (message_filter_directory2_transport != NULL &&
           strcmp(ti->name, message_filter_directory2_transport) == 0)
    transport_message_filter_directory2 = ti;
  }


/* Handle a request to deliver one or more messages that are already on the
queue. Values of msg_action other than MSG_DELIVER are dealt with above. This
is typically used for a small number when prodding by hand (when the option
forced_delivery will be set) or when re-execing to regain root privilege.
Each message delivery must happen in a separate process, so we fork a process
for each one, and run them sequentially so that debugging output doesn't get
intertwined, and to avoid spawning too many processes if a long list is given.
However, don't fork for the last one; this saves a process in the common case
when Exim is called to deliver just one message. */

if (msg_action_arg > 0)
  {
  if (prod_requires_admin && !admin_user)
    {
    fprintf(stderr, "exim: Permission denied\n");
    exim_exit(EXIT_FAILURE);
    }
  set_process_info("delivering specified messages");
  if (deliver_give_up) forced_delivery = deliver_force_thaw = TRUE;
  for (i = msg_action_arg; i < argc; i++)
    {
    int status;
    if (i == argc - 1)
      (void)deliver_message(argv[i], forced_delivery, deliver_give_up);
    else if (fork() == 0)
      {
      (void)deliver_message(argv[i], forced_delivery, deliver_give_up);
      _exit(EXIT_SUCCESS);
      }
    else wait(&status);
    }
  exim_exit(EXIT_SUCCESS);
  }


/* If only a single queue run is requested, without SMTP listening, we can just
turn into a queue runner, with an optional starting message id. */

if (queue_interval == 0 && !daemon_listen)
  {
  DEBUG(1) debug_printf("Single queue run%s%s%s%s\n",
    (start_queue_run_id == NULL)? "" : " starting at ",
    (start_queue_run_id == NULL)? "" : start_queue_run_id,
    (stop_queue_run_id == NULL)?  "" : " stopping at ",
    (stop_queue_run_id == NULL)?  "" : stop_queue_run_id);
  set_process_info("running the queue (single queue run)");
  queue_run(start_queue_run_id, stop_queue_run_id, FALSE);
  exit(EXIT_SUCCESS);
  }


/* Find the login name of the real user running this process. This is always
needed when receiving a message, because it is written into the spool file. It
may also be used to construct a from: or a sender: header, and in this case we
need the user's full name as well, so save a copy of it, checked for RFC822
syntax and munged if necessary, if it hasn't previously been set by the -F
argument. We try to get the passwd entry more than once, in case NIS or other
delays are in evidence. Save the home directory for use in filter testing
(only). */

for (i = 1; i <= 10; i++)
  {
  if ((pw = getpwuid(real_uid)) != NULL)
    {
    originator_login = string_copy(pw->pw_name);
    originator_home = string_copy(pw->pw_dir);

    /* If user name has not been set by -F, set it from the passwd entry
    unless -f has been used to set the sender address by a trusted user. */

    if (originator_name == NULL)
      {
      if (sender_address == NULL ||
           (!trusted_caller && filter_test == NULL))
        {
        char *name = pw->pw_gecos;
        char *amp = strchr(name, '&');
        char buffer[256];

        /* Most Unix specify that a '&' character in the gecos field is
        replaced by a copy of the login name, and some even specify that
        the first character should be upper cased, so that's what we do. */

        if (amp != NULL)
          {
          int loffset;
          string_format(buffer, sizeof(buffer), "%.*s%n%s%s",
            amp - name, name, &loffset, originator_login, amp + 1);
          buffer[loffset] = toupper(buffer[loffset]);
          name = buffer;
          }

        /* If a pattern for matching the gecos field was supplied, apply
        it and then expand the name string. */

        if (gecos_pattern != NULL && gecos_name != NULL)
          {
          pcre *re;
          re = regex_must_compile(gecos_pattern, FALSE, TRUE); /* Use malloc */

          if (regex_match_and_setup(re, name, 0, -1))
            {
            char *new_name = expand_string(gecos_name);
            expand_nmax = -1;
            if (new_name != NULL)
              {
              DEBUG(4) debug_printf("user name \"%s\" extracted from "
                "gecos field \"%s\"\n", new_name, name);
              name = new_name;
              }
            else DEBUG(4) debug_printf("failed to expand gecos_name string "
              "\"%s\": %s\n", gecos_name, expand_string_message);
            }
          else DEBUG(4) debug_printf("gecos_pattern \"%s\" did not match "
            "gecos field \"%s\"\n", gecos_pattern, name);
          store_free(re);
          }
        originator_name = string_copy(name);
        }

      /* A trusted caller has used -f but not -F */

      else originator_name = "";
      }

    /* Break the retry loop */

    break;
    }
  sleep(1);
  }

/* If we cannot get a user login, log the incident and give up, unless the
configuration specifies something to use. */

if (originator_login == NULL)
  {
  if (unknown_login != NULL)
    {
    originator_login = expand_string(unknown_login);
    if (originator_name == NULL && unknown_username != NULL)
      originator_name = expand_string(unknown_username);
    if (originator_name == NULL) originator_name = "";
    }
  if (originator_login == NULL)
    log_write(0, LOG_PANIC_DIE, "Failed to get user name for uid %d",
      (int)real_uid);
  }

/* Ensure that the user name is in a suitable form for use as a "phrase" in an
RFC822 address. NB big_buffer is used by parse_fix_phrase(). */

originator_name = string_copy(parse_fix_phrase(originator_name));

/* If a message is created by this call of Exim, the uid/gid of its originator
are those of the caller. These values are overridden if an existing message is
read in from the spool. */

originator_uid = real_uid;
originator_gid = real_gid;

DEBUG(9) debug_printf("originator: uid=%d gid=%d login=%s name=%s\n",
  originator_uid, originator_gid, originator_login, originator_name);

/* Run in daemon and/or queue-running mode. The function daemon_go() never
returns. We leave this till here so that the originator_ fields are available
for incoming messages via the daemon. */

if (daemon_listen || queue_interval > 0) daemon_go();

/* Handle the -brw option, which is for checking out rewriting rules. Cause log
writes (on errors) to go to stderr instead. Can't do this earlier, as want the
originator_* variables set. */

if (test_rewrite_arg >= 0)
  {
  really_exim = FALSE;
  if (test_rewrite_arg >= argc)
    {
    printf("-brw needs an address argument\n");
    exim_exit(EXIT_FAILURE);
    }
  rewrite_test(argv[test_rewrite_arg]);
  exim_exit(EXIT_SUCCESS);
  }

/* A locally-supplied message is considered to be coming from a local user
unless a trusted caller supplies a sender address with -f, or is passing in the
message via SMTP (inetd invocation or otherwise). */

if ((sender_address == NULL && !smtp_input) ||
    (!trusted_caller && filter_test == NULL))
  {
  sender_local = TRUE;

  /* A trusted caller can supply authenticated_sender and authenticated_id
  via -oMas and -oMai and if so, they will already be set. */

  #ifdef HAVE_AUTH
  if (authenticated_sender == NULL)
    authenticated_sender = string_sprintf("%s@%s", originator_login,
      qualify_domain_sender);
  if (authenticated_id == NULL) authenticated_id = originator_login;
  #endif
  }

/* Trusted callers are always permitted to specify the sender address.
Untrusted callers may specify it if untrusted_set_sender is set, or if what is
specified is the empty address. However, if a trusted caller does not specify a
sender address for SMTP input, we leave sender_address unset. This causes the
MAIL commands to be honoured. */

if ((!trusted_caller && !untrusted_set_sender) ||
    (!smtp_input && sender_address == NULL))
  {
  /* Either the caller is not permitted to set a general sender, or this is
  non-SMTP input and the trusted caller has not set a sender. If there is no
  sender, or if a sender other than <> is set, override with the originator's
  login (which will get qualified below), except when checking things. */

  if (sender_address == NULL             /* No sender_address set */
       ||                                /*         OR            */
       (sender_address[0] != 0 &&        /* Non-empty sender address, AND */
       !checking &&                      /* Not running tests, AND */
       filter_test == NULL))             /* Not testing a filter */
    {
    sender_address = originator_login;
    sender_address_forced = FALSE;
    sender_address_domain = 0;
    }
  }

/* Remember whether an untrusted caller set the sender address */

sender_set_untrusted = sender_address != originator_login && !trusted_caller;

/* Ensure that the sender address is fully qualified unless it is the empty
address, which indicates an error message, or doesn't exist (root caller, smtp
interface, no -f argument). */

if (sender_address != NULL && sender_address[0] != 0 &&
    sender_address_domain == 0)
  sender_address = string_sprintf("%s@%s", sender_address,
    qualify_domain_sender);

DEBUG(9) debug_printf("sender address = %s\n", sender_address);

/* Handle a request to verify a list of addresses, or test them for delivery.
This must follow the setting of the sender address, since drivers can now be
predicated upon the sender. If no arguments are given, read addresses from
stdin. Set debug_level to at least 1 to get full output for address testing. */

if (verify_only || address_test_mode)
  {
  int exit_value = 0;
  int flags = vopt_local;
  char *newaddr;
  char **newaddrptr;

  if (verify_only)
    {
    newaddrptr = &newaddr;
    if (!verify_as_sender) flags |= vopt_is_recipient;
    DEBUG(9) debug_print_ids("Verifying:");
    }

  else
    {
    newaddrptr = NULL;
    flags |= vopt_is_recipient;
    if (debug_level <= 0)
      {
      debug_level = 1;
      debug_file = stderr;
      debug_fd = fileno(debug_file);
      }
    DEBUG(9) debug_print_ids("Address testing:");
    }

  if (recipients_arg < argc)
    {
    while (recipients_arg < argc)
      {
      char *s = argv[recipients_arg++];
      while (*s != 0)
        {
        int rc;
        BOOL finished = FALSE;
        char *ss = parse_find_address_end(s, FALSE);
        if (*ss == ',') *ss = 0; else finished = TRUE;
        rc = verify_address(string_copy(s), stdout, NULL, newaddrptr, flags);
        if (rc == FAIL) exit_value = 2;
        else if (rc == DEFER)
          {
          if (exit_value == 0) exit_value = 1;
          }
        else if (newaddrptr != NULL)
          {
          if (newaddr != NULL)
            { DEBUG(2) debug_printf("%s verified ok as %s\n", s, newaddr); }
          else
            { DEBUG(2) debug_printf("%s verified ok\n", s); }
          }

        s = ss;
        if (!finished)
          while (*(++s) != 0 && (*s == ',' || isspace((uschar)*s)));
        }
      }
    }

  else
    {
    for (;;)
      {
      int rc;
      char buffer[256];
      printf("> ");
      if (fgets(buffer, 256, stdin) == NULL) break;
      rc = verify_address(string_copy(buffer), stdout, NULL, newaddrptr, flags);
      if (rc == FAIL) exit_value = 2;
      else if (rc == DEFER)
        {
        if (exit_value == 0) exit_value = 1;
        }
      else if (newaddrptr != NULL)
        {
        if (newaddr != NULL)
          { DEBUG(2) debug_printf("%s verified ok as %s\n", buffer, newaddr); }
        else
          { DEBUG(2) debug_printf("%s verified ok\n", buffer); }
        }
      }
    printf("\n");
    }
  direct_tidyup();
  route_tidyup();
  exim_exit(exit_value);
  }


/* Handle host checking: this facility mocks up an incoming SMTP call
from a given IP address so that the blocking and relay configuration can
be tested. */

if (host_checking)
  {
  host_build_sender_fullhost();
  smtp_input = TRUE;
  smtp_in = stdin;
  smtp_out = stdout;
  sender_local = FALSE;
  sender_host_notsocket = TRUE;
  debug_file = stderr;
  debug_fd = fileno(debug_file);
  fprintf(stdout, "\n**** SMTP testing session as if from host %s\n"
    "**** Not for real!\n\n", sender_host_address);

  if (smtp_start_session())
    {
    reset_point = store_get(0);
    for (;;)
      {
      store_reset(reset_point);
      if (smtp_setup_msg() <= 0) break;
      if (!accept_msg(smtp_in, FALSE)) break;
      }
    }
  exim_exit(EXIT_SUCCESS);
  }


/* Handle expansion checking */

if (expansion_test)
  {
  if (recipients_arg < argc)
    {
    while (recipients_arg < argc)
      {
      char *s = argv[recipients_arg++];
      char *ss = expand_string(s);
      if (ss == NULL)
        printf ("Failed: %s\n", expand_string_message);
      else printf("%s\n", ss);
      }
    }

  else for (;;)
    {
    char buffer[1024];
    char *ss;
    printf("> ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
    ss = expand_string(buffer);
    if (ss == NULL)
      printf ("Failed: %s\n", expand_string_message);
    else printf("%s", ss);
    }

  exim_exit(EXIT_SUCCESS);
  }


/* Arrange for message reception if recipients or SMTP were specified;
otherwise complain unless a version print happened or this is a filter
verification test. */

if (recipients_arg >= argc && !extract_recipients && !smtp_input)
  {
  if (version_printed) return EXIT_SUCCESS;
  if (filter_test == NULL)
    {
    fprintf(stderr, "exim: neither action flags nor mail addresses given\n");
    return EXIT_FAILURE;
    }
  }


/* Accept one or more new messages on the standard input; accept_msg
returns TRUE if there are more messages to be read (SMTP input), or
FALSE otherwise (not SMTP, or SMTP channel collapsed).

When a message has been read, its id is returned in message_id[]. If
doing immediate delivery, we fork a delivery process for each received
message, except for the last one, where we can save a process switch. */

/* If the sender ident has not been set (by a trusted caller) set it to
the caller. This will get overwritten below for an inetd call. If a trusted
caller has set it empty, unset it. */

if (sender_ident == NULL) sender_ident = originator_login;
  else if (sender_ident[0] == 0) sender_ident = NULL;

/* It is only in non-smtp mode that error_handling is allowed to be changed from
its default of ERRORS_SENDER by argument. (Idle thought: are any of the
sendmail error modes other than -oem ever actually used? Later: yes.) */

if (!smtp_input) error_handling = arg_error_handling;

/* If this is an inetd call, set up the interface address and host address and
make an identd call to get the sender_ident. Also ensure that stderr is NULL to
prevent panic logging being sent down the socket. */

else if (is_inetd)
  {
  #if HAVE_IPV6
  struct sockaddr_in6 interface_sockaddr;
  #else
  struct sockaddr interface_sockaddr;
  #endif

  int size = sizeof(interface_sockaddr);

  fclose(stderr);

  if (getsockname(0, (struct sockaddr *)(&interface_sockaddr), &size) == 0)
    interface_address = host_ntoa(-1, &interface_sockaddr, NULL, NULL);

  sender_host_address = host_ntoa(-1, (struct sockaddr *)(&inetd_sock), NULL,
    &sender_host_port);

  verify_get_ident(0);
  host_build_sender_fullhost();
  set_process_info("handling incoming connection from %s via inetd",
    sender_fullhost);
  }

/* If the sender host address has been set, build sender_fullhost if it hasn't
already been done (which it will have been for inetd). This caters for the
case when it is forced by -oMa. However, we must flag that is isn't a socket,
so that the test for IP options is skipped for -bs input. */

if (sender_host_address != NULL && sender_fullhost == NULL)
  {
  host_build_sender_fullhost();
  set_process_info("handling incoming connection from %s via -oMa",
    sender_fullhost);
  sender_host_notsocket = TRUE;
  }

/* Otherwise, set the sender host as unknown except for inetd calls. This
prevents host checking in the case of -bs not from inetd and also for -bS. */

else if (!is_inetd) sender_host_unknown = TRUE;

/* If stdout does not exist, then dup stdin to stdout. This can happen
if exim is started from inetd. In this case fd 0 will be set to the socket,
but fd 1 will not be set. This also happens for passed SMTP channels. */

if (fstat(1, &statbuf) < 0) dup2(0, 1);

/* Set up the incoming protocol name and the state of the program. Root
is allowed to force received protocol via the -oMr option above, and if we are
in a non-local SMTP state it means we have come via inetd and the process info
has already been set up. We don't set received_protocol here for smtp input,
as it varies according to batch/HELO/EHLO. */

if (smtp_input)
  {
  if (sender_local) set_process_info("accepting a local SMTP message from <%s>",
    sender_address);
  }
else
  {
  if (received_protocol == NULL)
    received_protocol = string_sprintf("local%s", called_as);
  set_process_info("accepting a local non-SMTP message from <%s>",
    sender_address);
  }

/* Initialize the local_queue-only flag that can change as a result of a check
on the system load, and also do the queuing via file check. */

queue_check_only();
local_queue_only = queue_only;

/* For non-SMTP and for batched SMTP input, check that there is enough space on
the spool if so configured. On failure, we must not attempt to send an error
message! (For interactive SMTP, the check happens at MAIL FROM and an SMTP
error code is given.) */

if ((!smtp_input || smtp_batched_input) && !accept_check_fs(0))
  {
  fprintf(stderr, "exim: insufficient disc space\n");
  return EXIT_FAILURE;
  }

/* If this is batched SMTP input and sender_verify_batch is off, turn off
the sender verification options. */

if (smtp_batched_input && !sender_verify_batch)
  sender_verify = sender_try_verify = FALSE;

/* If this is smtp input of any kind, handle the start of the SMTP
session. */

if (smtp_input)
  {
  smtp_in = stdin;
  smtp_out = stdout;
  if (!smtp_start_session())
    {
    mac_smtp_fflush();
    exim_exit(EXIT_SUCCESS);
    }
  }

/* Loop for several messages when reading SMTP input. Reset the pool store
each time. If we fork any child processes, we don't want to wait for them
unless synchronous delivery is requested, so set SIGCHLD to SIG_IGN in that
case. This is not the same as SIG_DFL, despite the fact that documentation
often lists the default as "ignore". At least on some systems, setting SIG_IGN
causes child processes that complete simply to go away without ever becoming
defunct. You can't therefore wait for them - but we don't want to wait for them
in the non-synchronous delivery case. However, it seems that on some OS, this
doesn't work, so the loop below contains a waitpid() just in case. */

if (!synchronous_delivery) signal(SIGCHLD, SIG_IGN);

/* Save the current store pool point, for resetting at the start of
each message, and save the real sender address, if any. */

reset_point = store_get(0);
real_sender_address = sender_address;

while (more)
  {
  store_reset(reset_point);
  message_id[0] = 0;

  /* In the SMTP case, we have to handle the initial SMTP input and
  build the recipients list, before calling accept_msg to read the
  message proper. Whatever sender address is actually given in the
  SMTP transaction is actually ignored for local senders - we use
  the actual sender, which is either the underlying user running this
  process or a -f argument provided by a trusted caller. This is indicated
  by the existence of sender_address, which will be NULL only for a trusted
  caller when no -f was given. It is saved in real_sender_address. The variable
  raw_sender_address holds the sender address before rewriting. */

  if (smtp_input)
    {
    int rc;
    if ((rc = smtp_setup_msg()) > 0)
      {
      if (real_sender_address != NULL)
        sender_address = raw_sender = real_sender_address;
      more = accept_msg(stdin, extract_recipients);
      if (message_id[0] == 0)
        {
        if (more) continue;
        exim_exit(EXIT_FAILURE);
        }
      accept_messagecount++;   /* Mainly for BSMTP errors */
      }
    else exim_exit((rc == 0)? EXIT_SUCCESS : EXIT_FAILURE);
    }

  /* In the non-SMTP case, we have all the information from the command
  line, but must process it in case it is in the more general RFC822
  format, and in any case, to detect syntax errors. Also, it appears that
  the use of comma-separated lists as single arguments is common, so we
  had better support them. */

  else
    {
    int i;
    int rcount = 0;
    int count = argc - recipients_arg;
    char **list = argv + recipients_arg;

    /* Save before any rewriting */

    raw_sender = string_copy(sender_address);

    /* Loop for each argument */

    for (i = 0; i < count; i++)
      {
      int start, end, domain;
      char *errmess;
      char *s = list[i];

      /* Loop for each comma-separated address */

      while (*s != 0)
        {
        BOOL finished = FALSE;
        char *receiver;
        char *ss = parse_find_address_end(s, FALSE);

        if (*ss == ',') *ss = 0; else finished = TRUE;

        /* Check max recipients - if -t was used, these aren't recipients */

        if (recipients_max > 0 && ++rcount > recipients_max &&
            !extract_recipients)
          {
          if (error_handling == ERRORS_STDERR)
            {
            fprintf(stderr, "exim: too many recipients\n");
            exim_exit(EXIT_FAILURE);
            }
          else
            {
            return
              moan_to_sender(ERRMESS_TOOMANYRECIP, NULL, NULL, stdin, TRUE)?
                errors_sender_rc : EXIT_FAILURE;
            }
          }

        receiver =
          parse_extract_address(s, &errmess, &start, &end, &domain, FALSE);
        if (receiver == NULL)
          {
          if (error_handling == ERRORS_STDERR)
            {
            fprintf(stderr, "exim: bad address \"%s\": %s\n",
              string_printing(list[i]), errmess);
            exim_exit(EXIT_FAILURE);
            }
          else
            {
            error_block eblock;
            eblock.next = NULL;
            eblock.text1 = string_printing(list[i]);
            eblock.text2 = errmess;
            return
              moan_to_sender(ERRMESS_BADARGADDRESS, &eblock, NULL, stdin, TRUE)?
                errors_sender_rc : EXIT_FAILURE;
            }
          }
        accept_add_recipient(receiver, NULL, 0, 0);
        s = ss;
        if (!finished)
          while (*(++s) != 0 && (*s == ',' || isspace((uschar)*s)));
        }
      }

    /* Show the recipients when debugging */

    DEBUG(9)
      {
      int i;
      if (sender_address != NULL) debug_printf("Sender: %s\n", sender_address);
      if (recipients_list != NULL)
        {
        debug_printf("Recipients:\n");
        for (i = 0; i < recipients_count; i++)
          debug_printf("  %s\n", recipients_list[i].address);
        }
      }

    /* Read the data for the message. If filter_test is true, this will
    just read the headers for the message, and not write anything onto
    the spool. */

    message_ended = END_NOTENDED;
    more = accept_msg(stdin, extract_recipients);

    /* more is always FALSE here (not SMTP message) when reading a message
    for real; when reading the headers of a message for filter testing,
    it is TRUE if the headers were terminated by '.' and FALSE otherwise. */

    if (message_id[0] == 0) exim_exit(EXIT_FAILURE);
    }  /* Non-SMTP message reception */

  /* If this is a filter testing run, there are headers in store, but
  no message on the spool. Run the filtering code in testing mode, setting
  the domain to the qualify domain and the local part to the current user,
  unless they have been set by options. The prefix and suffix are left unset
  unless specified. The the return path is set to to the sender unless it has
  already been set from a return-path header in the message. */

  if (filter_test != NULL)
    {
    deliver_domain = (ftest_domain != NULL)?
      ftest_domain : qualify_domain_recipient;
    deliver_domain_orig = deliver_domain;
    deliver_localpart = (ftest_localpart != NULL)?
      ftest_localpart : originator_login;
    deliver_localpart_orig = deliver_localpart;
    deliver_localpart_prefix = ftest_prefix;
    deliver_localpart_suffix = ftest_suffix;
    deliver_home = originator_home;
    if (return_path == NULL) return_path = string_copy(sender_address);

    printf("Sender    = %s\n", (sender_address[0] == 0)? "<>" : sender_address);

    accept_add_recipient(
      string_sprintf("%s%s%s@%s",
        (ftest_prefix == NULL)? "" : ftest_prefix,
        deliver_localpart,
        (ftest_suffix == NULL)? "" : ftest_suffix,
        deliver_domain),
      NULL, 0, 0);

    printf("Recipient = %s\n", recipients_list[0].address);
    if (ftest_prefix != NULL) printf("Prefix    = %s\n", ftest_prefix);
    if (ftest_suffix != NULL) printf("Suffix    = %s\n", ftest_suffix);

    printf("Testing Exim filter file %s\n\n", filter_test);

    exim_exit(filter_runtest(filter_fd, ftest_system, more)?
      EXIT_SUCCESS : EXIT_FAILURE);
    }

  /* Else act on the result of message reception. We should not get here unless
  message_id[0] is non-zero. If queue_only is set, local_queue_only will be
  TRUE. If it is not, check on the number of messages received in this
  connection. If that's OK and queue_only_load is set, check that the load
  average is below it. If it is not, set local_queue_only TRUE. Note that it
  then remains this way for any subsequent messages on the same SMTP connection.
  This is a deliberate choice; even though the load average may fall, it
  doesn't seem right to deliver later messages on the same call when not
  delivering earlier ones. */

  if (!local_queue_only)
    {
    if (smtp_accept_queue_per_connection > 0 &&
        ++message_count > smtp_accept_queue_per_connection)
      {
      local_queue_only = TRUE;
      queue_only_reason = 2;
      }
    else if (queue_only_load >= 0)
      {
      local_queue_only = (load_average = os_getloadavg()) > queue_only_load;
      if (local_queue_only) queue_only_reason = 3;
      }
    }

  /* Log the queueing here, when it will get a message id attached, but
  not if queue_only is set (case 0). Case 1 doesn't happen here (too many
  connections). */

  if (local_queue_only) switch(queue_only_reason)
    {
    case 2:
    log_write(4, LOG_MAIN, "no immediate delivery: more than %d messages "
      "received in one connection", smtp_accept_queue_per_connection);
    break;

    case 3:
    log_write(4, LOG_MAIN, "no immediate delivery: load average %.2f",
      (double)load_average/1000.0);
    break;
    }

  /* Else do the delivery */

  else
    {
    /* If running as root, or if we can regain root by seteuid (security level
    is 1), or if this binary is not setuid to root and was not called by root
    (an exceptional case) call the delivery function directly for synchronous
    delivery, or fork a clone of this process to deliver in the background. A
    fork failure is not a disaster, as the delivery will eventually happen on
    a subsequent queue run. */

    if (security_level <= 1 || security_level >= 4 || unprivileged)
      {
      if (synchronous_delivery)
        (void)deliver_message(message_id, FALSE, FALSE);
      else
        {
        if(fork() == 0)
          {
          close_unwanted();      /* Close unwanted file descriptors */
          (void)deliver_message(message_id, FALSE, FALSE);
          _exit(EXIT_SUCCESS);
          }
        }
      }

    /* Otherwise, we have to re-exec exim in order to regain root for the
    delivery of the message. */

    else
      {
      pid_t pid;

      if ((pid = fork()) == 0)
        {
        int i = 0;
        char *argv[13 + MAX_CLMACROS];

        argv[i++] = exim_path;
        if (clmacro_count > 0)
          {
          memcpy(argv + i, clmacros, clmacro_count * sizeof(char *));
          i += clmacro_count;
          }
        if (debug_level > 0)
          argv[i++] = string_sprintf("-d%d", debug_level);
        if (debug_trace_memory > 0) argv[i++] = "-dm";
        if (dont_deliver) argv[i++] = "-N";
        if (queue_smtp) argv[i++] = "-odqs";
        if (queue_remote) argv[i++] = "-odqr";
        if (synchronous_delivery) argv[i++] = "-odi";
        if (batch_max >= 0)
          argv[i++] = string_sprintf("-oB%d", batch_max);
        if (config_changed)
          {
          argv[i++] = "-C";
          argv[i++] = config_filename;
          }
        argv[i++] = "-Mc";
        argv[i++] = message_id;
        argv[i++] = NULL;

        DEBUG(4)
          {
          while (i < 10) argv[i++] = NULL;
          debug_printf("exec %s %s %s %s %s %s %s %s %s %s\n",
            argv[0], argv[1], argv[2],
            (argv[3] == NULL)? "" : argv[3],
            (argv[4] == NULL)? "" : argv[4],
            (argv[5] == NULL)? "" : argv[5],
            (argv[6] == NULL)? "" : argv[6],
            (argv[7] == NULL)? "" : argv[7],
            (argv[8] == NULL)? "" : argv[8],
            (argv[9] == NULL)? "" : argv[9]);
          }

        close_unwanted();      /* Close unwanted file descriptors */
        execv(argv[0], argv);
        _exit(errno);
        }

      /* In the parent, wait if synchronous delivery is required. */

      if (synchronous_delivery && pid > 0)
        {
        int status;
        while (wait(&status) != pid);
        }
      }
    }

  /* The loop will repeat if more is TRUE. It appears that, despite setting
  SIG_IGN for SIGCHLD above, some OS don't automatically reap children. So just
  in case there a lots of messages coming in from the same source, we clear
  away any finished subprocesses here. */

  while (waitpid(-1, NULL, WNOHANG) > 0);
  }

exim_exit(EXIT_SUCCESS);   /* Never returns */
return 0;                  /* To stop compiler warning */
}

/* End of exim.c */
