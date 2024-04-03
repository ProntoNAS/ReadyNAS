/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* Code for mail filtering functions. In order to recover the store used,
store_pool is set to POOL_TEMP while running the filter code in normal
situations. However, it must arrange that any created addresses are placed in
POOL_MAIN, because that data lives on beyond the running of the filter. Text
placed in error pointers does not behave like this: it is used immediately by
the caller. */


#include "exim.h"


typedef struct filter_cmd {
  struct filter_cmd *next;
  int command;
  BOOL seen;
  BOOL noerror;
  void *args[1];
} filter_cmd;

typedef struct condition_block {
  struct condition_block *parent;
  int type;
  BOOL testfor;
  void *left;
  void *right;
} condition_block;

typedef struct alias {
  struct alias *next;
  char *name;
} alias;


static char **error_pointer;
static char *log_filename;
static int *special_action;
static int  line_number;
static int  expect_endif;
static int  had_else_endif;
static int  log_fd;
static int  log_mode;
static int  output_indent;
static BOOL seen_force;
static BOOL seen_value;
static BOOL noerror_force;
static BOOL system_filtering = FALSE;

enum { had_neither, had_else, had_elif, had_endif };

static BOOL read_command_list(char **, filter_cmd ***, BOOL);


/* The string arguments for the mail command. The header line ones (that are
permitted to include \n followed by white space) first, and then the body text
one (it can have \n anywhere). Then the file names and once_repeat, which may
not contain \n. */

static char *mailargs[] = {  /* "to" must be first, and */
  "to",                      /* "cc" and "bcc" must follow */
  "cc",
  "bcc",
  "from",
  "reply_to",
  "subject",
  "text",
  "file",
  "log",
  "once",
  "once_repeat"
};

/* The count of string arguments */

#define mailargs_string_count (sizeof(mailargs)/sizeof(char *))

/* The count of string arguments that are actually passed over as strings
(once_repeat is converted to an int). */

#define mailargs_string_passed (mailargs_string_count - 1)

/* This defines the offsets for the arguments; first the string ones, and
then the non-string ones. The order must be as above. */

enum { mailarg_index_to,
       mailarg_index_cc,
       mailarg_index_bcc,
       mailarg_index_from,
       mailarg_index_reply_to,
       mailarg_index_subject,
       mailarg_index_text,         /* text is first after headers */
       mailarg_index_file,         /* between text and expand are filenames */
       mailarg_index_log,
       mailarg_index_once,
       mailarg_index_once_repeat,  /* a time string */
       mailarg_index_expand,       /* first non-string argument */
       mailarg_index_return,
       mailargs_total              /* total number of arguments */
       };

/* Offsets in the data structure for the string arguments (note that
once_repeat isn't a string argument at this point.) */

static int reply_offsets[] = {  /* must be in same order as above */
  offsetof(reply_item, to),
  offsetof(reply_item, cc),
  offsetof(reply_item, bcc),
  offsetof(reply_item, from),
  offsetof(reply_item, reply_to),
  offsetof(reply_item, subject),
  offsetof(reply_item, text),
  offsetof(reply_item, file),
  offsetof(reply_item, logfile),
  offsetof(reply_item, oncelog),
};

/* Condition identities and names, with negated versions for some
of them. */

enum { cond_and, cond_or, cond_personal, cond_begins, cond_BEGINS,
       cond_ends, cond_ENDS, cond_is, cond_IS, cond_matches,
       cond_MATCHES, cond_contains, cond_CONTAINS, cond_delivered,
       cond_above, cond_below, cond_errormsg, cond_firsttime,
       cond_manualthaw, cond_foranyaddress };

static char *cond_names[] = {
  "and", "or", "personal",
  "begins", "BEGINS", "ends", "ENDS",
  "is", "IS", "matches", "MATCHES", "contains",
  "CONTAINS", "delivered", "above", "below", "error_message",
  "first_delivery", "manually_thawed", "foranyaddress" };

static char *cond_not_names[] = {
  "", "", "not personal",
  "does not begin", "does not BEGIN",
  "does not end", "does not END",
  "is not", "IS not", "does not match",
  "does not MATCH", "does not contain", "does not CONTAIN",
  "not delivered", "not above", "not below", "not error_message",
  "not first_delivery", "not manually_thawed", "not foranyaddress" };

/* Tables of binary condition words and their corresponding types. Not easy
to amalgamate with the above because of the different variants. */

static char *cond_words[] = {
   "BEGIN",
   "BEGINS",
   "CONTAIN",
   "CONTAINS",
   "END",
   "ENDS",
   "IS",
   "MATCH",
   "MATCHES",
   "above",
   "begin",
   "begins",
   "below",
   "contain",
   "contains",
   "end",
   "ends",
   "is",
   "match",
   "matches"};

static int cond_word_count = (sizeof(cond_words)/sizeof(char *));

static int cond_types[] = { cond_BEGINS, cond_BEGINS, cond_CONTAINS,
  cond_CONTAINS, cond_ENDS, cond_ENDS, cond_IS, cond_MATCHES, cond_MATCHES,
  cond_above, cond_begins, cond_begins, cond_below, cond_contains,
  cond_contains, cond_ends, cond_ends, cond_is, cond_matches, cond_matches };

/* Command identities: must be kept in step with the list of command words
and the list of expanded argument counts which follow. Stupidly, there was a
"log" command as well as a "log" option on the mail command. The main command
is now "logwrite", but the old name must remain as a synonym. */

enum { add_command, deliver_command, elif_command, else_command, endif_command,
       finish_command, fail_command, freeze_command, headers_command,
       if_command, log_command, logfile_command, logwrite_command,
       mail_command, noerror_command, pipe_command, save_command,
       seen_command, testprint_command, unseen_command, vacation_command };

static char *command_list[] = {
  "add",     "deliver", "elif", "else", "endif",     "finish",   "fail",
  "freeze",  "headers", "if",   "log",  "logfile",   "logwrite", "mail",
  "noerror", "pipe",    "save", "seen", "testprint", "unseen",   "vacation"
};

static int command_list_count = sizeof(command_list)/sizeof(char *);

static int command_exparg_count[] = {
   2, /* add */
   2, /* deliver */
   0, /* elif */
   0, /* else */
   0, /* endif */
   0, /* finish */
   1, /* fail */
   1, /* freeze */
   1, /* headers */
   0, /* if */
   1, /* log */
   1, /* logfile */
   1, /* logwrite */
   mailargs_string_count, /* mail */
   0, /* noerror */
   0, /* pipe */
   1, /* save */
   0, /* seen */
   1, /* testprint */
   0, /* unseen */
   mailargs_string_count /* vacation */
};



/*************************************************
*          Find next significant char            *
*************************************************/

/* Function to skip over white space and, optionally, comments.

Arguments:
  ptr              pointer to next character
  comment_allowed  if TRUE, comments (# to \n) are skipped

Returns:           pointer to next non-whitespace character
*/

char *
nextsigchar(char *ptr, BOOL comment_allowed)
{
for (;;)
  {
  while (isspace((uschar)*ptr))
    {
    if (*ptr == '\n') line_number++;
    ptr++;
    }
  if (comment_allowed && *ptr == '#')
    {
    while (*(++ptr) != '\n' && *ptr != 0);
    continue;
    }
  else break;
  }
return ptr;
}



/*************************************************
*                Read one word                   *
*************************************************/

/* The terminator is white space unless bracket is TRUE, in which
case ( and ) terminate.

Arguments
  ptr       pointer to next character
  buffer    where to put the word
  size      size of buffer
  bracket   if TRUE, terminate on ( and ) as well as space

Returns:    pointer to the next significant character after the word
*/

char *
nextword(char *ptr, char *buffer, int size, BOOL bracket)
{
char *bp = buffer;
while (*ptr != 0 && !isspace((uschar)*ptr) &&
       (!bracket || (*ptr != '(' && *ptr != ')')))
  {
  if (bp - buffer < size - 1) *bp++ = *ptr++; else
    {
    *error_pointer = string_sprintf("word is too long in line %d of "
      "filter file (max = %d chars)", line_number, size);
    break;
    }
  }
*bp = 0;
return nextsigchar(ptr, TRUE);
}



/*************************************************
*                Read one item                   *
*************************************************/

/* Might be a word, or might be a quoted string; in the latter case
do the escape stuff.

Arguments:
  ptr        pointer to next character
  buffer     where to put the item
  size       size of buffer
  bracket    if TRUE, terminate non-quoted on ( and ) as well as space

Returns:     the next significant character after the item
*/

char *
nextitem(char *ptr, char *buffer, int size, BOOL bracket)
{
char *bp = buffer;
if (*ptr != '\"') return nextword(ptr, buffer, size, bracket);

while (*(++ptr) != 0 && *ptr != '\"' && *ptr != '\n')
  {
  if (bp - buffer >= size - 1)
    {
    *error_pointer = string_sprintf("string is too long in line %d of "
      "filter file (max = %d chars)", line_number, size);
    break;
    }

  if (*ptr != '\\') *bp++ = *ptr; else
    {
    if (isspace((uschar)ptr[1]))    /* \<whitespace>NL<whitespace> ignored */
      {
      char *p = ptr + 1;
      while (*p != '\n' && isspace((uschar)*p)) p++;
      if (*p == '\n')
        {
        line_number++;
        ptr = p;
        while (ptr[1] != '\n' && isspace((uschar)ptr[1])) ptr++;
        continue;
        }
      }

    *bp++ = string_interpret_escape(&ptr);
    }
  }

if (*ptr == '\"') ptr++;
  else if (*error_pointer == NULL)
    *error_pointer = string_sprintf("quote missing at end of string "
      "in line %d", line_number);

*bp = 0;
return nextsigchar(ptr, TRUE);
}




/*************************************************
*          Convert a string + K|M to a number    *
*************************************************/

