/* idn-free.c --- Invoke the `free' function releasing memory
 *                allocated by libidn functions.
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Simon Josefsson
 *
 * This file is part of GNU Libidn.
 *
 * GNU Libidn is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * GNU Libidn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GNU Libidn; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */

#include <config.h>

/* Get specification. */
#include "idn-free.h"

/* Get free. */
#include <stdlib.h>

/**
 * idn_free:
 * @ptr: memory region to deallocate, or %NULL.
 *
 * Deallocates memory region by calling free().  If @ptr is %NULL no
 * operation is performed.
 *
 * Normally applications de-allocate strings allocated by libidn by
 * calling free() directly.  Under Windows, different parts of the
 * same application may use different heap memory, and then it is
 * important to deallocate memory allocated within the same module
 * that allocated it.  This function makes that possible.
 **/
void
idn_free (void *ptr)
{
  free (ptr);
}
