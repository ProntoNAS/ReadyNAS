/*************************************************
*               Exim Monitor                     *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "em_hdr.h"


/* This module contains functions for displaying text in a
text widget. It is not used for the log widget, because that
is dynamically updated and has special scrolling requirements. */

/* Count of characters displayed */

static int text_count = 0;


/*************************************************
*               Empty the widget                 *
*************************************************/

void text_empty(Widget w)
{
XawTextBlock b;
b.firstPos = 0;
b.ptr = (char *)(&b);
b.format = FMT8BIT;
b.length = 0;
XawTextReplace(w, 0, text_count, &b);
text_count = 0;
XawTextSetInsertionPoint(w, text_count);
}


/*************************************************
*                 Display text                   *
*************************************************/

void text_show(Widget w, char *s)
{
XawTextBlock b;
b.firstPos = 0;
b.ptr = s;
b.format = FMT8BIT;
b.length = (int)strlen(s);
XawTextReplace(w, text_count, text_count, &b);
text_count += b.length;
XawTextSetInsertionPoint(w, text_count);
}


/*************************************************
*           Display text from format             *
*************************************************/

void text_showf(Widget w, char *s, ...)
{
va_list ap;
char buffer[1024];
va_start(ap, s);
vsprintf(buffer, s, ap);
va_end(ap);
text_show(w, buffer);
}

/* End of em_text.c */
