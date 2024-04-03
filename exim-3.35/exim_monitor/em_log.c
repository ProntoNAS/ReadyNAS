/*************************************************
*                 Exim Monitor                   *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* This module contains code for scanning the smaill log,
extracting information from it, and displaying a "tail". */

#include "em_hdr.h"

#define log_buffer_len 4096      /* For each log entry */

/* If anonymizing, don't alter these strings (this is all an ad hoc hack). */

#ifdef ANONYMIZE
static char *oklist[] = {
  "Completed",
  "defer",
  "from",
  "Connection timed out",
  "Start queue run: pid=",
  "End queue run: pid=",
  "host lookup did not complete",
  "unexpected disconnection while reading SMTP command from",
  "verify failed for SMTP recipient",
  "H=",
  "U=",
  "id=",
  "<",
  ">",
  "(",
  ")",
  "[",
  "]",
  "@",
  "=",
  "*",
  ".",
  "-",
  "\"",
  " ",
  "\n"};
static int oklist_size = sizeof(oklist) / sizeof(char *);
#endif



/*************************************************
*             Write to the log display           *
*************************************************/

static int visible = 0;
static int scrolled = FALSE;
static int size = 0;
static int top = 0;

static void show_log(char *s, ...)
{
int length, newtop;
va_list ap;
XawTextBlock b;
char buffer[log_buffer_len + 24];

/* Do nothing if not tailing a log */

if (log_widget == NULL) return;

/* Initialize the text block structure */

b.firstPos = 0;
b.ptr = buffer;
b.format = FMT8BIT;

/* We want to know whether the window has been scrolled back or not,
so that we can cease automatically scrolling with new text. This turns
out to be tricky with the text widget. We can detect whether the
scroll bar has been operated by checking on the "top" value, but it's
harder to detect that it has been returned to the bottom. The following
heuristic does its best. */

newtop = XawTextTopPosition(log_widget);
if (newtop != top)
  {
  if (!scrolled)
    {
    visible = size - top;      /* save size of window */
    scrolled = newtop < top;
    }
  else if (newtop > size - visible) scrolled = FALSE;
  top = newtop;
  }

/* Format the text that is to be written. */

va_start(ap, s);
vsprintf(buffer, s, ap);
va_end(ap);
length = (int)strlen(buffer);

/* If we are anonymizing for screen shots, flatten various things. */

#ifdef ANONYMIZE
  {
  char *p = buffer + 9;
  if (p[6] == '-' && p[13] == '-') p += 17;

  while (p < buffer + length)
    {
    int i;

    /* Check for strings to be left alone */

    for (i = 0; i < oklist_size; i++)
      {
      int len = (int)strlen(oklist[i]);
      if (strncmp(p, oklist[i], len) == 0)
        {
        p += len;
        break;
        }
      }
    if (i < oklist_size) continue;

    /* Leave driver names, size, protocol, alone */

    if ((*p == 'D' || *p == 'P' || *p == 'T' || *p == 'S' || *p == 'R') &&
        p[1] == '=')
      {
      p += 2;
      while (*p != ' ' && *p != 0) p++;
      continue;
      }

    /* Leave C= text alone */

    if (strncmp(p, "C=\"", 3) == 0)
      {
      p += 3;
      while (*p != 0 && *p != '"') p++;
      continue;
      }

    /* Flatten remaining chars */

    if (isdigit((uschar)*p)) *p++ = 'x';
    else if (isalpha((uschar)*p)) *p++ = 'x';
    else *p++ = '$';
    }
  }
#endif

/* If this would overflow the buffer, throw away 50% of the
current stuff in the buffer. Code defensively against odd
extreme cases that shouldn't actually arise. */

if (size + length > log_buffer_size)
  {
  if (size == 0) length = log_buffer_size/2; else
    {
    int cutcount = log_buffer_size/2;
    if (cutcount > size) cutcount = size; else
      {
      while (cutcount < size && log_display_buffer[cutcount] != '\n')
        cutcount++;
      cutcount++;
      }
    b.length = 0;
    XawTextReplace(log_widget, 0, cutcount, &b);
    size -= cutcount;
    top -= cutcount;
    if (top < 0) top = 0;
    if (top < cutcount) XawTextInvalidate(log_widget, 0, 999999);
    xs_SetValues(log_widget, 1, "displayPosition", top);
    }
  }

/* Insert the new text at the end of the buffer. */

b.length = length;
XawTextReplace(log_widget, 999999, 999999, &b);
size += length;

/* When not scrolled back, we want to keep the bottom line
always visible. */

if (!scrolled)
  {
  XawTextSetInsertionPoint(log_widget, size - 1);
  top = XawTextTopPosition(log_widget);
  }
}




/*************************************************
*            Function to read the log            *
*************************************************/

/* We read any new log entries, and use their data to
updated total counts for the configured stripcharts.
The count for the queue chart is handled separately.
We also munge the log entries and display a one-line
version in the log window. */

