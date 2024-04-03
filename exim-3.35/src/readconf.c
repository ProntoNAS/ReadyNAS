/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions for reading the configuration file, and for displaying
overall configuration values. */

#include "exim.h"


static char time_buffer[24];
static int configured_debug = -1;
static char *configured_smtp_service = NULL;

static BOOL dummy_collapse_source_routes = FALSE;


/*************************************************
*           Main configuration options           *
*************************************************/

/* The list of options that can be set in the main configuration file. This
must be in alphabetic order because it is searched by binary chop. */

static optionlist optionlist_config[] = {
  { "*set_exim_group",          opt_bool|opt_hidden, &exim_gid_set },
  { "*set_exim_user",           opt_bool|opt_hidden, &exim_uid_set },
  { "*set_message_filter_group",opt_bool|opt_hidden, &message_filter_gid_set },
  { "*set_message_filter_user", opt_bool|opt_hidden, &message_filter_uid_set },
  { "*set_nobody_group",        opt_bool|opt_hidden, &nobody_gid_set },
  { "*set_nobody_user",         opt_bool|opt_hidden, &nobody_uid_set },
  { "accept_8bitmime",          opt_bool,        &accept_8bitmime },
  { "accept_timeout",           opt_time,        &accept_timeout },
  { "admin_groups",             opt_gidlist,     &admin_groups },
  { "allow_mx_to_ip",           opt_bool,        &allow_mx_to_ip },
  { "always_bcc",               opt_bool,        &always_bcc },
#ifdef HAVE_AUTH
  { "auth_always_advertise",    opt_bool,        &auth_always_advertise },
  { "auth_hosts",               opt_stringptr,   &auth_hosts },
#ifdef SUPPORT_TLS
  { "auth_over_tls_hosts",      opt_stringptr,   &auth_over_tls_hosts },
#endif
#endif
  { "auto_thaw",                opt_time,        &auto_thaw },
  { "bi_command",               opt_stringptr,   &bi_command },
  { "check_log_inodes",         opt_int,         &check_log_inodes },
  { "check_log_space",          opt_int,         &check_log_space },
  { "check_spool_inodes",       opt_int,         &check_spool_inodes },
  { "check_spool_space",        opt_int,         &check_spool_space },
  { "collapse_source_routes",   opt_bool,        &dummy_collapse_source_routes },
  { "daemon_smtp_port",         opt_stringptr,   &configured_smtp_service },
  { "daemon_smtp_service",      opt_stringptr|opt_hidden,&configured_smtp_service },
  { "debug_level",              opt_int,         &configured_debug },
  { "delay_warning",            opt_timelist,    &delay_warning },
  { "delay_warning_condition",  opt_stringptr,   &delay_warning_condition },
  { "deliver_load_max",         opt_fixed,       &deliver_load_max },
  { "deliver_queue_load_max",   opt_fixed,       &deliver_queue_load_max },
  { "delivery_date_remove",     opt_bool,        &delivery_date_remove },
  { "dns_again_means_nonexist", opt_stringptr,   &dns_again_means_nonexist },
  { "dns_check_names",          opt_bool,        &check_dns_names },
  { "dns_check_names_pattern",  opt_stringptr,   &check_dns_names_pattern },
  { "dns_ipv4_lookup",          opt_bool,        &ipv4_address_lookup },
  { "dns_retrans",              opt_time,        &dns_retrans },
  { "dns_retry",                opt_int,         &dns_retry },
#ifdef SUPPORT_DSN
  { "dsn",                      opt_bool,        &dsn },
#endif
  { "envelope_to_remove",       opt_bool,        &envelope_to_remove },
  { "errmsg_file",              opt_stringptr,   &errmsg_file },
  { "errmsg_text",              opt_stringptr,   &errmsg_text },
  { "errors_address",           opt_stringptr,   &errors_address },
  { "errors_copy",              opt_stringptr,   &errors_copy },
  { "errors_reply_to",          opt_stringptr,   &errors_reply_to },
  { "exim_group",               opt_gid,         &exim_gid },
  { "exim_path",                opt_stringptr,   &exim_path },
  { "exim_user",                opt_uid,         &exim_uid },
  { "extract_addresses_remove_arguments", opt_bool, &extract_addresses_remove_arguments },
  { "finduser_retries",         opt_int,         &finduser_retries },
  { "forbid_domain_literals",   opt_bool,        &forbid_domain_literals },
  { "freeze_tell_mailmaster",   opt_bool,        &freeze_tell_mailmaster },
  { "gecos_name",               opt_stringptr,   &gecos_name },
  { "gecos_pattern",            opt_stringptr,   &gecos_pattern },
  { "headers_check_syntax",     opt_bool,        &headers_check_syntax },
  { "headers_checks_fail",      opt_bool,        &headers_checks_fail },
  { "headers_sender_verify",    opt_bool,        &headers_sender_verify },
  { "headers_sender_verify_errmsg", opt_bool,    &headers_sender_verify_errmsg },
  { "helo_accept_junk_hosts",   opt_stringptr,   &helo_accept_junk_hosts },
  { "helo_strict_syntax",       opt_bool,        &helo_strict_syntax },
  { "helo_verify",              opt_stringptr,   &helo_verify },
  { "hold_domains",             opt_stringptr,   &hold_domains },
  { "host_accept_relay",        opt_stringptr,   &host_accept_relay },
#ifdef HAVE_AUTH
  { "host_auth_accept_relay",   opt_stringptr,   &host_auth_accept_relay },
#endif
  { "host_lookup",              opt_stringptr,   &host_lookup },
  { "host_reject",              opt_stringptr,   &host_reject },
  { "host_reject_recipients",   opt_stringptr,   &host_reject_recipients },
  { "hosts_treat_as_local",     opt_stringptr,   &hosts_treat_as_local },
  { "ignore_errmsg_errors",     opt_bool,        &ignore_errmsg_errors },
  { "ignore_errmsg_errors_after", opt_time,      &ignore_errmsg_errors_after },
  { "ignore_fromline_hosts",    opt_stringptr,   &ignore_fromline_hosts },
  { "ignore_fromline_local",    opt_bool,        &ignore_fromline_local },
  { "ipv4_address_lookup",      opt_bool,        &ipv4_address_lookup },
  { "keep_malformed",           opt_time,        &keep_malformed },
  { "kill_ip_options",          opt_bool,        &kill_ip_options },
#ifdef LOOKUP_LDAP
  { "ldap_default_servers",     opt_stringptr,   &ldap_default_servers },
#endif
  { "local_domains",            opt_stringptr,   &local_domains },
  { "local_domains_include_host", opt_bool,      &local_domains_include_host },
  { "local_domains_include_host_literals", opt_bool, &local_domains_include_host_literals },
  { "local_from_check",         opt_bool,        &local_from_check },
  { "local_from_prefix",        opt_stringptr,   &local_from_prefix },
  { "local_from_suffix",        opt_stringptr,   &local_from_suffix },
  { "local_interfaces",         opt_stringptr,   &local_interfaces },
  { "localhost_number",         opt_stringptr,   &host_number_string },
  { "locally_caseless",         opt_bool,        &locally_caseless },
  { "log_all_parents",          opt_bool,        &log_all_parents },
  { "log_arguments",            opt_bool,        &log_arguments },
  { "log_file_path",            opt_stringptr,   &log_file_path },
  { "log_incoming_port",        opt_bool,        &log_incoming_port },
  { "log_ip_options",           opt_bool,        &log_ip_options },
  { "log_level",                opt_int,         &log_level },
  { "log_queue_run_level",      opt_int,         &log_queue_run_level },
  { "log_received_recipients",  opt_bool,        &log_received_recipients },
  { "log_received_sender",      opt_bool,        &log_received_sender },
  { "log_refused_recipients",   opt_bool,        &log_refused_recipients },
  { "log_rewrites",             opt_bool,        &log_rewrites },
  { "log_sender_on_delivery",   opt_bool,        &log_sender_on_delivery },
  { "log_smtp_confirmation",    opt_bool,        &log_smtp_confirmation },
  { "log_smtp_connections",     opt_bool,        &log_smtp_connections },
  { "log_smtp_syntax_errors",   opt_bool,        &log_smtp_syntax_errors },
  { "log_subject",              opt_bool,        &log_subject },
  { "lookup_open_max",          opt_int,         &lookup_open_max },
  { "max_username_length",      opt_int,         &max_username_length },
  { "message_body_visible",     opt_mkint,       &message_body_visible },
  { "message_filter",           opt_stringptr,   &message_filter },
  { "message_filter_directory2_transport",opt_stringptr,&message_filter_directory2_transport },
  { "message_filter_directory_transport", opt_stringptr,&message_filter_directory_transport },
  { "message_filter_file_transport",opt_stringptr,&message_filter_file_transport },
  { "message_filter_group",     opt_gid,         &message_filter_gid },
  { "message_filter_pipe_transport",opt_stringptr,&message_filter_pipe_transport },
  { "message_filter_reply_transport",opt_stringptr,&message_filter_reply_transport },
  { "message_filter_user",      opt_uid,         &message_filter_uid },
  { "message_id_header_text",   opt_stringptr,   &message_id_text },
  { "message_size_limit",       opt_mkint,       &message_size_limit },
  { "message_size_limit_count_recipients", opt_bool, &message_size_limit_count_recipients },
#ifdef SUPPORT_MOVE_FROZEN_MESSAGES
  { "move_frozen_messages",     opt_bool,        &move_frozen_messages },
#endif
#ifdef LOOKUP_MYSQL
  { "mysql_servers",            opt_stringptr,   &mysql_servers },
#endif
  { "never_users",              opt_uidlist,     &never_users },
  { "nobody_group",             opt_gid,         &nobody_gid },
  { "nobody_user",              opt_uid,         &nobody_uid },
  { "percent_hack_domains",     opt_stringptr,   &percent_hack_domains },
#ifdef EXIM_PERL
  { "perl_at_start",            opt_bool,        &opt_perl_at_start },
  { "perl_startup",             opt_stringptr,   &opt_perl_startup },
#endif
#ifdef LOOKUP_PGSQL
  { "pgsql_servers",            opt_stringptr,   &pgsql_servers },
#endif
  { "pid_file_path",            opt_stringptr,   &pid_file_path },
  { "preserve_message_logs",    opt_bool,        &preserve_message_logs },
  { "primary_hostname",         opt_stringptr,   &primary_hostname },
  { "print_topbitchars",        opt_bool,        &print_topbitchars },
  { "prod_requires_admin",      opt_bool,        &prod_requires_admin },
  { "prohibition_message",      opt_stringptr,   &prohibition_message },
  { "qualify_domain",           opt_stringptr,   &qualify_domain_sender },
  { "qualify_recipient",        opt_stringptr,   &qualify_domain_recipient },
  { "queue_list_requires_admin",opt_bool,        &queue_list_requires_admin },
  { "queue_only",               opt_bool,        &queue_only },
  { "queue_only_file",          opt_stringptr,   &queue_only_file },
  { "queue_only_load",          opt_fixed,       &queue_only_load },
  { "queue_remote_domains",     opt_stringptr,   &queue_remote_domains },
  { "queue_run_in_order",       opt_bool,        &queue_run_in_order },
  { "queue_run_max",            opt_int,         &queue_run_max },
  { "queue_smtp_domains",       opt_stringptr,   &queue_smtp_domains },
  { "rbl_domains",              opt_stringptr,   &rbl_domains },
  { "rbl_hosts",                opt_stringptr,   &rbl_hosts },
  { "rbl_log_headers",          opt_bool,        &rbl_log_headers },
  { "rbl_log_rcpt_count",       opt_bool,        &rbl_log_rcpt_count },
  { "rbl_reject_recipients",    opt_bool,        &rbl_reject_recipients },
  { "rbl_warn_header",          opt_bool,        &rbl_warn_header },
  { "received_header_text",     opt_stringptr,   &received_header_text },
  { "received_headers_max",     opt_int,         &received_headers_max },
  { "receiver_try_verify",      opt_bool,        &receiver_try_verify },
  { "receiver_unqualified_hosts", opt_stringptr, &receiver_unqualified_hosts },
  { "receiver_verify",          opt_bool,        &receiver_verify },
  { "receiver_verify_addresses", opt_stringptr,  &receiver_verify_addresses },
  { "receiver_verify_hosts",    opt_stringptr,   &receiver_verify_hosts },
  { "receiver_verify_senders",  opt_stringptr,   &receiver_verify_senders },
  { "recipients_max",           opt_int,         &recipients_max },
  { "recipients_max_reject",    opt_bool,        &recipients_max_reject },
  { "recipients_reject_except", opt_stringptr,   &recipients_reject_except },
  { "recipients_reject_except_senders", opt_stringptr, &recipients_reject_except_senders },
  { "refuse_ip_options",        opt_bool,        &refuse_ip_options },
  { "relay_domains",            opt_stringptr,   &relay_domains },
  { "relay_domains_include_local_mx", opt_bool,  &relay_domains_include_local_mx },
  { "relay_match_host_or_sender", opt_bool,      &relay_need_either },
  { "remote_max_parallel",      opt_int,         &remote_max_parallel },
  { "remote_sort",              opt_stringptr,   &remote_sort },
  { "retry_data_expire",        opt_time,        &retry_data_expire },
  { "retry_interval_max",       opt_time,        &retry_interval_max },
  { "return_path_remove",       opt_bool,        &return_path_remove },
  { "return_size_limit",        opt_mkint,       &return_size_limit },
  { "rfc1413_hosts",            opt_stringptr,   &rfc1413_hosts },
  { "rfc1413_query_timeout",    opt_time,        &rfc1413_query_timeout },
  { "security",                 opt_stringptr,   &security_type },
  { "sender_address_relay",     opt_stringptr,   &sender_address_relay },
  { "sender_address_relay_hosts", opt_stringptr, &sender_address_relay_hosts },
  { "sender_reject",            opt_stringptr,   &sender_reject },
  { "sender_reject_recipients", opt_stringptr,   &sender_reject_recipients },
  { "sender_try_verify",        opt_bool,        &sender_try_verify },
  { "sender_unqualified_hosts", opt_stringptr,   &sender_unqualified_hosts },
  { "sender_verify",            opt_bool,        &sender_verify },
  { "sender_verify_batch",      opt_bool,        &sender_verify_batch },
  { "sender_verify_callback_domains", opt_stringptr, &sender_verify_callback_domains },
  { "sender_verify_callback_timeout", opt_time,  &sender_verify_callback_timeout },
  { "sender_verify_fixup",      opt_bool,        &sender_verify_fixup },
  { "sender_verify_hosts",      opt_stringptr,   &sender_verify_hosts },
  { "sender_verify_hosts_callback", opt_stringptr, &sender_verify_hosts_callback },
  { "sender_verify_max_retry_rate", opt_int,     &sender_verify_max_retry_rate },
  { "sender_verify_reject",     opt_bool,        &sender_verify_reject },
  { "smtp_accept_keepalive",    opt_bool,        &smtp_accept_keepalive },
  { "smtp_accept_max",          opt_int,         &smtp_accept_max },
  { "smtp_accept_max_per_host", opt_int,         &smtp_accept_max_per_host },
  { "smtp_accept_queue",        opt_int,         &smtp_accept_queue },
  { "smtp_accept_queue_per_connection", opt_int, &smtp_accept_queue_per_connection },
  { "smtp_accept_reserve",      opt_int,         &smtp_accept_reserve },
  { "smtp_banner",              opt_stringptr,   &smtp_banner },
  { "smtp_check_spool_space",   opt_bool,        &smtp_check_spool_space },
  { "smtp_connect_backlog",     opt_int,         &smtp_connect_backlog },
  { "smtp_etrn_command",        opt_stringptr,   &smtp_etrn_command },
  { "smtp_etrn_hosts",          opt_stringptr,   &smtp_etrn_hosts },
  { "smtp_etrn_serialize",      opt_bool,        &smtp_etrn_serialize },
  { "smtp_expn_hosts",          opt_stringptr,   &smtp_expn_hosts },
  { "smtp_load_reserve",        opt_fixed,       &smtp_load_reserve },
  { "smtp_receive_timeout",     opt_time,        &smtp_receive_timeout },
  { "smtp_reserve_hosts",       opt_stringptr,   &smtp_reserve_hosts },
  { "smtp_verify",              opt_bool,        &smtp_verify },
  { "split_spool_directory",    opt_bool,        &split_spool_directory },
  { "spool_directory",          opt_stringptr,   &spool_directory },
  { "strip_excess_angle_brackets", opt_bool,     &strip_excess_angle_brackets },
  { "strip_trailing_dot",       opt_bool,        &strip_trailing_dot },
  { "syslog_timestamp",         opt_bool,        &syslog_timestamp },
  { "timeout_frozen_after",     opt_time,        &timeout_frozen_after },
  { "timestamps_utc",           opt_bool,        &timestamps_utc },
  { "timezone",                 opt_stringptr,   &timezone_string },
#ifdef SUPPORT_TLS
  { "tls_advertise_hosts",      opt_stringptr,   &tls_advertise_hosts },
  { "tls_certificate",          opt_stringptr,   &tls_certificate },
  { "tls_dhparam",              opt_stringptr,   &tls_dhparam },
  { "tls_host_accept_relay",    opt_stringptr,   &tls_host_accept_relay },
  { "tls_hosts",                opt_stringptr,   &tls_hosts },
  { "tls_log_cipher",           opt_bool,        &tls_log_cipher },
  { "tls_log_peerdn",           opt_bool,        &tls_log_peerdn },
  { "tls_privatekey",           opt_stringptr,   &tls_privatekey },
  { "tls_verify_certificates",  opt_stringptr,   &tls_verify_certificates },
  { "tls_verify_ciphers",       opt_stringptr,   &tls_verify_ciphers },
  { "tls_verify_hosts",         opt_stringptr,   &tls_verify_hosts },
#endif
  { "trusted_groups",           opt_gidlist,     &trusted_groups },
  { "trusted_users",            opt_uidlist,     &trusted_users },
  { "unknown_login",            opt_stringptr,   &unknown_login },
  { "unknown_username",         opt_stringptr,   &unknown_username },
  { "untrusted_set_sender",     opt_bool,        &untrusted_set_sender },
  { "uucp_from_pattern",        opt_stringptr,   &uucp_from_pattern },
  { "uucp_from_sender",         opt_stringptr,   &uucp_from_sender },
  { "warnmsg_file",             opt_stringptr,   &warnmsg_file }
};

