/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* Prototypes for functions that appear in various modules. Gathered
together to avoid having a lot of tiddly little headers with only a
couple of lines in them. */

#ifdef EXIM_PERL
extern char *call_perl_cat(char *, int *, int *, char **, char *, char **);
extern void  cleanup_perl(void);
extern char *init_perl(char *);
#endif

#ifdef SUPPORT_TLS
extern BOOL  tls_client_start(int, host_item *, address_item *, char *,
               char *, char *, char *, char *, int);
extern void  tls_close(BOOL);
extern int   tls_feof(FILE *);
extern int   tls_ferror(FILE *);
extern int   tls_getc(FILE *);
extern int   tls_read(char *, size_t);
extern BOOL  tls_server_start(void);
extern int   tls_ungetc(int, FILE *);
extern int   tls_write(const char *, size_t);
#endif

extern void  accept_add_recipient(char *, char *, int, int);
extern void  accept_bomb_out(char *);
extern BOOL  accept_check_fs(int);
extern BOOL  accept_msg(FILE *, BOOL);
extern void  accept_swallow_smtp(FILE *);
extern char *auth_b64encode(char *, int);
extern int   auth_b64decode(char *, char **);
extern BOOL  auth_call_pam(char *, char **);
extern int   auth_get_data(char **, char *);
extern char *auth_xtextencode(char *, int);
extern int   auth_xtextdecode(char *, char **);

extern int   child_close(pid_t, int);
extern pid_t child_open(char **, char **, int, uid_t *, gid_t *, int *, int *,
               char *, BOOL);
extern pid_t child_open_exim(int *);

extern void  daemon_go(void);
extern void  debug_printf(char *, ...);
extern void  debug_print_ids(char *);
extern void  debug_print_string(char *);
extern void  debug_vprintf(char *, va_list);
extern address_item *deliver_make_addr(char *, BOOL);
extern int   deliver_message(char *, BOOL, BOOL);
extern void  deliver_restore_expansions(void);
extern void  deliver_save_expansions(void);
extern void  deliver_set_expansions(address_item *);
extern BOOL  deliver_setlocal(address_item *);
extern void  deliver_succeeded(address_item *);
extern int   direct_address(address_item *, address_item **, address_item **,
               address_item **, address_item **, int);
extern int   direct_check_fsc(director_instance *, address_item *);
extern int   direct_check_prefix(char *, char *);
extern int   direct_check_suffix(char *, char *);
extern BOOL  direct_findgroup(char *, gid_t *);
extern BOOL  direct_finduser(char *, struct passwd **, uid_t *);
extern BOOL  direct_find_expanded_group(char *, char *, char *, gid_t *,
               char **);
extern BOOL  direct_find_expanded_user(char *, char *, char *,
               struct passwd **, uid_t *, char **);
extern int   direct_get_errors_address(address_item *, director_instance *,
               char *, BOOL, char **);
extern int   direct_get_munge_headers(address_item *, director_instance *,
               char *, header_line **, char **);
extern BOOL  direct_get_ugid(director_instance *, char *, address_item *,
               ugid_block *);
extern int   direct_match_directory(address_item *, char *, char *, char *);
extern void  direct_set_ugid(address_item *, ugid_block *);
extern void  direct_init(void);
extern void  direct_tidyup(void);
extern BOOL  directory_make(char *, char *, int, BOOL);
extern dns_address *dns_address_from_rr(dns_answer *, dns_record *);
extern void  dns_init(BOOL, BOOL);
extern int   dns_basic_lookup(dns_answer *, char *, int);
extern int   dns_lookup(dns_answer *, char *, int, char **);
extern dns_record *dns_next_rr(dns_answer *, dns_scan *, int);
extern char *dns_text_type(int);

extern void  exim_exit(int);
extern void  exim_setugid(uid_t, gid_t, char *);

extern BOOL  expand_check_condition(char *, char *, char *);
extern char *expand_getkeyed(char *, char *);
extern char *expand_string(char *);
extern char *expand_string_copy(char *);
extern char *expand_string_panic(char *, char *, char *);

extern BOOL  filter_interpret(char *, address_item **, BOOL *, int *, char **,
               BOOL, BOOL);
extern BOOL  filter_runtest(int, BOOL, BOOL);
extern BOOL  filter_system_interpret(address_item **, BOOL *, int *, char **);

extern void  header_add(int, char *, ...);
extern BOOL  header_checkname(header_line *, char *, int);

