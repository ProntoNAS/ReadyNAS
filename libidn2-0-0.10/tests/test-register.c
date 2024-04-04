/* test-register.c --- Self tests for IDNA processing
   Copyright (C) 2011-2014 Simon Josefsson

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#include <idn2.h>

struct idna
{
  const uint8_t *alabel;
  const uint8_t *ulabel;
  const char *out;
  int rc;
  int flags;
};

static const struct idna idna[] = {
  {"xn--rksmrgs-5wao1o", "räksmörgås", "xn--rksmrgs-5wao1o", IDN2_OK},
  {NULL, "sharpß", "xn--sharp-pqa", IDN2_OK},
  {"xn--sharp-pqa", "sharpß", "xn--sharp-pqa", IDN2_OK},
  {"foo", NULL, NULL, IDN2_INVALID_ALABEL},
  {NULL, "foo", "foo", IDN2_OK},
  {NULL, "räksmörgås", "xn--rksmrgs-5wao1o", IDN2_OK},
  /* U+00B7 MIDDLE DOT */
  {NULL, "·", "", IDN2_CONTEXTO},
  {NULL, "a·", "", IDN2_CONTEXTO},
  {NULL, "·a", "", IDN2_CONTEXTO},
  {NULL, "a·a", "", IDN2_CONTEXTO},
  {NULL, "l·l", "xn--ll-0ea", IDN2_OK},
  {NULL, "al·la", "xn--alla-6ha", IDN2_OK},
  /* U+0375 GREEK LOWER NUMERAL SIGN (KERAIA) */
  {NULL, "͵", "", IDN2_CONTEXTO},
  {NULL, "͵a", "", IDN2_CONTEXTO},
  {NULL, "͵a͵ϳ", "", IDN2_CONTEXTO},
  {NULL, "͵ϳ͵a", "", IDN2_CONTEXTO},
  {NULL, "͵ϳ", "xn--wva6w", IDN2_OK},
  {NULL, "͵ϳ͵ϳ", "xn--wvaa19ab", IDN2_OK},
  /* U+05F3 HEBREW PUNCTUATION GERESH */
  {NULL, "׳", "", IDN2_CONTEXTO},
  {NULL, "a׳", "", IDN2_CONTEXTO},
  {NULL, "a׳א׳", "", IDN2_CONTEXTO},
  {NULL, "א׳a׳", "", IDN2_CONTEXTO},
  {NULL, "א׳", "xn--4db4e", IDN2_OK},
  {NULL, "בא׳ב", "xn--4dbbb9k", IDN2_OK},
  /* U+05F4 HEBREW PUNCTUATION GERSHAYIM */
  {NULL, "״", "", IDN2_CONTEXTO},
  {NULL, "a״", "", IDN2_CONTEXTO},
  {NULL, "a״א", "", IDN2_CONTEXTO},
  {NULL, "א״", "xn--4db6e", IDN2_OK},
  {NULL, "בא״ב", "xn--4dbbb3l", IDN2_OK},
  /* U+0660..U+0669 ARABIC-INDIC DIGITS and
     U+06F0..U+06F9 EXTENDED ARABIC-INDIC DIGITS */
  {NULL, "٠", "", IDN2_BIDI},
  {NULL, "ء٠", "xn--ggb0k", IDN2_OK},
  {NULL, "ء۰", "xn--ggb82b", IDN2_OK},
  {NULL, "ء٠ءء", "xn--ggbaa4w", IDN2_OK},
  {NULL, "ء٠۰", "", IDN2_CONTEXTO},
  {NULL, "ء٠ءء۰", "", IDN2_CONTEXTO},
  {NULL, "ء۰ءء٠", "", IDN2_CONTEXTO},
  {NULL, "٠ء۰ءء٠", "", IDN2_CONTEXTO},
  /* U+30FB KATAKANA MIDDLE DOT */
  {NULL, "・", "", IDN2_CONTEXTO},
  {NULL, "foo・", "", IDN2_CONTEXTO},
  {NULL, "foo・bar", "", IDN2_CONTEXTO},
  {NULL, "foo・barぁbaz", /* U+3041 HIRAGANA LETTER SMALL A */
   "xn--foobarbaz-b23h61e", IDN2_OK},
  {NULL, "foo・barァbaz", /* U+30A1 KATAKANA LETTER SMALL A */
   "xn--foobarbaz-qu4h06a", IDN2_OK},
  {NULL, "foo・bar〇baz", /* U+3007 IDEOGRAPHIC NUMBER ZERO */
   "xn--foobarbaz-ql3hk3g", IDN2_OK},
  {NULL, "foo・bar㐀baz", /* U+3400 CJK UNIFIED IDEOGRAPH-3400 */
   "xn--foobarbaz-dl5hq7z", IDN2_OK},
};

int debug = 1;
int error_count = 0;
int break_on_error = 1;

void
fail (const char *format, ...)
{
  va_list arg_ptr;

  va_start (arg_ptr, format);
  vfprintf (stderr, format, arg_ptr);
  va_end (arg_ptr);
  error_count++;
  if (break_on_error)
    exit (EXIT_FAILURE);
}

void
hexprint (const char *str, size_t len)
{
  size_t i;

  printf ("\t;; ");
  if (str && len)
    for (i = 0; i < len; i++)
      {
	printf ("%02x ", (str[i] & 0xFF));
	if ((i + 1) % 8 == 0)
	  printf (" ");
	if ((i + 1) % 16 == 0 && i + 1 < len)
	  printf ("\n\t;; ");
      }
  printf ("\n");
}

int
main (void)
{
  uint8_t *out;
  size_t i;
  int rc;

  puts ("-----------------------------------------------------------"
	"-------------------------------------");
  puts ("                                          IDNA2008 Register\n");
  puts ("  #  Result                    Output                    A-label"
	" input             U-label input");
  puts ("-----------------------------------------------------------"
	"-------------------------------------");
  for (i = 0; i < sizeof (idna) / sizeof (idna[0]); i++)
    {
      rc = idn2_register_u8 (idna[i].ulabel, idna[i].alabel,
			     &out, idna[i].flags);
      printf ("%3d  %-25s %-25s %-25s %s\n", i, idn2_strerror_name (rc),
	      rc == IDN2_OK ? idna[i].out : "",
	      idna[i].alabel ? (char *) idna[i].alabel : "(null)",
	      idna[i].ulabel ? (char *) idna[i].ulabel : "(null)");
      if (rc != idna[i].rc)
	fail ("expected rc %d got rc %d\n", idna[i].rc, rc);
      else if (rc == IDN2_OK && strcmp (out, idna[i].out) != 0)
	fail ("expected: %s\ngot: %s\n", idna[i].out, out);

      if (rc == IDN2_OK)
	{
	  uint8_t *tmp;

	  rc = idn2_lookup_u8 (idna[i].ulabel, &tmp, idna[i].flags);
	  if (rc != IDN2_OK)
	    fail ("lookup failed?! tv %d", i);
	  if (strcmp (out, tmp) != 0)
	    fail ("lookup and register different? lookup %s register %s\n",
		  tmp, out);
	  free (tmp);
	  free (out);
	}
    }
  puts ("-----------------------------------------------------------"
	"-------------------------------------");

  return error_count;
}