/*
Arguments:
  s        points to text string
  OK       set TRUE if a valid number was read

Returns:   the number, or 0 on error (with *OK FALSE)
*/

static int
get_number(char *s, BOOL *OK)
{
int value, count;
*OK = FALSE;
if (sscanf(s, "%i%n", &value, &count) != 1) return 0;
if (tolower(s[count]) == 'k') { value *= 1024; count++; }
if (tolower(s[count]) == 'm') { value *= 1024*1024; count++; }
while (isspace((uschar)(s[count]))) count++;
if (s[count] != 0) return 0;
*OK = TRUE;
return value;
}



/*************************************************
*            Read one condition                  *
*************************************************/

/* A complete condition must be terminated by "then"; bracketed internal
conditions must be terminated by a closing bracket. They are read by calling
this function recursively.

Arguments:
  ptr             points to start of condition
  condition_block where to hang the created condition block
  toplevel        TRUE when called at the top level

Returns:          points to next character after "then"
*/

static char *
read_condition(char *ptr, condition_block **cond, BOOL toplevel)
{
char buffer[1024];
BOOL testfor = TRUE;
condition_block *current_parent = NULL;
condition_block **current = cond;

*current = NULL;

/* Loop to read next condition */

for (;;)
  {
  condition_block *c;

  /* reaching the end of the input is an error. */

  if (*ptr == 0)
    {
    *error_pointer = string_sprintf("\"then\" missing at end of filter file");
    break;
    }

  /* Opening bracket at the start of a condition introduces a nested
  condition, which must be terminated by a closing bracket. */

  if (*ptr == '(')
    {
    ptr = read_condition(nextsigchar(ptr+1, TRUE), &c, FALSE);
    if (*error_pointer != NULL) break;
    if (*ptr != ')')
      {
      *error_pointer = string_sprintf("expected \")\" in line %d of "
        "filter file", line_number);
      break;
      }
    if (!testfor)
      {
      c->testfor = !c->testfor;
      testfor = TRUE;
      }
    ptr = nextsigchar(ptr+1, TRUE);
    }


  /* Closing bracket at the start of a condition is an error. Give an
  explicit message, as otherwise "unknown condition" would be confusing. */

  else if (*ptr == ')')
    {
    *error_pointer = string_sprintf("unexpected \")\" in line %d of "
      "filter file", line_number);
    break;
    }

  /* Otherwise we expect a word or a string. */

  else
    {
    ptr = nextitem(ptr, buffer, sizeof(buffer), TRUE);
    if (*error_pointer != NULL) break;

    /* "Then" at the start of a condition is an error */

    if (strcmp(buffer, "then") == 0)
      {
      *error_pointer = string_sprintf("unexpected \"then\" near line %d of "
        "filter file", line_number);
      break;
      }

    /* "Not" at the start of a condition negates the testing condition. */

    if (strcmp(buffer, "not") == 0)
      {
      testfor = !testfor;
      continue;
      }

    /* Build a condition block from the specific word. */

    c = store_get(sizeof(condition_block));
    c->left = c->right = NULL;
    c->testfor = testfor;
    testfor = TRUE;

    /* Check for conditions that start with a keyword */

    if (strcmp(buffer, "delivered") == 0) c->type = cond_delivered;
    else if (strcmp(buffer, "error_message") == 0) c->type = cond_errormsg;
    else if (strcmp(buffer, "first_delivery") == 0) c->type = cond_firsttime;
    else if (strcmp(buffer, "manually_thawed") == 0) c->type = cond_manualthaw;

    /* Personal can be followed by any number of aliases */

    else if (strcmp(buffer, "personal") == 0)
      {
      c->type = cond_personal;
      for (;;)
        {
        alias *a;
        char *saveptr = ptr;
        ptr = nextword(ptr, buffer, sizeof(buffer), TRUE);
        if (*error_pointer != NULL) break;
        if (strcmp(buffer, "alias") != 0)
          {
          ptr = saveptr;
          break;
          }
        ptr = nextitem(ptr, buffer, sizeof(buffer), TRUE);
        if (*error_pointer != NULL) break;
        a = store_get(sizeof(alias));
        a->name = string_copy(buffer);
        a->next = (alias *)(c->left);
        c->left = (void *)a;
        }
      }

    /* Foranyaddress must be followed by a string and a condition enclosed
    in parentheses, which is handled as a subcondition. */

    else if (strcmp(buffer, "foranyaddress") == 0)
      {
      ptr = nextword(ptr, buffer, sizeof(buffer), TRUE);
      if (*error_pointer != NULL) break;
      if (*ptr != '(')
        {
        *error_pointer = string_sprintf("\"(\" expected after \"foranyaddress\" "
          "near line %d of filter file", line_number);
        break;
        }

      c->type = cond_foranyaddress;
      c->left = string_copy(buffer);

      ptr = read_condition(nextsigchar(ptr+1, TRUE),
        (condition_block **)(&(c->right)), FALSE);
      if (*error_pointer != NULL) break;
      if (*ptr != ')')
        {
        *error_pointer = string_sprintf("expected \")\" in line %d of "
          "filter file", line_number);
        break;
        }
      if (!testfor)
        {
        c->testfor = !c->testfor;
        testfor = TRUE;
        }
      ptr = nextsigchar(ptr+1, TRUE);
      }

    /* If it's not a word we recognize, then it must be the lefthand
    operand of one of the comparison words. */

    else
      {
      int i;
      char *isptr = NULL;

      c->left = string_copy(buffer);
      ptr = nextword(ptr, buffer, sizeof(buffer), TRUE);
      if (*error_pointer != NULL) break;

      /* Handle "does|is [not]", preserving the pointer after "is" in
      case it isn't that, but the form "is <string>". */

      if (strcmpic(buffer, "does") == 0 || strcmpic(buffer, "is") == 0)
        {
        if (buffer[0] == 'i') { c->type = cond_is; isptr = ptr; }
        if (buffer[0] == 'I') { c->type = cond_IS; isptr = ptr; }

        ptr = nextword(ptr, buffer, sizeof(buffer), TRUE);
        if (*error_pointer != NULL) break;
        if (strcmpic(buffer, "not") == 0)
          {
          c->testfor = !c->testfor;
          if (isptr != NULL) isptr = ptr;
          ptr = nextword(ptr, buffer, sizeof(buffer), TRUE);
          if (*error_pointer != NULL) break;
          }
        }

      for (i = 0; i < cond_word_count; i++)
        {
        if (strcmp(buffer, cond_words[i]) == 0)
          {
          c->type = cond_types[i];
          break;
          }
        }

      /* If an unknown word follows "is" or "is not"
      it's actually the argument. Reset to read it. */

      if (i >= cond_word_count)
        {
        if (isptr != NULL)
          {
          ptr = isptr;
          }
        else
          {
          *error_pointer = string_sprintf("unrecognized condition word \"%s\" "
            "near line %d of filter file", buffer, line_number);
          break;
          }
        }

      /* Get the RH argument. */

      ptr = nextitem(ptr, buffer, sizeof(buffer), TRUE);
      if (*error_pointer != NULL) break;
      c->right = string_copy(buffer);
      }
    }

  /* We have read some new condition and set it up in the condition block
  c; point the current pointer at it, and then deal with what follows. */

  *current = c;

  /* Closing bracket terminates if this is a lower-level condition. Otherwise
  it is unexpected. */

  if (*ptr == ')')
    {
    if (toplevel)
      *error_pointer = string_sprintf("unexpected \")\" in line %d of "
        "filter file", line_number);
    break;
    }

  /* Opening bracket following a condition is an error; give an explicit
  message to make it clearer what is wrong. */

  else if (*ptr == '(')
    {
    *error_pointer = string_sprintf("unexpected \"(\" in line %d of "
      "filter file", line_number);
    break;
    }

  /* Otherwise the next thing must be one of the words "and", "or" or "then" */

  else
    {
    char *saveptr = ptr;
    ptr = nextword(ptr, buffer, sizeof(buffer), FALSE);
    if (*error_pointer != NULL) break;

    /* "Then" terminates a toplevel condition; otherwise a closing bracket
    has been omitted. Put a string terminator at the start of "then" so
    that reflecting the condition can be done when testing. */

    if (strcmp(buffer, "then") == 0)
      {
      if (toplevel) *saveptr = 0;
        else *error_pointer = string_sprintf("missing \")\" at end of "
          "condition near line %d of filter file", line_number);
      break;
      }

    /* "And" causes a new condition block to replace the one we have
    just read, which becomes the left sub-condition. The current pointer
    is reset to the pointer for the right sub-condition. We have to keep
    track of the tree of sequential "ands", so as to traverse back up it
    if an "or" is met. */

    else if (strcmp(buffer, "and") == 0)
      {
      condition_block *andc = store_get(sizeof(condition_block));
      andc->parent = current_parent;
      andc->type = cond_and;
      andc->testfor = TRUE;
      andc->left = c;
      andc->right = NULL;    /* insurance */
      *current = andc;
      current = (condition_block **)(&(andc->right));
      current_parent = andc;
      }

    /* "Or" is similar, but has to be done a bit more carefully to
    ensure that "and" is more binding. If there's a parent set, we
    are following a sequence of "and"s and must track back to their
    start. */

    else if (strcmp(buffer, "or") == 0)
      {
      condition_block *orc = store_get(sizeof(condition_block));
      condition_block *or_parent = NULL;

      if (current_parent != NULL)
        {
        while (current_parent->parent != NULL &&
               current_parent->parent->type == cond_and)
          current_parent = current_parent->parent;

        /* If the parent has a parent, it must be an "or" parent. */

        if (current_parent->parent != NULL)
          or_parent = current_parent->parent;
        }

      orc->parent = or_parent;
      if (or_parent == NULL) *cond = orc; else
        or_parent->right = orc;
      orc->type = cond_or;
      orc->testfor = TRUE;
      orc->left = (current_parent == NULL)? c : current_parent;
      orc->right = NULL;   /* insurance */
      current = (condition_block **)(&(orc->right));
      current_parent = orc;
      }

    /* Otherwise there is a disaster */

    else
      {
      *error_pointer = string_sprintf("\"and\" or \"or\" or \"%s\" "
        "expected near line %d of filter file, but found \"%s\"",
          toplevel? "then" : ")", line_number, buffer);
      break;
      }
    }
  }

return nextsigchar(ptr, TRUE);
}



