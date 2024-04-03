/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Private structure for the private options. */

typedef struct {
  char *from;
  char *reply_to;
  char *to;
  char *cc;
  char *bcc;
  char *subject;
  char *headers;
  char *text;
  char *file;
  char *logfile;
  char *oncelog;
  int   mode;
  off_t once_file_size;
  time_t once_repeat;
  BOOL  file_expand;
  BOOL  file_optional;
  BOOL  return_message;
} autoreply_transport_options_block;

/* Data for reading the private options. */

extern optionlist autoreply_transport_options[];
extern int autoreply_transport_options_count;

/* Block containing default values. */

extern autoreply_transport_options_block autoreply_transport_option_defaults;

/* The main and init entry points for the transport */

extern BOOL autoreply_transport_entry(transport_instance *, address_item *);
extern void autoreply_transport_init(transport_instance *);

/* End of transports/autoreply.h */
