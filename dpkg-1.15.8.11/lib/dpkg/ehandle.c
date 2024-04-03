/*
 * libdpkg - Debian packaging suite library routines
 * ehandle.c - error handling
 *
 * Copyright © 1994,1995 Ian Jackson <ian@chiark.greenend.org.uk>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <compat.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <dpkg/macros.h>
#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>

static const char *errmsg; /* points to errmsgbuf or malloc'd */
static char errmsgbuf[4096];
/* 6x255 for inserted strings (%.255s &c in fmt; also %s with limited length arg)
 * 1x255 for constant text
 * 1x255 for strerror()
 * same again just in case.
 */

/* Incremented when we do some kind of generally necessary operation,
 * so that loops &c know to quit if we take an error exit. Decremented
 * again afterwards.
 */
volatile int onerr_abort = 0;

#define NCALLS 2

struct cleanupentry {
  struct cleanupentry *next;
  struct {
    int mask;
    void (*call)(int argc, void **argv);
  } calls[NCALLS];
  int cpmask, cpvalue;
  int argc;
  void *argv[1];
};         

struct errorcontext {
  struct errorcontext *next;
  jmp_buf *jbufp;
  struct cleanupentry *cleanups;
  void (*printerror)(const char *emsg, const char *contextstring);
  const char *contextstring;
};

static struct errorcontext *volatile econtext= NULL;
static struct { struct cleanupentry ce; void *args[20]; } emergency;

void set_error_display(error_printer *printerror,
                       const char *contextstring) {
  assert(econtext);
  econtext->printerror= printerror;
  econtext->contextstring= contextstring;
}

static void DPKG_ATTR_NORET
run_error_handler(void)
{
  if (onerr_abort) {
    /* We arrived here due to a fatal error from which we cannot recover,
     * and trying to do so would most probably get us here again. That's
     * why we will not try to do any error unwinding either. We'll just
     * abort. Hopefully the user can fix the situation (out of disk, out
     * of memory, etc).
     */
    fprintf(stderr, _("%s: unrecoverable fatal error, aborting:\n %s\n"),
            thisname, errmsg);
    exit(2);
  } else {
    longjmp(*econtext->jbufp, 1);
  }
}

void push_error_handler(jmp_buf *jbufp,
                        error_printer *printerror,
                        const char *contextstring) {
  struct errorcontext *necp;
  necp= malloc(sizeof(struct errorcontext));
  if (!necp) {
    int e= errno;
    snprintf(errmsgbuf, sizeof(errmsgbuf), "%s%s", 
	    _("out of memory pushing error handler: "), strerror(e));
    errmsg= errmsgbuf;
    if (econtext)
      run_error_handler();
    fprintf(stderr, "%s: %s\n", thisname, errmsgbuf); exit(2);
  }
  necp->next= econtext;
  necp->jbufp= jbufp;
  necp->cleanups= NULL;
  necp->printerror= printerror;
  necp->contextstring= contextstring;
  econtext= necp;
  onerr_abort= 0;
}

static void print_error_cleanup(const char *emsg, const char *contextstring) {
  fprintf(stderr, _("%s: error while cleaning up:\n %s\n"),thisname,emsg);
}

static void run_cleanups(struct errorcontext *econ, int flagsetin) {
  static volatile int preventrecurse= 0;
  struct cleanupentry *volatile cep;
  struct cleanupentry *ncep;
  struct errorcontext recurserr, *oldecontext;
  jmp_buf recurejbuf;
  volatile int i, flagset;

  if (econ->printerror) econ->printerror(errmsg,econ->contextstring);
     
  if (++preventrecurse > 3) {
    onerr_abort++;
    fprintf(stderr, _("dpkg: too many nested errors during error recovery !!\n"));
    flagset= 0;
  } else {
    flagset= flagsetin;
  }
  cep= econ->cleanups;
  oldecontext= econtext;
  while (cep) {
    for (i=0; i<NCALLS; i++) {
      if (cep->calls[i].call && cep->calls[i].mask & flagset) {
        if (setjmp(recurejbuf)) {
          run_cleanups(&recurserr, ehflag_bombout | ehflag_recursiveerror);
        } else {
          recurserr.jbufp= &recurejbuf;
          recurserr.cleanups= NULL;
          recurserr.next= NULL;
          recurserr.printerror= print_error_cleanup;
          recurserr.contextstring= NULL;
          econtext= &recurserr;
          cep->calls[i].call(cep->argc,cep->argv);
        }
        econtext= oldecontext;
      }
    }
    flagset &= cep->cpmask;
    flagset |= cep->cpvalue;
    ncep= cep->next;
    if (cep != &emergency.ce) free(cep);
    cep= ncep;
  }
  preventrecurse--;
}

