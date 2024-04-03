/*************************************************
*                  Exim Monitor                  *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "em_hdr.h"

/* This module contains code for handling the popup menus. */

static Widget menushell;
static Widget queue_text_sink;
static Widget dialog_shell, dialog_widget;

static Widget text_create(char *, int);

static int highlighted_start, highlighted_end, highlighted_x, highlighted_y;



static Arg queue_get_arg[] = {
  { "textSink",   (XtArgVal)NULL },
  { "textSource", (XtArgVal)NULL },
  { "string",     (XtArgVal)NULL } };

static Arg dialog_arg[] = {
  { "label",      (XtArgVal)"dialog" },
  { "value",      (XtArgVal)"value" } };

static Arg get_pos_args[] = {
  {"x",           (XtArgVal)NULL },
  {"y",           (XtArgVal)NULL } };

static Arg menushell_arg[] = {
  { "label",      (XtArgVal)NULL } };

static Arg button_arg[] = {
  { XtNfromVert, (XtArgVal) NULL },         /* must be first */
  { XtNlabel,    (XtArgVal) " Dismiss " },
  { "left",      XawChainLeft },
  { "right",     XawChainLeft },
  { "top",       XawChainBottom },
  { "bottom",    XawChainBottom } };

static Arg text_arg[] = {
  { XtNfromVert, (XtArgVal) NULL },         /* must be first */
  { "editType",  XawtextEdit },
  { "string",    (XtArgVal)"" },            /* dummy to get it going */
  { "scrollVertical", XawtextScrollAlways },
  { "wrap",      XawtextWrapWord },
  { "top",       XawChainTop },
  { "bottom",    XawChainBottom } };

static Arg item_1_arg[] = {
  { XtNfromVert,  (XtArgVal)NULL },         /* must be first */
  { "label",      (XtArgVal)" Message log" } };

static Arg item_2_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Headers" } };

static Arg item_3_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Body" } };

static Arg item_4_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Deliver message" } };

static Arg item_5_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Freeze message" } };

static Arg item_6_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Thaw message" } };

static Arg item_7_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Give up on msg" } };

static Arg item_8_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Remove message" } };

static Arg item_9_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)"----------------" } };

static Arg item_10_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Add recipient" } };

static Arg item_11_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Mark delivered" } };

static Arg item_12_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Mark all delivered" } };

static Arg item_13_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Edit sender" } };

static Arg item_14_arg[] = {
  { XtNfromVert,  (XtArgVal) NULL },        /* must be first */
  { "label",      (XtArgVal)" Edit body" } };



/*************************************************
*        Destroy the menu when popped down       *
*************************************************/

static void popdownAction(Widget w, XtPointer client_data, XtPointer call_data)
{
client_data = client_data;    /* Keep picky compilers happy */
call_data = call_data;
if (highlighted_x >= 0)
  XawTextSinkDisplayText(queue_text_sink,
    highlighted_x, highlighted_y,
    highlighted_start, highlighted_end, 0);
XtDestroyWidget(w);
menu_is_up = FALSE;
}



/*************************************************
*          Display the message log               *
*************************************************/

static void msglogAction(Widget w, XtPointer client_data, XtPointer call_data)
{
int i;
char buffer[256];
Widget text = text_create((char *)client_data, text_depth);
FILE *f = NULL;

w = w;      /* Keep picky compilers happy */
call_data = call_data;

/* End up with the split version, so message looks right when non-exist */

for (i = 0; i < (spool_is_split? 2:1); i++)
  {
  message_subdir[0] = (i != 0)? ((char *)client_data)[5] : 0;
  sprintf(buffer, "%s/msglog/%s/%s", spool_directory, message_subdir,
    (char *)client_data);
  f = fopen(buffer, "r");
  if (f != NULL) break;
  }

if (f == NULL)
  text_showf(text, "%s: %s\n", buffer, strerror(errno));
else
  {
  while (fgets(buffer, 256, f) != NULL) text_show(text, buffer);
  fclose(f);
  }
}



/*************************************************
*          Display the message body               *
*************************************************/