void read_log(void)
{
struct stat statdata;
char buffer[log_buffer_len];

/* If log is not yet open, skip all of this. */

if (LOG != NULL)
  {
  fseek(LOG, log_position, SEEK_SET);

  while (fgets(buffer, log_buffer_len, LOG) != NULL)
    {
    char *id;
    char *p = buffer;
    void *reset_point;
    int length = (int)strlen(buffer);
    int i;

    /* Skip totally blank lines (paranoia: there shouldn't be any) */

    while (*p == ' ' || *p == '\t') p++;
    if (*p == '\n') continue;

    /* We should now have a complete log entry in the buffer; check
    it for various regular expression matches and take appropriate
    action. Get the current store point so we can reset to it. */

    reset_point = store_get(0);

    /* First, update any stripchart data values, noting that the zeroth
    stripchart is the queue length, which is handled elsewhere, and the
    1st may the a size monitor. */

    for (i = stripchart_varstart; i < stripchart_number; i++)
      {
      if (pcre_exec(stripchart_regex[i], NULL, buffer, length, 0, PCRE_EOPT,
            NULL, 0) >= 0)
        stripchart_total[i]++;
      }

    /* Munge the log entry and display shortened form on one line.
    We omit the date and show only the time. */

    if (pcre_exec(yyyymmdd_regex,NULL,buffer,length,0,PCRE_EOPT,NULL,0) >= 0)
      {
      id = string_copyn(buffer + 20, MESSAGE_ID_LENGTH);
      show_log("%s", buffer+11);
      }
    else
      {
      id = "";
      show_log("%s", buffer);
      }

    /* Deal with frozen and unfrozen messages */

    if (strstric(buffer, "frozen", FALSE) != NULL)
      {
      queue_item *qq = find_queue(id, queue_noop, 0);
      if (qq != NULL)
        {
        if (strstric(buffer, "unfrozen", FALSE) != NULL)
          qq->frozen = FALSE;
        else qq->frozen = TRUE;
        }
      }

    /* Notice defer messages, and add the destination if it
    isn't already on the list for this message, with a pointer
    to the parent if we can. */

    if ((p = strstr(buffer, "==")) != NULL)
      {
      queue_item *qq = find_queue(id, queue_noop, 0);
      if (qq != NULL)
        {
        dest_item *d;
        char *q, *r;
        p += 2;
        while (isspace((uschar)*p)) p++;
        q = p;
        while (*p != 0 && !isspace((uschar)*p))
          {
          if (*p++ != '\"') continue;
          while (*p != 0)
            {
            if (*p == '\\') p += 2;
              else if (*p++ == '\"') break;
            }
          }
        *p++ = 0;
        if ((r = strstric(q, qualify_domain, FALSE)) != NULL &&
          *(--r) == '@') *r = 0;
        d = find_dest(qq, q, dest_add);
        if (d->parent == NULL)
          {
          while (isspace((uschar)*p)) p++;
          if (*p == '<')
            {
            dest_item *dd;
            q = ++p;
            while (*p != 0 && *p != '>') p++;
            *p = 0;
            if ((p = strstric(q, qualify_domain, FALSE)) != NULL &&
              *(--p) == '@') *p = 0;
            dd = find_dest(qq, q, dest_noop);
            if (dd != NULL && dd != d) d->parent = dd;
            }
          }
        }
      }

    store_reset(reset_point);
    }
  }


/* We have to detect when the log file is changed, and switch to the new file.
In practice, this means that some deliveries might go unrecorded, since they'll
be written to the old file, but this usually happens in the middle of the
night, and I don't think the hassle of keeping track of two log files is worth
it.

The test for a changed log file is to look up the inode of the file by name and
compare it with the saved inode of the file we currently are processing. This
accords with the usual interpretation of POSIX and other Unix specs that imply
"one file, one inode". However, it appears that on some Digital systems, if an
open file is unlinked, a new file may be created with the same inode while the
old file remains in existence. This can happen if the old log file is renamed,
processed in some way, and then deleted. To work round this, also test for a
link count of zero on the currently open file. */

if (LOG == NULL ||
    (fstat(fileno(LOG), &statdata) == 0 && statdata.st_nlink == 0) ||
    (stat(log_file, &statdata) == 0 && log_inode != statdata.st_ino))
  {
  FILE *TEST;

  /* Experiment shows that sometimes you can't immediately open
  the new log file - presumably immediately after the old one
  is renamed and before the new one exists. Therefore do a
  trial open first to be sure. */

  if ((TEST = fopen(log_file, "r")) != NULL)
    {
    if (LOG != NULL) fclose(LOG);
    LOG = TEST;
    fstat(fileno(LOG), &statdata);
    log_inode = statdata.st_ino;
    }
  }

/* Save the position we have got to in the log. */

if (LOG != NULL) log_position = ftell(LOG);
}

/* End of em_log.c */
