/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Private structure for the private options and other private data. */

typedef struct {
  char *hosts;
  char *fallback_hosts;
  host_item *hostlist;
  host_item *fallback_hostlist;
  char *helo_data;
  char *interface;
  char *service;
  char *protocol;
  char *mx_domains;
  char *serialize_hosts;
  char *authenticate_hosts;
  char *hosts_require_tls;
  char *hosts_avoid_tls;
  int   batch_max;
  int   command_timeout;
  int   connect_timeout;
  int   data_timeout;
  int   final_timeout;
  int   size_addition;
  int   max_rcpt;
  int   hosts_max_try;
  BOOL  allow_localhost;
  BOOL  gethostbyname;
  BOOL  dns_qualify_single;
  BOOL  dns_search_parents;
  BOOL  delay_after_cutoff;
  BOOL  hosts_override;
  BOOL  hosts_randomize;
  BOOL  keepalive;
  BOOL  max_rcpt_serialize;
  BOOL  retry_include_ip_address;
  #ifdef SUPPORT_TLS
  char *tls_certificate;
  char *tls_privatekey;
  char *tls_verify_certificates;
  char *tls_verify_ciphers;
  #endif
} smtp_transport_options_block;

/* Data for reading the private options. */

extern optionlist smtp_transport_options[];
extern int smtp_transport_options_count;

/* Block containing default values. */

extern smtp_transport_options_block smtp_transport_option_defaults;

/* The main, init, and closedown entry points for the transport */

extern BOOL smtp_transport_entry(transport_instance *, address_item *);
extern void smtp_transport_init(transport_instance *);
extern void smtp_transport_closedown(transport_instance *);

/* End of transports/smtp.h */