static void bodyAction(Widget w, XtPointer client_data, XtPointer call_data)
{
int i;
char buffer[256];
Widget text = text_create((char *)client_data, text_depth);
FILE *f = NULL;

w = w;      /* Keep picky compilers happy */
call_data = call_data;

for (i = 0; i < (spool_is_split? 2:1); i++)
  {
  message_subdir[0] = (i != 0)? ((char *)client_data)[5] : 0;
  sprintf(buffer, "%s/input/%s/%s-D", spool_directory, message_subdir,
    (char *)client_data);
  f = fopen(buffer, "r");
  if (f != NULL) break;
  }

if (f == NULL)
  text_showf(text, "Failed to open file: %s\n", strerror(errno));
else
  {
  int count = 0;
  while (fgets(buffer, 256, f) != NULL)
    {
    text_show(text, buffer);
    count += (int)strlen(buffer);
    if (count > body_max)
      {
      text_show(text, "\n*** Message length exceeds BODY_MAX ***\n");
      break;
      }
    }
  fclose(f);
  }
}



/*************************************************
*        Do something to a message               *
*************************************************/

/* The output is not shown in a window for non-delivery actions that succeed,
unless action_output is set. We can't, however, tell until we have run
the command whether we want the output or not, so the pipe has to be set up in
all cases. */

static void ActOnMessage(char *id, char *action, char *address_arg)
{
int pid;
int pipe_fd[2];
int delivery = strcmp(action + (int)strlen(action) - 2, "-M") == 0;
int editing = strcmp(action + (int)strlen(action) - 4, "-Meb") == 0;
char *quote = "";
char *at = "";
char *qualify = "";
char buffer[256];
queue_item *qq;
Widget text = NULL;


/* Editing is handled in a subprocess whose output is not messed about
in any way. Don't wait for it - the SIGCHLD handler will clear it away
when it is finished. */

if (editing)
  {
  if ((pid = fork()) == 0)
    {
    sprintf(buffer, "xterm -n %s -title %s -e %s %s %s %s %s", id, id,
      exim_path,
      (alternate_config == NULL)? "" : "-C",
      (alternate_config == NULL)? "" : alternate_config,
      action, id);
    system(buffer);
    _exit(0);
    }
  return;
  }


/* The command for everything else is the same; if the address arg is not empty
and does not contain @ and there is a qualify domain, qualify it. */

if (address_arg[0] != 0)
  {
  quote = "\'";
  if (strchr(address_arg, '@') == NULL && qualify_domain != NULL &&
      qualify_domain[0] != 0)
    {
    at = "@";
    qualify = qualify_domain;
    }
  }
sprintf(buffer, "%s %s %s %s %s %s%s%s%s%s", exim_path,
  (alternate_config == NULL)? "" : "-C",
  (alternate_config == NULL)? "" : alternate_config,
  action, id, quote, address_arg, at, qualify, quote);

/* If we know we are going to need the window, create it now. */

if (action_output || delivery)
  {
  text = text_create(id, text_depth);
  text_showf(text, "%s\n", buffer);
  }

/* Create the pipe for output. Remember, on most systems pipe[0] is
for reading and pipe[1] is for writing! Solaris, with its two-way
pipes is a trap! */

if (pipe(pipe_fd) != 0)
  {
  if (text == NULL)
    {
    text = text_create(id, text_depth);
    text_showf(text, "%s\n", buffer);
    }
  text_show(text, "*** Failed to create pipe ***\n");
  return;
  }

fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK);
fcntl(pipe_fd[1], F_SETFL, O_NONBLOCK);

/* Delivering a message can take some time, and we want to show the
output as it goes along. This requires subprocesses and is coded below. For
other commands, we can assume an immediate response, and so need not waste
resources with subprocesses. If action_output is FALSE, don't show the
output at all. */

