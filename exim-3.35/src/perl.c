/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) 1998 Malcolm Beattie */
/* Modified by PH to get rid of the "na" usage, March 1999 */

/* This Perl add-on can be distributed under the same terms as Exim itself. */
/* See the file NOTICE for conditions of use and distribution. */

#include "exim.h"

#define EXIM_TRUE TRUE
#undef TRUE

#define EXIM_FALSE FALSE
#undef FALSE

#define EXIM_DEBUG DEBUG
#undef DEBUG

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#ifndef ERRSV
#define ERRSV (GvSV(errgv))
#endif

extern void boot_DynaLoader(CV *cv);

static PerlInterpreter *interp_perl = 0;

XS(xs_expand_string)
{
  dXSARGS;
  char *str;
  STRLEN len;

  if (items != 1)
    croak("Usage: Exim::expand_string(string)");

  str = expand_string(SvPV(ST(0), len));
  ST(0) = sv_newmortal();
  if (str)
    sv_setpv(ST(0), str);
  else if (!expand_string_forcedfail)
    croak("syntax error in Exim::expand_string argument: %s",
      expand_string_message);
}

static void xs_init(void)
{
  char *file = __FILE__;
  newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
  newXS("Exim::expand_string", xs_expand_string, file);
}

char *
init_perl(char *startup_code)
{
  dSP;
/***
  static int argc = 3;
  static char *argv[4] = { "exim-perl", "-e", "0", 0 };
***/
  static int argc = 2;
  static char *argv[3] = { "exim-perl", "/dev/null", 0 };
  SV *sv;
  STRLEN len;

  if (interp_perl) return 0;
  interp_perl = perl_alloc();
  perl_construct(interp_perl);
  perl_parse(interp_perl, xs_init, argc, argv, 0);
  perl_run(interp_perl);
  sv = newSVpv(startup_code, 0);
  PUSHMARK(SP);
  perl_eval_sv(sv, G_SCALAR|G_DISCARD|G_KEEPERR);
  SvREFCNT_dec(sv);
  if (SvTRUE(ERRSV)) return SvPV(ERRSV, len);
  return 0;
}

void
cleanup_perl(void)
{
  if (!interp_perl)
    return;
  perl_destruct(interp_perl);
  perl_free(interp_perl);
  interp_perl = 0;
}

char *
call_perl_cat(char *yield, int *sizep, int *ptrp, char **errstrp,
  char *name, char **arg)
{
  dSP;
  SV *sv;
  STRLEN len;
  char *str;
  int items;

  if (!interp_perl)
    {
    *errstrp = "the Perl interpreter has not been started";
    return 0;
    }

  ENTER;
  SAVETMPS;
  PUSHMARK(SP);
  while (*arg) XPUSHs(newSVpv(*arg++, 0));
  PUTBACK;
  items = perl_call_pv(name, G_SCALAR|G_EVAL);
  SPAGAIN;
  sv = POPs;
  PUTBACK;
  if (SvTRUE(ERRSV))
    {
    *errstrp = SvPV(ERRSV, len);
    return 0;
    }
  if (!SvOK(sv))
    {
    *errstrp = 0;
    return 0;
    }
  str = SvPV(sv, len);
  yield = string_cat(yield, sizep, ptrp, str, (int)len);
  FREETMPS;
  LEAVE;

  return yield;
}

/* End of perl.c */
