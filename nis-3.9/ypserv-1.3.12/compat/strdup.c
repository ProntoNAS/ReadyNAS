#include <stdlib.h>
#include <string.h>
#include <config.h>

static char *
strdup (char *s)
{
  char *ret;
  ret = malloc (strlen (s) + 1);
  strcpy (ret, s);
  return ret;
}