if (!delivery)
  {
  int count, rc;
  int save_stdout = dup(1);
  int save_stderr = dup(2);

  close(1);
  close(2);

  dup2(pipe_fd[1], 1);
  dup2(pipe_fd[1], 2);
  close(pipe_fd[1]);

  rc = system(buffer);

  close(1);
  close(2);

  if (action_output || rc != 0)
    {
    if (text == NULL)
      {
      text = text_create(id, text_depth);
      text_showf(text, "%s\n", buffer);
      }
    while ((count = read(pipe_fd[0], buffer, 254)) > 0)
      {
      buffer[count] = 0;
      text_show(text, buffer);
      }
    }

  close(pipe_fd[0]);

  dup2(save_stdout, 1);
  dup2(save_stderr, 2);
  close(save_stdout);
  close(save_stderr);

  /* If action was to change the sender, and it succeeded, we have to
  update the in-store data. */

  if (rc == 0 && strcmp(action + (int)strlen(action) - 4, "-Mes") == 0)
    {
    queue_item *q = find_queue(id, queue_noop, 0);
    if (q != NULL)
      {
      if (q->sender != NULL) store_free(q->sender);
      q->sender = store_malloc((int)strlen(address_arg) + 1);
      strcpy(q->sender, address_arg);
      }
    }

  /* If configured, cause a display update and return */

  if (action_queue_update) tick_queue_accumulator = 999999;
  return;
  }

/* Message is to be delivered. Ensure that it is marked unfrozen,
because nothing will get written to the log to show that this has
happened. (Other freezing/unfreezings get logged and picked up from
there.) */

qq = find_queue(id, queue_noop, 0);
if (qq != NULL) qq->frozen = FALSE;

/* New, asynchronous code runs in a subprocess for commands that
will take some time. The main process does not wait. There is a
SIGCHLD handler in the main program that cleans up any terminating
sub processes. */

if ((pid = fork()) == 0)
  {
  close(1);
  close(2);

  dup2(pipe_fd[1], 1);
  dup2(pipe_fd[1], 2);
  close(pipe_fd[1]);

  system(buffer);

  close(1);
  close(2);
  close(pipe_fd[0]);
  _exit(0);
  }

/* Main process - set up an item for the main ticker to watch. */

if (pid < 0) text_showf(text, "Failed to fork: %s\n", strerror(pid)); else
  {
  pipe_item *p = (pipe_item *)store_malloc(sizeof(pipe_item));

  if (p == NULL)
    {
    text_show(text, "Run out of store\n");
    return;
    }

  p->widget = text;
  p->fd = pipe_fd[0];

  p->next = pipe_chain;
  pipe_chain = p;

  close(pipe_fd[1]);
  }
}




/*************************************************
*        Cause a message to be delivered         *
*************************************************/

static void deliverAction(Widget w, XtPointer client_data, XtPointer call_data)
{
w = w;      /* Keep picky compilers happy */
call_data = call_data;
ActOnMessage((char *)client_data, "-v -M", "");
}



/*************************************************
*        Cause a message to be Frozen            *
*************************************************/

static void freezeAction(Widget w, XtPointer client_data, XtPointer call_data)
{
w = w;      /* Keep picky compilers happy */
call_data = call_data;
ActOnMessage((char *)client_data, "-Mf", "");
}



/*************************************************
*        Cause a message to be thawed            *
*************************************************/

static void thawAction(Widget w, XtPointer client_data, XtPointer call_data)
{
w = w;      /* Keep picky compilers happy */
call_data = call_data;
ActOnMessage((char *)client_data, "-Mt", "");
}



/*************************************************
*          Take action using dialog data         *
*************************************************/

/* This function is called after a dialog box has been filled
in. It is global because it is set up in the action table at
start-up time. If the string is empty, do nothing. */

XtActionProc dialogAction(Widget w, XEvent *event, String *ss, Cardinal *c)
{
char *s = XawDialogGetValueString(dialog_widget);

w = w;      /* Keep picky compilers happy */
event = event;
ss = ss;
c = c;

XtPopdown((Widget)dialog_shell);
XtDestroyWidget((Widget)dialog_shell);
while (isspace((uschar)*s)) s++;
if (s[0] != 0)
  {
  if (actioned_message[0] != 0)
    ActOnMessage(actioned_message, action_required, s);
  else
    NonMessageDialogue(s);    /* When called from somewhere else */
  }
return NULL;
}



/*************************************************
*              Create a dialog box               *
*************************************************/

/* The focus is grabbed exclusively, so nothing else can
be done to the application until the box is filled in. This
function is also used by the Hide button handler. */

