/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions concerned with running Exim as a daemon */


#include "exim.h"



/*************************************************
*               Local static variables           *
*************************************************/

static volatile BOOL daemon_sigalrm_seen;
static volatile BOOL sigchld_seen;
static volatile BOOL sighup_seen;

static int   accept_retry_count = 0;
static int   accept_retry_errno;
static BOOL  accept_retry_select_failed;

static int   queue_run_count = 0;
static pid_t *queue_pid_slots;
static pid_t *smtp_pid_slots;
static char  **smtp_host_address_slots;





/*************************************************
*             SIGALRM Handler                    *
*************************************************/

/* All this handler does is to set a flag and re-enable the signal.

Argument: the signal number
Returns:  nothing
*/

static void
daemon_sigalrm_handler(int sig)
{
sig = sig;    /* Keep picky compilers happy */
daemon_sigalrm_seen = TRUE;
os_non_restarting_signal(SIGALRM, daemon_sigalrm_handler);
}



/*************************************************
*             SIGHUP Handler                     *
*************************************************/

/* All this handler does is to set a flag and re-enable the signal.

Argument: the signal number
Returns:  nothing
*/

static void
sighup_handler(int sig)
{
sig = sig;    /* Keep picky compilers happy */
sighup_seen = TRUE;
signal(SIGHUP, sighup_handler);
}



/*************************************************
*     SIGCHLD handler for main daemon process    *
*************************************************/

/* Don't re-enable the handler here, since we aren't doing the
waiting here. If the signal is re-enabled, there will just be an
infinite sequence of calls to this handler. The SIGCHLD signal is
used just as a means of waking up the daemon so that it notices
terminated subprocesses as soon as possible.

Argument: the signal number
Returns:  nothing
*/

static void
main_sigchld_handler(int sig)
{
sig = sig;    /* Keep picky compilers happy */
sigchld_seen = TRUE;
signal(SIGCHLD, SIG_DFL);
}




/*************************************************
*            Handle a connected SMTP call        *
*************************************************/

/* This function is called when an SMTP connection has been accepted.
If there are too many, give an error message and close down. Otherwise
spin off a sub-process to handle the call. The list of listening sockets
is required so that they can be closed in the sub-process. Take care not to
leak store in this process - reset the stacking pool at the end.

Arguments:
  listen_sockets        sockets which are listening for incoming calls
  listen_socket_count   count of listening sockets
  accept_socket         socket of the current accepted call
  accepted              socket information about the current call

Returns:            nothing
*/

