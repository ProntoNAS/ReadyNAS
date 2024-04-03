/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "exim.h"


/* This module contains tables that define the lookup methods and drivers
that are actually included in the binary. Its contents are controlled by
various macros in config.h that ultimately come from Local/Makefile. They are
all described in src/EDITME. */


/* The OSF1 (Digital Unix) linker puts out a worrying warning if any sections
contain no executable code. It says

Warning: Linking some objects which contain exception information sections
        and some which do not. This may cause fatal runtime exception handling
        problems.

As this may cause people to worry needlessly, include a dummy function here
to stop the message from appearing. Make it call itself to stop picky compilers
compilers complaining that it is unused, and put in a dummy argument to stop
even pickier compilers complaining about infinite loops. */

static void dummy(int x) { dummy(x-1); }


/* Table of information about all possible lookup methods. The entries are
always present, but the "open" and "find" functions are set to NULL for those
that are not compiled into the binary. The "check" and "close" functions can
be NULL for methods that don't need them. */

#ifdef LOOKUP_CDB
#include "lookups/cdb.h"
#endif

#ifdef LOOKUP_DBM
#include "lookups/dbmdb.h"
#endif

#ifdef LOOKUP_DNSDB
#include "lookups/dnsdb.h"
#endif

#ifdef LOOKUP_LDAP
#include "lookups/ldap.h"
#endif

#ifdef LOOKUP_LSEARCH
#include "lookups/lsearch.h"
#endif

#ifdef LOOKUP_MYSQL
#include "lookups/mysql.h"
#endif

#ifdef LOOKUP_NIS
#include "lookups/nis.h"
#endif

#ifdef LOOKUP_NISPLUS
#include "lookups/nisplus.h"
#endif

#ifdef LOOKUP_PGSQL
#include "lookups/pgsql.h"
#endif

#ifdef LOOKUP_TESTDB
#include "lookups/testdb.h"
#endif


/* The second field in each item below is a set of bit flags:

  lookup_querystyle     => this is a query-style lookup,
                             else single-key (+ file) style
  lookup_absfile        => an absolute file name is required,
                             (for single-key style only)
*/


