/*
 * stream.h
 *
 * This file is part of msmtp, an SMTP client.
 *
 * Copyright (C) 2005
 * Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   msmtp is released under the GPL with the additional exemption that
 *   compiling, linking, and/or using OpenSSL is allowed.
 */

#ifndef STREAM_H
#define STREAM_H

#include <stdio.h>


/* All 'errstr' arguments must point to buffers that are at least
 * 'errstr_bufsize' characters long.
 * If a function with an 'errstr' argument returns a value != STREAM_EOK,
 * 'errstr' will contain an error message (maybe just "").
 * If such a function returns STREAM_EOK, 'errstr' will not be changed.
 */
extern const size_t errstr_bufsize;
#define STREAM_EOK		0	/* no error */
#define STREAM_EIO		1	/* Input/output error */

/*
 * stream_gets()
 * 
 * Reads in at most one less than 'size' characters from 'f' and stores them
 * into the buffer pointed to by 'str'. Reading stops after an EOF or a newline.
 * If a newline is read, it is stored into the buffer. A '\0' is stored after 
 * the last character in the buffer. The length of the resulting string (the
 * number of characters excluding the terminating '\0') will be stored in 'len'.
 * Used error codes: STREAM_EIO
 */
int stream_gets(FILE *f, char *str, size_t size, size_t *len, char *errstr);

#endif