static void
handle_smtp_call(int *listen_sockets, int listen_socket_count,
  int accept_socket, struct sockaddr *accepted)
{
pid_t pid;
int dup_accept_socket = dup(accept_socket);
void *reset_point = store_get(0);

/* Make the address available in ASCII representation, and also fish out
the remote port. */

sender_host_address = host_ntoa(-1, accepted, NULL, &sender_host_port);
DEBUG(1) debug_printf("Connection request from %s/%d\n", sender_host_address,
  sender_host_port);

/* Set up the file for writing replies, then check that duplication of the
socket worked, and if so, make an input file. */

smtp_out = fdopen(accept_socket, "w");
if (dup_accept_socket < 0)
  {
  log_write(0, LOG_MAIN, "Couldn't dup socket descriptor: %s\n",
    strerror(errno));
  DEBUG(1) debug_printf("421 %s: Connection setup failed: %s\n",
    primary_hostname, strerror(errno));
  fprintf(smtp_out, "421 %s: Connection setup failed: %s\r\n",
    primary_hostname, strerror(errno));
  goto ERROR_RETURN;
  }
smtp_in = fdopen(dup_accept_socket, "r");

/* Check maximum number of connections. We do not check for reserved
connections or unacceptable hosts here. That is done in the subprocess because
it might take some time. */

if (smtp_accept_max > 0 && smtp_accept_count >= smtp_accept_max)
  {
  DEBUG(1) debug_printf("rejecting SMTP connection: count=%d max=%d\n",
    smtp_accept_count, smtp_accept_max);
  DEBUG(1) debug_printf("421 %s: Too many concurrent SMTP connections; "
    "please try again later.\n", primary_hostname);
  fprintf(smtp_out, "421 %s: Too many concurrent SMTP connections; "
    "please try again later.\r\n", primary_hostname);
  log_write(4, LOG_MAIN, "Connection from %s refused: too many connections",
    sender_host_address);
  goto ERROR_RETURN;
  }

/* If a load limit above which only reserved hosts are acceptable is defined,
get the load average here, and if there are in fact no reserved hosts, do
the test right away (saves a fork). If there are hosts, do the check in the
subprocess because it might take time. */

if (smtp_load_reserve >= 0)
  {
  load_average = os_getloadavg();
  if (smtp_reserve_hosts == NULL && load_average > smtp_load_reserve)
    {
    DEBUG(1) debug_printf("rejecting SMTP connection: load average = %.2f\n",
      (double)load_average/1000.0);
    DEBUG(1) debug_printf("421 %s: Too much load; please try again later.\n",
      primary_hostname);
    fprintf(smtp_out, "421 %s: Too much load; please try again later.\r\n",
      primary_hostname);
    log_write(4, LOG_MAIN, "Connection from %s refused: load average = %.2f",
      sender_host_address, (double)load_average/1000.0);
    goto ERROR_RETURN;
    }
  }

/* Check that one specific host (strictly, IP address) is not hogging
resources. This is done here to prevent a denial of service attack by someone
forcing you to fork lots of times to then deny him service. If we have fewer
connections than smtp_accept_max_per_host, we can skip the tedious per
host_address checks. Note that at this stage smtp_accept_count contains
the count of *other* connections, not including this one. */

if ((smtp_accept_max_per_host > 0) &&
    (smtp_accept_count >= smtp_accept_max_per_host))
  {
  int i;
  int host_accept_count = 0;
  int other_host_count = 0;    /* keep a count of non matches to optimise */

  for (i = 0; i < smtp_accept_max; ++i)
    {
    if (smtp_host_address_slots[i] != NULL)
      {
      if (strcmp(sender_host_address, smtp_host_address_slots[i]) == 0)
       host_accept_count++;
      else
       other_host_count++;

      /* Testing all these strings is expensive - see if we can drop out
      early, either by hitting the target, or finding there are not enough
      connections left to make the target. */

      if ((host_accept_count >= smtp_accept_max_per_host) ||
	 ((smtp_accept_count - other_host_count) < smtp_accept_max_per_host))
       break;
      }
    }

  if (host_accept_count >= smtp_accept_max_per_host)
    {
    DEBUG(1) debug_printf("rejecting SMTP connection (too many from one "
      "address: count=%d max=%d\n",
      host_accept_count, smtp_accept_max_per_host);
    DEBUG(1) debug_printf("421 %s: Too many concurrent SMTP connections "
      "from one IP address; please try again later.\n", primary_hostname);
    fprintf(smtp_out, "421 %s: Too many concurrent SMTP connections "
      "from one IP address; please try again later.\r\n", primary_hostname);
    log_write(4, LOG_MAIN, "Connection from %s refused: too many connections "
      "from that address", sender_host_address);
    goto ERROR_RETURN;
    }
  }

/* OK, the connection count checks have been passed. Now we can fork the
accepting process */

pid = fork();

/* Handle the child process */

if (pid == 0)
  {
  int i;
  int message_count = 0;
  int queue_only_reason = 0;
  BOOL local_queue_only;
  union sockaddr_46 interface_sockaddr;

  /* Get the local interface address into some permanent store */

  int size = sizeof(interface_sockaddr);
  if (getsockname(accept_socket, (struct sockaddr *)(&interface_sockaddr),
       &size) == 0)
    {
    char *buffer = store_malloc(sizeof(ip_address_item));
    interface_address = host_ntoa(-1, &interface_sockaddr, buffer, NULL);
    DEBUG(9) debug_printf("interface address = %s\n", interface_address);
    }

  /* Initialize the queueing flags */

  queue_check_only();
  local_queue_only = queue_only;

  /* Close the listening sockets, and set the SIGCHLD handler to SIG_IGN.
  This is not the same as SIG_DFL, despite the fact that documentation often
  lists the default as "ignore". At least on some systems, setting SIG_IGN
  causes child processes that complete simply to go away without ever becoming
  defunct. You can't therefore wait for them - but in this process we don't
  want to wait for them as they are doing independent deliveries. */

  for (i = 0; i < listen_socket_count; i++) close(listen_sockets[i]);
  signal(SIGCHLD, SIG_IGN);

  /* Attempt to get an id from the sending machine via the RFC 1413
  protocol. We do this in the sub-process in order not to hold up the
  main process if there is any delay. Then set up the fullhost information
  in case there is no HELO/EHLO. */

  verify_get_ident(accept_socket);
  host_build_sender_fullhost();

  DEBUG(1)
    debug_printf("Process %d is handling incoming connection from %s\n",
      getpid(), sender_fullhost);

  /* If there are too many child processes for immediate delivery,
  set the local_queue_only flag, which is initialized from the
  configured value and may therefore already be TRUE. Leave logging
  till later so it will have a message id attached. */

  if (smtp_accept_queue > 0 && smtp_accept_count >= smtp_accept_queue)
    {
    local_queue_only = TRUE;
    queue_only_reason = 1;
    }

  /* Handle the start of the SMTP session, then loop, accepting incoming
  messages from the SMTP connection. The end will come at the QUIT command,
  when smtp_setup_msg() returns 0. A break in the connection causes the
  process to die (see accept.c). */

  if (!smtp_start_session())
    {
    mac_smtp_fflush();
    search_tidyup();
    _exit(EXIT_SUCCESS);
    }

  for (;;)
    {
    int rc;
    message_id[0] = 0;            /* Clear out any previous message_id */
    reset_point = store_get(0);   /* Save current store high water point */

    set_process_info("handling incoming connection from %s",
      sender_fullhost);

    DEBUG(1) debug_printf("ready for new message\n");

    /* Smtp_setup_msg() returns 0 on QUIT or if the call is from an
    unacceptable host, -1 on connection lost, and +1 on validly reaching
    DATA. Accept_msg() always returns TRUE when smtp_input is true; just
    retry if no message was accepted (can happen for invalid message
    parameters). */

    if ((rc = smtp_setup_msg()) > 0)
      {
      (void) accept_msg(smtp_in, FALSE);
      search_tidyup();                    /* Close cached databases */
      if (message_id[0] == 0) continue;
      }
    else
      {
      mac_smtp_fflush();
      search_tidyup();
      _exit((rc == 0)? EXIT_SUCCESS : EXIT_FAILURE);
      }

    /* Show the recipients when debugging */

    DEBUG(2)
      {
      int i;
      if (sender_address != NULL)
        debug_printf("Sender: %s\n", sender_address);
      if (recipients_list != NULL)
        {
        debug_printf("Recipients:\n");
        for (i = 0; i < recipients_count; i++)
          debug_printf("  %s\n", recipients_list[i].address);
        }
      }

    /* A message has been accepted. Clean up any previous delivery processes
    that have now completed and are defunct. I tried to get SIGCHLD to do
    this, but it gave problems. This way, at most one delivery process hangs
    around until the next message is received on systems where setting SIGCHLD
    to SIG_IGN does not have the effect of causing them to go away by
    themselves. (I don't know if there are any such systems, but leaving this
    code here can do no harm.) */

    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
      DEBUG(1) debug_printf("child %d ended\n", pid);

    /* Reclaim up the store used in accepting this message */

    store_reset(reset_point);

    /* If queue_only is set or if there are too many incoming connections in
    existence, local_queue_only will be TRUE. If it is not, check whether we
    have received too many messages in this session for immediate delivery. If
    not, and queue_only_load is set, check that the load average is below it.
    Note that, once set, local_queue_only remains set for any subsequent
    messages on the same SMTP connection. This is a deliberate choice; even
    though the load average may fall, it doesn't seem right to deliver later
    messages on the same call when not delivering earlier ones. */

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
    not if queue_only is set (case 0). */

    if (local_queue_only) switch(queue_only_reason)
      {
      case 1:
      log_write(4, LOG_MAIN, "no immediate delivery: too many connections "
        "(%d, max %d)", smtp_accept_count, smtp_accept_queue);
      break;

      case 2:
      log_write(4, LOG_MAIN, "no immediate delivery: more than %d messages "
        "received in one connection", smtp_accept_queue_per_connection);
      break;

      case 3:
      log_write(4, LOG_MAIN, "no immediate delivery: load average %.2f",
        (double)load_average/1000.0);
      break;
      }

    /* If a delivery attempt is required, spin off a new process to handle it.
    If we are not root, we have to re-exec exim unless root can be regained by
    the use of seteuid, or the security level is 4, which means everything
    happens unprivileged. */

    else
      {
      /* Before forking, ensure that the C output buffer is flushed. Otherwise
      anything that it in it will get duplicated, leading to duplicate copies
      of the pending output. */

      mac_smtp_fflush();

      if ((pid = fork()) == 0)
        {
        fclose(smtp_in);
        fclose(smtp_out);
        smtp_in = NULL;

        /* Don't ever molest the parent's SSL connection, but do clean up
        the data structures if necessary. */

        #ifdef SUPPORT_TLS
        tls_close(FALSE);
        #endif

        if (security_level == 3 || security_level == 2)
          {
          int i = 0;
          char *argv[11 + MAX_CLMACROS];
          argv[i++] = exim_path;
          if (clmacro_count > 0)
            {
            memcpy(argv + i, clmacros, clmacro_count * sizeof(char *));
            i += clmacro_count;
            }
          if (debug_level > 0)
            argv[i++] = string_sprintf("-d%d", debug_level);
          if (debug_trace_memory != 0) argv[i++] = "-dm";
          if (dont_deliver) argv[i++] = "-N";
          if (queue_smtp) argv[i++] = "-odqs";
          if (queue_remote) argv[i++] = "-odqr";
          if (config_changed)
            {
            argv[i++] = "-C";
            argv[i++] = config_filename;
            }
          argv[i++] = "-Mc";
          argv[i++] = message_id;
          argv[i++] = (char *)0;
          execv(argv[0], argv);
          log_write(0, LOG_PANIC_DIE, "exec of exim -Mc failed: %s",
            strerror(errno));
          }



        /* No need to re-exec */
        (void)deliver_message(message_id, FALSE, FALSE);
        search_tidyup();
        _exit(EXIT_SUCCESS);
        }
      }

    if (pid > 0) DEBUG(1) debug_printf("forked delivery process %d\n", pid);
    }
  }


/* Carrying on in the parent daemon process... Can't do much if the fork
failed. Otherwise, keep count of the number of accepting processes and
remember the pid for ticking off when the child completes. */

if (pid < 0)
  {
  DEBUG(1) debug_printf("fork of SMTP accept process failed: %s\n",
    strerror(errno));
  fprintf(smtp_out, "421 %s: Fork failed: %s\r\n", primary_hostname,
    strerror(errno));
  }
else
  {
  int i;
  for (i = 0; i < smtp_accept_max; ++i)
    {
    if (smtp_pid_slots[i] <= 0)
      {
      smtp_pid_slots[i] = pid;
      if (smtp_accept_max_per_host > 0)
	smtp_host_address_slots[i] = string_copy_malloc(sender_host_address);
      smtp_accept_count++;
      break;
      }
    }
  DEBUG(2) debug_printf("%d SMTP accept process%s running\n",
    smtp_accept_count, (smtp_accept_count == 1)? "" : "es");
  }

/* Get here via goto in error cases */

ERROR_RETURN:

/* Close the streams associated with the socket which will also
close the socket fds in this process. */

if (smtp_in != NULL)
  {
  fclose(smtp_in);
  smtp_in = NULL;
  }
fclose(smtp_out);

/* Release any store used in this process, including the store used for holding
the incoming host address. */

store_reset(reset_point);
sender_host_address = NULL;
}




