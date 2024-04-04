/* --*- C -*-- 
 * 
 * By Jani Averbach, Jaa@iki.fi, 2001
 * 
 * Released under version 2 of the Gnu Public License.
 *
 * $Author: ahu $ 
 * $Revision: 1.1.1.1 $ 
 * $Date: 2010-03-10 00:12:27 $ 
 * $Source: /cvs/memtest86+-4.00/screen_buffer.h,v $  (for CVS)
 * 
 */
#ifndef SCREEN_BUFFER_H_1D10F83B_INCLUDED
#define SCREEN_BUFFER_H_1D10F83B_INCLUDED

#include "config.h"

char get_scrn_buf(const int y, const int x);
void set_scrn_buf(const int y, const int x, const char val);
void clear_screen_buf(void);
void tty_print_region(const int pi_top,const int pi_left, const int pi_bottom,const int pi_right);
void tty_print_line(int y, int x, const char *text);
void tty_print_screen(void);
void print_error(char *pstr);
#endif /* SCREEN_BUFFER_H_1D10F83B_INCLUDED */