/*************************************************
*             Ouput the current indent           *
*************************************************/

static void
indent(void)
{
int i;
for (i = 0; i < output_indent; i++) debug_printf(" ");
}



/*************************************************
*          Condition printer: for debugging      *
*************************************************/

/*
Arguments:
  c           the block at the top of the tree
  toplevel    TRUE at toplevel - stops overall brackets

Returns:      nothing
*/

static void
print_condition(condition_block *c, BOOL toplevel)
{
char *name = (c->testfor)? cond_names[c->type] : cond_not_names[c->type];
switch(c->type)
  {
  case cond_personal:
  case cond_delivered:
  case cond_errormsg:
  case cond_firsttime:
  case cond_manualthaw:
  debug_printf("%s", name);
  break;

  case cond_is:
  case cond_IS:
  case cond_matches:
  case cond_MATCHES:
  case cond_contains:
  case cond_CONTAINS:
  case cond_begins:
  case cond_BEGINS:
  case cond_ends:
  case cond_ENDS:
  case cond_above:
  case cond_below:
  debug_printf("%s %s %s", c->left, name, c->right);
  break;

  case cond_and:
  if (!c->testfor) debug_printf("not (");
  print_condition(c->left, FALSE);
  debug_printf(" %s ", cond_names[c->type]);
  print_condition(c->right, FALSE);
  if (!c->testfor) debug_printf(")");
  break;

  case cond_or:
  if (!c->testfor) debug_printf("not (");
  else if (!toplevel) debug_printf("(");
  print_condition(c->left, FALSE);
  debug_printf(" %s ", cond_names[c->type]);
  print_condition(c->right, FALSE);
  if (!toplevel || !c->testfor) debug_printf(")");
  break;

  case cond_foranyaddress:
  debug_printf("%s %s (", name, c->left);
  print_condition(c->right, FALSE);
  debug_printf(")");
  break;
  }
}




/*************************************************
*            Read one filtering command          *
*************************************************/

/*
Arguments:
   pptr        points to pointer to first character of command; the pointer
                 is updated to point after the last character read
   lastcmdptr  points to pointer to pointer to last command; used for hanging
                 on the newly read command

Returns:       TRUE if command successfully read, else FALSE
*/