static int optionlist_config_size =
  sizeof(optionlist_config)/sizeof(optionlist);





/*************************************************
*         Find the name of an option             *
*************************************************/

/* This function is to aid debugging. Various functions take arguments that are
variables in the options table above. For debugging output, it is useful to be
able to find the name of the option which is currently being processed. This
function finds it, if it exists, by searching the table.

Arguments:   a value that is presumed to be in the table above
Returns:     the option name, or an empty string
*/

char *
readconf_find_option(void *p)
{
int i;
for (i = 0; i < optionlist_config_size; i++)
  if (p == optionlist_config[i].value) return optionlist_config[i].name;
return "";
}





/*************************************************
*            Read configuration line             *
*************************************************/

/* A logical line of text is read from the configuration file into the big
buffer, taking account of continuations. The size of big_buffer is increased if
necessary. The count of configuration lines is maintained. Physical input lines
starting with # (ignoring leading white space) and empty logical lines are
always ignored. Leading and trailing spaces are removed and the line is scanned
for macros, which are replaced.

Arguments:      none

Returns:        a pointer to the first non-blank in the line,
                or NULL if eof is reached
*/

static char *
get_config_line(void)
{
int len = 0;
int startoffset = 0;
int newlen;
char *s, *ss;
macro_item *m;

/* Loop for handling continuation lines and skipping comments. */

for (;;)
  {
  if (fgets(big_buffer+len, big_buffer_size-len, config_file) == NULL)
    {
    if (len == 0) return NULL;  /* EOF at start of logical line */
    break;                      /* EOF after continuation */
    }

  config_lineno++;
  newlen = len + (int)strlen(big_buffer + len);

  /* Handle pathologically long physical lines - yes, it did happen - by
  extending big_buffer at this point. The code also copes with very long
  logical lines. */

  while (newlen == big_buffer_size - 1 && big_buffer[newlen - 1] != '\n')
    {
    char *newbuffer;
    big_buffer_size += BIG_BUFFER_SIZE;
    newbuffer = store_malloc(big_buffer_size);
    strcpy(newbuffer, big_buffer);
    store_free(big_buffer);
    big_buffer = newbuffer;
    if (fgets(big_buffer+newlen, big_buffer_size-newlen, config_file) == NULL)
      break;
    newlen += (int)strlen(big_buffer + newlen);
    }

  /* Find the true start of the physical line - leading spaces are always
  ignored. */

  s = ss = big_buffer + len;
  while (isspace((uschar)*ss)) ss++;

  /* Check for comment lines - these are physical lines. */

  if (*ss == '#') continue;

  /* If this is the start of the logical line, remember where the non-blank
  data starts. Otherwise shuffle down continuation lines to remove leading
  white space */

  if (len == 0)
    startoffset = ss - big_buffer;
  else
    {
    if (ss != s)
      {
      memmove(s, ss, big_buffer + newlen - ss);
      newlen -= ss - s;
      }
    }

  /* Accept the new addition to the line. Remove trailing white space. */

  len = newlen;
  while (len > 0 && isspace((uschar)big_buffer[len-1])) len--;
  big_buffer[len] = 0;

  /* We are done if the line does not end in backslash and contains some data.
  Empty logical lines are ignored. For continuations, remove the backslash and
  go round the loop to read the continuation line. */

  if (len > 0)
    {
    if (big_buffer[len-1] != '\\') break;
    big_buffer[--len] = 0;
    }
  }     /* Loop for reading physical lines */

/* Handle macro expansion. This is done very simply. However, we must take care
not to expand a macro name at the start of a definition. */

s = big_buffer + startoffset;
ss = big_buffer + len;

for (m = macros; m != NULL; m = m->next)
  {
  char *p, *pp;
  char *t = s;

  /* Skip over any line that starts with an upper case character followed by
  a sequence of name characters and an equals sign, because that is the
  definition of a new macro. */

  if (isupper((uschar)*t))
    {
    while (isalnum((uschar)*t) || *t == '_') t++;
    while (isspace((uschar)*t)) t++;
    if (*t != '=') t = s;
    }

  while ((p = strstr(t, m->name)) != NULL)
    {
    int moveby;
    int namelen = (int)strlen(m->name);
    int replen = (int)strlen(m->replacement);

    /* Expand the buffer if necessary */

    while (ss - big_buffer - namelen + replen + 1 > big_buffer_size)
      {
      int newsize = big_buffer_size + BIG_BUFFER_SIZE;
      char *newbuffer = store_malloc(newsize);
      strcpy(newbuffer, big_buffer);
      s = newbuffer  + (s - big_buffer);
      ss = newbuffer + (ss - big_buffer);
      t = newbuffer  + (t - big_buffer);
      p = newbuffer  + (p - big_buffer);
      big_buffer_size = newsize;
      store_free(big_buffer);
      big_buffer = newbuffer;
      }

    /* Shuffle the remaining characters up or down in the buffer before
    copying in the replacement text. Don't rescan the replacement for this same
    macro. */

    pp = p + namelen;
    moveby = replen - namelen;
    if (moveby != 0)
      {
      memmove(p + replen, pp, ss - pp + 1);
      ss += moveby;
      }
    strncpy(p, m->replacement, replen);
    t = p + replen;
    }
  }

/* Return the first non-blank character. */

return s;
}



/*************************************************
*             Read a name                        *
*************************************************/

/* The yield is the pointer to the next char. Names longer than the
output space are silently truncated.

Arguments:
  name      where to put the name
  len       length of name
  s         input pointer

Returns:    new input pointer
*/

static char *
readconf_readname(char *name, int len, char *s)
{
int p = 0;
while (isspace((uschar)*s)) s++;
if (isalpha((uschar)*s))
  {
  while (isalnum((uschar)*s) || *s == '_')
    {
    if (p < len-1) name[p++] = *s;
    s++;
    }
  }
name[p] = 0;
while (isspace((uschar)*s)) s++;
return s;
}




/*************************************************
*          Read a time value                     *
*************************************************/

