/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/*************************************************
*       Build configuration header for Exim      *
*************************************************/

/* This auxiliary program builds the file config.h by the following
process:

First it reads Makefile, looking for certain OS-specific definitions which it
uses to define macros. Then it reads the defaults file config.h.defaults.

The defaults file contains normal C #define statements for various macros; if
the name of a macro is found in the environment, the environment value replaces
the default. If the default #define does not contain any value, then that macro
is not copied to the created file unless there is some value in the
environment.

This program is compiled and run as part of the Make process and is not
normally called independently. */


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *name;
  int *flag;
} have_item;

typedef struct {
  char *name;
  char *data;
} save_item;

static int have_seteuid = 0;
static int have_setresuid = 0;
static int have_ipv6 = 0;

static char errno_quota[256];
static char ostype[256];
static char cc[256];

/* If any entry is an initial substring of another, the longer one must
appear first. */

static have_item have_list[] = {
  { "HAVE_SETEUID",   &have_seteuid },
  { "HAVE_SETRESUID", &have_setresuid },
  { "HAVE_IPV6",      &have_ipv6 },
  { NULL, NULL}
};

static save_item save_list[] = {
  { "ERRNO_QUOTA",    errno_quota },
  { "OSTYPE",         ostype },
  { "CC",             cc },
  { NULL, NULL}
};


/* Subroutine to check a string for precisely one instance of "%s". If not,
bomb out. */

void
check_percent_ess(char *value, char *name)
{
int OK = 0;
char *p = strstr(value, "%s");
if (p != NULL) OK = strstr(p+2, "%s") == NULL;
if (!OK)
  {
  printf("\n*** \"%s\" (%s) must contain precisely one occurrence of\n"
    "*** \"%%s\". Please review your configuration.\n\n/", value, name);
  exit(1);
  }
}


/* Main program */

