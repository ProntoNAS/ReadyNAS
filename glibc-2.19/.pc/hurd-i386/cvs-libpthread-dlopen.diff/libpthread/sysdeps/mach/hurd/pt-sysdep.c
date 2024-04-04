/* System dependent pthreads code.  Hurd version.
   Copyright (C) 2000, 2002, 2005 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <mach.h>
#include <mach/mig_support.h>

#include <pt-internal.h>

__thread struct __pthread *___pthread_self;

/* Forward.  */
static void *init_routine (void);

/* OK, the name of this variable isn't really appropriate, but I don't
   want to change it yet.  */
void *(*_cthread_init_routine)(void) = &init_routine;

/* This function is called from the Hurd-specific startup code.  It
   should return a new stack pointer for the main thread.  The caller
   will switch to this new stack before doing anything serious.  */
static void *
init_routine (void)
{
  struct __pthread *thread;
  int err;

  /* Initialize the library.  */
  __pthread_init ();

  /* Create the pthread structure for the main thread (i.e. us).  */
  err = __pthread_create_internal (&thread, 0, 0, 0);
  assert_perror (err);

  /* XXX The caller copies the command line arguments and the environment
     to the new stack.  Pretend it wasn't allocated so that it remains
     valid if the main thread terminates.  */
  thread->stack = 0;

  ___pthread_self = thread;

  /* Decrease the number of threads, to take into account that the
     signal thread (which will be created by the glibc startup code
     when we return from here) shouldn't be seen as a user thread.  */
  __pthread_total--;

  /* Make MiG code thread aware.  */
  __mig_init (thread->stackaddr);

  return thread->mcontext.sp;
}