/* This function is also called from outside, to read argument
time values. The format of a time value is:

  [<n>w][<n>d][<n>h][<n>m][<n>s]

as long as at least one is present. If a format error is encountered,
return a negative value. The value must be terminated by the given
terminator.

Arguments:
  s           input pointer
  terminator  required terminating character

Returns:      the time value, in seconds, or -1 on syntax error
*/

int
readconf_readtime(char *s, int terminator)
{
int yield = 0;
for (;;)
  {
  int value, count;
  if (!isdigit((uschar)*s)) return -1;
  (void)sscanf(s, "%d%n", &value, &count);
  s += count;
  switch (*s)
    {
    case 'w': value *= 7;
    case 'd': value *= 24;
    case 'h': value *= 60;
    case 'm': value *= 60;
    case 's': s++;
    break;

    default: return -1;
    }
  yield += value;
  if (*s == terminator) return yield;
  }
/* Control never reaches here. */
}



/*************************************************
*          Read a fixed point value              *
*************************************************/

/* The value is returned *1000

Arguments:
  s           input pointer
  terminator  required terminator

Returns:      the value, or -1 on error
*/

static int
readconf_readfixed(char *s, int terminator)
{
int yield = 0;
int value, count;
if (!isdigit((uschar)*s)) return -1;
(void)sscanf(s, "%d%n", &value, &count);
s += count;
yield = value * 1000;
if (*s == '.')
  {
  int m = 100;
  while (isdigit((uschar)(*(++s))))
    {
    yield += (*s - '0') * m;
    m /= 10;
    }
  }

return (*s == terminator)? yield : (-1);
}



/*************************************************
*            Find option in list                 *
*************************************************/

/* The lists are always in order, so binary chop can be used.

Arguments:
  name      the option name to search for
  ol        the first entry in the option list
  last      one more than the offset of the last entry in the option list

Returns:    pointer to an option entry, or NULL if not found
*/

static optionlist *
find_option(char *name, optionlist *ol, int last)
{
int first = 0;
while (last > first)
  {
  int middle = (first + last)/2;
  int c = strcmp(name, ol[middle].name);
  if (c == 0) return ol + middle;
    else if (c > 0) first = middle + 1;
      else last = middle;
  }
return NULL;
}



/*************************************************
*      Find a set flag in option list            *
*************************************************/

/* Because some versions of Unix make no restrictions on the values of uids and
gids (even negative ones), we cannot represent "unset" by a special value.
There is therefore a separate boolean variable for each one indicating whether
a value is set or not. This function returns a pointer to the boolean, given
the original option name. It is a major disaster if the flag cannot be found.

Arguments:
  name          the name of the uid or gid option
  oltop         points to the start of the relevant option list
  last          one more than the offset of the last item in the option list
  data_block    NULL when reading main options => data values in the option
                  list are absolute addresses; otherwise they are byte offsets
                  in data_block (used for driver options)

Returns:        a pointer to the boolean flag.
*/

static BOOL *
get_set_flag(char *name, optionlist *oltop, int last, void *data_block)
{
optionlist *ol;
char name2[64];
sprintf(name2, "*set_%.50s", name);
ol = find_option(name2, oltop, last);
if (ol == NULL) log_write(0, LOG_PANIC_DIE,
  "Exim internal error: missing set flag for %s", name);
return (data_block == NULL)? (BOOL *)(ol->value) :
  (BOOL *)((char *)data_block + (long int)(ol->value));
}




/*************************************************
*              Read rewrite information          *
*************************************************/

/* Each line of rewrite information contains:

.  A complete address in the form user@domain, possibly with
   leading * for each part; or alternatively, a regex.

.  A replacement string (which will be expanded).

.  An optional sequence of one-letter flags, indicating which
   headers etc. to apply this rule to.

All this is decoded and placed into a control block. The OR of the flags is
maintained in a common word.

Arguments:
  p           points to the string that makes up the rule
  existflags  points to the overall flag word
  isglobal    TRUE if reading global rewrite rules

Returns:      the control block for the parsed rule.
*/

static rewrite_rule *
readconf_one_rewrite(char *p, int *existflags, BOOL isglobal)
{
char *pp = p;
rewrite_rule *next = store_get(sizeof(rewrite_rule));
next->next = NULL;

while (*p != 0 && !isspace((uschar)*p)) p++;
next->key = string_copyn(pp, p-pp);

/* A non-regular expression must contain a local-part and a domain,
unless is is a single file lookup. */

if (pp[0] != '^' && strchr(next->key, ';') == NULL)
  {
  pp = strchr(next->key, '@');
  if (pp == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "domain missing in rewrite key string \"%s\" in line %d",
        next->key, config_lineno);
  }

while (isspace((uschar)*p)) p++;
if (*p == 0)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "missing rewrite replacement string in line %d", config_lineno);

next->flags = 0;

/* The replacement is permitted to be entirely in quotes, to enable
it to contain white space, which was not envisaged when I first wrote
this code. Note that concatenation has been handled above. Pity I didn't
organize a better "read string in quotes" function for Exim at the start,
but rather than stir things now, just do the necessary here. */

if (*p == '\"')
  {
  int size = 128;
  int count = 0;
  char *sptr = store_get(size);
  while (*(++p) != 0)
    {
    if (*p == '\"') { p++; break; }
    if (*p == '\\' && p[1] != 0)
      {
      char ch = string_interpret_escape(&p);
      sptr = string_cat(sptr, &size, &count, &ch, 1);
      }
    else sptr = string_cat(sptr, &size, &count, p, 1);
    }
  sptr[count] = 0;
  next->replacement = sptr;
  }

/* Not in quotes; terminate on white space */

else
  {
  pp = p;
  while (*p != 0 && !isspace((uschar)*p)) p++;
  next->replacement = string_copyn(pp, p-pp);
  }

while (*p != 0) switch (*p++)
  {
  case ' ': case '\t': break;

  case 'q': next->flags |= rewrite_quit; break;
  case 'w': next->flags |= rewrite_whole; break;

  case 'h': next->flags |= rewrite_all_headers; break;
  case 's': next->flags |= rewrite_sender; break;
  case 'f': next->flags |= rewrite_from; break;
  case 't': next->flags |= rewrite_to;   break;
  case 'c': next->flags |= rewrite_cc;   break;
  case 'b': next->flags |= rewrite_bcc;  break;
  case 'r': next->flags |= rewrite_replyto; break;

  case 'E': next->flags |= rewrite_all_envelope; break;
  case 'F': next->flags |= rewrite_envfrom; break;
  case 'T': next->flags |= rewrite_envto; break;

  case 'Q': next->flags |= rewrite_qualify; break;
  case 'R': next->flags |= rewrite_repeat; break;

  case 'X': next->flags |= rewrite_X; break;

  case 'S':
  next->flags |= rewrite_smtp;
  if (next->key[0] != '^')
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "rewrite rule in line %d has the S flag but is not a regular "
      "expression", config_lineno);
  break;

  default:
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "unknown rewrite flag character (%c) in line %d "
      "(could be missing quotes round replacement item)",
      p[-1], config_lineno);
  }

/* If no action flags are set, set all the "normal" rewrites. */

if ((next->flags & (rewrite_all | rewrite_smtp | rewrite_X)) == 0)
  next->flags |= isglobal? rewrite_all : rewrite_all_headers;

/* Remember which exist, for optimization, and return the rule */

*existflags |= next->flags;
return next;
}




/*************************************************
*          Read global rewrite information       *
*************************************************/

/* Each line is a single rewrite rule; it is parsed into a control block
by readconf_one_rewrite(), and its flags are ORed into the global flag
word rewrite_existflags. */

void
readconf_rewrites(void)
{
rewrite_rule **chain = &global_rewrite_rules;
char *p;

while ((p = get_config_line()) != NULL && strcmpic(p, "end") != 0)
  {
  rewrite_rule *next = readconf_one_rewrite(p, &rewrite_existflags, TRUE);
  *chain = next;
  chain = &(next->next);
  }
}



/*************************************************
*            Handle option line                  *
*************************************************/

/* This function is called from several places to process a line containing the
setting of an option. The first argument is the line to be decoded; it has been
checked not to be empty and not to start with '#'. Trailing newlines and white
space have been removed. The second argument is a pointer to the list of
variable names that are to be recognized, together with their types and
locations, and the third argument gives the number of entries in the list.

The fourth argument is a pointer to a data block. If it is NULL, then the data
values in the options list are absolute addresses. Otherwise, they are byte
offsets in the data block.

String option data may continue onto several lines; this function reads further
data from config_file if necessary.

The yield of this function is normally zero. If a string continues onto
multiple lines, then the data value is permitted to be followed by a comma
or a semicolon (for use in drivers) and the yield is that character.

Arguments:
  buffer        contains the configuration line to be handled
  oltop         points to the start of the relevant option list
  last          one more than the offset of the last item in the option list
  data_block    NULL when reading main options => data values in the option
                  list are absolute addresses; otherwise they are byte offsets
                  in data_block when they have opt_public set; otherwise
                  they are byte offsets in data_block->options_block.
  unknown_txt   format string to use in panic message for unknown option;
                  must contain %s for option name and %d for line number
                if given as NULL, don't panic on unknown option

Returns:        TRUE if an option was read successfully,
                FALSE false for an unknown option if unknown_txt == NULL,
                  otherwise panic and die on an unknown option
*/

static BOOL
readconf_handle_option(char *buffer, optionlist *oltop, int last,
  void *data_block, char *unknown_txt)
{
int ptr = 0;
int offset = 0;
int n, count, type, value;
int issecure = 0;
uid_t uid;
gid_t gid;
BOOL boolvalue = TRUE;
BOOL freesptr = TRUE;
optionlist *ol, *ol2;
transport_instance *tp;
struct passwd *pw;
void *reset_point;
char *intformat = "%i%n";
char *inttype = "";
char *sptr;
char *s = buffer;
char name[64];
char name2[64];


/* There may be leading spaces */

while (isspace((uschar)*s)) s++;

/* Read the name of the option, and skip any subsequent white space. If
it turns out that what we read was "hide", set the flag indicating that
this is a secure option, and loop to read the next word. */

for (n = 0; n < 2; n++)
  {
  while (isalnum((uschar)*s) || *s == '_')
    {
    if (ptr < sizeof(name)-1) name[ptr++] = *s;
    s++;
    }
  name[ptr] = 0;
  while (isspace((uschar)*s)) s++;
  if (strcmp(name, "hide") != 0) break;
  issecure = opt_secure;
  ptr = 0;
  }

/* Deal with "no_" or "not_" here for booleans */

if (strncmp(name, "no_", 3) == 0)
  {
  boolvalue = FALSE;
  offset = 3;
  }

if (strncmp(name, "not_", 4) == 0)
  {
  boolvalue = FALSE;
  offset = 4;
  }

/* Search the list for the given name. A non-existent name, or an option that
is set twice, is a disaster. */

ol = find_option(name + offset, oltop, last);

if (ol == NULL)
  {
  if (unknown_txt == NULL) return FALSE;
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG, unknown_txt, name, config_lineno);
  }

if ((ol->type & opt_set) != 0)
  {
  char *mname = name;
  if (strncmp(mname, "no_", 3) == 0) mname += 3;
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "\"%s\" option set for the second "
    "time in line %d", mname, config_lineno);
  }

ol->type |= opt_set | issecure;
type = ol->type & opt_mask;

/* Types with data values must be followed by '='; the "no[t]_" prefix
applies only to boolean values. */

if (type != opt_bool && type != opt_bool_verify && type != opt_bool_set)
  {
  if (offset != 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "negation prefix applied to a non-boolean option in line %d",
      config_lineno);
  if (*s == 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "unexpected end of line (data missing) after %s in line %d", name,
      config_lineno);
  if (*s != '=')
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "missing \"=\" after %s in line %d", name, config_lineno);
  }

/* If a boolean wasn't preceded by "no[t]_" it can be followed by = and
true/false/yes/no */

else if (*s != 0)
  {
  if (offset != 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "extra characters follow boolean value "
      "for %s in line %d", name, config_lineno);
  if (*s != '=')
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "missing \"=\" after %s in line %d", name, config_lineno);
  }

/* If there is a data block and the opt_public flag is not set, change
the data block pointer to the private options block. */

