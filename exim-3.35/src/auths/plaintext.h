/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Private structure for the private options. */

typedef struct {
  char *server_condition;
  char *server_prompts;
  char *client_send;
} auth_plaintext_options_block;

/* Data for reading the private options. */

extern optionlist auth_plaintext_options[];
extern int auth_plaintext_options_count;

/* Block containing default values. */

extern auth_plaintext_options_block auth_plaintext_option_defaults;

/* The entry points for the mechanism */

extern void auth_plaintext_init(auth_instance *);
extern int auth_plaintext_server(auth_instance *, char *);
extern int auth_plaintext_client(auth_instance *, smtp_inblock *,
                                 smtp_outblock *, int, char *, int);

/* End of plaintext.h */