/*************************************************
*              Exim Daemon Mainline              *
*************************************************/

/* The daemon can do two jobs, either of which is optional:

(1) Listens for incoming SMTP calls and spawns off a sub-process to handle
each one. This is requested by the -bd option, with -oX specifying the SMTP
port on which to listen (for testing).

(2) Spawns a queue-running process every so often. This is controlled by the
-q option with a an interval time. (If no time is given, a single queue run
is done from the main function, and control doesn't get here.)

Root privilege is required in order to attach to port 25. Some systems require
it when calling socket() rather than bind(). To cope with all cases, we run as
root for both socket() and bind(). Some systems also require root in order to
write to the pid file directory. This function is entered as root for security
levels 0 or 2; otherwise seteuid to exim.

Once the socket is bound and the pid file written, root privilege is given up
if there is an exim uid, either by seteuid for security level 1, or by setuid
for higher levels. In the latter case, a re-exec is subsequently required to
cause a received message to be delivered.

There are no arguments to this function, and it never returns. */

void
daemon_go(void)
{
int *listen_sockets = NULL;
int listen_socket_count = 0;
u_short net_port;
FILE *f;
char buff[256];

/* Create the name of the file holding the daemon's pid. This is written just
to make it easier to find the daemon process. There's nothing to stop multiple
daemons running, as long as no more than one listens on a given TCP/IP port on
the same interface(s). We put a non-standard port number in the file name to
distinguish running/testing versions, and use different names for non-listening
and non-queue-running daemons, but if someone runs two similar daemons
simultaneously, then only the last started will get its pid written.

Note re use of sprintf: spool_directory and pid_file_path are checked on input
to be < 200 characters. */

if (smtp_port < 0)
  {
  if (pid_file_path[0] == 0)
    sprintf(buff, "%s/exim-daemon.pid", spool_directory);
  else
    sprintf(buff, pid_file_path, "");
  }
else
  {
  if (pid_file_path[0] == 0)
    sprintf(buff, "%s/exim-daemon.%d.pid", spool_directory, smtp_port);
  else
    {
    char dbuff[12];
    sprintf(dbuff, ".%d", smtp_port);
    sprintf(buff, pid_file_path, dbuff);
    }
  }

/* For daemons that aren't both listening and queue-running, add the option
which they are doing to the file name. */

if (!daemon_listen)
  sprintf(buff+(int)strlen(buff), "-q%s", readconf_printtime(queue_interval));
else if (queue_interval <= 0) strcat(buff, "-bd");

/* Close all open file descriptors that we know about, and disconnect from the
controlling terminal, if we are not debugging. Most modern Unixes seem to have
setsid() for getting rid of the controlling terminal. For any OS that doesn't,
setsid() can be #defined as a no-op, or as something else. */

if (debug_level <= 0)
  {
  log_close_all();  /* Just in case anything was logged earlier */
  search_tidyup();  /* Just in case any were used in reading the config. */
  close(0);         /* Get rid of stdin/stdout/stderr */
  close(1);
  close(2);
  log_stderr = NULL;  /* So no attempt to copy paniclog output */

  /* If the parent process of this one has pid == 1, we are re-initializing the
  daemon as the result of a SIGHUP. In this case, there is no need to do any
  forking, because the controlling terminal has long gone. Otherwise, fork,
  in case current process is a process group leader (see 'man setsid' for an
  explanation). */

  if (getppid() != 1)
    {
    pid_t pid = fork();
    if (pid < 0) log_write(0, LOG_PANIC_DIE, "fork() failed: %s",
      strerror(errno));
    if (pid > 0) exit(EXIT_SUCCESS);      /* in parent process, just exit */
    (void)setsid();                       /* release controlling terminal */
    }
  }

/* Ensure root privilege. It will only not exist at this stage if seteuid
can be used to regain it. */

if (geteuid() != root_uid) mac_seteuid(root_uid);

/* If SMTP listening is requested, set up a socket on the SMTP port or
a given port, and compile the verification acceptance data, if any, so
that it is available to all spawned processes. If local_interfaces is set,
we have to set up one or more sockets on specific IP addresses; otherwise
we listen on all addresses on a single socket. Note the slight complication
about the IPv6 "any" address, which is a structure. */

if (daemon_listen)
  {
  int sk;
  int on = 1;
  ip_address_item *addresses, *ipa;
  union sockaddr_46 sin;

  #if HAVE_IPV6
  struct in6_addr anyaddr6 = in6addr_any;
  #endif

  /* Find the standard SMTP port if no port number given; otherwise
  convert the given port to network order. */

  if (smtp_port < 0)
    {
    struct servent *smtp_service;
    if ((smtp_service = getservbyname("smtp", "tcp")) == NULL)
      log_write(0, LOG_PANIC_DIE, "cannot find smtp/tcp service");
    net_port = smtp_service->s_port;
    smtp_port = ntohs(net_port);
    }
  else net_port = htons(smtp_port);

  DEBUG(9) debug_printf("port = %d\n", smtp_port);

  /* If local_interfaces is set, create the list of local interfaces and
  set up a vector for holding the listening sockets. */

  if (local_interfaces != NULL)
    {
    addresses = local_interface_data = host_find_interfaces();
    for (ipa = addresses; ipa != NULL; ipa = ipa->next)
      listen_socket_count++;
    }

  /* Otherwise we set up things to listen on all interfaces. In an IPv4 world,
  this is just a single, empty address. On systems with IPv6, several different
  implementation approaches have been taken. This code is now supposed to work
  with all of them. The point of difference is whether an IPv6 socket that is
  listening on all interfaces will receive incoming IPv4 calls or not.

  . On Solaris, an IPv6 socket will accept IPv4 calls, and give them as mapped
    addresses. However, if an IPv4 socket is also listening on all interfaces,
    calls are directed to the appropriate socket.

  . On (some versions of) Linux, an IPv6 socket will accept IPv4 calls, and
    give them as mapped addresses, but an attempt also to listen on an IPv4
    socket on all interfaces causes an error.

  . On OpenBSD, an IPv6 socket will not accept IPv4 calls. You have to set up
    two sockets if you want to accept both kinds of call.

  . FreeBSD is like OpenBSD, but it has the IPV6_V6ONLY socket option, which
    can be turned off, to make it behave like the versions of Linux described
    above.

  . I heard a report that the USAGI IPv6 stack for Linux has implemented
    IPV6_V6ONLY.

  So, what we do is as follows:

   (1) At this point we set up two addresses, one containing ":" to indicate
   an IPv6 wildcard address, and an empty one to indicate an IPv4 wildcard
   address.

   (2) Later, when we create the IPv6 socket, we set IPV6_V6ONLY if that option
   is defined.

   (3) We listen on the v6 socket first. If that fails, there is a serious
   error.

   (4) We listen on the v4 socket second. If that fails with the error
   EADDRINUSE, assume we are in the situation where just a single socket is
   permitted, and ignore the error. */

  else
    {
    addresses = store_get(sizeof(ip_address_item));

    #if HAVE_IPV6
    addresses->next = store_get(sizeof(ip_address_item));
    addresses->address[0] = ':';
    addresses->address[1] = 0;
    addresses->next->next = NULL;
    addresses->next->address[0] = 0;
    listen_socket_count = 2;

    #else
    addresses->next = NULL;
    addresses->address[0] = 0;
    listen_socket_count = 1;
    #endif  /* HAVE_IPV6 */
    }

  /* Get a vector to remember all the sockets in */

  listen_sockets = store_get(sizeof(int *) * listen_socket_count);

  /* If any option requiring a load average to be available during the
  reception of a message is set, call os_getloadavg() while we are root
  for those OS for which this is necessary the first time it is called (in
  order to perform an "open" on the kernel memory file). */

  #ifdef LOAD_AVG_NEEDS_ROOT
  if (queue_only_load >= 0 || smtp_load_reserve >= 0) (void)os_getloadavg();
  #endif

  /* For each IP address, create a socket and bind it to the appropriate
  port. See comments above about IPv6 sockets that may or may not accept IPv4
  calls when listening on all interfaces. We also have to cope with the case of
  a system with IPv6 libraries, but no IPv6 support in the kernel. In this
  case, we must ignore failure to create an IPv6 socket for wildcard listening.
  The second socket (IPv4) should then get used instead - we have to shuffle
  it down into first place. */

  for (ipa = addresses, sk = 0; sk < listen_socket_count; ipa = ipa->next, sk++)
    {
    int i;
    int af = (strchr(ipa->address, ':') != NULL)? AF_INET6 : AF_INET;

    listen_sockets[sk] = socket(af, SOCK_STREAM, 0);
    if (listen_sockets[sk] < 0)
      {
      /* Just log failure for an IPv6 wildcard socket */

      if (af == AF_INET6 && local_interfaces == NULL)
        {
        log_write(0, LOG_MAIN, "Failed to create IPv6 socket for wildcard "
          "listening (%s): falling back to IPv4", strerror(errno));

        addresses = addresses->next;   /* Chop IPv6 off the list */
        sk--;                          /* Back up the count */
        listen_socket_count--;         /* Reduce the total */
        continue;                      /* With the IPv4 socket */
        }

      /* Not a failure to create an IPv6 socket for wildcard listening */

      else
        log_write(0, LOG_PANIC_DIE, "IPv%c socket creation failed: %s",
          (af == AF_INET6)? '6' : '4', strerror(errno));
      }

    /* If this is an IPv6 wildcard socket, set IPV6_V6ONLY if that option is
    available. Just log failure (can get protocol not available, just like
    socket creation can). */

    #ifdef IPV6_V6ONLY
    if (local_interfaces == NULL && af == AF_INET6 &&
        setsockopt(listen_sockets[sk], SOL_SOCKET, IPV6_V6ONLY, (char *)(&on),
          sizeof(on)) < 0)
      log_write(0, LOG_MAIN, "Setting IPV6_V6ONLY on daemon's IPv6 wildcard "
        "socket failed (%s): carrying on without it", strerror(errno));
    #endif  /* IPV6_V6ONLY */

    /* Set SO_REUSEADDR so that the daemon can be restarted while a connection
    is being handled.  Without this, a connection will prevent reuse of the
    smtp port for listening. */

    if (setsockopt(listen_sockets[sk], SOL_SOCKET, SO_REUSEADDR, (char *)(&on),
      sizeof(on)) < 0)
        log_write(0, LOG_PANIC_DIE, "setting SO_REUSEADDR on socket failed: %s",
          strerror(errno));

    /* Now bind the socket to the required port; if Exim is being restarted
    it may not always be possible to bind immediately, even with SO_REUSEADDR
    set, so try 10 times, waiting 30 seconds between each try. */

    memset(&sin, 0, sizeof(sin));

    /* Setup code when using an IPv6 socket. The wildcard address is ":", to
    ensure an IPv6 socket is used. */

    #if HAVE_IPV6
    if (af == AF_INET6)
      {
      sin.v6.sin6_family = AF_INET6;
      sin.v6.sin6_port = net_port;
      if (ipa->address[0] == ':' && ipa->address[1] == 0)
        {
        sin.v6.sin6_addr = anyaddr6;
        DEBUG(9) debug_printf("listening on all interfaces (IPv6)\n");
        }
      else
        {
        if (inet_pton(AF_INET6, ipa->address, &sin.v6.sin6_addr) != 1)
          log_write(0, LOG_PANIC_DIE, "unable to parse \"%s\"", ipa->address);
        DEBUG(9) debug_printf("listening on %s\n", ipa->address);
        }
      }
    else
    #endif  /* HAVE_IPV6 */

    /* Setup code when using IPv4 socket. The wildcard address is "". */

      {
      sin.v4.sin_family = AF_INET;
      sin.v4.sin_port = net_port;
      if (ipa->address[0] == 0)
        {
        sin.v4.sin_addr.s_addr = (S_ADDR_TYPE)INADDR_ANY;
        DEBUG(9) debug_printf("listening on all interfaces (IPv4)\n");
        }
      else
        {
        sin.v4.sin_addr.s_addr = (S_ADDR_TYPE)inet_addr(ipa->address);
        DEBUG(9) debug_printf("listening on %s\n", ipa->address);
        }
      }

    /* Retry loop. Need to get the length of sin right for IPv4/IPv6
    for some operating systems that check it. Although AF_INET6 is always
    available (even on old IPv4 systems), the sin.v6 field isn't. */

    for (i = 9; i >= 0; i--)
      {
      if (bind(listen_sockets[sk], (struct sockaddr *)&sin,
               #if HAVE_IPV6
               (af == AF_INET6)? sizeof(sin.v6) : sizeof(sin.v4)
               #else
               sizeof(sin)
               #endif /* HAVE_IPV6 */
           ) < 0)
        {
        char *msg = strerror(errno);
        char *addr = (ipa->address[0] == 0)? "(any IPv4)" :
          (ipa->address[0] == ':' && ipa->address[1] == 0)? "(any IPv6)" :
          ipa->address;
        if (i == 0)
          log_write(0, LOG_MAIN|LOG_PANIC_DIE,
            "socket bind() to port %d for address %s failed: %s: "
            "daemon abandoned", smtp_port, addr, msg);
        log_write(0, LOG_MAIN, "socket bind() to port %d for address %s "
          "failed: %s: waiting before trying again", smtp_port, addr, msg);
        sleep(30);
        }
      else break;
      }
    }

  /* Do a sanity check on the max connects value just to save us from getting
  a huge amount of store. */

  if (smtp_accept_max > 4095) smtp_accept_max = 4096;

  /* There's no point setting smtp_accept_queue unless it is less than the max
  connects limit. The configuration reader ensures that the max is set if the
  queue-only option is set. */

  if (smtp_accept_queue > smtp_accept_max) smtp_accept_queue = 0;

  /* Get somewhere to keep the list of SMTP accepting pids if we are keeping
  track of them for total number and queue/host limits. */

  if (smtp_accept_max > 0)
    {
    int i;
    smtp_pid_slots = store_get(smtp_accept_max * sizeof(pid_t));
    smtp_host_address_slots = store_get(smtp_accept_max * sizeof(char *));
    for (i = 0; i < smtp_accept_max; i++)
      {
      smtp_pid_slots[i] = 0;
      smtp_host_address_slots[i] = NULL;
      }
    }

  /* Start listening on the bound sockets, establishing the maximum backlog of
  connections that is allowed. In an IPv6 environment, if listen() fails with
  the error EADDRINUSE and we are doing wildcard listening and this is the
  second (i.e last) socket, ignore the error on the grounds that we must be
  in a system where the IPv6 socket accepts both kinds of call. */

  for (sk = 0; sk < listen_socket_count; sk++)
    {
    if (listen(listen_sockets[sk], smtp_connect_backlog) < 0)
      {
      if (errno == EADDRINUSE && local_interfaces == NULL && sk > 0)
        {
        DEBUG(9) debug_printf("wildcard IPv4 listen() failed after IPv6 "
          "listen() success; EADDRINUSE ignored\n");
        close(listen_sockets[sk]);
        listen_socket_count--;       /* OK because we know we are in the */
        break;                       /* last iteration of this loop */
        }
      else
        {
        int skf;
        for (ipa = addresses, skf = 0; skf < sk; ipa = ipa->next, skf++);
        log_write(0, LOG_PANIC_DIE, "listen() failed on interface %s: %s",
          (ipa->address[0] == 0)? "(any IPv4)" :
          (ipa->address[0] == ':' && ipa->address[1] == ':')? "(any IPv6)" :
          ipa->address,
          strerror(errno));
        }
      }
    }
  }

/* Set up the handler for SIGHUP, which causes a restart of the daemon. */

sighup_seen = FALSE;
signal(SIGHUP, sighup_handler);

/* Write the pid to a known file for assistance in identification. Make it
read-only. We do this before giving up root privilege, because on some systems
it is necessary to be root in order to write into the pid file directory. */

f = fopen(buff, "w");
if (f != NULL)
  {
  fprintf(f, "%d\n", (int)getpid());
  fchmod(fileno(f), 0644);
  fclose(f);
  DEBUG(2) debug_printf("pid written to %s\n", buff);
  }
else
  DEBUG(2) debug_printf("%s\n", string_open_failed(errno, "pid file %s", buff));

/* If exim_uid is set, give up root privilege at this point, using setuid or
seteuid as appopriate. The macros expand to -1 on systems that don't have
the sete{g,u}id functions, but the security level cannot be set to values
implying the use of these functions on such systems. */

if (exim_uid_set)
  {
  if (security_level >= 2)
    exim_setugid(exim_uid, exim_gid, "running the daemon");
  else
    {
    mac_setegid(exim_gid);
    mac_seteuid(exim_uid);
    }
  }

/* Get somewhere to keep the list of queue-runner pids if we are keeping track
of them (and also if we are doing queue runs). */

if (queue_interval > 0 && queue_run_max > 0)
  {
  int i;
  queue_pid_slots = store_get(queue_run_max * sizeof(pid_t));
  for (i = 0; i < queue_run_max; i++) queue_pid_slots[i] = 0;
  }

/* Set up the handler for termination of child processes. */

sigchld_seen = FALSE;
signal(SIGCHLD, main_sigchld_handler);

/* If we are to run the queue periodically, pretend the alarm has just
gone off. This will cause the first queue-runner to get kicked off straight
away, and the alarm to be reset. */

daemon_sigalrm_seen = (queue_interval > 0);


/* Log the start up of a daemon. */

if (queue_interval > 0)
  {
  if (daemon_listen)
    {
    log_write(0, LOG_MAIN,
      "exim %s daemon started: pid=%d, -q%s, listening for SMTP on port %d",
      version_string, getpid(), readconf_printtime(queue_interval), smtp_port);
    set_process_info("daemon: -q%s, listening on port %d",
      readconf_printtime(queue_interval), smtp_port);
    }
  else
    {
    log_write(0, LOG_MAIN,
      "exim %s daemon started: pid=%d, -q%s, not listening for SMTP",
      version_string, getpid(), readconf_printtime(queue_interval));
    set_process_info("daemon: -q%s, not listening",
      readconf_printtime(queue_interval));
    }
  }
else
  {
  log_write(0, LOG_MAIN,
    "exim %s daemon started: pid=%d, no queue runs, listening for SMTP on port %d",
     version_string, getpid(), smtp_port);
  set_process_info("daemon: no queue runs, port %d", smtp_port);
  }

/* Close the log so it can be renamed and moved. This process doesn't write
to the log again, unless it is about to die or exec and in the latter case
it closes the log first. */

log_close_all();

DEBUG(2) debug_print_ids("daemon running with");

/* Any messages accepted via this route are going to be SMTP. */

smtp_input = TRUE;

/* Enter the never-ending loop... */

for (;;)
  {
  #if HAVE_IPV6
  struct sockaddr_in6 accepted;
  #else
  struct sockaddr_in accepted;
  #endif

  int len = sizeof(accepted);
  int status;
  pid_t pid;

  /* This code is placed first in the loop, so that it gets obeyed at the
  start, before the first wait. This causes the first queue-runner to be
  started immediately. */

  if (daemon_sigalrm_seen)
    {
    DEBUG(9) debug_printf("SIGALRM received\n");

    /* Do a full queue run in a child process, if required, unless we already
    have enough queue runners on the go. If we are not running as root, a
    re-exec is required. */

    if (queue_interval > 0 &&
       (queue_run_max <= 0 || queue_run_count < queue_run_max))
      {
      if ((pid = fork()) == 0)
        {
        int sk;
        DEBUG(1) debug_printf("Starting queue-runner: pid %d\n", getpid());

        /* Close any open listening sockets in the child */

        for (sk = 0; sk < listen_socket_count; sk++) close(listen_sockets[sk]);

        /* Reset signals in the child */

        signal(SIGALRM, SIG_DFL);
        signal(SIGHUP,  SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        /* Re-exec if privilege has been given up */

        if (geteuid() != root_uid)
          {
          int i = 0;
          char *argv[5 + MAX_CLMACROS];
          char opt[16];
          char *p = opt;

          argv[i++] = exim_path;
          if (clmacro_count > 0)
            {
            memcpy(argv + i, clmacros, clmacro_count * sizeof(char *));
            i += clmacro_count;
            }
          if (config_changed)
            {
            argv[i++] = "-C";
            argv[i++] = config_filename;
            }
          argv[i++] = opt;
          *p++ = '-';
          *p++ = 'q';
          if (queue_2stage) *p++ = 'q';
          if (queue_run_force) *p++ = 'f';
          if (queue_run_local) *p++ = 'l';
          *p = 0;
          argv[i++] = (char *)0;
          execv(argv[0], argv);
          log_write(0, LOG_PANIC_DIE, "exec of exim %s failed: %s", opt,
            strerror(errno));
          }

        /* No need to re-exec */

        queue_run(NULL, NULL, FALSE);
        _exit(EXIT_SUCCESS);
        }

      if (pid < 0)
        {
        log_write(0, LOG_PANIC, "fork of queue-runner process failed");
        }
      else
        {
        int i;
        for (i = 0; i < queue_run_max; ++i)
          {
          if (queue_pid_slots[i] <= 0)
            {
            queue_pid_slots[i] = pid;
            queue_run_count++;
            break;
            }
          }
        DEBUG(2) debug_printf("%d queue-runner process%s running\n",
          queue_run_count, (queue_run_count == 1)? "" : "es");
        }
      }

    /* Reset the alarm time */

    daemon_sigalrm_seen = FALSE;
    os_non_restarting_signal(SIGALRM, daemon_sigalrm_handler);
    alarm(queue_interval);
    }


  /* Sleep till a connection happens if listening, and handle the connection if
  that is why we woke up. The FreeBSD operating system requires the use of
  select() before accept() because the latter function is not interrupted by
  a signal, and we want to wake up for SIGCHLD and SIGALRM signals. Some other
  OS do notice signals in accept() but it does no harm to have the select()
  in for all of them - and it won't then be a lurking problem for ports to
  new OS. In fact, the later addition of listening on specific interfaces only
  requires this way of working anyway. */

  if (daemon_listen)
    {
    int sk, lcount;
    int max_socket = 0;
    BOOL select_failed = FALSE;
    fd_set select_listen;

    FD_ZERO(&select_listen);
    for (sk = 0; sk < listen_socket_count; sk++)
      {
      FD_SET(listen_sockets[sk], &select_listen);
      if (listen_sockets[sk] > max_socket) max_socket = listen_sockets[sk];
      }

    DEBUG(2) debug_printf("listening on port %d...\n", smtp_port);

    if ((lcount = select(max_socket + 1, (SELECT_ARG2_TYPE *)&select_listen,
         NULL, NULL, NULL)) < 0)
      {
      select_failed = TRUE;
      lcount = 1;
      }

    /* Loop for all the sockets that are currently ready to go. If select
    actually failed, we have set the count to 1 and a flag, so as to use the
    common error code for select/accept below. */

    while (lcount-- > 0)
      {
      int accept_socket = -1;
      if (!select_failed)
        {
        for (sk = 0; sk < listen_socket_count; sk++)
          {
          if (FD_ISSET(listen_sockets[sk], &select_listen))
            {
            accept_socket = accept(listen_sockets[sk],
              (struct sockaddr *)&accepted, &len);
            FD_CLR(listen_sockets[sk], &select_listen);
            break;
            }
          }
        }

      /* If select or accept has failed and this was not caused by an
      interruption, log the incident and try again. With asymmetric TCP/IP
      routing errors such as "No route to network" have been seen here. Also
      "connection reset by peer" has been seen. These cannot be classed as
      disastrous errors, but they could fill up a lot of log. The code in smail
      crashes the daemon after 10 successive failures of accept, on the grounds
      that some OS fail continuously. Exim originally followed suit, but this
      appears to have caused problems. Now it just keeps going, but instead of
      logging each error, it batches them up when they are continuous. */

      if (accept_socket < 0 && errno != EINTR)
        {
        if (accept_retry_count == 0)
          {
          accept_retry_errno = errno;
          accept_retry_select_failed = select_failed;
          }
        else
          {
          if (errno != accept_retry_errno ||
              select_failed != accept_retry_select_failed ||
              accept_retry_count >= 50)
            {
            log_write(0, LOG_MAIN | ((accept_retry_count >= 50)? LOG_PANIC : 0),
              "%d %s() failure%s: %s",
              accept_retry_count,
              accept_retry_select_failed? "select" : "accept",
              (accept_retry_count == 1)? "" : "s",
              strerror(accept_retry_errno));
            accept_retry_count = 0;
            accept_retry_errno = errno;
            accept_retry_select_failed = select_failed;
            }
          }
        accept_retry_count++;
        }

      else
        {
        if (accept_retry_count > 0)
          {
          log_write(0, LOG_MAIN, "%d %s() failure%s: %s",
            accept_retry_count,
            accept_retry_select_failed? "select" : "accept",
            (accept_retry_count == 1)? "" : "s",
            strerror(accept_retry_errno));
          accept_retry_count = 0;
          }
        }

      /* If select/accept succeeded, deal with the connection. */

      if (accept_socket >= 0)
        handle_smtp_call(listen_sockets, listen_socket_count, accept_socket,
	  (struct sockaddr *)&accepted);
      }
    }

  /* If not listening, then just sleep for the queue interval. If we woke
  up early the last time for some other signal, it won't matter because
  the alarm signal will wake at the right time. This code originally used
  sleep() but it turns out that on the FreeBSD system, sleep() is not inter-
  rupted by signals, so it wasn't waking up for SIGALRM or SIGCHLD. Luckily
  select() can be used as an interruptible sleep() on all versions of Unix. */

  else
    {
    struct timeval tv;
    tv.tv_sec = queue_interval;
    tv.tv_usec = 0;
    select(0, NULL, NULL, NULL, &tv);
    }

  /* Handle the termination of a child process. Theoretically, this need
  be done only when sigchld_seen is TRUE, but rumour has it that some systems
  lose SIGCHLD signals at busy times, so to be on the safe side, just
  do it each time round. It shouldn't be too expensive. */

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
    int i;
    DEBUG(2) debug_printf("child %d ended: status=0x%x\n", pid, status);

    /* If it's a listening daemon, deal with an accepting process. */

    if (daemon_listen)
      {
      for (i = 0; i < smtp_accept_max; i++)
        {
        if (smtp_pid_slots[i] == pid)
          {
          smtp_pid_slots[i] = 0;
          if (smtp_host_address_slots[i] != NULL)
            {
	    store_free(smtp_host_address_slots[i]);
	    smtp_host_address_slots[i] = NULL;
            }
          if (--smtp_accept_count < 0) smtp_accept_count = 0;
          DEBUG(2) debug_printf("%d SMTP accept process%s now running\n",
            smtp_accept_count, (smtp_accept_count == 1)? "" : "es");
          break;
          }
        }
      if (i < smtp_accept_max) continue;  /* Found an accepting process */
      }

    /* If it wasn't an accepting process, see if it was a queue-runner
    process, if we are keeping track of them. */

    if (queue_interval > 0)
      {
      for (i = 0; i < queue_run_max; i++)
        {
        if (queue_pid_slots[i] == pid)
          {
          queue_pid_slots[i] = 0;
          if (--queue_run_count < 0) queue_run_count = 0;
          DEBUG(2) debug_printf("%d queue-runner process%s now running\n",
            queue_run_count, (queue_run_count == 1)? "" : "es");
          break;
          }
        }
      }
    }

  /* Re-enable the SIGCHLD handler if it has been run. It can't do it
  for itself, because it isn't doing the waiting itself. */

  if (sigchld_seen)
    {
    sigchld_seen = FALSE;
    signal(SIGCHLD, main_sigchld_handler);
    }

  /* Handle being woken by SIGHUP. We know at this point that the result
  of accept() has been dealt with, so we can re-exec exim safely, first
  closing the listening sockets so that they can be reused. All log files
  get closed by the close-on-exec flag. */

  if (sighup_seen)
    {
    int sk;
    log_write(0, LOG_MAIN, "pid %d: SIGHUP received: re-exec daemon",
      getpid());
    for (sk = 0; sk < listen_socket_count; sk++) close(listen_sockets[sk]);
    alarm(0);
    signal(SIGHUP, SIG_IGN);
    sighup_argv[0] = exim_path;
    execv(exim_path, sighup_argv);
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "pid %d: exec of %s failed: %s",
      getpid(), exim_path, strerror(errno));
    }

  }   /* End of main loop */

/* Control never reaches here */
}

/* End of exim_daemon.c */