if (data_block != NULL && (ol->type & opt_public) == 0)
  data_block = (void *)(((driver_instance *)data_block)->options_block);

/* Skip white space after = */

while (isspace((uschar)(*(++s))));

/* Now get the data according to the type. */

switch (type)
  {
  /* If a string value is not enclosed in quotes, it consists of
  the rest of the current line, verbatim. Otherwise, string escapes
  are processed.

  A transport is specified as a string, which is then looked up in the
  list of transports. A search type is specified as one of a number of
  known strings.

  A set or rewrite rules for a driver is specified as a string, which is
  then parsed into a suitable chain of control blocks.

  Uids and gids are specified as strings which are then looked up in the
  passwd file. Lists of uids and gids are similarly specified as colon-
  separated strings. */

  case opt_stringptr:
  case opt_uid:
  case opt_gid:
  case opt_expand_uid:
  case opt_expand_gid:
  case opt_uidlist:
  case opt_gidlist:
  case opt_searchtype:
  case opt_transportptr:
  case opt_rewrite:
  case opt_local_batch:

  /* Strings are read into the normal store pool. As long we aren't too
  near the end of the current block, the string will just use what is necessary
  on the top of the stacking pool, because string_cat() uses the extension
  mechanism. If keeping the actual string is not required, freesptr is set
  TRUE, and at the end we reset the pool. */

  n = (int)strlen(s) + 1;
  reset_point = sptr = store_get(n);
  *sptr = 0;                                /* insurance */

  if (*s != '\"') strcpy(sptr, s); else
    {
    int count = 0;

    for(;;)
      {
      if (*(++s) == 0)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "missing quote at end of string value for %s in line %d",
          name, config_lineno);

      if (*s == '\"')
        {
        if (*(++s) == 0) break;
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "extra characters follow string value "
          "for %s in line %d", name, config_lineno);
        }

      /* Still in the string. Handle escape characters. If nothing follows
      the \ we do nothing. The main loop then discovers an unexpected eol
      and fails. */

      else if (*s == '\\')
        {
        if (s[1] != 0)
          {
          char ch = string_interpret_escape(&s);
          sptr = string_cat(sptr, &n, &count, &ch, 1);
          }
        }

      /* Neither " nor \ so just add to the string */

      else sptr = string_cat(sptr, &n, &count, s, 1);
      }

    /* End of string scanning loop. Terminate the string; string_cat always
    leaves room */

    sptr[count] = 0;
    }

  /* Having read a string, we now have several different ways of using it,
  depending on the data type, so do another switch. */

  switch (type)
    {
    /* If this was a string, set the variable to point to the new string,
    and set the flag so its store isn't reclaimed. If it was a list of rewrite
    rules, we still keep the string (for printing), and parse the rules into a
    control block and flags word. */

    case opt_stringptr:
    case opt_rewrite:
    if (data_block == NULL)
      *((char **)(ol->value)) = sptr;
    else
      *((char **)((char *)data_block + (long int)(ol->value))) = sptr;
    freesptr = FALSE;
    if (type == opt_rewrite)
      {
      int sep = 0;
      int *flagptr;
      char *p = sptr;
      rewrite_rule **chain;
      optionlist *ol3;

      sprintf(name2, "*%.50s_rules", name);
      ol2 = find_option(name2, oltop, last);
      sprintf(name2, "*%.50s_flags", name);
      ol3 = find_option(name2, oltop, last);

      if (ol2 == NULL || ol3 == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "rewrite rules not available for driver in line %d",
            config_lineno);

      if (data_block == NULL)
        {
        chain = (rewrite_rule **)(ol2->value);
        flagptr = (int *)(ol3->value);
        }
      else
        {
        chain = (rewrite_rule **)((char *)data_block + (long int)(ol2->value));
        flagptr = (int *)((char *)data_block + (long int)(ol3->value));
        }

      while ((p = string_nextinlist(&sptr, &sep, big_buffer, BIG_BUFFER_SIZE))
              != NULL)
        {
        rewrite_rule *next = readconf_one_rewrite(p, flagptr, FALSE);
        *chain = next;
        chain = &(next->next);
        }

      if ((*flagptr & (rewrite_all_envelope | rewrite_smtp | rewrite_X)) != 0)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "rewrite rule specifies a non-header rewrite - not allowed at "
          "transport time - in line %d", config_lineno);
      }
    break;

    /* If it was a transport - look it up in the transports list unless
    the string contains a $ character, which implies expansion at directing
    or routing time. In that case, save the string in the corresponding
    *expand_transport field. */

    case opt_transportptr:
    if (strchr(sptr, '$') == NULL)
      {
      for (tp = transports; tp != NULL; tp = tp->next)
        {
        if (strcmp(tp->name, sptr) == 0)
          {
          if (data_block == NULL)
            *((transport_instance **)(ol->value)) = tp;
          else
            *((transport_instance **)((char *)data_block +
              (long int)(ol->value))) = tp;
          break;
          }
        }
      if (tp == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "%s transport, referred to in line %d, "
          "was not found", sptr, config_lineno);
      }

    /* Handle expanded transport */

    else
      {
      sprintf(name2, "*expand_%.50s", name);
      ol2 = find_option(name2, oltop, last);
      if (ol2 == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "expanded transport not available for driver in line %d",
            config_lineno);
      if (data_block == NULL)
        *((char **)(ol2->value)) = sptr;
      else
        *((char **)((char *)data_block + (long int)(ol2->value))) = sptr;
      freesptr = FALSE;
      }
    break;

    /* If it was an expanded uid, see if there is any expansion to be
    done by checking for the presence of a $ character. If there is, save it
    in the corresponding *expand_user option field. Otherwise, fall through
    to treat it as a fixed uid. Ensure mutual exclusivity of the two kinds
    of data. */

    case opt_expand_uid:
    sprintf(name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      char *ss = (strchr(sptr, '$') != NULL)? sptr : NULL;

      if (data_block == NULL)
        *((char **)(ol2->value)) = ss;
      else
        *((char **)((char *)data_block + (long int)(ol2->value))) = ss;

      if (ss != NULL)
        {
        *(get_set_flag(name, oltop, last, data_block)) = FALSE;
        freesptr = FALSE;
        break;
        }
      }

    /* Look up a fixed uid, and also make use of the corresponding gid
    if a passwd entry is returned and the gid has not been set. */

    case opt_uid:
    if (!direct_finduser(sptr, &pw, &uid))
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
        "user %s, referred to in line %d, "
        "was not found", sptr, config_lineno);
    if (data_block == NULL)
      *((uid_t *)(ol->value)) = uid;
    else
      *((uid_t *)((char *)data_block + (long int)(ol->value))) = uid;

    /* Set the flag indicating a fixed value is set */

    *(get_set_flag(name, oltop, last, data_block)) = TRUE;

    /* Handle matching gid if we have a passwd entry: done by finding the
    same name with terminating "user" changed to "group"; if not found,
    ignore. Also ignore if the value is already set. */

    if (pw == NULL) break;
    strcpy(name+(int)strlen(name)-4, "group");
    ol2 = find_option(name, oltop, last);
    if (ol2 != NULL && ((ol2->type & opt_mask) == opt_gid ||
        (ol2->type & opt_mask) == opt_expand_gid))
      {
      BOOL *set_flag = get_set_flag(name, oltop, last, data_block);
      if (! *set_flag)
        {
        if (data_block == NULL)
          *((gid_t *)(ol2->value)) = pw->pw_gid;
        else
          *((gid_t *)((char *)data_block + (long int)(ol2->value))) = pw->pw_gid;
        *set_flag = TRUE;
        }
      }
    break;

    /* If it was an expanded gid, see if there is any expansion to be
    done by checking for the presence of a $ character. If there is, save it
    in the corresponding *expand_user option field. Otherwise, fall through
    to treat it as a fixed gid. Ensure mutual exclusivity of the two kinds
    of data. */

    case opt_expand_gid:
    sprintf(name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      char *ss = (strchr(sptr, '$') != NULL)? sptr : NULL;

      if (data_block == NULL)
        *((char **)(ol2->value)) = ss;
      else
        *((char **)((char *)data_block + (long int)(ol2->value))) = ss;

      if (ss != NULL)
        {
        *(get_set_flag(name, oltop, last, data_block)) = FALSE;
        freesptr = FALSE;
        break;
        }
      }

    /* Handle freestanding gid */

    case opt_gid:
    if (!direct_findgroup(sptr, &gid))
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
        "group %s, referred to in line %d, "
        "was not found", sptr, config_lineno);
    if (data_block == NULL)
      *((gid_t *)(ol->value)) = gid;
    else
      *((gid_t *)((char *)data_block + (long int)(ol->value))) = gid;
    *(get_set_flag(name, oltop, last, data_block)) = TRUE;
    break;

    /* If it was a uid list, look up each individual entry, and build
    a vector of uids, with a count in the first element. Put the vector
    in permanent store so we can free the string. */

    case opt_uidlist:
      {
      int count = 1;
      uid_t *list;
      int ptr = 0;
      char *p = sptr;

      if (*p != 0) count++;
      while (*p != 0) if (*p++ == ':') count++;
      list = store_malloc(count*sizeof(uid_t));
      list[ptr++] = (uid_t)(count - 1);

      if (data_block == NULL)
        *((uid_t **)(ol->value)) = list;
      else
        *((uid_t **)((char *)data_block + (long int)(ol->value))) = list;

      p = sptr;
      while (count-- > 1)
        {
        int sep = 0;
        (void)string_nextinlist(&p, &sep, big_buffer, BIG_BUFFER_SIZE);
        if (!direct_finduser(big_buffer, NULL, &uid))
          log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
            "user %s, referred to in line %d, "
            "was not found", big_buffer, config_lineno);
        list[ptr++] = uid;
        }
      }
    break;

    /* If it was a gid list, look up each individual entry, and build
    a vector of gids, with a count in the first element. Put the vector
    in permanent store so we can free the string. */

    case opt_gidlist:
      {
      int count = 1;
      gid_t *list;
      int ptr = 0;
      char *p = sptr;

      if (*p != 0) count++;
      while (*p != 0) if (*p++ == ':') count++;
      list = store_malloc(count*sizeof(gid_t));
      list[ptr++] = (gid_t)(count - 1);

      if (data_block == NULL)
        *((gid_t **)(ol->value)) = list;
      else
        *((gid_t **)((char *)data_block + (long int)(ol->value))) = list;

      p = sptr;
      while (count-- > 1)
        {
        int sep = 0;
        (void)string_nextinlist(&p, &sep, big_buffer, BIG_BUFFER_SIZE);
        if (!direct_findgroup(big_buffer, &gid))
          log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
            "group %s, referred to in line %d, "
            "was not found", big_buffer, config_lineno);
        list[ptr++] = gid;
        }
      }
    break;

    /* Search types are known names, possibly preceded by "partial-",
    in which case we have to find the corresponding "*partial_match"
    option and set it. A number may precede the - sign. */

    case opt_searchtype:
      {
      char *sp = sptr;
      char *error;
      int search_type = 0;
      int pv = -1;
      int len;

      /* Sort out the partial matching value. */

      if (strncmp(sp, "partial", 7) == 0)
        {
        sp += 7;
        if (isdigit ((uschar)*sp))
          {
          pv = 0;
          while (isdigit((uschar)*sp)) pv = pv*10 + *sp++ - '0';
          }
        else pv = 2;

        if (*sp++ != '-')
          log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
            "unrecognized search type \"%s\" in line %d", sptr,
            config_lineno);
        }

      /* If the final character of a search type is "*" it requests
      default wildcard looking up. If the final two characters are "*@" they
      request wildcard local-part lookup. These are independent of partial
      matching, but are encoded by adding 1024 or 2048 to the partial match
      value. */

      len = (int)strlen(sp);

      if (len > 1 && strcmp(sp+len-2, "*@") == 0)
        {
        sp[len-2] = 0;
        pv += 1024 + 2048;
        }

      else if (len > 0 && sp[len-1] == '*')
        {
        sp[len-1] = 0;
        pv += 1024;
        }

      /* Put the partial + "*" value into the appropriate place. */

      if (pv >= 0)
        {
        void *value2;
        optionlist *ol2 = find_option("*partial_match", oltop, last);
        if (ol2 == NULL)
          log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
            "\"partial\" or \"*\" not permitted here (line %d)", config_lineno);
        value2 = ol2->value;
        if (data_block != NULL)
          value2 = (void *)((char *)data_block + (long int)value2);
        *((int *)value2) = pv;
        }

      /* Now check the search type proper. Query-style lookups cannot be
      partial. */

      search_type = search_findtype(sp, &error);
      if (search_type < 0)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "%s in line %d", error,
          config_lineno);

      if (pv >= 0 && mac_islookup(search_type, lookup_querystyle))
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "\"partial\" not supported for query-style lookup (line %d)",
          config_lineno);

      /* OK, it's valid */

      if (data_block == NULL)
        *((int *)(ol->value)) = search_type;
      else
        *((int *)((char *)data_block + (long int)(ol->value))) = search_type;

      }
    break;

    /* Local batch options are known names */

    case opt_local_batch:
      {
      int type = 0;
      if (strcmp(sptr, "one") == 0) type = local_batch_one;
      else if (strcmp(sptr, "domain") == 0) type = local_batch_domain;
      else if (strcmp(sptr, "all") == 0) type = local_batch_all;
      else if (strcmp(sptr, "none") == 0) type = local_batch_off;
      else log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
        "local batch type %s unknown in line %d", sptr, config_lineno);
      if (data_block == NULL)
        *((int *)(ol->value)) = type;
      else
        *((int *)((char *)data_block + (long int)(ol->value))) = type;
      }
    break;
    }

  /* Release store if the value of the string doesn't need to be kept. */

  if (freesptr) store_reset(reset_point);
  break;

  /* Boolean: if no characters follow, the value is boolvalue. Otherwise
  look for yes/not/true/false. There's a special fudge for verify settings;
  without a suffix they set both xx_sender and xx_recipient. The table
  points to the sender value; search subsequently for the recipient. There's
  another special case: opt_bool_set also notes when a boolean has been set. */

  case opt_bool:
  case opt_bool_verify:
  case opt_bool_set:
  if (*s != 0)
    {
    s = readconf_readname(name2, 64, s);
    if (strcmpic(name2, "true") == 0 || strcmpic(name2, "yes") == 0)
      boolvalue = TRUE;
    else if (strcmpic(name2, "false") == 0 || strcmpic(name2, "no") == 0)
      boolvalue = FALSE;
    else log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "\"%s\" is not a valid value for the \"%s\" option in line %d",
      name2, name, config_lineno);
    }
  if (data_block == NULL)
    *((BOOL *)(ol->value)) = boolvalue;
  else
    *((BOOL *)((char *)data_block + (long int)(ol->value))) = boolvalue;

  /* Verify fudge */

  if (type == opt_bool_verify)
    {
    sprintf(name2, "%.50s_recipient", name + offset);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      if (data_block == NULL)
        *((BOOL *)(ol2->value)) = boolvalue;
      else
        *((BOOL *)((char *)data_block + (long int)(ol2->value))) = boolvalue;
      }
    }

  /* Note that opt_bool_set type is set, if there is somewhere to do so */

  else if (type == opt_bool_set)
    {
    sprintf(name2, "*set_%.50s", name + offset);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      if (data_block == NULL)
        *((BOOL *)(ol2->value)) = TRUE;
      else
        *((BOOL *)((char *)data_block + (long int)(ol2->value))) = TRUE;
      }
    }
  break;

  /* Octal integer */

  case opt_octint:
  intformat = "%o%n";
  inttype = "octal ";

  /*  Integer: a simple(ish) case; allow octal and hex formats, and
  suffixes K and M. The different types affect output, not input. */

  case opt_mkint:
  case opt_int:
  if (sscanf(s, intformat, &value, &count) != 1)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "%sinteger expected "
      "for %s in line %d", inttype, name, config_lineno);

  if (tolower(s[count]) == 'k') { value *= 1024; count++; }
  if (tolower(s[count]) == 'm') { value *= 1024*1024; count++; }

  while (isspace((uschar)s[count])) count++;

  if (s[count] != 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "extra characters follow %sinteger value "
      "for %s in line %d", inttype, name, config_lineno);

  if (data_block == NULL)
    *((int *)(ol->value)) = value;
  else
    *((int *)((char *)data_block + (long int)(ol->value))) = value;
  break;

  /*  Fixed-point number: held to 3 decimal places. */

  case opt_fixed:
  if (sscanf(s, "%d%n", &value, &count) != 1)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "fixed-point number expected "
      "for %s in line %d", name, config_lineno);

  value *= 1000;
  if (s[count] == '.')
    {
    int d = 100;
    while (isdigit((uschar)s[++count]))
      {
      value += (s[count] - '0') * d;
      d /= 10;
      }
    }

  while (isspace((uschar)s[count])) count++;

  if (s[count] != 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "extra characters follow fixed-point "
      "value for %s in line %d", name, config_lineno);

  if (data_block == NULL)
    *((int *)(ol->value)) = value;
  else
    *((int *)((char *)data_block + (long int)(ol->value))) = value;
  break;

  /* There's a special routine to read time values. */

  case opt_time:
  value = readconf_readtime(s, 0);
  if (value < 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "invalid time value "
      "for %s in line %d", name, config_lineno);
  if (data_block == NULL)
    *((int *)(ol->value)) = value;
  else
    *((int *)((char *)data_block + (long int)(ol->value))) = value;
  break;

  /* A time list is a list of colon-separated times, with the first
  element holding the size of the list and the second the number of
  entries used. */

  case opt_timelist:
    {
    int count = 0;
    int *list = (data_block == NULL)?
      (int *)(ol->value) :
      (int *)((char *)data_block + (long int)(ol->value));

    for (count = 1; count <= list[0] - 2; count++)
      {
      int terminator = 0;
      char *snext = strchr(s, ':');
      if (snext != NULL)
        {
        char *ss = snext;
        while (ss > s && isspace((uschar)ss[-1])) ss--;
        terminator = *ss;
        }
      value = readconf_readtime(s, terminator);
      if (value < 0)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "invalid time value "
          "for %s in line %d", name, config_lineno);
      if (count > 1 && value <= list[count])
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "time value out of order "
          "for %s in line %d", name, config_lineno);
      list[count+1] = value;
      if (snext == NULL) break;
      s = snext + 1;
      while (isspace((uschar)*s)) s++;
      }

    if (count > list[0] - 2)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "too many time values "
        "for %s in line %d", name, config_lineno);
    if (count > 0 && list[2] == 0) count = 0;
    list[1] = count;
    }

  break;
  }