int
main(int argc, char **argv)
{
FILE *base;
FILE *new;
int last_initial = 'A';
int linecount = 0;
int have_auth = 0;
int support_crypteq = 0;
char buffer[1024];

if (argc != 1)
  {
  printf("*** Buildconfig: called with incorrect arguments\n");
  exit(1);
  }

new = fopen("config.h", "w");
if (new == NULL)
  {
  printf("*** Buildconfig: failed to open config.h for output\n");
  exit(1);
  }

printf("Building configuration file config.h\n");

fprintf(new, "/*************************************************\n");
fprintf(new, "*           Configuration header for Exim        *\n");
fprintf(new, "*************************************************/\n\n");

fprintf(new, "/* This file was automatically generated from Makefile and "
  "config.h.defaults,\n");
fprintf(new, "using values specified in the configuration file Local/Makefile.\n");
fprintf(new, "Do not edit it. Instead, edit Local/Makefile and "
  "rerun make. */\n\n");

/* First, search the makefile for certain settings */

base = fopen("Makefile", "r");
if (base == NULL)
  {
  printf("*** Buildconfig: failed to open Makefile\n");
  fclose(new);
  exit(1);
  }

errno_quota[0] = 0;    /* no over-riding value set */
ostype[0] = 0;         /* just in case */
cc[0] = 0;

while (fgets(buffer, sizeof(buffer), base) != NULL)
  {
  have_item *h;
  save_item *s;
  char *p = buffer + (int)strlen(buffer);
  linecount++;
  while (p > buffer && isspace((unsigned char)p[-1])) p--;
  *p = 0;
  p = buffer;
  while (isspace((unsigned char)*p)) p++;

  /* Items where we just save a boolean */

  for (h = have_list; h->name != NULL; h++)
    {
    int len = (int)strlen(h->name);
    if (strncmp(p, h->name, len) == 0)
      {
      p += len;
      while (isspace((unsigned char)*p)) p++;
      if (*p++ != '=')
        {
        printf("*** Buildconfig: syntax error in Makefile line %d\n", linecount);
        exit(1);
        }
      while (isspace((unsigned char)*p)) p++;
      if (strcmp(p, "YES") == 0 || strcmp(p, "yes") == 0) *(h->flag) = 1;
        else *(h->flag) = 0;   /* Must reset in case multiple instances */
      break;
      }
    }

  if (h->name != NULL) continue;

  /* Items where we save the complete string */

  for (s = save_list; s->name != NULL; s++)
    {
    int len = (int)strlen(s->name);
    if (strncmp(p, s->name, len) == 0)
      {
      p += len;
      while (isspace((unsigned char)*p)) p++;
      if (*p++ != '=')
        {
        printf("*** Buildconfig: syntax error in Makefile line %d\n", linecount);
        exit(1);
        }
      while (isspace((unsigned char)*p)) p++;
      strcpy(s->data, p);
      }
    }
  }

fprintf(new, "#define HAVE_IPV6             %s\n",
  have_ipv6? "TRUE" : "FALSE");
fprintf(new, "#define HAVE_SETEUID          %s\n",
  have_seteuid? "TRUE" : "FALSE");
fprintf(new, "#define HAVE_SETRESUID        %s\n",
  have_setresuid? "TRUE" : "FALSE");
fprintf(new, "#define mac_seteuid(a)        %s\n",
  have_seteuid? "seteuid(a)" : have_setresuid? "setresuid(-1,a,-1)" : "(-1)");
fprintf(new, "#define mac_setegid(a)        %s\n",
  have_seteuid? "setegid(a)" : have_setresuid? "setresgid(-1,a,-1)" : "(-1)");

if (errno_quota[0] != 0)
  fprintf(new, "\n#define ERRNO_QUOTA           %s\n", errno_quota);

if (strcmp(cc, "gcc") == 0 && strstr(ostype, "IRIX") != NULL)
  {
  fprintf(new, "\n/* This switch includes the code to fix the inet_ntoa() */");
  fprintf(new, "\n/* bug when using gcc on an IRIX system. */");
  fprintf(new, "\n#define USE_INET_NTOA_FIX");
  }

fprintf(new, "\n");
fclose(base);


/* Now handle the macros listed in the defaults */

base = fopen("../src/config.h.defaults", "r");
if (base == NULL)
  {
  printf("*** Buildconfig: failed to open ../src/config.h.defaults\n");
  fclose(new);
  exit(1);
  }


while (fgets(buffer, sizeof(buffer), base) != NULL)
  {
  char name[256];
  char *value;
  char *p = buffer;
  char *q = name;

  while (*p == ' ' || *p == '\t') p++;

  if (strncmp(p, "#define ", 8) != 0) continue;

  p += 8;
  while (*p == ' ' || *p == '\t') p++;

  if (*p < last_initial) fprintf(new, "\n");
  last_initial = *p;

  while (*p && (isalnum((unsigned char)*p) || *p == '_')) *q++ = *p++;
  *q = 0;

  /* Value exists in the environment. Remember if it is an AUTH setting or
  SUPPORT_CRYPTEQ. */

  if ((value = getenv(name)) != NULL)
    {
    int len;
    len = 21 - (int)strlen(name);

    if (strncmp(name, "AUTH_", 5) == 0) have_auth = 1;
    if (strncmp(name, "SUPPORT_CRYPTEQ", 15) == 0) support_crypteq = 1;

    /* The text value of LDAP_LIB_TYPE refers to a macro that gets set. */

    if (strcmp(name, "LDAP_LIB_TYPE") == 0)
      {
      if (strcmp(value, "NETSCAPE") == 0 ||
          strcmp(value, "UMICHIGAN") == 0 ||
          strcmp(value, "OPENLDAP1") == 0 ||
          strcmp(value, "OPENLDAP2") == 0 ||
          strcmp(value, "SOLARIS") == 0 ||
          strcmp(value, "SOLARIS7") == 0)              /* Compatibility */
        {
        fprintf(new, "#define LDAP_LIB_%s\n", value);
        }
      else
        {
        printf("\n*** LDAP_LIB_TYPE=%s is not a recognized LDAP library type."
          "\n*** Please review your configuration.\n\n", value);
        return 1;
        }
      }

    /* Other macros get set to the environment value */

    else
      {
      fprintf(new, "#define %s ", name);
      while(len-- > 0) fputc(' ', new);

      /* LOG_FILE_PATH is now messy because it can be a path containing %s or
      it can be "syslog" or ":syslog" or "syslog:path" or even "path:syslog". */

      if (strcmp(name, "LOG_FILE_PATH") == 0)
        {
        char *ss = value;
        for(;;)
          {
          char *pp;
          char *sss = strchr(ss, ':');
          if (sss != NULL)
            {
            strncpy(buffer, ss, sss-ss);
            buffer[sss-ss] = 0;  /* For empty case */
            }
          else strcpy(buffer, ss);
          pp = buffer + (int)strlen(buffer);
          while (pp > buffer && isspace((unsigned char)pp[-1])) pp--;
          *pp = 0;
          if (buffer[0] != 0 && strcmp(buffer, "syslog") != 0)
            check_percent_ess(buffer, name);
          if (sss == NULL) break;
          ss = sss + 1;
          while (isspace((unsigned char)*ss)) ss++;
          }
        fprintf(new, "\"%s\"\n", value);
        }

      /* Timezone values get quoted */

      else if (strcmp(name, "TIMEZONE_DEFAULT") == 0)
        fprintf(new, "\"%s\"\n", value);

      /* For others, quote any paths and don't quote anything else */

      else
        {
        if (value[0] == '/') fprintf(new, "\"%s\"\n", value);
          else fprintf(new, "%s\n", value);
        if (strcmp(name, "LOG_FILE_PATH") == 0 ||
            strcmp(name, "PID_FILE_PATH") == 0)
          check_percent_ess(value, name);
        }
      }
    }

  /* Value not defined in the environment */

  else
    {
    char *t = p;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '\n') fputs(buffer, new); else
      {
      *t = 0;
      if (strcmp(name, "BIN_DIRECTORY")   == 0 ||
          strcmp(name, "CONFIGURE_FILE")  == 0)
        {
        printf("\n*** %s has not been defined in any of the Makefiles in the\n"
          "    \"Local\" directory. "
          "Please review your configuration.\n\n", name);
        return 1;
        }

      if (strcmp(name, "TIMEZONE_DEFAULT") == 0)
        {
        char *tz = getenv("TZ");
        fprintf(new, "#define TIMEZONE_DEFAULT      ");
        if (tz == NULL) fprintf(new, "NULL\n"); else
          fprintf(new, "\"%s\"\n", tz);
        }

      else fprintf(new, "/* %s not set */\n", name);
      }
    }
  }

fclose(base);

/* If any AUTH macros were defined, define HAVE_AUTH, and ensure that
SUPPORT_CRYPTEQ is also defined. */

if (have_auth)
  {
  fprintf(new, "\n#define HAVE_AUTH\n");
  if (!support_crypteq) fprintf(new, "/* Force SUPPORT_CRYPTEQ for AUTH */\n"
    "#define SUPPORT_CRYPTEQ\n");
  }

/* End off */

fprintf(new, "\n/* End of config.h */\n");
fclose(new);
return 0;
}

/* End of buildconfig.c */