static BOOL
read_command(char **pptr, filter_cmd ***lastcmdptr)
{
int command, i;
filter_cmd *new, **newlastcmdptr;
BOOL yield = TRUE;
BOOL was_seen_or_unseen = FALSE;
BOOL was_noerror = FALSE;
char buffer[1024];
char *ptr = *pptr;
char *fmsg = NULL;

/* Read the next word and find which command it is. Command words are normally
terminated by white space, but there are two exceptions, which are the "if" and
"elif" commands. We must allow for them to be terminated by an opening bracket,
as brackets are allowed in conditions and users will expect not to require
white space here. */

if (strncmp(ptr, "if(", 3) == 0)
  {
  strcpy(buffer, "if");
  ptr += 2;
  }
else if (strncmp(ptr, "elif(", 5) == 0)
  {
  strcpy(buffer, "elif");
  ptr += 4;
  }
else
  {
  ptr = nextword(ptr, buffer, sizeof(buffer), FALSE);
  if (*error_pointer != NULL) return FALSE;
  }

for (command = 0; command < command_list_count; command++)
  if (strcmp(buffer, command_list[command]) == 0) break;

/* The log command is an obsolete synonym for logwrite. */

if (command == log_command) command = logwrite_command;

/* Handle the individual commands */

switch (command)
  {
  /* Add takes two arguments, separated by the word "to". Headers has two
  arguments, but the first must be "add" or "remove", and it gets stored in the
  second argument slot. Neither may be preceded by seen, unseen or noerror. */

  case add_command:
  case headers_command:
  if (seen_force || noerror_force)
    {
    *error_pointer = string_sprintf("\"seen\", \"unseen\", or \"noerror\" "
      "found before an \"%s\" command near line %d",
        command_list[command], line_number);
    yield = FALSE;
    }
  /* Fall through */

  /* Logwrite, logfile, pipe, and testprint all take a single argument, save
  and logfile can have an option second argument for the mode, and deliver can
  have "errors_to <address>" in a system filter, or in a user filter if the
  address is the current one. */

  case deliver_command:
  case logfile_command:
  case logwrite_command:
  case pipe_command:
  case save_command:
  case testprint_command:

  ptr = nextitem(ptr, buffer, sizeof(buffer), FALSE);
  if (*buffer == 0)
    *error_pointer = string_sprintf("\"%s\" requires an argument "
      "near line %d of filter file", command_list[command], line_number);

  if (*error_pointer != NULL) yield = FALSE; else
    {
    char *argument = NULL;
    char *second_argument = NULL;

    if (command == add_command)
      {
      argument = string_copy(buffer);
      ptr = nextitem(ptr, buffer, sizeof(buffer), FALSE);
      if (*buffer == 0 || strcmp(buffer, "to") != 0)
        *error_pointer = string_sprintf("\"to\" expected in \"add\" command "
          "near line %d of filter file", line_number);
      else
        {
        ptr = nextitem(ptr, buffer, sizeof(buffer), FALSE);
        if (*buffer == 0)
          *error_pointer = string_sprintf("value missing after \"to\" "
            "near line %d of filter file", line_number);
        else second_argument = string_copy(buffer);
        }
      }

    else if (command == headers_command)
      {
      if (!system_filtering)
        {
        *error_pointer = string_sprintf("filtering command \"%s\" is "
          "available only in system filters: near line %d of filter file",
          command_list[command], line_number);
        yield = FALSE;
        break;
        }

      if (strcmp(buffer, "add") == 0)
        second_argument = (void *)TRUE;
      else
        if (strcmp(buffer, "remove") == 0) second_argument = (void *)FALSE;
      else
        {
        *error_pointer = string_sprintf("\"add\" or \"remove\" expected "
          "after \"headers\" near line %d of filter file", line_number);
        yield = FALSE;
        }

      if (yield)
        {
        ptr = nextitem(ptr, buffer, sizeof(buffer), FALSE);
        if (*buffer == 0)
          *error_pointer = string_sprintf("value missing after \"add\" "
            "or \"remove\" near line %d of filter file", line_number);
        else argument = string_copy(buffer);
        }
      }

    /* The argument for the logwrite command must end in a newline, and the save
    and logfile commands can have an optional mode argument. The deliver
    command can have an optional "errors_to <address>" for a system filter,
    or for a user filter if the address is the user's address. Accept the
    syntax here - the check is later. */

    else
      {
      if (command == logwrite_command)
        {
        int len = (int)strlen(buffer);
        if (len == 0 || buffer[len-1] != '\n') strcat(buffer, "\n");
        }

      argument = string_copy(buffer);

      if (command == save_command || command == logfile_command)
        {
        if (isdigit((uschar)*ptr))
          {
          ptr = nextword(ptr, buffer, sizeof(buffer), FALSE);
          second_argument = (void *)strtol(buffer, NULL, 8);
          }
        else second_argument = (void *)(-1);
        }

      else if (command == deliver_command)
        {
        char *save_ptr = ptr;
        ptr = nextword(ptr, buffer, sizeof(buffer), FALSE);
        if (strcmp(buffer, "errors_to") == 0)
          {
          ptr = nextword(ptr, buffer, sizeof(buffer), FALSE);
          second_argument = string_copy(buffer);
          }
        else ptr = save_ptr;
        }
      }

    /* Set up the command block. Seen defaults TRUE for delivery commands,
    FALSE for logging commands, and it doesn't matter for testprint, as
    that doesn't change the "delivered" flag. */

    if (*error_pointer != NULL) yield = FALSE; else
      {
      new = store_get(sizeof(filter_cmd) + sizeof(void *));
      new->next = NULL;
      **lastcmdptr = new;
      *lastcmdptr = &(new->next);
      new->command = command;
      new->seen = seen_force? seen_value :
        (command != logwrite_command && command != logfile_command &&
         command != add_command);
      new->noerror = noerror_force;
      new->args[0] = argument;
      new->args[1] = second_argument;
      }
    }
  break;


  /* Elif, else and endif just set a flag if expected. */

  case elif_command:
  case else_command:
  case endif_command:
  if (expect_endif > 0)
    had_else_endif = (command == elif_command)? had_elif :
                     (command == else_command)? had_else : had_endif;
  else
    {
    *error_pointer = string_sprintf("unexpected \"%s\" command near "
      "line %d of filter file", buffer, line_number);
    yield = FALSE;
    }
  break;


  /* Freeze and fail are available only if there is a pointer to pass
  back special actions; these are available only for system filters. */

  case fail_command:
  case freeze_command:
  if (special_action == NULL)
    {
    *error_pointer = string_sprintf("filtering command \"%s\" is available "
      "only in system filters: near line %d of filter file", buffer,
      line_number);
    yield = FALSE;
    break;
    }

  /* A text message can be provided after the "text" keyword, or
  as a string in quotes. */

  else
    {
    char *saveptr = ptr;
    ptr = nextitem(ptr, buffer, sizeof(buffer), FALSE);
    if (*saveptr != '\"' && (*buffer == 0 || strcmp(buffer, "text") != 0))
      ptr = saveptr;
    else
      {
      if (*saveptr != '\"')
        ptr = nextitem(ptr, buffer, sizeof(buffer), FALSE);
      fmsg = string_copy(buffer);
      }
    }

  /* Drop through and treat as "finish" */

  /* Finish has no arguments; fmsg defaults to NULL */

  case finish_command:
  new = store_get(sizeof(filter_cmd));
  new->next = NULL;
  **lastcmdptr = new;
  *lastcmdptr = &(new->next);
  new->command = command;
  new->seen = seen_force? seen_value : FALSE;
  new->args[0] = fmsg;
  break;


  /* Seen, unseen, and noerror are not allowed before if, which takes a
  condition argument and then and else sub-commands. */

  case if_command:
  if (seen_force || noerror_force)
    {
    *error_pointer = string_sprintf("\"seen\", \"unseen\", or \"noerror\" "
      "found before an \"if\" command near line %d",
        line_number);
    yield = FALSE;
    }

  /* Set up the command block for if */

  new = store_get(sizeof(filter_cmd) + 4 * sizeof(void *));
  new->next = NULL;
  **lastcmdptr = new;
  *lastcmdptr = &(new->next);
  new->command = command;
  new->args[0] = NULL;
  new->args[1] = new->args[2] = NULL;
  new->args[3] = ptr;

  /* Read the condition */

  ptr = read_condition(ptr, (condition_block **)(&(new->args[0])), TRUE);
  if (*error_pointer != NULL) { yield = FALSE; break; }

  /* Read the commands to be obeyed if the condition is true */

  newlastcmdptr = (filter_cmd **)(&(new->args[1]));
  if (!read_command_list(&ptr, &newlastcmdptr, TRUE)) yield = FALSE;

  /* If commands were successfully read, handle the various possible
  terminators. There may be a number of successive "elif" sections. */

  else
    {
    while (had_else_endif == had_elif)
      {
      filter_cmd *newnew =
        store_get(sizeof(filter_cmd) + 4 * sizeof(void *));
      new->args[2] = newnew;
      new = newnew;
      new->next = NULL;
      new->command = command;
      new->args[0] = NULL;
      new->args[1] = new->args[2] = NULL;
      new->args[3] = ptr;

      ptr = read_condition(ptr, (condition_block **)(&(new->args[0])), TRUE);
      if (*error_pointer != NULL) { yield = FALSE; break; }
      newlastcmdptr = (filter_cmd **)(&(new->args[1]));
      if (!read_command_list(&ptr, &newlastcmdptr, TRUE))
        yield = FALSE;
      }

    if (yield == FALSE) break;

    /* Handle termination by "else", possibly following one or more
    "elsif" sections. */

    if (had_else_endif == had_else)
      {
      newlastcmdptr = (filter_cmd **)(&(new->args[2]));
      if (!read_command_list(&ptr, &newlastcmdptr, TRUE))
        yield = FALSE;
      else if (had_else_endif != had_endif)
        {
        *error_pointer = string_sprintf("\"endif\" missing near line %d of "
          "filter file", line_number);
        yield = FALSE;
        }
      }

    /* Otherwise the terminator was "endif" - this is checked by
    read_command_list(). The pointer is already set to NULL. */
    }

  /* Reset the terminator flag. */

  had_else_endif = had_neither;
  break;


  /* The mail & vacation commands have a whole slew of keyworded arguments.
  The final argument values are the file expand and return message booleans,
  whose offsets are defined in mailarg_index_{expand,return}. Although they
  are logically booleans, because they are stored in a char * value, we use
  NULL and not FALSE, to keep 64-bit compilers happy. */

  case mail_command:
  case vacation_command:
  new = store_get(sizeof(filter_cmd) + mailargs_total * sizeof(void *));
  new->next = NULL;
  new->command = command;
  new->seen = seen_force? seen_value : FALSE;
  new->noerror = noerror_force;
  for (i = 0; i < mailargs_total; i++) new->args[i] = NULL;

  /* Read keyword/value pairs until we hit one that isn't. The data
  must contain only printing chars plus tab, though the "text" value
  can also contain newlines. The "file" keyword can be preceded by the
  word "expand", and "return message" has no data. */

  for (;;)
    {
    char *saveptr = ptr;
    ptr = nextword(ptr, buffer, sizeof(buffer), FALSE);
    if (*error_pointer != NULL)
      {
      yield = FALSE;
      break;
      }

    /* Ensure "return" is followed by "message"; that's a complete option */

    if (strcmp(buffer, "return") == 0)
      {
      new->args[mailarg_index_return] = "";  /* not NULL => TRUE */
      ptr = nextword(ptr, buffer, sizeof(buffer), FALSE);
      if (strcmp(buffer, "message") != 0)
        {
        *error_pointer = string_sprintf("\"return\" not followed by \"message\" "
          " near line %d of filter file", line_number);
        yield = FALSE;
        break;
        }
      continue;
      }

    /* Ensure "expand" is followed by "file", then fall through to process the
    file keyword. */

    if (strcmp(buffer, "expand") == 0)
      {
      new->args[mailarg_index_expand] = "";  /* not NULL => TRUE */
      ptr = nextword(ptr, buffer, sizeof(buffer), FALSE);
      if (strcmp(buffer, "file") != 0)
        {
        *error_pointer = string_sprintf("\"expand\" not followed by \"file\" "
          " near line %d of filter file", line_number);
        yield = FALSE;
        break;
        }
      }

    /* Scan for the keyword */

    for (i = 0; i < mailargs_string_count; i++)
      if (strcmp(buffer, mailargs[i]) == 0) break;

    /* Not found keyword; assume end of this command */

    if (i >= mailargs_string_count)
      {
      ptr = saveptr;
      break;
      }

    /* Found keyword, read the data item */

    ptr = nextitem(ptr, buffer, sizeof(buffer), FALSE);
    if (*error_pointer != NULL)
      {
      yield = FALSE;
      break;
      }
    else new->args[i] = string_copy(buffer);
    }

  /* If this is the vacation command, apply some default settings to
  some of the arguments. */

  if (command == vacation_command)
    {
    if (new->args[mailarg_index_file] == NULL)
      {
      new->args[mailarg_index_file] = string_copy(".vacation.msg");
      new->args[mailarg_index_expand] = "";   /* not NULL => TRUE */
      }
    if (new->args[mailarg_index_log] == NULL)
      new->args[mailarg_index_log] = string_copy(".vacation.log");
    if (new->args[mailarg_index_once] == NULL)
      new->args[mailarg_index_once] = string_copy(".vacation");
    if (new->args[mailarg_index_once_repeat] == NULL)
      new->args[mailarg_index_once_repeat] = string_copy("7d");
    }

  /* Join the address on to the chain of generated addresses */

  **lastcmdptr = new;
  *lastcmdptr = &(new->next);
  break;


  /* Seen and unseen just set flags */

  case seen_command:
  case unseen_command:
  if (seen_force)
    {
    *error_pointer = string_sprintf("\"seen\" or \"unseen\" repeated "
      "near line %d", line_number);
    yield = FALSE;
    }
  seen_value = (command == seen_command);
  seen_force = TRUE;
  was_seen_or_unseen = TRUE;
  break;


  /* So does noerror */

  case noerror_command:
  noerror_force = TRUE;
  was_noerror = TRUE;
  break;


  /* Oops */

  default:
  *error_pointer = string_sprintf("unknown filtering command \"%s\" "
    "near line %d of filter file", buffer, line_number);
  yield = FALSE;
  break;
  }

if (!was_seen_or_unseen && !was_noerror)
  {
  seen_force = FALSE;
  noerror_force = FALSE;
  }

*pptr = ptr;
return yield;
}



/*************************************************
*              Read a list of commands           *
*************************************************/

/* If condional is TRUE, the list must be terminated
by the words "else" or "endif".

Arguments:
  pptr        points to pointer to next character; the pointer is updated
  lastcmdptr  points to pointer to pointer to previously-read command; used
                for hanging on the new command
  conditional TRUE if this command is the subject of a condition

Returns:      TRUE on success
*/

static BOOL
read_command_list(char **pptr, filter_cmd ***lastcmdptr, BOOL conditional)
{
if (conditional) expect_endif++;
had_else_endif = had_neither;
while (**pptr != 0 && had_else_endif == had_neither)
  {
  if (!read_command(pptr, lastcmdptr)) return FALSE;
  *pptr = nextsigchar(*pptr, TRUE);
  }
if (conditional)
  {
  expect_endif--;
  if (had_else_endif == had_neither)
    {
    *error_pointer = "\"endif\" missing at end of filter file";
    return FALSE;
    }
  }
return TRUE;
}




/*************************************************
*   Test a header for the "personal" condition   *
*************************************************/

/* A bit more efficient than doing a lot of explicit expansions. We pass
in a chain of alias blocks and a variable-length list of explicit text
strings.

Arguments:
  s          header name, including the trailing colon
  cond       value to return if the header contains any of the strings
               or aliases
  aliases    points to a chain of alias blocks
  count      number of strings
  ...        the strings

Returns:     cond if the header exists and contains one of the aliases or
               strings; otherwise !cond
*/

static BOOL
test_header(char *s, BOOL cond, alias *aliases, int count, ...)
{
header_line *h;
int slen = (int)strlen(s);

for (h = header_list; h != NULL; h = h->next)
  {
  int i;
  alias *a;
  va_list ap;

  if (h->type == htype_old || slen > h->slen ||
    strncmpic(s, h->text, slen) != 0)
      continue;

  for (a = aliases; a != NULL; a = a->next)
    if (strstric(h->text + slen, a->name, FALSE) != NULL) return cond;

  va_start(ap, count);
  for (i = 0; i < count; i++)
    {
    char *p = va_arg(ap, char *);
    if (strstric(h->text + slen, p, FALSE) != NULL) return cond;
    }
  va_end(ap);
  }

return !cond;
}




/*************************************************
*             Test a condition                   *
*************************************************/

/*
Arguments:
  c              points to the condition block; c->testfor indicated whether
                   it's a positive or negative condition
  delivered      points to the "delivered" state of the filtering
  toplevel       TRUE if called from "if" directly; FALSE otherwise

Returns:         TRUE if the condition is met
*/