void create_dialog(char *label, char *value)
{
Arg warg[4];
Dimension x, y, xx, yy;
XtTranslations pop_trans;
Widget text;

/* Get the position of a reference widget so the dialog box can be put
near to it. */

get_pos_args[0].value = (XtArgVal)(&x);
get_pos_args[1].value = (XtArgVal)(&y);
XtGetValues(dialog_ref_widget, get_pos_args, 2);

/* When this is not a message_specific thing, the position of the
reference widget is relative to the window. Get the position of the top level
widget and add to the position. */

if (dialog_ref_widget != menushell)
  {
  get_pos_args[0].value = (XtArgVal)(&xx);
  get_pos_args[1].value = (XtArgVal)(&yy);
  XtGetValues(toplevel_widget, get_pos_args, 2);
  x += xx;
  y += yy;
  }

/* Create a transient shell for the dialog box. */

XtSetArg(warg[0], XtNtransientFor, queue_widget);
XtSetArg(warg[1], XtNx, x + 50);
XtSetArg(warg[2], XtNy, y + 50);
XtSetArg(warg[3], XtNallowShellResize, True);
dialog_shell = XtCreatePopupShell("forDialog", transientShellWidgetClass,
   toplevel_widget, warg, 4);

/* Create the dialog box. */

dialog_arg[0].value = (XtArgVal)label;
dialog_arg[1].value = (XtArgVal)value;
dialog_widget = XtCreateManagedWidget("dialog", dialogWidgetClass, dialog_shell,
  dialog_arg, XtNumber(dialog_arg));

/* Get the text widget from within the dialog box, give it the keyboard focus,
make it wider than the default, and override its translations to make Return
call the dialog action function. */

text = XtNameToWidget(dialog_widget, "value");
XtSetKeyboardFocus(dialog_widget, text);
xs_SetValues(text, 1, "width", 200);
pop_trans = XtParseTranslationTable(
  "<Key>Return:         dialogAction()\n");
XtOverrideTranslations(text, pop_trans);

/* Pop the thing up. */

XtPopup(dialog_shell, XtGrabExclusive);
XFlush(X_display);
}





/*************************************************
*        Cause a recipient to be added           *
*************************************************/

/*This just sets up the dialog box; the action happens when it is
filled in. */

static void addrecipAction(Widget w, XtPointer client_data, XtPointer call_data)
{
w = w;      /* Keep picky compilers happy */
call_data = call_data;
strcpy(actioned_message, (char *)client_data);
action_required = "-Mar";
dialog_ref_widget = menushell;
create_dialog("Recipient address to add?", "");
}



/*************************************************
*    Cause an address to be marked delivered     *
*************************************************/

static void markdelAction(Widget w, XtPointer client_data, XtPointer call_data)
{
w = w;      /* Keep picky compilers happy */
call_data = call_data;
strcpy(actioned_message, (char *)client_data);
action_required = "-Mmd";
dialog_ref_widget = menushell;
create_dialog("Recipient address to mark delivered?", "");
}


/*************************************************
*   Cause all addresses to be marked delivered   *
*************************************************/

static void markalldelAction(Widget w, XtPointer client_data, XtPointer call_data)
{
w = w;      /* Keep picky compilers happy */
call_data = call_data;
ActOnMessage((char *)client_data, "-Mmad", "");
}


/*************************************************
*        Edit the message's sender               *
*************************************************/

static void editsenderAction(Widget w, XtPointer client_data,
  XtPointer call_data)
{
queue_item *q;
char *sender;
w = w;      /* Keep picky compilers happy */
call_data = call_data;
strcpy(actioned_message, (char *)client_data);
q = find_queue(actioned_message, queue_noop, 0);
sender = (q == NULL)? "" : q->sender;
action_required = "-Mes";
dialog_ref_widget = menushell;
create_dialog("New sender address?", sender);
}



/*************************************************
*        Cause a message body to be edited       *
*************************************************/

static void editbodyAction(Widget w, XtPointer client_data, XtPointer call_data)
{
w = w;      /* Keep picky compilers happy */
call_data = call_data;
ActOnMessage((char *)client_data, "-Meb", "");
}



/*************************************************
*    Cause a message to be returned to sender    *
*************************************************/

static void giveupAction(Widget w, XtPointer client_data, XtPointer call_data)
{
w = w;      /* Keep picky compilers happy */
call_data = call_data;
ActOnMessage((char *)client_data, "-v -Mg", "");
}



/*************************************************
*      Cause a message to be cancelled           *
*************************************************/

