/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Private structure for the private options. */

typedef struct {
  char *server_secret;
  char *client_secret;
  char *client_name;
} auth_cram_md5_options_block;

/* Data for reading the private options. */

extern optionlist auth_cram_md5_options[];
extern int auth_cram_md5_options_count;

/* Block containing default values. */

extern auth_cram_md5_options_block auth_cram_md5_option_defaults;

/* The entry points for the mechanism */

extern void auth_cram_md5_init(auth_instance *);
extern int auth_cram_md5_server(auth_instance *, char *);
extern int auth_cram_md5_client(auth_instance *, smtp_inblock *,
                                smtp_outblock *, int, char *, int);

/* End of cram_md5.h */
