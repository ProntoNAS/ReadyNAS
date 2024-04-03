/* test_ntlm.c --- Test module for libntlm.
 * Copyright (C) 2008 Simon Josefsson
 * Copyright (C) 2004, 2005 Frediano Ziglio
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <byteswap.h>

#include "ntlm.h"
#include "md4.h"

static int
md4file (const char *name, unsigned char *hash)
{
  FILE *f;
  int res = 0;

  f = fopen (name, "r");
  if (!f)
    return 1;

  res = md4_stream (f, hash);

  fclose (f);

  return res;
}

static int
diffFile (const char *name1, const char *name2)
{
  unsigned char hash1[24], hash2[24];

  if (md4file (name1, hash1) || md4file (name2, hash2))
    return 1;

  if (memcmp (hash1, hash2, 16) != 0)
    return 1;

  return 0;
}

static void
dumpRaw (FILE * fp, const unsigned char *buf, size_t len)
{
  size_t i;

  for (i = 0; i < len; ++i)
    {
      if (i != 0 && (i & 0xf) == 0)
	fprintf (fp, "\n");
      fprintf (fp, "%02x ", buf[i]);
    }

  fprintf (fp, "\n");
}

static uint16
intelEndian16 (uint16 n)
{
  uint16 u;
  unsigned char *buf = (unsigned char *) &u;
  buf[0] = n & 0xff;
  buf[1] = (n >> 8) & 0xff;
  return u;
}

static uint32
intelEndian32 (uint32 n)
{
  uint32 u;
  unsigned char *buf = (unsigned char *) &u;
  buf[0] = n & 0xff;
  buf[1] = (n >> 8) & 0xff;
  buf[2] = (n >> 16) & 0xff;
  buf[3] = (n >> 24) & 0xff;
  return u;
}

static void
fillUnicode (tSmbStrHeader * header, char *buffer, int buffer_start,
	     int *idx, const char *s)
{
  int len = strlen (s);
  header->len = header->maxlen = intelEndian16 (len * 2);
  header->offset = intelEndian32 (*idx + buffer_start);
  *idx += len * 2;

  for (; len; --len)
    {
      *buffer++ = *s++;
      *buffer++ = 0;
    }
}

static void
fillChallenge (tSmbNtlmAuthChallenge * challenge, const char *domain)
{
  int idx = 0;

  memset (challenge, 0, sizeof (*challenge));
  memcpy (challenge->ident, "NTLMSSP\0\0\0", 8);
  challenge->msgType = intelEndian32 (2);
  fillUnicode (&challenge->uDomain, challenge->buffer,
	       challenge->buffer - ((uint8 *) challenge), &idx, domain);
  challenge->flags = intelEndian32 (0);
  memcpy (challenge->challengeData, "\x01\x02\x03\x04\xf5\xc3\xb2\x82", 8);
  challenge->bufIndex = idx;
}

#define DUMP_REQUEST(req) dumpRaw(f, (unsigned char*) req, SmbLength(req))

int
main (void)
{
  tSmbNtlmAuthRequest request;
  tSmbNtlmAuthChallenge challenge;
  tSmbNtlmAuthResponse response;

  FILE *f = fopen ("test.out", "w");
  if (!f)
    return 1;

  printf ("ntlm.h %s libntlm %s\n", NTLM_VERSION, ntlm_check_version (NULL));

  if (!ntlm_check_version (NTLM_VERSION))
    return 1;

  /* do some test then dump */
  buildSmbNtlmAuthRequest (&request, "myuser", "mydomain");
  dumpSmbNtlmAuthRequest (f, &request);
  DUMP_REQUEST (&request);

  buildSmbNtlmAuthRequest (&request, "Test_!@xXxX.&", NULL);
  dumpSmbNtlmAuthRequest (f, &request);
  DUMP_REQUEST (&request);

  fillChallenge (&challenge, "mydomain");
  dumpSmbNtlmAuthChallenge (f, &challenge);
  DUMP_REQUEST (&challenge);

  buildSmbNtlmAuthResponse (&challenge, &response, "otheruser", "mypasswd");
  dumpSmbNtlmAuthResponse (f, &response);
  DUMP_REQUEST (&response);

  fclose (f);

  /* now reopen the file and do a diff */
  return diffFile ("test.out", NTLM_SRCDIR "/test.txt");
}