return TRUE;
}



/*************************************************
*               Print a time value               *
*************************************************/

/*
Argument:  a time value in seconds
Returns:   pointer to a fixed buffer containing the time as a string,
           in readconf_readtime() format
*/

char *
readconf_printtime(int t)
{
int s, m, h, d, w;
char *p = time_buffer;

s = t % 60;
t /= 60;
m = t % 60;
t /= 60;
h = t % 24;
t /= 24;
d = t % 7;
w = t/7;

if (w > 0) { sprintf(p, "%dw", w); while (*p) p++; }
if (d > 0) { sprintf(p, "%dd", d); while (*p) p++; }
if (h > 0) { sprintf(p, "%dh", h); while (*p) p++; }
if (m > 0) { sprintf(p, "%dm", m); while (*p) p++; }
if (s > 0 || p == time_buffer) sprintf(p, "%ds", s);

return time_buffer;
}



/*************************************************
*      Print an individual option value          *
*************************************************/

/* This is used by the -bP option, so prints to the standard output.
The entire options list is passed in as an argument, because some options come
in pairs - typically uid/gid settings, which can either be explicit numerical
values, or strings to be expanded later. If the numerical value is unset,
search for "*expand_<name>" to see if there is a string equivalent.

Arguments:
  ol             option entry, or NULL for an unknown option
  name           option name
  options_block  NULL for main configuration options; otherwise points to
                   a driver block; if the option doesn't have opt_public
                   set, then options_block->options_block is where the item
                   resides.
  oltop          points to the option list in which ol exists
  last           one more than the offset of the last entry in optop

Returns:         nothing
*/

static void
print_ol(optionlist *ol, char *name, void *options_block,
  optionlist *oltop, int last)
{
struct passwd *pw;
struct group *gr;
transport_instance *t;
optionlist *ol2;
void *value;
uid_t *uidlist;
gid_t *gidlist;
char *s;
char name2[64];

if (ol == NULL)
  {
  printf("%s is not a known option\n", name);
  return;
  }

/* Non-admin callers cannot see options that have been flagged secure by the
"hide" prefix. */

if (!admin_user && (ol->type & opt_secure) != 0)
  {
  printf("%s = <value not displayable>\n", name);
  return;
  }

/* Else show the value of the option */

value = ol->value;
if (options_block != NULL)
  {
  if ((ol->type & opt_public) == 0)
    options_block = (void *)(((driver_instance *)options_block)->options_block);
  value = (void *)((char *)options_block + (long int)value);
  }

switch(ol->type & opt_mask)
  {
  case opt_stringptr:
  case opt_rewrite:        /* Show the text value */
  s = *((char **)value);
  printf("%s = %s\n", name, (s == NULL)? "" : string_printing(s));
  break;

  /* If fixed transport is unset, look for dynamic expanding string */

  case opt_transportptr:
  t = *((transport_instance **)value);
  if (t != NULL) printf("%s = %s\n", name, t->name); else
    {
    sprintf(name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      void *value2 = ol2->value;
      if (options_block != NULL)
        value2 = (void *)((char *)options_block + (long int)value2);
      s = *((char **)value2);
      printf("%s = %s\n", name, (s == NULL)? "" : string_printing(s));
      }
    else printf("%s =\n", name);
    }
  break;

  case opt_int:
  printf("%s = %d\n", name, *((int *)value));
  break;

  case opt_mkint:
    {
    int x = *((int *)value);
    if (x != 0 && (x & 1023) == 0)
      {
      int c = 'K';
      x >>= 10;
      if ((x & 1023) == 0)
        {
        c = 'M';
        x >>= 10;
        }
      printf("%s = %d%c\n", name, x, c);
      }
    else printf("%s = %d\n", name, x);
    }
  break;

  case opt_octint:
  printf("%s = %#o\n", name, *((int *)value));
  break;

  /* Can be negative only when "unset", in which case integer */

  case opt_fixed:
    {
    int x = *((int *)value);
    int f = x % 1000;
    int d = 100;
    if (x < 0) printf("%s =\n", name); else
      {
      printf("%s = %d.", name, x/1000);
      do
        {
        printf("%d", f/d);
        f %= d;
        d /= 10;
        }
      while (f != 0);
      printf("\n");
      }
    }
  break;

  /* If the numerical value is unset, try for the string value */

  case opt_expand_uid:
  if (! *get_set_flag(name, oltop, last, options_block))
    {
    sprintf(name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL)
      {
      void *value2 = ol2->value;
      if (options_block != NULL)
        value2 = (void *)((char *)options_block + (long int)value2);
      s = *((char **)value2);
      printf("%s = %s\n", name, (s == NULL)? "" : string_printing(s));
      break;
      }
    }

  /* Else fall through */

  case opt_uid:
  if (! *get_set_flag(name, oltop, last, options_block))
    printf("%s =\n", name);
  else
    {
    pw = getpwuid(*((uid_t *)value));
    if (pw == NULL)
      printf("%s = %ld\n", name, (long int)(*((uid_t *)value)));
    else printf("%s = %s\n", name, pw->pw_name);
    }
  break;

  /* If the numerical value is unset, try for the string value */

  case opt_expand_gid:
  if (! *get_set_flag(name, oltop, last, options_block))
    {
    optionlist *ol2;
    sprintf(name2, "*expand_%.50s", name);
    ol2 = find_option(name2, oltop, last);
    if (ol2 != NULL && (ol2->type & opt_mask) == opt_stringptr)
      {
      void *value2 = ol2->value;
      if (options_block != NULL)
        value2 = (void *)((char *)options_block + (long int)value2);
      s = *((char **)value2);
      printf("%s = %s\n", name, (s == NULL)? "" : string_printing(s));
      break;
      }
    }

  /* Else fall through */

  case opt_gid:
  if (! *get_set_flag(name, oltop, last, options_block))
    printf("%s =\n", name);
  else
    {
    gr = getgrgid(*((int *)value));
    if (gr == NULL)
       printf("%s = %ld\n", name, (long int)(*((int *)value)));
    else printf("%s = %s\n", name, gr->gr_name);
    }
  break;

  case opt_uidlist:
  uidlist = *((uid_t **)value);
  printf("%s =", name);
  if (uidlist != NULL)
    {
    int i;
    char sep = ' ';
    for (i = 1; i <= (int)(uidlist[0]); i++)
      {
      char *name = NULL;
      pw = getpwuid(uidlist[i]);
      if (pw != NULL) name = pw->pw_name;
      if (name != NULL) printf("%c%s", sep, name);
        else printf("%c%ld", sep, (long int)(uidlist[i]));
      sep = ':';
      }
    }
  printf("\n");
  break;

  case opt_gidlist:
  gidlist = *((gid_t **)value);
  printf("%s =", name);
  if (gidlist != NULL)
    {
    int i;
    char sep = ' ';
    for (i = 1; i <= (int)(gidlist[0]); i++)
      {
      char *name = NULL;
      gr = getgrgid(gidlist[i]);
      if (gr != NULL) name = gr->gr_name;
      if (name != NULL) printf("%c%s", sep, name);
        else printf("%c%ld", sep, (long int)(gidlist[i]));
      sep = ':';
      }
    }
  printf("\n");
  break;

  case opt_time:
  printf("%s = %s\n", name, readconf_printtime(*((int *)value)));
  break;

  case opt_timelist:
    {
    int i;
    int *list = (int *)value;
    printf("%s = ", name);
    for (i = 0; i < list[1]; i++)
      printf("%s%s", (i == 0)? "" : ":", readconf_printtime(list[i+2]));
    printf("\n");
    }
  break;

  case opt_bool:
  case opt_bool_verify:
  case opt_bool_set:
  printf("%s%s\n", (*((BOOL *)value))? "" : "no_", name);
  break;

  /* If *partial_match exists and the value is >= 0, add "partial-"
  to the output. First, however, disentangle the "*" matching feature,
  which is encoded by adding 1024 to the partial value. */

  case opt_searchtype:
    {
    int type = *((int *)value);
    char *star = "";
    char partial[32];
    optionlist *ol2 = find_option("*partial_match", oltop, last);

    partial[0] = 0;
    if (ol2 != NULL)
      {
      int pv;
      void *value2 = ol2->value;
      if (options_block != NULL)
        {
        value2 = (void *)((char *)options_block + (long int)value2);
        }
      pv = *((int *)value2);
      if (pv > 1000)
        {
        star = "*";
        pv -= 1024;
        }
      if (pv == 2) strcpy(partial, "partial-");
        else if (pv >= 0) sprintf(partial, "partial%d-", pv);
      }

    printf("%s = %s%s%s\n", name, partial,
      (type < 0)? "" : lookup_list[type].name, star);
    }
  break;

  case opt_local_batch:
    {
    int type = *((int *)value);
    printf("%s = %s\n", name, (type == local_batch_one)? "one" :
      (type == local_batch_domain)? "domain" :
      (type == local_batch_all)? "all" : "none");
    }
  break;
  }
}