extern char *host_and_ident(char *, char *);
extern int   host_aton(char *, int *);
extern void  host_build_hostlist(host_item **, char *, BOOL);
extern void  host_build_sender_fullhost(void);
extern int   host_check_rbl(char *, char *, char *, char **);
extern int   host_extract_port(char *);
extern BOOL  host_find_byname(host_item *, char *, char **, BOOL);
extern BOOL  host_find_bydns(host_item *, char *, BOOL, BOOL, BOOL, BOOL,
               char **, BOOL *);
extern ip_address_item *host_find_interfaces(void);
extern void  host_mask(int, int *, int);
extern BOOL  host_name_lookup(void);
extern int   host_nmtoa(int, int *, int, char *);
extern char *host_ntoa(int, const void *, char *, int *);
extern int   host_scan_for_local_hosts(host_item *, host_item *, BOOL *);
extern int   host_self_action(address_item *, host_item *, int, BOOL, char *);

extern void  log_close_all(void);
extern void  log_write(int, int, char *format, ...);

extern BOOL  match_address_list(char *, int, BOOL, char **, int, int *, BOOL);
extern int   match_exists(char *, char **);
extern BOOL  match_isinlist(char *, char **, BOOL, BOOL, char **);
extern BOOL  match_check_string(char *, char *, int, BOOL, BOOL, char **);
extern int   match_sender(char *);
extern void  md5_end(md5 *, const uschar *, int, uschar *);
extern void  md5_mid(md5 *, const uschar *);
extern void  md5_start(md5 *);
extern char *moan_check_errorcopy(char *);
extern BOOL  moan_send_message(char *, int, error_block *, header_line *,
               FILE *);
extern BOOL  moan_skipped_syntax_errors(char *, char *, error_block *, char *,
               BOOL, char *);
extern void  moan_smtp_batch(char *, char *, ...);
extern void  moan_tell_someone(char *, address_item *, char *, char *, ...);
extern BOOL  moan_to_sender(int, error_block *, header_line *, FILE *, BOOL);

extern ip_address_item *os_common_find_running_interfaces(void);
extern int   os_getloadavg(void);
extern void  os_restarting_signal(int, void (*)(int));
extern void  os_non_restarting_signal(int, void (*)(int));
extern char *os_strexit(int);
extern char *os_strsignal(int);

extern char *parse_extract_address(char *, char **, int *, int *, int *, BOOL);
extern int   parse_extract_addresses(char *, char *, address_item **, char **,
               BOOL, BOOL, BOOL, char *, error_block **);
extern char *parse_find_address_end(char *, BOOL);
extern char *parse_find_at(char *);
extern char *parse_fix_phrase(char *);
extern char *parse_qp(char *);

extern BOOL  queue_action(char *, int, char **, int, int);
extern void  queue_check_only(void);
extern void  queue_list(int, char **, int);
extern void  queue_count(void);
extern void  queue_run(char *, char *, BOOL);

extern int   random_number(int);
extern BOOL  readconf_depends(driver_instance *, char *);
extern void  readconf_driver_init(char *, driver_instance **,
               driver_info *, int, void *, int, optionlist *, int);
extern char *readconf_find_option(void *);
extern void  readconf_main(void);
extern void  readconf_print(char *, char *);
extern char *readconf_printtime(int);
extern int   readconf_readtime(char *, int);
extern void  readconf_retries(void);
extern char *readconf_retry_error(char *, char *, int *, int *);
extern void  readconf_rewrites(void);
extern BOOL  regex_match_and_setup(pcre *, char *, int, int);
extern pcre *regex_must_compile(char *, BOOL, BOOL);
extern void  retry_add_item(address_item *, char *, BOOL, char *, BOOL);
extern BOOL  retry_check_address(char *, host_item *, char *, BOOL, char **,
               char **);
extern retry_config *retry_find_config(char *, char *, int, int);
extern void  retry_update(address_item **, address_item **, address_item **);
extern char *rewrite_address(char *, BOOL, BOOL, rewrite_rule *, int);
extern char *rewrite_address_qualify(char *, BOOL);
extern header_line *rewrite_header(header_line *, char *, char *,
               rewrite_rule *, int, BOOL);
extern char *rewrite_one(char *, int, BOOL *, BOOL, char *, rewrite_rule *);
extern void  rewrite_test(char *);
extern int   route_address(address_item *, address_item **, address_item **,
               address_item **, int);
extern void  route_init(void);
extern BOOL  route_queue(address_item *, address_item **, address_item **,
               router_instance *, ugid_block *, char *);
extern BOOL  route_get_transport(transport_instance *, char *, address_item *,
               transport_instance **, char *, char *);
