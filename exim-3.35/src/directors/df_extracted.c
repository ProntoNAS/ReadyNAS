/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "dfunctions.h"


/*************************************************
*     Handle failures to extract from a list     *
*************************************************/

/* This function is called by a director after parse_extract_addresses() yields
a non-zero return. It analyzes the error code and sets the return code
appropriately. If verifying, set a flag that causes non-empty messages from
:defer: and :fail: to be passed back to the caller - in particular, this gets
them into the response to an SMTP command.

Arguments:
  extracted      a non-zero yield of parse_extract_addresses()
                   >0 if failed to stat or open an :include: file
                   <0 for other errors
  dblock         the director block
  addr           the address being processed
  error          the error text from parse_extract_addresses
  verify         TRUE if verifying
  freeze_missing_include  if TRUE, do what it says
  text           text to include in catch-all error message

Returns:         return code for the director
*/

int
df_extracted(int extracted, director_instance *dblock, address_item *addr,
  char *error, BOOL verify, BOOL freeze_missing_include, char *text)
{
switch (extracted)
  {
  case EXTRACTED_DEFER:
  addr->message = string_sprintf("forced defer: %s", error);
  if (verify && *error != 0) verify_forced_errmsg = error;
  return DEFER;

  case EXTRACTED_UNKNOWN:
  DEBUG(2) debug_printf("%s director declined by :unknown:\n", dblock->name);
  return DECLINE;

  case EXTRACTED_FAIL:
  addr->message = string_sprintf("forced failure: %s", error);
  if (verify && *error != 0) verify_forced_errmsg = error;
  return FORCEFAIL;

  default:
  addr->basic_errno = ERRNO_BADALIAS;
  addr->message = string_sprintf("error in %s: %s", text, error);
  if (extracted > 0 && !freeze_missing_include) return DEFER;
  addr->special_action = SPECIAL_FREEZE;
  return ERROR;
  }
}
/* End of df_extracted.c */