/*************************************************
*        Print value from main configuration     *
*************************************************/

/* This function, called as a result of encountering the -bP option,
causes the value of any main configuration variable to be output if the
second argument is NULL. There are some special values:

  all                print all main configuration options
  configure_file     print the name of the configuration file
  directors          print the directors' configurations
  routers            print the routers' configurations
  transports         print the transports' configuration
  authenticators     print the authenticators' configuration (iff HAVE_AUTH)
  director_list      print a list of director names
  router_list        print a list of router names
  transport_list     print a list of transport names
  authenticator_list print a list of authentication mechanism names
                       (iff HAVE_AUTH)

If the second argument is not NULL, it must be one of "director", "router",
"transport", or "authenticator" in which case the first argument identifies the
driver whose options are to be printed.

Arguments:
  name        option name if type == NULL; else driver name
  type        NULL or driver type name, as described above

Returns:      nothing
*/

void
readconf_print(char *name, char *type)
{
BOOL names_only = FALSE;
optionlist *ol;
optionlist *ol2 = NULL;
driver_instance *d = NULL;
int size = 0;

if (type == NULL)
  {
  if (strcmp(name, "configure_file") == 0)
    {
    printf("%s\n", config_filename);
    return;
    }

  if (strcmp(name, "all") == 0)
    {
    for (ol = optionlist_config;
         ol < optionlist_config + optionlist_config_size; ol++)
      {
      if ((ol->type & opt_hidden) == 0)
        print_ol(ol, ol->name, NULL, optionlist_config, optionlist_config_size);
      }
    return;
    }

  if (strcmp(name, "directors") == 0)
    {
    type = "director";
    name = NULL;
    }
  else if (strcmp(name, "routers") == 0)
    {
    type = "router";
    name = NULL;
    }
  else if (strcmp(name, "transports") == 0)
    {
    type = "transport";
    name = NULL;
    }

  #ifdef HAVE_AUTH
  else if (strcmp(name, "authenticators") == 0)
    {
    type = "authenticator";
    name = NULL;
    }

  else if (strcmp(name, "authenticator_list") == 0)
    {
    type = "authenticator";
    name = NULL;
    names_only = TRUE;
    }
  #endif

  else if (strcmp(name, "director_list") == 0)
    {
    type = "director";
    name = NULL;
    names_only = TRUE;
    }
  else if (strcmp(name, "router_list") == 0)
    {
    type = "router";
    name = NULL;
    names_only = TRUE;
    }
  else if (strcmp(name, "transport_list") == 0)
    {
    type = "transport";
    name = NULL;
    names_only = TRUE;
    }
  else
    {
    print_ol(find_option(name, optionlist_config, optionlist_config_size),
      name, NULL, optionlist_config, optionlist_config_size);
    return;
    }
  }

/* Handle the options for a director, router, or transport. Skip options that
are flagged as hidden. Some of these are options with names starting with '*',
used for internal alternative representations of other options (which the
printing function will sort out). Others are synonyms kept for backward
compatibility. */

if (strcmp(type, "director") == 0)
  {
  d = (driver_instance *)directors;
  ol2 = optionlist_directors;
  size = optionlist_directors_size;
  }
else if (strcmp(type, "router") == 0)
  {
  d = (driver_instance *)routers;
  ol2 = optionlist_routers;
  size = optionlist_routers_size;
  }
else if (strcmp(type, "transport") == 0)
  {
  d = (driver_instance *)transports;
  ol2 = optionlist_transports;
  size = optionlist_transports_size;
  }
#ifdef HAVE_AUTH
else if (strcmp(type, "authenticator") == 0)
  {
  d = (driver_instance *)auths;
  ol2 = optionlist_auths;
  size = optionlist_auths_size;
  }
#endif

if (names_only)
  {
  for (; d != NULL; d = d->next) printf("%s\n", d->name);
  return;
  }

/* Either search for a given driver, or print all of them */

for (; d != NULL; d = d->next)
  {
  if (name == NULL)
    printf("\n%s %s:\n", d->name, type);
  else if (strcmp(d->name, name) != 0) continue;

  for (ol = ol2; ol < ol2 + size; ol++)
    {
    if ((ol->type & opt_hidden) == 0)
      print_ol(ol, ol->name, d, ol2, size);
    }

  for (ol = d->info->options;
       ol < d->info->options + *(d->info->options_count); ol++)
    {
    if ((ol->type & opt_hidden) == 0)
      print_ol(ol, ol->name, d, d->info->options, *(d->info->options_count));
    }
  if (name != NULL) return;
  }
if (name != NULL) printf("%s %s not found\n", type, name);
}



/*************************************************
*         Read main configuration options        *
*************************************************/

/* This function is the first to be called for configuration reading.
It opens the configuration file and reads general configuration settings
until it reaches a line containing "end". The file is then left open so
that delivery configuration data can subsequently be read if needed.

The configuration file must be owned either by root or exim, and be writeable
only by root or uid/gid exim. The values for Exim's uid and gid can be changed
in the config file, so the test is done on the compiled in values. A slight
anomaly, to be carefully documented.

The name of the configuration file is included in the binary of Exim. It can
be altered from the command line, but if that is done, root privilege is
immediately withdrawn.

For use on multiple systems that share file systems, first look for a
configuration file whose name has the current node name on the end. If that
is not found, try the generic name. For really contorted configurations, that
run multiple Exims with different uid settings, first try adding the effective
uid before the node name. These complications are going to waste resources
on most systems. Therefore they are available only when requested by
compile-time options. */