static BOOL
test_condition(condition_block *c, BOOL *delivered, BOOL toplevel)
{
BOOL yield;
pcre *re;
char *exp[2], *p, *pp;
const char *regcomp_error = NULL;
int regcomp_error_offset;
int val[2];
int i;

if (c == NULL) return TRUE;  /* does this ever occur? */

switch (c->type)
  {
  case cond_and:
  yield = test_condition(c->left, delivered, FALSE) &&
          *error_pointer == NULL &&
          test_condition(c->right, delivered, FALSE);
  break;

  case cond_or:
  yield = test_condition(c->left, delivered, FALSE) ||
          (*error_pointer == NULL &&
          test_condition(c->right, delivered, FALSE));
  break;

  /* The personal test is meaningless in a system filter */

  case cond_personal:
  if (system_filtering) yield = FALSE; else
    {
    alias *aliases = (alias *)(c->left);
    char *self, *self_from, *self_to;
    char *psself = NULL;
    char *psself_from = NULL, *psself_to = NULL;
    int to_count = 2;
    int from_count = 5;

    self = string_sprintf("%s@%s", deliver_localpart, deliver_domain);
    self_from = rewrite_one(self, rewrite_from, NULL, FALSE, "",
      global_rewrite_rules);
    self_to   = rewrite_one(self, rewrite_to, NULL, FALSE, "",
      global_rewrite_rules);
    if (self_from == NULL) self_from = self;
    if (self_to == NULL) self_to = self;

    /* If there's a prefix or suffix set, we must include the prefixed/
    suffixed version of the local part in the tests. */

    if (deliver_localpart_prefix != NULL || deliver_localpart_suffix != NULL)
      {
      psself = string_sprintf("%s%s%s@%s",
        (deliver_localpart_prefix == NULL)? "" : deliver_localpart_prefix,
        deliver_localpart,
        (deliver_localpart_suffix == NULL)? "" : deliver_localpart_suffix,
        deliver_domain);
      psself_from = rewrite_one(psself, rewrite_from, NULL, FALSE, "",
        global_rewrite_rules);
      psself_to   = rewrite_one(psself, rewrite_to, NULL, FALSE, "",
        global_rewrite_rules);
      if (psself_from == NULL) self_from = psself;
      if (psself_to == NULL) self_to = psself;
      to_count += 2;
      from_count += 2;
      }

    /* Do all the necessary tests; the counts are adjusted for {pre,suf}fix */

    yield = test_header("to:", TRUE, aliases, to_count, self, self_to,
              psself, psself_to) &&
            test_header("from:", FALSE, aliases, from_count, "server@",
              "daemon@", "root@", self, self_from, psself, psself_from) &&
            test_header("subject:", FALSE, NULL, 1, "circular") &&
            test_header("precedence:", FALSE, NULL, 1, "bulk") &&
            test_header("precedence:", FALSE, NULL, 1, "list") &&
            test_header("precedence:", FALSE, NULL, 1, "junk") &&
            (sender_address == NULL || sender_address[0] != 0);
    }
  break;

  case cond_delivered:
  yield = *delivered;
  break;

  /* Only TRUE if a message is actually being processed; FALSE for address
  testing and verification. */

  case cond_errormsg:
  yield = message_id[0] != 0 &&
    (sender_address == NULL || sender_address[0] == 0);
  break;

  /* Only FALSE if a message is actually being processed; TRUE for address
  and filter testing and verification. */

  case cond_firsttime:
  yield = filter_test != NULL || message_id[0] == 0 || deliver_firsttime;
  break;

  /* Only TRUE if a message is actually being processed; FALSE for address
  testing and verification. */

  case cond_manualthaw:
  yield = message_id[0] != 0 && deliver_manual_thaw;
  break;

  /* The foranyaddress condition loops through a list of addresses */

  case cond_foranyaddress:
  p = (char *)(c->left);
  pp = expand_string(p);
  if (pp == NULL)
    {
    *error_pointer = string_sprintf("failed to expand \"%s\" in "
      "filter file: %s", p, expand_string_message);
    return FALSE;
    }

  yield = FALSE;
  while (*pp != 0)
    {
    char *error;
    int start, end, domain;
    int saveend;

    p = parse_find_address_end(pp, FALSE);
    saveend = *p;

    *p = 0;
    filter_thisaddress =
      parse_extract_address(pp, &error, &start, &end, &domain, FALSE);
    *p = saveend;

    if (filter_thisaddress != NULL)
      {
      if ((filter_test != NULL && debug_level > 0) || debug_level >= 10)
        {
        indent();
        debug_printf("Extracted address %s\n", filter_thisaddress);
        }
      yield = test_condition(c->right, delivered, FALSE);
      }

    if (yield) break;
    if (saveend == 0) break;
    pp = p + 1;
    }
  break;

  /* All other conditions have left and right values that need expanding;
  on error, it doesn't matter what value is returned. */

  default:
  p = (char *)(c->left);
  for (i = 0; i < 2; i++)
    {
    exp[i] = expand_string(p);
    if (exp[i] == NULL)
      {
      *error_pointer = string_sprintf("failed to expand \"%s\" in "
        "filter file: %s", p, expand_string_message);
      return FALSE;
      }
    p = (char *)(c->right);
    }

  /* Inner switch for the different cases */

  switch(c->type)
    {
    case cond_is:
    yield = strcmpic(exp[0], exp[1]) == 0;
    break;

    case cond_IS:
    yield = strcmp(exp[0], exp[1]) == 0;
    break;

    case cond_contains:
    yield = strstric(exp[0], exp[1], FALSE) != NULL;
    break;

    case cond_CONTAINS:
    yield = strstr(exp[0], exp[1]) != NULL;
    break;

    case cond_begins:
    yield = strncmpic(exp[0], exp[1], (int)strlen(exp[1])) == 0;
    break;

    case cond_BEGINS:
    yield = strncmp(exp[0], exp[1], (int)strlen(exp[1])) == 0;
    break;

    case cond_ends:
    case cond_ENDS:
      {
      int len = (int)strlen(exp[1]);
      char *s = exp[0] + (int)strlen(exp[0]) - len;
      yield = (s < exp[0])? FALSE :
        ((c->type == cond_ends)? strcmpic(s, exp[1]) : strcmp(s, exp[1])) == 0;
      }
    break;

    case cond_matches:
    case cond_MATCHES:
    if ((filter_test != NULL && debug_level > 0) || debug_level >= 10)
      {
      debug_printf("Match expanded arguments:\n");
      debug_printf("  Subject = %s\n", exp[0]);
      debug_printf("  Pattern = %s\n", exp[1]);
      }

    re = pcre_compile(exp[1],
      PCRE_COPT | ((c->type == cond_matches)? PCRE_CASELESS : 0),
      &regcomp_error, &regcomp_error_offset, NULL);

    if (re == NULL)
      {
      *error_pointer = string_sprintf("error while compiling "
        "regular expression \"%s\": %s at offset %d",
        exp[1], regcomp_error, regcomp_error_offset);
      return FALSE;
      }

    yield = regex_match_and_setup(re, exp[0], PCRE_EOPT, -1);
    break;

    /* For above and below, convert the strings to numbers */

    case cond_above:
    case cond_below:
    for (i = 0; i < 2; i++)
      {
      val[i] = get_number(exp[i], &yield);
      if (!yield)
        {
        *error_pointer = string_sprintf("malformed numerical string \"%s\"",
          exp[i]);
        return FALSE;
        }
      }
    yield = (c->type == cond_above)? (val[0] > val[1]) : (val[0] < val[1]);
    break;
    }
  break;
  }

if ((filter_test != NULL && debug_level > 0) || debug_level >= 10)
  {
  indent();
  debug_printf("%sondition is %s: ",
    toplevel? "C" : "Sub-c",
    (yield == c->testfor)? "true" : "false");
  print_condition(c, TRUE);
  debug_printf("\n");
  }

return yield == c->testfor;
}



/*************************************************
*          Interpret chain of commands           *
*************************************************/

/* In testing state, just say what would be done rather than doing it. The
testprint command just expands and outputs its argument in testing state, and
does nothing otherwise.

Arguments:
  commands    points to chain of commands to interpret
  generated   where to hang newly-generated addresses
  delivered   points to the "delivered" state of the filtering
  rewrite     TRUE if "deliver" addresses are to be rewritten
  log_access  control value for writing to a log file

Returns:      FALSE if there was an error, or if "finish", "freeze" or
                "fail" was obeyed
*/