lookup_info lookup_list[] = {

/* cdb lookup in single file */

  {
  "cdb",			 /* lookup name */
  lookup_absfile,		 /* uses absolute file name */
#ifdef LOOKUP_CDB
  cdb_open,			 /* open function */
  cdb_check,			 /* check function */
  cdb_find,			 /* find function */
  cdb_close,			 /* close function */
  NULL,				 /* no tidy function */
  NULL                           /* no quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* DBM file lookup; called "dbm" because that is the name in Exim,
but the code is called dbmdb to avoid name clashes. */

  {
  "dbm",                         /* lookup name */
  lookup_absfile,                /* uses absolute file name */
#ifdef LOOKUP_DBM
  dbmdb_open,                    /* open function */
  dbmdb_check,                   /* check function */
  dbmdb_find,                    /* find function */
  dbmdb_close,                   /* close function */
  NULL,                          /* no tidy function */
  NULL                           /* no quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* This variant of DBM does not include the binary zero on the end
of the key strings. */

  {
  "dbmnz",                       /* lookup name */
  lookup_absfile,                /* uses absolute file name */
#ifdef LOOKUP_DBM
  dbmdb_open,      /* sic */     /* open function */
  dbmdb_check,     /* sic */     /* check function */
  dbmnz_find,                    /* find function */
  dbmdb_close,     /* sic */     /* close function */
  NULL,                          /* no tidy function */
  NULL                           /* no quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* Using DNS TXT records as a database */

  {
  "dnsdb",                       /* lookup name */
  lookup_querystyle,             /* query style */
#ifdef LOOKUP_DNSDB
  dnsdb_open,                    /* open function */
  NULL,                          /* check function */
  dnsdb_find,                    /* find function */
  NULL,                          /* no close function */
  NULL,                          /* no tidy function */
  NULL                           /* no quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* LDAP lookup, allowing data from only one entry to be returned */

  {
  "ldap",                        /* lookup name */
  lookup_querystyle,             /* query-style lookup */
#ifdef LOOKUP_LDAP
  eldap_open,                    /* open function */
  NULL,                          /* check function */
  eldap_find,                    /* find function */
  NULL,                          /* no close function */
  eldap_tidy,                    /* tidy function */
  eldap_quote                    /* quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* LDAP lookup, allowing data from more than one entry to be returned */

  {
  "ldapm",                       /* lookup name */
  lookup_querystyle,             /* query-style lookup */
#ifdef LOOKUP_LDAP
  eldap_open,       /* sic */    /* open function */
  NULL,                          /* check function */
  eldapm_find,                   /* find function */
  NULL,                          /* no close function */
  eldap_tidy,       /* sic */    /* tidy function */
  eldap_quote       /* sic */    /* quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* LDAP lookup, allowing the DN from more one entry to be returned */

  {
  "ldapdn",                       /* lookup name */
  lookup_querystyle,             /* query-style lookup */
#ifdef LOOKUP_LDAP
  eldap_open,       /* sic */    /* open function */
  NULL,                          /* check function */
  eldapdn_find,                  /* find function */
  NULL,                          /* no close function */
  eldap_tidy,       /* sic */    /* tidy function */
  eldap_quote       /* sic */    /* quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* Linear search of single file */

  {
  "lsearch",                     /* lookup name */
  lookup_absfile,                /* uses absolute file name */
#ifdef LOOKUP_LSEARCH
  lsearch_open,                  /* open function */
  lsearch_check,                 /* check function */
  lsearch_find,                  /* find function */
  lsearch_close,                 /* close function */
  NULL,                          /* no tidy function */
  NULL                           /* no quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* MYSQL lookup */

  {
  "mysql",                       /* lookup name */
  lookup_querystyle,             /* query-style lookup */
#ifdef LOOKUP_MYSQL
  mysql_open,                    /* open function */
  NULL,                          /* no check function */
  mysql_find,                    /* find function */
  NULL,                          /* no close function */
  mysql_tidy,                    /* tidy function */
  mysql_quote                    /* quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* NIS lookup, excluding trailing 0 from key */

  {
  "nis",                         /* lookup name */
  0,                             /* not abs file, not query style*/
#ifdef LOOKUP_NIS
  nis_open,                      /* open function */
  NULL,                          /* check function */
  nis_find,                      /* find function */
  NULL,                          /* no close function */
  NULL,                          /* no tidy function */
  NULL                           /* no quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* NIS lookup, including trailing 0 in key */

  {
  "nis0",                        /* lookup name */
  0,                             /* not absfile, not query style */
#ifdef LOOKUP_NIS
  nis_open,    /* sic */         /* open function */
  NULL,                          /* check function */
  nis0_find,                     /* find function */
  NULL,                          /* no close function */
  NULL,                          /* no tidy function */
  NULL                           /* no quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* NIS+ lookup */

  {
  "nisplus",                     /* lookup name */
  lookup_querystyle,             /* query-style lookup */
#ifdef LOOKUP_NISPLUS
  nisplus_open,                  /* open function */
  NULL,                          /* check function */
  nisplus_find,                  /* find function */
  NULL,                          /* no close function */
  NULL,                          /* no tidy function */
  nisplus_quote                  /* quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL /* lookup not present */
#endif
  },

/* PGSQL lookup */

  {
  "pgsql",                       /* lookup name */
  lookup_querystyle,             /* query-style lookup */
#ifdef LOOKUP_PGSQL
  pgsql_open,                    /* open function */
  NULL,                          /* no check function */
  pgsql_find,                    /* find function */
  NULL,                          /* no close function */
  pgsql_tidy,                    /* tidy function */
  pgsql_quote                    /* quoting function */
#else
  NULL, NULL, NULL, NULL, NULL, NULL   /* lookup not present */
#endif
  },

/* Testdb lookup is for testing Exim, not useful for normal running.
For that reason, we omit the entry entirely when not building it into
the binary, so that attempts to use it give "unknown lookup type" instead
of "lookup type not available". */

#ifdef LOOKUP_TESTDB
  {
  "testdb",                      /* lookup name */
  lookup_querystyle,             /* query-style lookup */
  testdb_open,                   /* open function */
  NULL,                          /* check function */
  testdb_find,                   /* find function */
  NULL,                          /* no close function */
  NULL,                          /* no tidy function */
  NULL                           /* no quoting function */
  },
#endif

/* Terminating entry */

{ "", 0, NULL, NULL, NULL, NULL, NULL, NULL }
};


/* Table of information about all possible authentication mechamisms. All
entries are always present if any mechanism is declared, but the functions are
set to NULL for those that are not compiled into the binary. */

#ifdef HAVE_AUTH

#ifdef AUTH_CRAM_MD5
#include "auths/cram_md5.h"
#endif

#ifdef AUTH_PLAINTEXT
#include "auths/plaintext.h"
#endif

auth_info auths_available[] = {

/* Checking by an expansion condition on plain text */

#ifdef AUTH_CRAM_MD5
  {
  "cram_md5",				     /* lookup name */
  auth_cram_md5_server,                      /* server function */
  auth_cram_md5_init,                        /* init function */
  auth_cram_md5_client,                      /* client function */
  auth_cram_md5_options,
  &auth_cram_md5_options_count,
  &auth_cram_md5_option_defaults,
  sizeof(auth_cram_md5_options_block)
  },
#endif

#ifdef AUTH_PLAINTEXT
  {
  "plaintext",				     /* lookup name */
  auth_plaintext_server,                     /* server function */
  auth_plaintext_init,                       /* init function */
  auth_plaintext_client,                     /* client function */
  auth_plaintext_options,
  &auth_plaintext_options_count,
  &auth_plaintext_option_defaults,
  sizeof(auth_plaintext_options_block)
  },
#endif

{ "", NULL, NULL, NULL, NULL, 0, NULL, 0  }
};

#endif  /* HAVE_AUTH */


/* Tables of information about which directors, routers and transports are
included in the exim binary. */

/* Pull in the necessary header files */

#include "directors/dfunctions.h"

#ifdef DIRECTOR_ALIASFILE
#include "directors/aliasfile.h"
#endif

#ifdef DIRECTOR_FORWARDFILE
#include "directors/forwardfile.h"
#endif

#ifdef DIRECTOR_LOCALUSER
#include "directors/localuser.h"
#endif

#ifdef DIRECTOR_SMARTUSER
#include "directors/smartuser.h"
#endif

#ifdef ROUTER_DOMAINLIST
#include "routers/domainlist.h"
#endif

#ifdef ROUTER_IPLITERAL
#include "routers/ipliteral.h"
#endif

#ifdef ROUTER_IPLOOKUP
#include "routers/iplookup.h"
#endif

#ifdef ROUTER_LOOKUPHOST
#include "routers/lookuphost.h"
#endif

#ifdef ROUTER_QUERYPROGRAM
#include "routers/queryprogram.h"
#endif

#ifdef TRANSPORT_APPENDFILE
#include "transports/appendfile.h"
#endif

#ifdef TRANSPORT_AUTOREPLY
#include "transports/autoreply.h"
#endif

#ifdef TRANSPORT_DEBUG
#include "transports/debug.h"
#endif

#ifdef TRANSPORT_LMTP
#include "transports/lmtp.h"
#endif

#ifdef TRANSPORT_PIPE
#include "transports/pipe.h"
#endif

#ifdef TRANSPORT_SMTP
#include "transports/smtp.h"
#endif


/* Now set up the structures, terminated by an entry with a null name. */

director_info directors_available[] = {
#ifdef DIRECTOR_ALIASFILE
  {
  "aliasfile",
  aliasfile_director_entry,
  aliasfile_director_init,
  NULL,    /* no tidyup entry */
  aliasfile_director_options,
  &aliasfile_director_options_count,
  &aliasfile_director_option_defaults,
  sizeof(aliasfile_director_options_block)
  },
#endif
#ifdef DIRECTOR_FORWARDFILE
  {
  "forwardfile",
  forwardfile_director_entry,
  forwardfile_director_init,
  NULL,    /* no tidyup entry */
  forwardfile_director_options,
  &forwardfile_director_options_count,
  &forwardfile_director_option_defaults,
  sizeof(forwardfile_director_options_block)
  },
#endif
#ifdef DIRECTOR_LOCALUSER
  {
  "localuser",
  localuser_director_entry,
  localuser_director_init,
  NULL,    /* no tidyup entry */
  localuser_director_options,
  &localuser_director_options_count,
  &localuser_director_option_defaults,
  sizeof(localuser_director_options_block)
  },
#endif
#ifdef DIRECTOR_SMARTUSER
  {
  "smartuser",
  smartuser_director_entry,
  smartuser_director_init,
  NULL,    /* no tidyup entry */
  smartuser_director_options,
  &smartuser_director_options_count,
  &smartuser_director_option_defaults,
  sizeof(smartuser_director_options_block)
  },
#endif
{ "", NULL, NULL, NULL, NULL, 0, NULL, 0 }
};



router_info routers_available[] = {
#ifdef ROUTER_DOMAINLIST
  {
  "domainlist",
  domainlist_router_entry,
  domainlist_router_init,
  NULL,    /* no tidyup entry */
  domainlist_router_options,
  &domainlist_router_options_count,
  &domainlist_router_option_defaults,
  sizeof(domainlist_router_options_block)
  },
#endif
#ifdef ROUTER_IPLITERAL
  {
  "ipliteral",
  ipliteral_router_entry,
  ipliteral_router_init,
  NULL,    /* no tidyup entry */
  ipliteral_router_options,
  &ipliteral_router_options_count,
  &ipliteral_router_option_defaults,
  sizeof(ipliteral_router_options_block)
  },
#endif
#ifdef ROUTER_IPLOOKUP
  {
  "iplookup",
  iplookup_router_entry,
  iplookup_router_init,
  NULL,    /* no tidyup entry */
  iplookup_router_options,
  &iplookup_router_options_count,
  &iplookup_router_option_defaults,
  sizeof(iplookup_router_options_block)
  },
#endif
#ifdef ROUTER_LOOKUPHOST
  {
  "lookuphost",
  lookuphost_router_entry,
  lookuphost_router_init,
  NULL,    /* no tidyup entry */
  lookuphost_router_options,
  &lookuphost_router_options_count,
  &lookuphost_router_option_defaults,
  sizeof(lookuphost_router_options_block)
  },
#endif
#ifdef ROUTER_QUERYPROGRAM
  {
  "queryprogram",
  queryprogram_router_entry,
  queryprogram_router_init,
  NULL,    /* no tidyup entry */
  queryprogram_router_options,
  &queryprogram_router_options_count,
  &queryprogram_router_option_defaults,
  sizeof(queryprogram_router_options_block)
  },
#endif
{ "", NULL, NULL, NULL, NULL, 0, NULL, 0 }
};



transport_info transports_available[] = {
#ifdef TRANSPORT_APPENDFILE
  {
  "appendfile",                                /* driver name */
  appendfile_transport_entry,                  /* main entry point */
  appendfile_transport_init,                   /* init entry point */
  NULL,                                        /* no tidyup entry */
  appendfile_transport_options,                /* local options table */
  &appendfile_transport_options_count,         /* number of entries */
  &appendfile_transport_option_defaults,       /* private options defaults */
  sizeof(appendfile_transport_options_block),  /* size of private block */
  TRUE,                                        /* local flag */
  NULL                                         /* no closedown entry */
  },
#endif
#ifdef TRANSPORT_AUTOREPLY
  {
  "autoreply",                                 /* driver name */
  autoreply_transport_entry,                   /* main entry point */
  autoreply_transport_init,                    /* init entry point */
  NULL,                                        /* no tidyup entry */
  autoreply_transport_options,                 /* local options table */
  &autoreply_transport_options_count,          /* number of entries */
  &autoreply_transport_option_defaults,        /* private options defaults */
  sizeof(autoreply_transport_options_block),   /* size of private block */
  TRUE,                                        /* local flag */
  NULL                                         /* no closedown entry */
  },
#endif
#ifdef TRANSPORT_LMTP
  {
  "lmtp",                                      /* driver name */
  lmtp_transport_entry,                        /* main entry point */
  lmtp_transport_init,                         /* init entry point */
  NULL,                                        /* no tidyup entry */
  lmtp_transport_options,                      /* local options table */
  &lmtp_transport_options_count,               /* number of entries */
  &lmtp_transport_option_defaults,             /* private options defaults */
  sizeof(lmtp_transport_options_block),        /* size of private block */
  TRUE,                                        /* local flag */
  NULL                                         /* no closedown entry */
  },
#endif
#ifdef TRANSPORT_PIPE
  {
  "pipe",                                      /* driver name */
  pipe_transport_entry,                        /* main entry point */
  pipe_transport_init,                         /* init entry point */
  NULL,                                        /* no tidyup entry */
  pipe_transport_options,                      /* local options table */
  &pipe_transport_options_count,               /* number of entries */
  &pipe_transport_option_defaults,             /* private options defaults */
  sizeof(pipe_transport_options_block),        /* size of private block */
  TRUE,                                        /* local flag */
  NULL                                         /* no closedown entry */
  },
#endif
#ifdef TRANSPORT_SMTP
  {
  "smtp",                                      /* driver name */
  smtp_transport_entry,                        /* main entry point */
  smtp_transport_init,                         /* init entry point */
  NULL,                                        /* no tidyup entry */
  smtp_transport_options,                      /* local options table */
  &smtp_transport_options_count,               /* number of entries */
  &smtp_transport_option_defaults,             /* private options defaults */
  sizeof(smtp_transport_options_block),        /* size of private block */
  FALSE,                                       /* local flag */
  smtp_transport_closedown                     /* close down passed channel */
  },
#endif
{ "", NULL, NULL, NULL, NULL, 0, NULL, 0, FALSE }
};

/* End of drtables.c */