void
readconf_main(void)
{
struct stat statbuf;
char *s;

/* Try for the node-specific file if a node name exists */

#ifdef CONFIGURE_FILE_USE_NODE
struct utsname uts;
if (uname(&uts) >= 0)
  {
  #ifdef CONFIGURE_FILE_USE_EUID
  sprintf(big_buffer, "%.256s.%ld.%.256s", config_filename,
    (long int)original_euid, uts.nodename);
  config_file = fopen(big_buffer, "r");
  if (config_file != NULL)
    config_filename=strdup(big_buffer);
  else
  #endif
    {
    sprintf(big_buffer, "%.256s.%.256s", config_filename, uts.nodename);
    config_file = fopen(big_buffer, "r");
    if (config_file != NULL)
      config_filename=strdup(big_buffer);
    }
  }
#endif

/* Otherwise, try the generic name, possibly with the euid added */

#ifdef CONFIGURE_FILE_USE_EUID
if (config_file == NULL)
  {
  sprintf(big_buffer, "%.256s.%ld", config_filename, (long int)original_euid);
  config_file = fopen(big_buffer, "r");
  }
#endif

/* Finally, try the unadorned name */

sprintf(big_buffer, "%.256s", config_filename);
if (config_file == NULL) config_file = fopen(big_buffer, "r");

/* Failure to open the configuration file is a serious disaster. */

if (config_file == NULL)
  log_write(0, LOG_PANIC_DIE, "Failed to open configuration file %s",
    big_buffer);

/* Check the status of the file we have opened, unless it was specified on
the command line, in which case privilege was given away at the start. */

if (!config_changed)
  {
  if (fstat(fileno(config_file), &statbuf) != 0)
    log_write(0, LOG_PANIC_DIE, "Failed to stat configuration file %s",
      big_buffer);

  if ((statbuf.st_uid != root_uid &&                     /* owner not root & */
      (!exim_uid_set || statbuf.st_uid != exim_uid)) ||  /* owner not exim   */
                                                         /* or */
/*     ((!exim_gid_set || statbuf.st_gid != exim_gid) && /* group not exim & */
/*      (statbuf.st_mode & 020) != 0) ||                 /* group writeable  */
                                                         /* or */
      ((statbuf.st_mode & 2) != 0))                      /* world writeable  */

    log_write(0, LOG_PANIC_DIE, "Exim configuration file %s has the wrong "
      "owner, group, or mode", big_buffer);
  }

/* Process the main configuration settings. They all begin with a lower case
letter. If we see something starting with an upper case letter, it is taken as
a macro definition. */

while ((s = get_config_line()) != NULL && strcmpic(s, "end") != 0)
  {
  if (isupper((uschar)s[0]))
    {
    macro_item *m;
    macro_item *mlast = NULL;
    char name[64];
    int namelen = 0;

    while (isalnum((uschar)*s) || *s == '_')
      {
      if (namelen >= sizeof(name) - 1)
        log_write(0, LOG_CONFIG|LOG_PANIC_DIE, "macro name too long in line %d"
          " (maximum is %d characters)", config_lineno, sizeof(name) - 1);
      name[namelen++] = *s++;
      }
    name[namelen] = 0;
    while (isspace((uschar)*s)) s++;
    if (*s++ != '=')
      log_write(0, LOG_CONFIG|LOG_PANIC_DIE, "malformed macro definition in "
        "line %d", config_lineno);
    while (isspace((uschar)*s)) s++;

    /* If an existing macro of the same name was defined on the command line,
    we just skip this definition. Otherwise it's an error to attempt to
    redefine a macro. It is also an error to define a macro whose name begins
    with the name of a previously-defined macro. */

    for (m = macros; m != NULL; m = m->next)
      {
      int len = (int)strlen(m->name);

      if (strcmp(m->name, name) == 0)
        {
        if (m->command_line) break;
        log_write(0, LOG_CONFIG|LOG_PANIC_DIE, "macro \"%s\" is already "
          "defined", name);
        }

      if (len < namelen && strstr(name, m->name) != NULL)
        log_write(0, LOG_CONFIG|LOG_PANIC_DIE, "\"%s\" cannot be defined as "
          "a macro because previously defined macro \"%s\" is a substring",
          name, m->name);

      mlast = m;
      }
    if (m != NULL) continue;   /* Found an overriding command-line definition */

    m = store_get(sizeof(macro_item) + namelen);
    m->next = NULL;
    m->command_line = FALSE;
    if (mlast == NULL) macros = m; else mlast->next = m;
    strcpy(m->name, name);
    m->replacement = string_copy(s);
    }

  else
    (void) readconf_handle_option(s, optionlist_config, optionlist_config_size,
      NULL, "main option \"%s\" unknown in line %d");
  }

/* If the timezone string is empty, set it to NULL, implying no TZ variable
wanted. */

if (timezone_string != NULL && *timezone_string == 0) timezone_string = NULL;

/* If the debug level is set in the config file and not in the command line,
then use the config file value. */

if (debug_level < 0 && configured_debug >= 0)
  {
  debug_level = configured_debug;
  debug_file = stderr;
  }

/* The primary host name may be required for expansion of spool_directory
and log_file_path, so make sure it is set asap. It is obtained from uname(),
but if that yields an unqualified value, make a FQDN by using gethostbyname to
canonize it. Some people like upper case letters in their host names, so we
don't force the case. */

if (primary_hostname == NULL)
  {
  char *hostname;
  struct utsname uts;
  if (uname(&uts) < 0)
    log_write(0, LOG_PANIC_DIE, "uname() failed to yield host name");
  hostname = uts.nodename;

  if (strchr(hostname, '.') == NULL)
    {
    struct hostent *hostdata;
    int af = (ipv4_address_lookup)? AF_INET : AF_INET6;

    for (;;)
      {
      #if HAVE_IPV6
        #if HAVE_GETIPNODEBYNAME
        int error_num;
        hostdata = getipnodebyname(hostname, af, 0, &error_num);
        #else
        hostdata = gethostbyname2(hostname, af);
        #endif
      #else
      hostdata = gethostbyname(hostname);
      #endif

      if (hostdata != NULL)
        {
        hostname = hostdata->h_name;
        break;
        }

      if (af == AF_INET) break;
      af = AF_INET;
      }
    }

  primary_hostname = string_copy(hostname);
  }

/* Set up caseless version */

primary_hostname_lc = string_copylc(primary_hostname);

/* If spool_directory wasn't set in the build-time configuration, it
must have got set above. Of course, writing to the log may not work if
log_file_path is not set, but it will at least get to syslog or somewhere, with
any luck. */

if (*spool_directory == 0)
  log_write(0, LOG_PANIC_DIE, "spool_directory undefined: cannot proceed");

/* Expand the spool directory name; it may, for example, contain the primary
host name. Same comment about failure. */

s = expand_string(spool_directory);
if (s == NULL)
  log_write(0, LOG_PANIC_DIE, "failed to expand spool_directory=\"%s\"",
    spool_directory);
spool_directory = s;

/* Same for log_file_path, which must contain "%s" in any component that isn't
the null string or "syslog". */

if (*log_file_path != 0)
  {
  char *ss, *sss;
  int sep = ':';                       /* Fixed for log file path */
  s = expand_string(log_file_path);
  if (s == NULL)
    log_write(0, LOG_PANIC_DIE, "failed to expand log_file_path=\"%s\"",
      log_file_path);

  ss = s;
  while ((sss = string_nextinlist(&ss,&sep,big_buffer,big_buffer_size)) != NULL)
    {
    if (sss[0] == 0 || strcmp(sss, "syslog") == 0) continue;
    if (strstr(sss, "%s") == NULL)
      log_write(0, LOG_PANIC_DIE, "log_file_path \"%s\" does not contain "
        "\"%%s\"", sss);
    }

  log_file_path = s;
  }

/* Same for pid_file_path, likewise */

if (*pid_file_path != 0)
  {
  s = expand_string(pid_file_path);
  if (s == NULL)
    log_write(0, LOG_PANIC_DIE, "failed to expand pid_file_path=\"%s\"",
      pid_file_path);
  if (strstr(s, "%s") == NULL)
    log_write(0, LOG_PANIC_DIE, "pid_file_path \"%s\" does not contain \"%%s\"",
      s);
  pid_file_path = s;
  }

/* Compile the regex for matching a UUCP-style "From_" line in an incoming
message. */

regex_From = regex_must_compile(uucp_from_pattern, FALSE, TRUE);

/* If there wasn't a -oX option and daemon_smtp_port was set, find the
port number from it. */

if (daemon_listen && smtp_port < 0 && configured_smtp_service != NULL)
  {
  if (isdigit((uschar)*configured_smtp_service))
    {
    char *end;
    smtp_port = strtol(configured_smtp_service, &end, 0);
    if (end != configured_smtp_service + (int)strlen(configured_smtp_service))
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "invalid SMTP service: %s",
        configured_smtp_service);
    }
  else
    {
    struct servent *smtp_service =
      getservbyname(configured_smtp_service, "tcp");
    if (smtp_service == NULL)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "TCP service \"%s\" not found",
        configured_smtp_service);
    smtp_port = ntohs(smtp_service->s_port);
    }
  }

/* Paranoia check of maximum lengths of certain strings. There is a check
on the length of the log file path in log.c, which will come into effect
if there are any calls to write the log earlier than this. However, if we
get this far but the string is very long, it is better to stop now than to
carry on and (e.g.) receive a message and then have to collapse. The call to
log_write() from here will cause the ultimate panic collapse if the complete
file name exceeds the buffer length. */

if ((int)strlen(log_file_path) > 200)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "log_file_path is longer than 200 chars: aborting");

if ((int)strlen(pid_file_path) > 200)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "pid_file_path is longer than 200 chars: aborting");

if ((int)strlen(spool_directory) > 200)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "spool_directory is longer than 200 chars: aborting");

/* The qualify domains default to the primary host name */

if (qualify_domain_sender == NULL)
  qualify_domain_sender = primary_hostname;
if (qualify_domain_recipient == NULL)
  qualify_domain_recipient = qualify_domain_sender;

/* Default local_domains to the qualify domain. */

if (local_domains == NULL)
  local_domains = string_copylc(qualify_domain_recipient);

/* If local_domains starts with a colon, flatten it. No legitimate
domain or IP literal may start with a colon. */

s = local_domains;
while (*s == ':') *s++ = ' ';

/* Add the lowercased primary host name if requested, but avoid having it
twice. */

if (local_domains_include_host &&
    !match_isinlist(primary_hostname_lc, &local_domains, TRUE, TRUE, NULL))
  {
  HDEBUG(9) debug_printf("adding primary host name %s to local_domains\n",
    primary_hostname);
  local_domains = string_sprintf("%s:%s", primary_hostname_lc, local_domains);
  }

/* Include host literals if requested. Omit both forms of loopback address.
Note that for IPv6 addresses, we have to duplicate the colons when the list
separator is a colon - in fact, this applies in general. */

if (local_domains_include_host_literals)
  {
  ip_address_item *ip;
  char *s = local_domains;
  int ptr, size;
  int sep = ':';

  /* Find non-default list separator */

  while (isspace((uschar)*s)) s++;
  if (*s == '<' && ispunct((uschar)s[1]))
    {
    sep = s[1];
    s += 2;
    }

  /* Remove existing trailing separators, but not the one after < if that
  is the only one! */

  ptr = (int)strlen(s);
  while (ptr > 0 && s[ptr-1] == sep) ptr--;
  s[ptr] = 0;

  /* Set up for extending the string, and add each address to it in
  turn, scanning for doubling where necessary. */

  ptr = (int)strlen(local_domains);
  size = ptr + 1;

  for (ip = host_find_interfaces(); ip != NULL; ip = ip->next)
    {
    char *s = ip->address;
    char *p = big_buffer;
    if (strcmp(s, "127.0.0.1") == 0 || strcmp(s, "::1") == 0) continue;
    *p++ = sep;
    *p++ = '[';
    while (*s != 0)
      {
      *p++ = *s;
      if (*s++ == sep) *p++ = sep;
      }
    *p++ = ']';
    *p = 0;
    local_domains = string_cat(local_domains, &size, &ptr, big_buffer,
      (int)strlen(big_buffer));
    }

  local_domains[ptr] = 0;
  }

/* Setting exim_user in the configuration sets the gid as well if a name is
given, but a numerical value does not. Also, we may have a compiled-in uid and
no gid. */

if (exim_uid_set && !exim_gid_set)
  {
  struct passwd *pw = getpwuid(exim_uid);
  if (pw == NULL)
    log_write(0, LOG_PANIC_DIE, "Failed to look up uid %ld",
      (long int)exim_uid);
  exim_gid = pw->pw_gid;
  exim_gid_set = TRUE;
  }

/* Similarly for message_filter_user */

if (message_filter_uid_set && !message_filter_gid_set)
  {
  struct passwd *pw = getpwuid(message_filter_uid);
  if (pw == NULL)
    log_write(0, LOG_PANIC_DIE, "Failed to look up uid %ld",
      (long int)message_filter_uid);
  message_filter_gid = pw->pw_gid;
  message_filter_gid_set = TRUE;
  }

/* If exim_uid is set at runtime to be root, it has the effect of unsetting
it. */

if (exim_uid == root_uid) exim_uid_set = exim_gid_set = FALSE;

/* If no security type was specified in the configuration file, use level
2 or 3 if an exim uid has been defined. */

if (security_type == NULL && exim_uid_set)
  security_type = have_seteuid? "setuid+seteuid" : "setuid";

/* If a security type is set and there is no exim_uid, complain. */

if (security_type != NULL && !exim_uid_set)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "%s security requested in configuration file but no exim user defined");

/* Check that the security type is valid, and set the int version of it. If
seteuid is requested, check that this operating system has it (or at least,
exim has been configured to say that it has!) */

if (exim_uid_set)
  {
  if (strcmp(security_type, "setuid") == 0) security_level = 2;
  else if (strcmp(security_type, "seteuid") == 0) security_level = 1;
  else if (strcmp(security_type, "setuid+seteuid") == 0) security_level = 3;
  else if (strcmp(security_type, "unprivileged") == 0) security_level = 4;
  else log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "Unknown security setting: %s", security_type);

  if (!have_seteuid && (security_level == 1 || security_level == 3))
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "seteuid requested in configuration file but not configured in "
      "Makefile-<osname>");
  }

/* If the errors_reply_to field is set, check that it is syntactically valid
and ensure it contains a domain. */

if (errors_reply_to != NULL)
  {
  char *errmess;
  int start, end, domain;
  char *recipient = parse_extract_address(errors_reply_to, &errmess,
    &start, &end, &domain, FALSE);

  if (recipient == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "error in errors_reply_to (%s): %s", errors_reply_to, errmess);

  if (domain == 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "errors_reply_to (%s) does not contain a domain", errors_reply_to);
  }

/* Kill_ip_options => log_ip_options */

if (kill_ip_options) log_ip_options = TRUE;

/* If relay_match_host_or_sender is set, check that sender_address_relay is
set, because it doesn't make sense otherwise. */

if (relay_need_either && sender_address_relay == NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "relay_match_host_or_sender is set with nothing in sender_address_relay");

/* If smtp_accept_queue or smtp_accept_max_per_host is set, then
smtp_accept_max must also be set. */

if (smtp_accept_max == 0 &&
    (smtp_accept_queue > 0 || smtp_accept_max_per_host > 0))
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "smtp_accept_max must be set if smtp_accept_queue or "
    "smtp_accept_max_per_host is set");

/* Set up the host number if anything is specified. It is an expanded string
so that it can be computed from the host name, for example. We do this last
so as to ensure that everything else is set up before the expansion. */