static BOOL
interpret_commands(filter_cmd *commands, address_item **generated,
  BOOL *delivered, BOOL rewrite, int log_access)
{
char *s;
int mode;
int pool_reset = store_pool;
address_item *addr;
BOOL condition_value;

while (commands != NULL)
  {
  char *expargs[mailargs_string_count];

  int i, n[2];

  /* Expand the relevant number of arguments for the command that are
  not NULL. */

  for (i = 0; i < command_exparg_count[commands->command]; i++)
    {
    char *ss = (char *)(commands->args[i]);
    if (ss == NULL)
      {
      expargs[i] = NULL;
      }
    else
      {
      expargs[i] = expand_string(ss);
      if (expargs[i] == NULL)
        {
        *error_pointer = string_sprintf("failed to expand \"%s\" in "
          "%s command: %s", ss, command_list[commands->command],
          expand_string_message);
        return FALSE;
        }
      }
    }

  /* Now switch for each command */

  switch(commands->command)
    {
    case add_command:
    for (i = 0; i < 2; i++)
      {
      char *ss = expargs[i];
      char *end;

      if (i == 1 && (*ss++ != 'n' || ss[1] != 0))
        {
        *error_pointer = string_sprintf("unknown variable \"%s\" in \"add\" "
          "command", expargs[i]);
        return FALSE;
        }

      /* Allow for "--" at the start of the value (from -$n0) for example */
      if (i == 0) while (ss[0] == '-' && ss[1] == '-') ss += 2;

      n[i] = (int)strtol(ss, &end, 0);
      if (*end != 0)
        {
        *error_pointer = string_sprintf("malformed number \"%s\" in \"add\" "
          "command", ss);
        return FALSE;
        }
      }

    filter_n[n[1]] += n[0];
    if (filter_test != NULL) printf("Add %d to n%d\n", n[0], n[1]);
    break;

    case deliver_command:
    if (commands->seen) *delivered = TRUE;

    /* A deliver command's argument must be a valid address. Its optional
    second argument (system filter only) must also be a valid address. */

    for (i = 0; i < 2; i++)
      {
      s = expargs[i];
      if (s != NULL)
        {
        int start, end, domain;
        char *error;
        char *ss = parse_extract_address(s, &error, &start, &end, &domain,
          FALSE);
        if (ss != NULL)
          expargs[i] = rewrite?
            rewrite_address(ss, TRUE, FALSE, global_rewrite_rules,
              rewrite_existflags) :
            rewrite_address_qualify(ss, TRUE);
        else
          {
          *error_pointer = string_sprintf("malformed address \"%s\" in "
            "filter file: %s", s, error);
          return FALSE;
          }
        }
      }

    /* Stick the errors address into a simple variable, as it will
    be referenced a few times. Check that the caller is permitted to
    specify it. */

    s = expargs[1];

    if (s != NULL && !system_filtering)
      {
      char *ownaddress = expand_string("$local_part@$domain");
      if (strcmpic(ownaddress, s) != 0)
        {
        *error_pointer = "errors_to must point to the caller's address";
        return FALSE;
        }
      }

    /* Test case: report what would happen */

    if (filter_test != NULL)
      {
      indent();
      printf("%seliver message to: %s%s%s%s\n",
        (commands->seen)? "D" : "Unseen d",
        expargs[0],
        commands->noerror? " (noerror)" : "",
        (s != NULL)? " errors_to " : "",
        (s != NULL)? s : "");
      }

    /* Real case: cause deliver argument to be rewritten. */

    else
      {
      DEBUG(10) debug_printf("Filter: %sdeliver message to: %s%s%s%s\n",
        (commands->seen)? "" : "unseen ",
        expargs[0],
        commands->noerror? " (noerror)" : "",
        (s != NULL)? " errors_to " : "",
        (s != NULL)? s : "");

      /* Create the new address and add it to the chain, setting the
      af_ignore_error flag if necessary, and the errors address, which can be
      set in a system filter and to the local address in user filters. Ensure
      that the addresses are placed in the MAIN storage pool. */

      store_pool = POOL_MAIN;
      addr = deliver_make_addr(expargs[0], TRUE);  /* TRUE => copy s */
      addr->errors_address = (s == NULL)?
        s : string_copy(s);                        /* Default is NULL */
      if (commands->noerror) setflag(addr, af_ignore_error);
      addr->next = *generated;
      *generated = addr;
      store_pool = pool_reset;
      }
    break;

    case save_command:
    if (commands->seen) *delivered = TRUE;
    s = expargs[0];
    mode = (int)(commands->args[1]);

    /* Test case: report what would happen */

    if (filter_test != NULL)
      {
      indent();
      if (mode < 0)
        printf("%save message to: %s%s\n", (commands->seen)?
          "S" : "Unseen s", s, commands->noerror? " (noerror)" : "");
      else
        printf("%save message to: %s %04o%s\n", (commands->seen)?
          "S" : "Unseen s", s, mode, commands->noerror? " (noerror)" : "");
      }

    /* Real case: Ensure save command starts with /. */

    else
      {
      DEBUG(10) debug_printf("Filter: %ssave message to: %s%s\n",
        (commands->seen)? "" : "unseen ", s,
        commands->noerror? " (noerror)" : "");
      if (s[0] != '/') s = string_sprintf("%s/%s", deliver_home, s);

      /* Create the new address and add it to the chain, setting the
      af_pfr flag, the af_ignore_error flag if necessary, and the mode value.
      Ensure that the address is placed in the MAIN storage pool. */

      store_pool = POOL_MAIN;
      addr = deliver_make_addr(s, TRUE);  /* TRUE => copy s */
      setflag(addr, af_pfr);
      if (commands->noerror) setflag(addr, af_ignore_error);
      addr->mode = mode;
      addr->next = *generated;
      *generated = addr;
      store_pool = pool_reset;
      }
    break;

    case pipe_command:
    if (commands->seen) *delivered = TRUE;
    s = string_copy((char *)(commands->args[0]));
    if (filter_test != NULL)
      {
      indent();
      printf("%sipe message to: %s%s\n", (commands->seen)?
        "P" : "Unseen p", s, commands->noerror? " (noerror)" : "");
      }
    else /* Ensure pipe command starts with | */
      {
      DEBUG(10) debug_printf("Filter: %spipe message to: %s%s\n",
        (commands->seen)? "" : "unseen ", s,
        commands->noerror? " (noerror)" : "");
      if (s[0] != '|') s = string_sprintf("|%s", s);

      /* Create the new address and add it to the chain, setting the
      af_ignore_error flag if necessary. Set the af_expand_pipe flag so that
      each command argument is expanded in the transport after the command
      has been split up into separate arguments. Ensure the address is
      placed in the MAIN storage pool */

      store_pool = POOL_MAIN;
      addr = deliver_make_addr(s, TRUE);  /* TRUE => copy s */
      setflag(addr, af_pfr|af_expand_pipe);
      if (commands->noerror) setflag(addr, af_ignore_error);
      addr->next = *generated;
      *generated = addr;

      /* If there are any numeric variables in existence (e.g. after a regex
      condition), take a copy of them for use in the expansion. */

      if (expand_nmax >= 0)
        {
        int i;
        char **ss = store_get(sizeof(char *) * (expand_nmax + 2));
        addr->pipe_expandn = ss;
        for (i = 0; i <= expand_nmax; i++)
          *ss++ = string_copyn(expand_nstring[i], expand_nlength[i]);
        *ss = NULL;
        }

      store_pool = pool_reset;
      }
    break;

    /* Set up the file name and mode, and close any previously open
    file. */

    case logfile_command:
    if (commands->seen) *delivered = TRUE;
    log_mode = (int)(commands->args[1]);
    if (log_mode == -1) log_mode = 0600;
    if (log_fd >= 0)
      {
      close(log_fd);
      log_fd = -1;
      }
    log_filename = expargs[0];
    if (filter_test != NULL)
      {
      indent();
      printf("%sogfile %s\n", (commands->seen)? "Seen l" : "L", log_filename);
      }
    break;

    case logwrite_command:
    if (commands->seen) *delivered = TRUE;
    s = expargs[0];

    if (filter_test != NULL)
      {
      indent();
      printf("%sogwrite \"%s\"\n", (commands->seen)? "Seen l" : "L",
        string_printing(s));
      }

    /* Attempt to write to a log file only if configured as permissible and
    running as a user, i.e. if seteuid has been used to give up a privileged
    uid. This will be the case for normal user filter files unless the security
    option set to 2 and seteuid is not set for the forwardfile director.

    The log_access values are:
      0     OK
      1     skip (verifying)
      2     locked out by configuration
      3     not running as user
    */

    else if (log_access >= 2)   /* Locked out */
      {
      DEBUG(10)
        debug_printf("filter log command aborted: euid=%ld log_access=%d\n",
        (long int)geteuid(), log_access);
      *error_pointer = (log_access == 2)? "logwrite command forbidden" :
        "logwrite command not permitted when filter not running as user";
      return FALSE;
      }
    else if (log_access == 0)   /* Real delivery */
      {
      int len;
      DEBUG(10) debug_printf("writing filter log as euid %ld\n",
        (long int)geteuid());
      if (log_fd < 0)
        {
        if (log_filename == NULL)
          {
          *error_pointer = "attempt to obey \"logwrite\" command "
            "without a previous \"logfile\"";
          return FALSE;
          }
        log_fd = open(log_filename, O_CREAT|O_APPEND|O_WRONLY, log_mode);
        if (log_fd < 0)
          {
          *error_pointer = string_open_failed(errno, "filter log file \"%s\"",
            log_filename);
          return FALSE;
          }
        }
      len = (int)strlen(s);
      if (write(log_fd, s, len) != len)
        {
        *error_pointer = string_sprintf("write error on file \"%s\": %s",
          log_filename, strerror(errno));
        return FALSE;
        }
      }
    else
      {
      DEBUG(10) debug_printf("skipping logwrite (verifying or testing)\n");
      }
    break;

    /* Headers is available only in the system filter. The command is
    rejected at parse time otherwise. */

    case headers_command:
      {
      BOOL adding = (BOOL)(commands->args[1]);
      s = expargs[0];

      if (filter_test != NULL)
        printf("Headers %s \"%s\"\n", adding? "add" : "remove",
          string_printing(s));

      else if (adding)
        {
        while (isspace((uschar)*s)) s++;
        if (s[0] != 0)
          {
          header_add(htype_other, "%s%s", s, (s[(int)strlen(s)-1] == '\n')?
            "" : "\n");
          }
        }

      else
        {
        header_line *h;
        for (h = header_list; h != NULL; h = h->next)
          {
          int sep = 0;
          char *ss, *tt;
          char *list = s;
          char buffer[128];
          if (h->type == htype_old) continue;
          while ((ss = string_nextinlist(&list, &sep, buffer, sizeof(buffer)))
                 != NULL)
            {
            int len = (int)strlen(ss);
            if (strncmpic(h->text, ss, len) != 0) continue;
            tt = h->text + len;
            while (*tt == ' ' || *tt == '\t') tt++;
            if (*tt == ':') break;
            }
          if (ss == NULL) continue;
          h->type = htype_old;
          }
        }
      }
    break;

    /* Freeze and fail are available only when there is somewhere to pass
    back the action. These commands are rejected at parse time otherwise.
    Ensure the message length is not too ridiculous (can become so from, e.g.
    the inclusion of message header lines). And force printing characters. */

    case fail_command:
    case freeze_command:
    filter_fmsg = expargs[0];
    if (filter_fmsg == NULL)
      {
      if (filter_test != NULL) filter_fmsg = "";
      }
    else
      {
      if (strlen(filter_fmsg) > 1024)
        strcpy(filter_fmsg + 1000, " ... (truncated)");
      filter_fmsg = string_printing(filter_fmsg);
      }

    if (commands->command == fail_command)
      {
      *special_action = SPECIAL_FAIL;
      if (filter_test != NULL)
        {
        indent();
        printf("Fail text \"%s\"\n", filter_fmsg);
        }
      else DEBUG(10) debug_printf("Filter: fail \"%s\"\n", filter_fmsg);
      }
    else
      {
      *special_action = SPECIAL_FREEZE;
      if (filter_test != NULL)
        {
        indent();
        printf("Freeze text \"%s\"\n", filter_fmsg);
        }
      else DEBUG(10) debug_printf("Filter: freeze \"%s\"\n", filter_fmsg);
      }
    /* Do NOT set "delivered" for freeze or fail. */
    return FALSE;        /* (This is needed for -bF testing) */

    case finish_command:
    if (commands->seen) *delivered = TRUE;
    if (filter_test != NULL)
      {
      indent();
      printf("%sinish\n", (commands->seen)? "Seen f" : "F");
      }
    else
      DEBUG(10) debug_printf("Filter: %sfinish\n",
        (commands->seen)? " Seen " : "");
    return FALSE;

    case if_command:
      {
      char *save_address = filter_thisaddress;
      BOOL ok = TRUE;
      condition_value =
        test_condition((condition_block *)(commands->args[0]), delivered, TRUE);
      if (*error_pointer != NULL) ok = FALSE; else
        {
        output_indent += 2;
        ok = interpret_commands((filter_cmd *)
          (commands->args[condition_value? 1:2]),
            generated, delivered, rewrite, log_access);
        output_indent -= 2;
        }
      filter_thisaddress = save_address;
      if (!ok) return FALSE;
      }
    break;


    /* To try to catch runaway loops, do not generate mail if the
    return path is unset or if a non-trusted user supplied -f <>
    as the return path. */

    case mail_command:
    case vacation_command:
    if (return_path == NULL || return_path[0] == 0)
      {
      if (filter_test != NULL)
        printf("%s command ignored because return_path is empty\n",
          command_list[commands->command]);
      else DEBUG(10) debug_printf("%s command ignored because return_path "
        "is empty\n", command_list[commands->command]);
      break;
      }

    /* Check the contents of the strings. The type of string can be deduced
    from the value of i. If it is > mailarg_index_text, we are dealing with a
    file name, which cannot contain non-printing characters. If it is less
    than mailarg_index_text we are dealing with something that will go in a
    message header line, where newlines must be followed by white space. If
    i is equal to mailarg_index_text its a text string for the body, where
    anything goes. */

    for (i = 0; i < mailargs_string_count; i++)
      {
      char *p;
      char *s = expargs[i];

      if (s == NULL) continue;

      for (p = s; *p != 0; p++)
        {
        int c = (uschar)*p;
        if (i > mailarg_index_text && !mac_isprint(c))
          {
          *error_pointer = string_sprintf("non-printing character in \"%s\" "
            "in %s command", string_printing(s),
            command_list[commands->command]);
          return FALSE;
          }
        else if (i < mailarg_index_text && c == '\n' && !isspace((uschar)p[1]))
          {
          *error_pointer = string_sprintf("\\n not followed by space in "
            "\"%.1024s\" in %s command", string_printing(s),
            command_list[commands->command]);
          return FALSE;
          }
        }

      /* The string is OK */

      commands->args[i] = s;
      }

    /* Proceed with mail or vacation command */

    if (commands->seen) *delivered = TRUE;
    if (filter_test != NULL)
      {
      char *to = (char *)(commands->args[mailarg_index_to]);
      indent();
      printf("%sail to: %s%s%s\n", (commands->seen)? "Seen m" : "M",
        (to == NULL)? "<default>" : to,
        (commands->command == vacation_command)? " (vacation)" : "",
        (commands->noerror)? " (noerror)" : "");
      for (i = 1; i < mailargs_string_count; i++)
        {
        char *arg = (char *)(commands->args[i]);
        if (arg != NULL)
          {
          int len = (int)strlen(mailargs[i]);
          int indent = (debug_level > 0)? output_indent : 0;
          while (len++ < 7 + indent) printf(" ");
          printf("%s: %s%s\n", mailargs[i], string_printing(arg),
            (commands->args[mailarg_index_expand] != NULL &&
              strcmp(mailargs[i], "file") == 0)? " (expanded)" : "");
          }
        }
      if (commands->args[mailarg_index_return] != NULL)
        printf("Return original message\n");
      }
    else
      {
      char *to = (char *)(commands->args[mailarg_index_to]);
      if (to == NULL) to = expand_string("$reply_address");
      while (isspace((uschar)*to)) to++;

      DEBUG(10)
        {
        debug_printf("Filter: %smail to: %s%s%s\n",
          (commands->seen)? "seen " : "",
          to,
          (commands->command == vacation_command)? " (vacation)" : "",
          (commands->noerror)? " (noerror)" : "");
        for (i = 1; i < mailargs_string_count; i++)
          {
          char *arg = (char *)(commands->args[i]);
          if (arg != NULL)
            {
            int len = (int)strlen(mailargs[i]);
            while (len++ < 15) debug_printf(" ");
            debug_printf("%s: %s%s\n", mailargs[i], string_printing(arg),
              (commands->args[mailarg_index_expand] != NULL &&
                strcmp(mailargs[i], "file") == 0)? " (expanded)" : "");
            }
          }
        }

      /* Ensure the address is created in the MAIN storage pool. */

      store_pool = POOL_MAIN;
      addr = deliver_make_addr(string_sprintf(">%s", to), FALSE);
      setflag(addr, af_pfr);
      if (commands->noerror) setflag(addr, af_ignore_error);
      addr->next = *generated;
      *generated = addr;
      addr->reply = store_get(sizeof(reply_item));
      addr->reply->headers = NULL;    /* Can't set that from here (yet?) */
      addr->reply->from = NULL;
      addr->reply->to = string_copy(to);
      addr->reply->file_expand =
        commands->args[mailarg_index_expand] != NULL;
      addr->reply->forbid_exists = expand_forbid_exists;
      addr->reply->forbid_lookup = expand_forbid_lookup;
      addr->reply->forbid_perl = expand_forbid_perl;
      addr->reply->return_message =
        commands->args[mailarg_index_return] != NULL;

      if (commands->args[mailarg_index_once_repeat] != NULL)
        {
        addr->reply->once_repeat =
          readconf_readtime(commands->args[mailarg_index_once_repeat], 0);
        if (addr->reply->once_repeat < 0)
          {
          *error_pointer = string_sprintf("Bad time value for \"once_repeat\" "
            "in mail or vacation command: %s",
            commands->args[mailarg_index_once_repeat]);
          return FALSE;
          }
        }

      for (i = 1; i < mailargs_string_passed; i++)
        {
        char *ss = (char *)(commands->args[i]);
        *((char **)(((char *)(addr->reply)) + reply_offsets[i])) =
          (ss == NULL)? NULL : string_copy(ss);
        }
      store_pool = pool_reset;
      }
    break;

    case testprint_command:
    if (filter_test != NULL || debug_level >= 10)
      {
      char *s = string_printing(expargs[0]);
      if (filter_test == NULL)
        debug_printf("Filter: testprint: %s\n", s);
      else
        printf("Testprint: %s\n", s);
      }
    }

  commands = commands->next;
  }

return TRUE;
}