static void removeAction(Widget w, XtPointer client_data, XtPointer call_data)
{
w = w;      /* Keep picky compilers happy */
call_data = call_data;
ActOnMessage((char *)client_data, "-Mrm", "");
}



/*************************************************
*             Display a message's headers        *
*************************************************/

static void headersAction(Widget w, XtPointer client_data, XtPointer call_data)
{
char buffer[256];
header_line *h, *next;
Widget text = text_create((char *)client_data, text_depth);
void *reset_point;

w = w;      /* Keep picky compilers happy */
call_data = call_data;

/* Remember the point in the dynamic store so we can recover to it afterwards.
Then use Exim's function to read the header. */

reset_point = store_get(0);

sprintf(buffer, "%s-H", (char *)client_data);
if (spool_read_header(buffer, TRUE, FALSE) != spool_read_OK)
  {
  if (errno == ERRNO_SPOOLFORMAT)
    {
    struct stat statbuf;
    sprintf(big_buffer, "%s/input/%s", spool_directory, buffer);
    if (stat(big_buffer, &statbuf) == 0)
      text_showf(text, "Format error in spool file %s: size=%d\n", buffer,
        statbuf.st_size);
    else text_showf(text, "Format error in spool file %s\n", buffer);
    }
  else text_showf(text, "Read error for spool file %s\n", buffer);
  store_reset(reset_point);
  return;
  }

if (sender_address != NULL)
  {
  text_showf(text, "%s sender: <%s>\n", sender_local? "Local" : "Remote",
    sender_address);
  }

if (recipients_list != NULL)
  {
  int i;
  text_show(text, "Recipients:\n");
  for (i = 0; i < recipients_count; i++)
    {
    text_showf(text, "  %s %s\n",
      (tree_search(tree_nonrecipients, recipients_list[i].address) == NULL)?
        " ":"*", recipients_list[i].address);
    }
  text_show(text, "\n");
  }

for (h = header_list; h != NULL; h = next)
  {
  next = h->next;
  text_showf(text, "%c ", h->type);   /* Don't push h->text through a %s */
  text_show(text, h->text);           /* expansion as it may be v large */
  }

store_reset(reset_point);
}




/*************************************************
*              Dismiss a text window             *
*************************************************/

static void dismissAction(Widget w, XtPointer client_data, XtPointer call_data)
{
pipe_item *p = pipe_chain;

w = w;      /* Keep picky compilers happy */
call_data = call_data;

XtPopdown((Widget)client_data);
XtDestroyWidget((Widget)client_data);

/* If this is a text widget for a sub-process, clear it out of
the chain so that subsequent data doesn't try to use it. We have
to search the parents of the saved widget to see if one of them
is what we have just destroyed. */

while (p != NULL)
  {
  Widget pp = p->widget;
  while (pp != NULL)
    {
    if (pp == (Widget)client_data) { p->widget = NULL; return; }
    pp = XtParent(pp);
    }
  p = p->next;
  }
}



/*************************************************
*             Set up popup text window           *
*************************************************/

static Widget text_create(char *name, int height)
{
Widget textshell, form, text, button;

/* Create a popup shell widget to display as an additional
toplevel window. */

textshell = XtCreatePopupShell("textshell", topLevelShellWidgetClass,
  toplevel_widget, NULL, 0);
xs_SetValues(textshell, 4,
  "title",     name,
  "iconName",  name,
  "minWidth",  100,
  "minHeight", 100);

/* Create a form widget, containing the text widget and the
dismiss button widget. */

form = XtCreateManagedWidget("textform", formWidgetClass,
  textshell, NULL, 0);
xs_SetValues(form, 1, "defaultDistance", 8);

text = XtCreateManagedWidget("texttext", asciiTextWidgetClass,
  form, text_arg, XtNumber(text_arg));
xs_SetValues(text, 4,
  "editType",        XawtextAppend,
  "width",           700,
  "height",          height,
  "translations",    text_trans);
XawTextDisplayCaret(text, TRUE);

/* Use the same font as for the queue display */

if (queue_font != NULL)
  {
  XFontStruct *f = XLoadQueryFont(X_display, queue_font);
  if (f != NULL) xs_SetValues(text, 1, "font", f);
  }

button_arg[0].value = (XtArgVal)text;
button = XtCreateManagedWidget("dismiss", commandWidgetClass,
  form, button_arg, XtNumber(button_arg));
XtAddCallback(button, "callback",  dismissAction, (XtPointer)textshell);

/* Get the toplevel popup displayed, and yield the text widget so
that text can be put into it. */

XtPopup(textshell, XtGrabNone);
return text;
}