if (host_number_string != NULL)
  {
  char *end;
  char *s = expand_string(host_number_string);
  long int n = strtol(s, &end, 0);
  while (isspace((uschar)*end)) end++;
  if (*end != 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "localhost_number value is not a number: %s", s);
  if (n > 255)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "localhost_number is greater than 255");
  host_number = n;
  }
}



/*************************************************
*          Initialize one driver                 *
*************************************************/

/* This is called once the driver's generic options, if any, have been read.
We can now find the driver, set up defaults for the private options, and
unset any "set" bits in the private options table (which might have been
set by another incarnation of the same driver).

Arguments:
  d                   pointer to driver instance block, with generic
                        options filled in
  drivers_available   vector of available drivers
  size_of_info        size of each block in drivers_available
  class               class of driver, for error message

Returns:              pointer to the driver info block
*/

static driver_info *
init_driver(driver_instance *d, driver_info *drivers_available,
  int size_of_info, char *class)
{
driver_info *dd;

for (dd = drivers_available; dd->driver_name[0] != 0;
     dd = (driver_info *)(((char *)dd) + size_of_info))
  {
  if (strcmp(d->driver_name, dd->driver_name) == 0)
    {
    int i;
    int len = dd->options_len;
    d->info = dd;
    d->options_block = store_get(len);
    memcpy(d->options_block, dd->options_block, len);
    for (i = 0; i < *(dd->options_count); i++)
      dd->options[i].type &= ~opt_set;
    return dd;
    }
  }

log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
  "%s %s: cannot find %s driver \"%s\" in line %d", class, d->name, class,
    d->driver_name, config_lineno);

return NULL;   /* never obeyed */
}




/*************************************************
*             Initialize driver list             *
*************************************************/

/* This function is called for directors, routers, transports, and
authentication mechanisms. It reads the data from the current point in the
configuration file up to a line containing "end", and sets up a chain of
instance blocks according to the file's contents. The file will already have
been opened by a call to readconf_main, and must be left open for subsequent
reading of further data.

Any errors cause a panic crash. Note that the blocks with names driver_info and
driver_instance must map the first portions of all the _info and _instance
blocks for this shared code to work. */

void
readconf_driver_init(
  char *class,                   /* "director", "router", "transport", or "auth" */
  driver_instance **anchor,      /* &directors, &routers, &transports, &auths */
  driver_info *drivers_available,/* available drivers */
  int size_of_info,              /* size of each info block */
  void *instance_default,        /* points to default data */
  int  instance_size,            /* size of instance block */
  optionlist *driver_optionlist, /* generic option list */
  int  driver_optionlist_count)  /* count of same */
{
driver_instance **p = anchor;
driver_instance *d = NULL;
char *buffer;

/* Now process the configuration lines */

while ((buffer = get_config_line()) != NULL && strcmpic(buffer, "end") != 0)
  {
  char name[64];

  /* Read the first name on the line and test for the start of a new driver.
  Auth names are permitted to contain hyphens, to satisfy the RFC. If this
  isn't the start of a new driver, the line will be re-read. */

  char *s = readconf_readname(name, sizeof(name), buffer);

  /* If the line starts with a name terminated by a colon, we are at the
  start of the definition of a new driver. The rest of the line must be
  blank. */

  if (*s++ == ':')
    {
    int i;

    /* Finish off initializing the previous driver. */

    if (d != NULL)
      {
      if (d->driver_name == NULL)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "no driver name given for %s %s", class, d->name);
      (d->info->init)(d);
      }

    /* Set up a new driver instance data block on the chain, with
    its default values installed. */

    d = store_get(instance_size);
    memcpy(d, instance_default, instance_size);
    *p = d;
    p = &(d->next);
    d->name = string_copy(name);

    /* Clear out the "set" bits in the generic options */

    for (i = 0; i < driver_optionlist_count; i++)
      driver_optionlist[i].type &= ~opt_set;

    /* If nothing more on this line, do the next loop iteration. */

    while (isspace((uschar)*s)) s++;
    if (*s != 0)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
        "extraneous characters after driver name \"%s\" in line %d",
          name, config_lineno);
    continue;
    }

  /* Give an error if we have not set up a current driver yet. */

  if (d == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "%s name missing in line %d", class, config_lineno);

  /* First look to see if this is a generic option; if it is "driver",
  initialise the driver. If is it not a generic option, we can look for a
  private option provided that the driver has been previously set up. */

  if (readconf_handle_option(buffer, driver_optionlist,
        driver_optionlist_count, d, NULL))
    {
    if (d->info == NULL && d->driver_name != NULL)
      init_driver(d, drivers_available, size_of_info, class);
    }

  /* Handle private options - pass the generic block because some may
  live therein. A flag with each option indicates if it is in the public
  block. */

  else if (d->info != NULL)
    {
    readconf_handle_option(buffer, d->info->options,
      *(d->info->options_count), d, "option \"%s\" unknown in line %d");
    }

  /* The option is not generic and the driver name has not yet been given. */

  else log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "option \"%s\" unknown in line %d (\"driver\" must be specified "
    "before any private options)", name, config_lineno);
  }

/* Run the initialization function for the final driver. */

if (d != NULL)
  {
  if (d->driver_name == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "no driver name given for %s %s", class, d->name);
  (d->info->init)(d);
  }
}



/*************************************************
*            Check driver dependency             *
*************************************************/

/* This function is passed a driver instance and a string. It checks whether
any of the string options for the driver contains the given string as an
expansion variable.

Arguments:
  d        points to a driver instance block
  s        the string to search for

Returns:   TRUE if a dependency is found
*/

BOOL
readconf_depends(driver_instance *d, char *s)
{
int count = *(d->info->options_count);
optionlist *ol;
char *ss;

for (ol = d->info->options; ol < d->info->options + count; ol++)
  {
  void *options_block;
  char *value;
  int type = ol->type & opt_mask;
  if (type != opt_stringptr) continue;
  options_block = ((ol->type & opt_public) == 0)? d->options_block : (void *)d;
  value = *(char **)((char *)options_block + (long int)(ol->value));
  if (value != NULL && (ss = strstr(value, s)) != NULL)
    {
    if (ss <= value || (ss[-1] != '$' && ss[-1] != '{') ||
      isalnum((uschar)ss[(int)strlen(s)])) continue;
    DEBUG(9) debug_printf("driver %s option %s depends on %s\n",
      d->name, ol->name, s);
    return TRUE;
    }
  }

DEBUG(9) debug_printf("driver %s does not depend on %s\n", d->name, s);
return FALSE;
}




/*************************************************
*      Decode an error type for retries          *
*************************************************/

/* This function is global because it is also called from the main
program when testing retry information. It decodes strings such as "quota_7d"
into numerical error codes.

Arguments:
  pp           points to start of text
  p            points past end of text
  basic_errno  points to an int to receive the main error number
  more_errno   points to an int to receive the secondary error data

Returns:       NULL if decoded correctly; else points to error text
*/

char *
readconf_retry_error(char *pp, char *p, int *basic_errno, int *more_errno)
{
int len;
char *q = pp;
while (q < p && *q != '_') q++;
len = q - pp;

if (len == 5 && strncmpic(pp, "quota", len) == 0)
  {
  *basic_errno = ERRNO_EXIMQUOTA;
  if (q != p && (*more_errno = readconf_readtime(q+1, *p)) < 0)
      return "bad time value";
  }

else if (len == 7 && strncmpic(pp, "refused", len) == 0)
  {
  *basic_errno = ECONNREFUSED;
  if (q != p)
    {
    if (strncmpic(q+1, "MX", p-q-1) == 0) *more_errno = 'M';
    else if (strncmpic(q+1, "A", p-q-1) == 0) *more_errno = 'A';
    else return "A or MX expected after \"refused\"";
    }
  }

else if (len == 7 && strncmpic(pp, "timeout", len) == 0)
  {
  *basic_errno = ETIMEDOUT;
  if (q != p)
    {
    if (strncmpic(q+1, "DNS", p-q-1) == 0) *more_errno = 'D';
    else if (strncmpic(q+1, "connect", p-q-1) == 0) *more_errno = 'C';
    else return "DNS or CONNECT expected after \"timeout\"";
    }
  }

else if (len != 1 || strncmp(pp, "*", 1) != 0) return "unknown error name";
return NULL;
}




/*************************************************
*                Read retry information          *
*************************************************/

/* Each line of retry information contains:

.  A domain name, possibly a regex, or a name starting with *; for local
   retry data, local_part@domain is used.

.  An error name, possibly with additional data, or *;

.  An optional sequence of retry items, each consisting of an identifying
   letter, a cutoff time, and optional parameters.

All this is decoded and placed into a control block. */


/* Subroutine to read an argument, preceded by a comma and terminated
by comma, semicolon, whitespace, or newline. The types are: 0 = time value,
1 = fixed point number (returned *1000).

Arguments:
  paddr     pointer to pointer to current character; updated
  type      0 => read a time; 1 => read a fixed point number

Returns:    time in seconds or fixed point number * 1000
*/

static int
retry_arg(char **paddr, int type)
{
char *p = *paddr;
char *pp;

if (*p++ != ',')
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "comma expected in line %d", config_lineno);

while (isspace((uschar)*p)) p++;
pp = p;
while (isalnum((uschar)*p)|| (type == 1 && *p == '.')) p++;

if (*p != 0 && !isspace((uschar)*p) && *p != ',' && *p != ';')
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "comma or semicolon expected in line %d", config_lineno);

*paddr = p;
switch (type)
  {
  case 0:
  return readconf_readtime(pp, *p);
  case 1:
  return readconf_readfixed(pp, *p);
  }
return 0;    /* Keep picky compilers happy */
}

/* The function proper */

void
readconf_retries(void)
{
retry_config **chain = &retries;
retry_config *next;
char *p;

while ((p = get_config_line()) != NULL && strcmpic(p, "end") != 0)
  {
  retry_rule **rchain;
  char *pp, *error;

  next = store_get(sizeof(retry_config));
  next->next = NULL;
  *chain = next;
  chain = &(next->next);
  next->basic_errno = next->more_errno = 0;
  next->rules = NULL;
  rchain = &(next->rules);

  pp = p;
  while (mac_isgraph(*p)) p++;
  if (p - pp <= 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "missing domain name in line %d", config_lineno);
  next->destination = string_copyn(pp, p-pp);

  if (next->destination[0] != '^')
    {
    char *at;
    if ((at = strchr(next->destination, '@')) != NULL)
      {
      if (!match_isinlist(at+1, &local_domains, TRUE, TRUE, NULL))
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "key %s in retry rules contains a local part with a non-local "
          "domain", next->destination);
      }
    }

  while (isspace((uschar)*p)) p++;
  pp = p;
  while (mac_isgraph(*p)) p++;
  if (p - pp <= 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "missing error type in line %d", config_lineno);

  /* Test error names for things we understand. */

  if ((error = readconf_retry_error(pp, p, &(next->basic_errno),
       &(next->more_errno))) != NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG, "%s in line %d", error,
      config_lineno);

  /* Now the retry rules. Keep the maximum timeout encountered. */

  while (isspace((uschar)*p)) p++;
  while (*p != 0)
    {
    retry_rule *rule = store_get(sizeof(retry_rule));
    *rchain = rule;
    rchain = &(rule->next);
    rule->next = NULL;

    rule->rule = toupper(*p++);
    rule->timeout = retry_arg(&p, 0);
    if (rule->timeout > retry_maximum_timeout)
      retry_maximum_timeout = rule->timeout;

    switch (rule->rule)
      {
      case 'F':   /* Fixed interval */
      rule->p1 = retry_arg(&p, 0);
      break;

      case 'G':   /* Geometrically increasing intervals */
      rule->p1 = retry_arg(&p, 0);
      rule->p2 = retry_arg(&p, 1);
      if (rule->p1 == 0 || rule->p2 < 1000)
        log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
          "bad parameters for geometric retry rule in line %d",
          config_lineno);
      break;

      default:
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
        "unknown retry rule letter in line %d", config_lineno);
      break;
      }

    while (isspace((uschar)*p)) p++;
    if (*p == ';')
      {
      p++;
      while (isspace((uschar)*p)) p++;
      }
    else if (*p != 0)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "semicolon expected in line %d", config_lineno);
    }
  }
}

/* End of readconf.c */