/*************************************************
*            Interpret a mail filter file        *
*************************************************/

/*
Arguments:
  filter      points to the entire file, read into store as a single string
  generated   where to hang newly-generated addresses
  delivered   points to the "delivered" state of the filtering
  action      if non-null, allow special actions and return here
  error       where to pass back an error text
  log_access  0 => OK to obey the "logwrite" command
              1 => just skip it (verifying or testing)
              2 => locked out by configuration
              3 => not running as user
  rewrite     if TRUE, newly generated addresses are rewritten

Returns:      TRUE on success
*/

BOOL
filter_interpret(char *filter, address_item **generated, BOOL *delivered,
  int *action, char **error, int log_access, BOOL rewrite)
{
int i;
char *ptr = filter;
filter_cmd *commands = NULL;
filter_cmd **lastcmdptr = &commands;

DEBUG(9) debug_printf("Filter: start of processing\n");

/* Initialize "not delivered" and outside an if command, set the
global flag that is always TRUE while filtering, and zero the variables. */

*delivered = FALSE;
expect_endif = 0;
output_indent = 0;
filter_running = TRUE;
filter_fmsg = NULL;
for (i = 0; i < FILTER_VARIABLE_COUNT; i++) filter_n[i] = 0;

/* To save having to pass this about all the time, make it static, and
initialize to no error. Also initialize the line number, for error messages and
the log file variables. */

if (action != NULL) *action = SPECIAL_NONE;
special_action = action;
error_pointer = error;
*error_pointer = NULL;
line_number = 1;
log_fd = -1;
log_mode = 0600;
log_filename = NULL;

/* Scan filter file for syntax and build up an interpretation thereof,
and interpret the compiled commands, and if testing, say whether we ended
up delivered or not, unless something went wrong. */

seen_force = FALSE;
ptr = nextsigchar(ptr, TRUE);
if (read_command_list(&ptr, &lastcmdptr, FALSE))
  {
  interpret_commands(commands, generated, delivered, rewrite, log_access);
  if (*error_pointer == NULL)
    {
    if (filter_test != NULL)
      {
      printf("Filter processing ended:\n");
      if (action != NULL && *action != SPECIAL_NONE)
        {
        printf("  Filtering ended with the \"%s\" command.\n",
          (*action == SPECIAL_FREEZE)? "freeze" : "fail");
        }
      else if (*delivered)
        printf("  Filtering set up at least one significant delivery "
               "or other action.\n"
               "  No other deliveries will occur.\n");
      else
        printf("  Filtering did not set up a significant delivery.\n"
               "  Normal delivery will occur.\n");
      }
    else DEBUG(10)
      debug_printf("Filter: at end \"delivered\" is %s\n",
        (*delivered)? "true" : "false");
    }
  }

/* Close the log file if it was opened, and kill off any numerical variables
before returning. */

if (log_fd >= 0) close(log_fd);
expand_nmax = -1;
filter_running = FALSE;

DEBUG(9) debug_printf("Filter: end of processing\n");
return *error_pointer == NULL;
}