/*************************************************
*            Set up menu in queue window         *
*************************************************/

/* We have added an action table that causes this function to
be called, and set up button 2 in the text widgets to call it. */

void menu_create(Widget w, XEvent *event, String *actargs, Cardinal *count)
{
int line;
int i;
char *s;
XawTextPosition p;
Widget src, menu_line, item_1, item_2, item_3, item_4,
  item_5, item_6, item_7, item_8, item_9, item_10, item_11,
  item_12, item_13, item_14;
XtTranslations menu_trans = XtParseTranslationTable(
  "<EnterWindow>:   highlight()\n\
   <LeaveWindow>:   unhighlight()\n\
   <BtnMotion>:     highlight()\n\
   <BtnUp>:         MenuPopdown()notify()unhighlight()\n\
  ");

actargs = actargs;   /* Keep picky compilers happy */
count = count;

/* Get the sink and source and the current text pointer */

queue_get_arg[0].value = (XtArgVal)(&queue_text_sink);
queue_get_arg[1].value = (XtArgVal)(&src);
queue_get_arg[2].value = (XtArgVal)(&s);
XtGetValues(w, queue_get_arg, 3);

/* Find the line number of the pointer in the window, and the
character offset of the top lefthand of the window. */

line = (event->xbutton).y / XawTextSinkMaxHeight(queue_text_sink, 1);
p = XawTextTopPosition(w);

/* Find the start of the line on which the button was clicked. */

i = line;
while (i-- > 0)
  {
  while (s[p] != 0 && s[p++] != '\n');
  }

/* Now pointing either at 0 or 1st char after \n, or very 1st char.
If 0, the click was beyond the end of the data; just set up a dummy
menu. (Not easy to ignore as several actions are specified for the
mouse click and it expects this one to set up a menu.) If on a
continuation line, move back to the main line. */

if (s[p] == 0)
  {
  menushell_arg[0].value = (XtArgVal)"No message selected";
  menushell = XtCreatePopupShell("menu", simpleMenuWidgetClass,
    queue_widget, menushell_arg, XtNumber(menushell_arg));
  XtAddCallback(menushell, "popdownCallback", popdownAction, NULL);
  xs_SetValues(menushell, 2,
    "cursor",       XCreateFontCursor(X_display, XC_arrow),
    "translations", menu_trans);
  highlighted_x = -1;
  return;
  }

while (p > 0 && s[p+11] == ' ')
  {
  line--;
  p--;
  while (p > 0 && s[p-1] != '\n') p--;
  }

/* Now pointing at first character of a main line. */

strncpy(message_id, s+p+11, MESSAGE_ID_LENGTH);
message_id[MESSAGE_ID_LENGTH] = 0;

/* Highlight the line being menued, and save its parameters so that it
can be de-highlighted at popdown. */

highlighted_start = highlighted_end = p;
while (s[highlighted_end] != '\n') highlighted_end++;
highlighted_x = 17;
highlighted_y = line * XawTextSinkMaxHeight(queue_text_sink, 1) + 2;

XawTextSinkDisplayText(queue_text_sink,
  highlighted_x, highlighted_y,
  highlighted_start, highlighted_end, 1);

/* Create the popup shell and the other widgets that comprise the menu.
Set the translations and pointer shape, and add the callback pointers. */

menushell_arg[0].value = (XtArgVal)message_id;
menushell = XtCreatePopupShell("menu", simpleMenuWidgetClass,
  queue_widget, menushell_arg, XtNumber(menushell_arg));
XtAddCallback(menushell, "popdownCallback", popdownAction, NULL);

xs_SetValues(menushell, 2,
  "cursor",       XCreateFontCursor(X_display, XC_arrow),
  "translations", menu_trans);

menu_line = XtCreateManagedWidget("line", smeLineObjectClass, menushell,
  NULL, 0);

item_1_arg[0].value = (XtArgVal)menu_line;
item_1 = XtCreateManagedWidget("item1", smeBSBObjectClass, menushell,
  item_1_arg, XtNumber(item_1_arg));
XtAddCallback(item_1, "callback",  msglogAction, (XtPointer)message_id);

item_2_arg[0].value = (XtArgVal)item_1;
item_2 = XtCreateManagedWidget("item2", smeBSBObjectClass, menushell,
  item_2_arg, XtNumber(item_2_arg));
XtAddCallback(item_2, "callback",  headersAction, (XtPointer)message_id);

item_3_arg[0].value = (XtArgVal)item_2;
item_3 = XtCreateManagedWidget("item3", smeBSBObjectClass, menushell,
  item_3_arg, XtNumber(item_3_arg));
XtAddCallback(item_3, "callback",  bodyAction, (XtPointer)message_id);

item_4_arg[0].value = (XtArgVal)item_3;
item_4 = XtCreateManagedWidget("item4", smeBSBObjectClass, menushell,
  item_4_arg, XtNumber(item_4_arg));
XtAddCallback(item_4, "callback",  deliverAction, (XtPointer)message_id);

item_5_arg[0].value = (XtArgVal)item_4;
item_5 = XtCreateManagedWidget("item5", smeBSBObjectClass, menushell,
  item_5_arg, XtNumber(item_5_arg));
XtAddCallback(item_5, "callback",  freezeAction, (XtPointer)message_id);

item_6_arg[0].value = (XtArgVal)item_5;
item_6 = XtCreateManagedWidget("item6", smeBSBObjectClass, menushell,
  item_6_arg, XtNumber(item_6_arg));
XtAddCallback(item_6, "callback",  thawAction, (XtPointer)message_id);

item_7_arg[0].value = (XtArgVal)item_6;
item_7 = XtCreateManagedWidget("item7", smeBSBObjectClass, menushell,
  item_7_arg, XtNumber(item_7_arg));
XtAddCallback(item_7, "callback",  giveupAction, (XtPointer)message_id);

item_8_arg[0].value = (XtArgVal)item_7;
item_8 = XtCreateManagedWidget("item8", smeBSBObjectClass, menushell,
  item_8_arg, XtNumber(item_8_arg));
XtAddCallback(item_8, "callback",  removeAction, (XtPointer)message_id);

item_9_arg[0].value = (XtArgVal)item_8;
item_9 = XtCreateManagedWidget("item9", smeBSBObjectClass, menushell,
  item_9_arg, XtNumber(item_9_arg));

item_10_arg[0].value = (XtArgVal)item_9;
item_10 = XtCreateManagedWidget("item10", smeBSBObjectClass, menushell,
  item_10_arg, XtNumber(item_10_arg));
XtAddCallback(item_10, "callback",  addrecipAction, (XtPointer)message_id);

item_11_arg[0].value = (XtArgVal)item_10;
item_11 = XtCreateManagedWidget("item11", smeBSBObjectClass, menushell,
  item_11_arg, XtNumber(item_11_arg));
XtAddCallback(item_11, "callback",  markdelAction, (XtPointer)message_id);

item_12_arg[0].value = (XtArgVal)item_11;
item_12 = XtCreateManagedWidget("item12", smeBSBObjectClass, menushell,
  item_12_arg, XtNumber(item_12_arg));
XtAddCallback(item_12, "callback",  markalldelAction, (XtPointer)message_id);

item_13_arg[0].value = (XtArgVal)item_12;
item_13 = XtCreateManagedWidget("item13", smeBSBObjectClass, menushell,
  item_13_arg, XtNumber(item_13_arg));
XtAddCallback(item_13, "callback",  editsenderAction, (XtPointer)message_id);

item_14_arg[0].value = (XtArgVal)item_13;
item_14 = XtCreateManagedWidget("item14", smeBSBObjectClass, menushell,
  item_14_arg, XtNumber(item_14_arg));
XtAddCallback(item_14, "callback",  editbodyAction, (XtPointer)message_id);

/* Arrange that the menu pops up with the first item selected. */

xs_SetValues(menushell, 1, "popupOnEntry", item_1);

/* Flag that the menu is up to suppress queue updates. */

menu_is_up = TRUE;
}

/* End of em_menu.c */