extern BOOL  route_skip_driver(router_instance *, char *, char *, int, BOOL *,
               char *);
extern void  route_tidyup(void);
extern void  route_unseen(char *, address_item *, address_item **,
               address_item **, address_item **);

extern int   search_check_file(int, char *, int, uid_t *, gid_t *, char *,
               char **);
extern char *search_find(void *, char *, char *, int, int *, char **);
extern int   search_findtype(char *, char **);
extern void *search_open(char *, int, int, uid_t *, gid_t *, char **);
extern void  search_tidyup(void);
extern void  set_process_info(char *, ...);
extern void  sigalrm_handler(int);
extern void  smtp_closedown(char *);
extern int   smtp_connect(host_item *, int, char *, int, BOOL);
extern int   smtp_feof(FILE *);
extern int   smtp_ferror(FILE *);
extern int   smtp_getc(FILE *);
extern void  smtp_printf(char *, ...);
extern BOOL  smtp_read_response(smtp_inblock *, uschar *, int, int, int);
extern void  smtp_send_prohibition_message(int, char *);
extern int   smtp_setup_msg(void);
extern BOOL  smtp_start_session(void);
extern int   smtp_ungetc(int, FILE *);
extern int   smtp_write_command(smtp_outblock *, BOOL, char *, ...);
extern BOOL  spool_move_message(char *, char *, char *, char *);
extern BOOL  spool_open_datafile(char *);
extern int   spool_open_temp(char *);
extern int   spool_read_header(char *, BOOL, BOOL);
extern int   spool_write_header(char *, int, char **);
extern BOOL  store_extend_3(void *, int, int, char *, int);
extern void  store_free_3(void *, char *, int);
extern void *store_get_3(int, char *, int);
extern void *store_malloc_3(int, char *, int);
extern void  store_release_3(void *, char *, int);
extern void  store_reset_3(void *, char *, int);
extern char *string_base62(unsigned long int);
extern char *string_cat(char *, int *, int *, char *, int);
extern char *string_copy(char *);
extern char *string_copy_malloc(char *);
extern char *string_copylc(char *);
extern char *string_copyn(char *, int);
extern char *string_copynlc(char *, int);
extern char *string_dequote(char **);
extern BOOL  string_format(char *, int, char *, ...);
extern char *string_format_size(int, char *);
extern int   string_interpret_escape(char **);
extern int   string_is_ip_address(char *, int *);
extern char *string_log_address(address_item *, address_item *, BOOL, BOOL);
extern char *string_nextinlist(char **, int *, char *, int);
extern char *string_open_failed(int, char *, ...);
extern char *string_printing(char *);
extern char *string_sprintf(char *, ...);
extern BOOL  string_vformat(char *, int, char *, va_list);
extern int   strcmpic(char *, char *);
extern int   strncmpic(char *, char *, int);
extern char *strstric(char *, char *, BOOL);

extern char *tod_stamp(int);
extern BOOL  transport_check_serialized(char *, char *);
extern BOOL  transport_check_waiting(char *, char *, int, char *, BOOL *);
extern void  transport_end_serialized(char *, char *);
extern void  transport_init(void);
extern BOOL  transport_pass_socket(char *, char *, char *, int);
extern BOOL  transport_set_up_command(char ***, char *, BOOL, int,
               address_item *, char *, char **);
extern void  transport_update_waiting(host_item *, char *);
extern BOOL  transport_write_block(int, char *, int);
extern BOOL  transport_write_string(int, char *, ...);
extern BOOL  transport_write_message(address_item *, int, int, int, char *,
               char *, char *, char *, rewrite_rule *, int);
extern void  tree_add_duplicate(char *, BOOL);
extern void  tree_add_nonrecipient(char *, BOOL);
extern void  tree_add_unusable(host_item *);
extern int   tree_insertnode(tree_node **, tree_node *);
extern void  tree_print(tree_node *, FILE *);
extern tree_node *tree_search(tree_node *, char *);
extern tree_node *tree_search_addr(tree_node *, char *, BOOL);
extern void  tree_write(tree_node *, FILE *);

extern int  verify_address(char *, FILE *, BOOL *, char **, int);
extern BOOL verify_check_host(char **, BOOL);
extern BOOL verify_check_this_host(char **, BOOL, char*, char *, char *);
extern void verify_get_ident(int);
extern BOOL verify_sender(int *, char **);
extern BOOL verify_sender_preliminary(int *, char **);
extern void version_init(void);

/* End of functions.h */