/*************************************************
*      Interpret a system-wide filter            *
*************************************************/

/* This is called from deliver_message before the real process of delivery
begins. It is intended as a spam and mail-bomb filtering defence. It runs the
filtering code on message_filter, using message_filter_uid/gid.

Arguments:
  generated     where to hang generated addresses
  delivered     set TRUE if significant delivery happens
  action        if non-null, allow special actions and return here
  error         place to point error message

Returns:        TRUE if filtering succeeds
*/

BOOL
filter_system_interpret(address_item **generated, BOOL *delivered,
  int *action, char **error)
{
BOOL yield;
FILE *filter;
char *filebuf;
size_t size_read;
uid_t saved_euid = (uid_t)(-1);   /* keep picky compiler happy */
gid_t saved_egid = (gid_t)(-1);   /* (value is never used) */
struct stat statbuf;

/* Arrange to run under the correct uid/gid. If we are not root at this point,
we can become root by seteuid(), because that is the state in
deliver_message(). */

if (message_filter_uid_set)
  {
  saved_euid = geteuid();
  saved_egid = getegid();
  if (saved_euid != root_uid) mac_seteuid(root_uid);
  mac_setegid(message_filter_gid);
  mac_seteuid(message_filter_uid);
  }

DEBUG(9) debug_print_ids("running system filter as");

/* Now open the filter file and read it into memory. */

filter = fopen(message_filter, "r");
if (filter == NULL)
  {
  *error = string_open_failed(errno, "\"%s\"", message_filter);
  yield = FALSE;
  goto RESTORE_UID;
  }

if (fstat(fileno(filter), &statbuf) != 0)
  {
  *error = string_sprintf("failed to stat \"%s\"", message_filter);
  yield = FALSE;
  fclose(filter);
  goto RESTORE_UID;
  }

filebuf = store_get(statbuf.st_size + 1);
size_read = fread(filebuf, 1, statbuf.st_size, filter);
fclose(filter);

if (size_read != statbuf.st_size)
  {
  *error = string_sprintf("error while reading \"%s\": %s",
    message_filter, strerror(errno));
  yield = FALSE;
  goto RESTORE_UID;
  }
filebuf[size_read] = 0;

/*Now we can call the standard filter interpreter, flagging that it's the
system filter. */

system_filtering = TRUE;
yield = filter_interpret(filebuf, generated, delivered, action, error,
  0, TRUE);
system_filtering = FALSE;

/* Restore the original effective uid/gid before returning. */

RESTORE_UID:

if (message_filter_uid_set)
  {
  mac_seteuid(root_uid);
  mac_setegid(saved_egid);
  mac_seteuid(saved_euid);
  DEBUG(9) debug_print_ids("after system filter restored");
  }

return yield;
}




/*************************************************
*            Test a mail filter                  *
*************************************************/

/* This is called when exim is run with the -bf option. The name
of the filter file is in filter_test, and we are running under an
unprivileged uid/gid. A test message's headers have been read into
store, and the body of the message is still accessible on the
standard input.

Argument:
  fd          the standard input fd, containing the message body
  is_system   TRUE if testing is to be as a system filter
  dot_ended   TRUE if message already terminated by '.'

Returns:      TRUE if no errors
*/

BOOL
filter_runtest(int fd, BOOL is_system, BOOL dot_ended)
{
int rc, body_len, action, header_size;
register int ch;
BOOL yield, delivered;
struct stat statbuf;
address_item *generated = NULL;
char *body, *error, *filebuf, *s;

/* Read the filter file into store as will be done by the director
in a real case. */

if (fstat(fd, &statbuf) != 0)
  {
  printf("exim: failed to get size of %s: %s\n", filter_test, strerror(errno));
  return FALSE;
  }

filebuf = store_get(statbuf.st_size + 1);
rc = read(fd, filebuf, statbuf.st_size);
close(fd);

if (rc != statbuf.st_size)
  {
  printf("exim: error while reading %s: %s\n", filter_test, strerror(errno));
  return FALSE;
  }

filebuf[statbuf.st_size] = 0;

/* Check that a user filter file starts with # Exim filter, as the director
does. If it does not, treat it as an ordinary .forward file and check that.
System filters do not do this check - they are always filters. */

if (!is_system)
  {
  char *tag = "# exim filter";
  s = filebuf;
  while (isspace((uschar)*s)) s++;      /* Skips initial blank lines */
  for (; *tag != 0; s++, tag++)
    {
    if (*tag == ' ')
      {
      while (*s == ' ' || *s == '\t') s++;
      s--;
      }
    else if (tolower(*s) != tolower(*tag))
      {
      yield = parse_extract_addresses(filebuf,
        deliver_domain,                 /* incoming domain for \name */
        &generated,                     /* for generated addresses */
        &error,                         /* for errors */
        TRUE,                           /* no addresses => no error */
        FALSE,                          /* don't recognize :blackhole: */
        TRUE,                           /* do rewrite */
        NULL,                           /* no check on includes */
        NULL);                          /* fail on syntax errors */

      if (yield != OK)
        {
        printf("exim: error in forward file: %s\n", error);
        return FALSE;
        }

      if (generated == NULL)
        printf("exim: no addresses generated from forward file\n");

      else
        {
        printf("exim: forward file generated:\n");
        while (generated != NULL)
          {
          printf("  %s\n", generated->orig);
          generated = generated->next;
          }
        }

      return TRUE;
      }
    }
  }

/* We have to read the remainder of the message in order to find its size, so
we might as well set up the message_body variable at the same time (when
*really* filtering this is not read unless needed). The reading code is written
out here rather than having options in read_message_data, in order to keep that
function as efficient as possible. Handling message_body_end is somewhat more
tedious. Pile it all into a circular buffer and sort out at the end. */

message_body = body = store_malloc(message_body_visible + 1);
message_body_end = store_malloc(message_body_visible + 1);
s = message_body_end;
body_len = 0;
body_linecount = 0;
header_size = message_size;

if (!dot_ended && !feof(stdin))
  {
  if (!dot_ends)
    {
    while ((ch = getc(stdin)) != EOF)
      {
      if (ch == '\n') body_linecount++;
      if (body_len < message_body_visible) message_body[body_len++] = ch;
      *s++ = ch;
      if (s > message_body_end + message_body_visible) s = message_body_end;
      message_size++;
      }
    }
  else
    {
    int ch_state = 1;
    while ((ch = getc(stdin)) != EOF)
      {
      switch (ch_state)
        {
        case 0:                         /* Normal state */
        if (ch == '\n') { body_linecount++; ch_state = 1; }
        break;

        case 1:                         /* After "\n" */
        if (ch == '.')
          {
          ch_state = 2;
          continue;
          }
        if (ch != '\n') ch_state = 0;
        break;

        case 2:                         /* After "\n." */
        if (ch == '\n') goto READ_END;
        if (body_len < message_body_visible) message_body[body_len++] = '.';
        *s++ = '.';
        if (s > message_body_end + message_body_visible)
          s = message_body_end;
        message_size++;
        ch_state = 0;
        break;
        }
      if (body_len < message_body_visible) message_body[body_len++] = ch;
      *s++ = ch;
      if (s > message_body_end + message_body_visible) s = message_body_end;
      message_size++;
      }
    READ_END: ch = ch;  /* Some compilers don't like null statements */
    }
  if (s == message_body_end || s[-1] != '\n') body_linecount++;
  }

message_body[body_len] = 0;
message_body_size = message_size - header_size;

/* body_len stops at message_body_visible; it if got there, we may have
wrapped round in message_body_end. */

if (body_len >= message_body_visible)
  {
  int below = s - message_body_end;
  int above = message_body_visible - below;
  if (above > 0)
    {
    char *temp = store_get(below);
    memcpy(temp, message_body_end, below);
    memmove(message_body_end, s+1, above);
    memcpy(message_body_end + above, temp, below);
    message_body_end[message_body_visible] = 0;
    }
  }
else *s = 0;

/* Convert newlines in the body variables to spaces */

while (*body != 0)
  {
  if (*body == '\n') *body = ' ';
  body++;
  }

body = message_body_end;
while (*body != 0)
  {
  if (*body == '\n') *body = ' ';
  body++;
  }

/* Now pass the filter file to the function that interprets it. Because
filter_test is not NULL, the interpreter will output comments about what
it is doing, but an error message will have to be output here. No need to
clean up store. The second-last argument is 0 because Exim has given up root
privilege when running a filter test, and in any case, as it is a test,
is isn't going to try writing any files. */

if (is_system)
  {
  system_filtering = TRUE;
  enable_dollar_recipients = TRUE; /* Permit $recipients in system filter */
  yield = filter_interpret(filebuf, &generated, &delivered, &action, &error,
    0, TRUE);
  enable_dollar_recipients = FALSE;
  system_filtering = FALSE;
  }
else yield = filter_interpret(filebuf, &generated, &delivered, NULL, &error,
    0, TRUE);

if (!yield) printf("Filter error: %s\n", error);
return yield;
}

/* End of filter.c */