void error_unwind(int flagset) {
  struct errorcontext *tecp;

  tecp= econtext;
  econtext= tecp->next;
  run_cleanups(tecp,flagset);
  free(tecp);
}

void push_checkpoint(int mask, int value) {
  /* This will arrange that when error_unwind() is called,
   * all previous cleanups will be executed with
   *  flagset= (original_flagset & mask) | value
   * where original_flagset is the argument to error_unwind
   * (as modified by any checkpoint which was pushed later).
   */
  struct cleanupentry *cep;
  int i;
  
  cep= m_malloc(sizeof(struct cleanupentry) + sizeof(char*));
  for (i=0; i<NCALLS; i++) { cep->calls[i].call=NULL; cep->calls[i].mask=0; }
  cep->cpmask= mask; cep->cpvalue= value;
  cep->argc= 0; cep->argv[0]= NULL;
  cep->next= econtext->cleanups;
  econtext->cleanups= cep;
}

void push_cleanup(void (*call1)(int argc, void **argv), int mask1,
                  void (*call2)(int argc, void **argv), int mask2,
                  unsigned int nargs, ...) {
  struct cleanupentry *cep;
  void **argv;
  int e;
  va_list args;

  onerr_abort++;
  
  cep= malloc(sizeof(struct cleanupentry) + sizeof(char*)*(nargs+1));
  if (!cep) {
    if (nargs > array_count(emergency.args))
      ohshite(_("out of memory for new cleanup entry with many arguments"));
    e= errno; cep= &emergency.ce;
  }
  cep->calls[0].call= call1; cep->calls[0].mask= mask1;
  cep->calls[1].call= call2; cep->calls[1].mask= mask2;
  cep->cpmask=~0; cep->cpvalue=0; cep->argc= nargs;
  va_start(args, nargs);
  argv = cep->argv;
  while (nargs-- > 0)
    *argv++ = va_arg(args, void *);
  *argv++ = NULL;
  va_end(args);
  cep->next= econtext->cleanups;
  econtext->cleanups= cep;
  if (cep == &emergency.ce) { e= errno; ohshite(_("out of memory for new cleanup entry")); }

  onerr_abort--;
}

void pop_cleanup(int flagset) {
  struct cleanupentry *cep;
  int i;

  cep= econtext->cleanups;
  econtext->cleanups= cep->next;
  for (i=0; i<NCALLS; i++) {
    if (cep->calls[i].call && cep->calls[i].mask & flagset)
      cep->calls[i].call(cep->argc,cep->argv);
  }
  if (cep != &emergency.ce) free(cep);
}

void ohshit(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  vsnprintf(errmsgbuf, sizeof(errmsgbuf), fmt, args);
  va_end(args);
  errmsg= errmsgbuf;

  run_error_handler();
}

void print_error_fatal(const char *emsg, const char *contextstring) {
  fprintf(stderr, "%s: %s\n",thisname,emsg);
}

void
ohshitv(const char *fmt, va_list args)
{
  vsnprintf(errmsgbuf, sizeof(errmsgbuf), fmt, args);
  errmsg= errmsgbuf;

  run_error_handler();
}

void ohshite(const char *fmt, ...) {
  int e;
  va_list args;
  char buf[1024];

  e=errno;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  snprintf(errmsgbuf,sizeof(errmsgbuf),"%s: %s",buf,strerror(e));
  errmsg= errmsgbuf; 

  run_error_handler();
}

void
warning(const char *fmt, ...)
{
  va_list args;
  char buf[1024];

  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  fprintf(stderr, _("%s: warning: %s\n"), thisname, buf);
}

void werr(const char *fn) {
  ohshite(_("error writing `%s'"),fn);
}

void
do_internerr(const char *file, int line, const char *fmt, ...)
{
  va_list args;
  char buf[1024];

  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  fprintf(stderr, _("%s:%s:%d: internal error: %s\n"),
          thisname, file, line, buf);

  abort();
}


