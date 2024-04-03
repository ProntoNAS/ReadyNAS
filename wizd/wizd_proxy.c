#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <dirent.h>

#include "wizd.h"

extern unsigned char *base64(unsigned char *str);
extern int line_receive(int accept_socket, unsigned char *line_buf_p, int line_max);
extern void set_blocking_mode(int fd, int flag);
extern int http_uri_to_scplaylist_create(int accept_socket, char *uri_string);
extern int http_dir_to_scplaylist_create(int accept_socket, char *buff, char *recv, int p, int aliasnum, int start);
extern int http_dvd_to_scplaylist_create(int accept_socket, char *buff, HTTP_RECV_INFO *http_recv_info_p);
extern int find_real_path(char *org_name, HTTP_RECV_INFO *http_recv_info_p, char *work_filename);

/* ソケットを作成し、相手に接続するラッパ. 失敗 = -1 */
static int sock_connect(char *host, int port)
{
	int sock;
	struct sockaddr_in sockadd;
	struct hostent *hent;

	debug_log_output("sock_connect: %s:%d", host, port);
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) return -1;
	debug_log_output("sock: %d", sock);

	if (NULL == (hent = gethostbyname(host))) {
		close(sock);
		return -1;
	}
	debug_log_output("hent: %p", hent);
	bzero((char *)&sockadd, sizeof(sockadd));
	memcpy(&sockadd.sin_addr, hent->h_addr, hent->h_length);
	sockadd.sin_port = htons(port);
	sockadd.sin_family = AF_INET;
	if (connect(sock, (struct sockaddr*)&sockadd, sizeof(sockadd)) != 0) {
		debug_log_output("connect: %s  at %s:%d\n", strerror(errno), host, port);
		close(sock);
		return -1;
	}

	return sock;
}

int need_html_encoding_convert(char *buf)
{
	if (my_strcasestr(buf, "charset") == NULL) return 1;

	if (my_strcasestr(buf, "gb2312") != NULL	// only available in MediaWiz??
	 || my_strcasestr(buf, "big5") != NULL		// only available in MediaWiz??
	 || my_strcasestr(buf, "shift_jis") != NULL
	 || my_strcasestr(buf, "euc-jp") != NULL
	 || my_strcasestr(buf, "iso-2022-jp") != NULL
	 || my_strcasestr(buf, "iso-8859-1") != NULL
	 || my_strcasestr(buf, "windows-1250") != NULL
	) {
		return 0;
	}
	return 1;
}

#define MAX_LINE 100	/* 記憶する、HTTPヘッダの最大行数 */
#define LINE_BUF_SIZE 2048	/* 行バッファ */

int http_proxy_response(int accept_socket, HTTP_RECV_INFO *http_recv_info_p)
{
	char *HTTP_RECV_CONTENT_TYPE = "Content-type: text/html";
	char *HTTP_RECV_CONTENT_LENGTH = "Content-Length: ";
	char *HTTP_RECV_LOCATION = "Location: ";
	char *p_target_host_name, *p_uri_string;
	char send_http_header_buf[2048];
	char line_buf[MAX_LINE][LINE_BUF_SIZE + 5];
	char proxy_pre_string[2048];
	char base_url[2048];
	char *p_url, *p;
	char *p_auth = NULL;
	struct stat dir_stat;
	int port = 80;
	int sock;
	unsigned long long content_length = 0;
	int len = 0;
	int line = 0;
	int i;
	int content_is_html = 0;
	int flag_conv_html_code = 1;
	int flag_pc = http_recv_info_p->flag_pc;
	char buff[WIZD_FILENAME_MAX];

	debug_log_output("in http_proxy_response, %s\n", http_recv_info_p->request_uri);

	if (!strcmp(http_recv_info_p->request_method, "POST")) {
		debug_log_output("POST is not allowed yet...");
		return -1;
	}
	p_uri_string = http_recv_info_p->request_uri;

	if (!strncmp(p_uri_string, "/-.-playlist.pls?", 17)) {

		if (p_uri_string[17] == '/') {
			snprintf(buff, sizeof(buff), "http://%s%s"
				, http_recv_info_p->recv_host, p_uri_string+17);
		} else {
			strncpy(buff, p_uri_string+17, sizeof(buff));
		}
		replase_character(buff, sizeof(buff), ".pls", "");

		if (strstr(buff, ".VOB") != 0) {
			// add dvd chapters
			return http_dvd_to_scplaylist_create(accept_socket, buff, http_recv_info_p);
		} else
			return http_uri_to_scplaylist_create(accept_socket, buff);
	} else if (strstr(http_recv_info_p->recv_uri, "wizd_photolist.pls") != 0) {
		if (find_real_path("", http_recv_info_p, buff)) {
			if (stat(buff, &dir_stat) == 0) {
				strcpy(http_recv_info_p->send_filename, buff);
				http_recv_info_p->default_file_type = TYPE_PLAYLIST;
				http_listfile_to_playlist_create(accept_socket, http_recv_info_p);
				return(0);
			}
		}

		strcpy(buff, "http://");
		strcat(buff, http_recv_info_p->recv_host);
		strcat(buff, p_uri_string);
		p = strstr(buff, "wizd_photolist.pls");
		*p = 0;
		p = strstr(p_uri_string, "imagefile=");
		p = strchr(p, '=');
		p++;
		strcat(buff, p);
		// printf("sending %d|%d||%s|\r\n", global_param.slide_show_seconds, global_param.slide_show_transition, buff );
		http_send_ok_header(accept_socket, 0, NULL);
		send_printf(accept_socket, "%d|%d|%s|%s|\r\n", 
			global_param.slide_show_seconds, global_param.slide_show_transition, "", buff );
		return(0);
	}
	
	if (strcmp(http_recv_info_p->action, "allplay") == 0) {
		strcpy(buff, p_uri_string);
		p = strchr(buff, '?');
		*p = 0;
		if (http_dir_to_scplaylist_create(accept_socket, buff, http_recv_info_p->recv_host, 1, -1, http_recv_info_p->startitem) == 0)
			return(-1);
		return(0);
	}

	if (strncmp(p_uri_string, "/-.-http://", 11)) {
		return -1;
	}

	if (!global_param.flag_allow_proxy) {
		return -1;
	}

	strncpy(base_url, p_uri_string, 2048);
	p = strrchr(base_url, '/');
	p[1] = '\0';

	p_target_host_name = p_uri_string + 11;

	p_url = strchr(p_target_host_name, '/');
	if (p_url == NULL) return -1;
	*p_url++ = '\0';

	strncpy(proxy_pre_string, p_uri_string, 2048);

	p = strchr(p_target_host_name, '@');
	if (p != NULL) {
		// there is a user name.
		p_auth = p_target_host_name;
		p_target_host_name = p + 1;
		*p = '\0';
	}
	p = strchr(p_target_host_name, ':');
	if (p != NULL) {
		port = atoi(p+1);
		*p = '\0';
	}

	debug_log_output("proxy:target_host_name: %s", p_target_host_name);
	debug_log_output("proxy:authenticate: %s", p_auth ? p_auth : "NULL");
	debug_log_output("proxy:url: %s", p_url);
	debug_log_output("proxy:prestring: %s", proxy_pre_string);
	debug_log_output("proxy:base_url: %s", base_url);
	debug_log_output("proxy:port: %d", port);

	p = send_http_header_buf;
	p += sprintf(p, "GET /%s HTTP/1.0\r\n", p_url);
	p += sprintf(p, "Host: %s:%u\r\n", p_target_host_name, port);
	if (global_param.user_agent_proxy_override[0]) {
		p += sprintf(p, "User-agent: %s\r\n", global_param.user_agent_proxy_override);
	} else {
		if (http_recv_info_p->user_agent[0]) {
			p += sprintf(p, "User-agent: %s\r\n", http_recv_info_p->user_agent);
		} else {
			p += sprintf(p, "User-agent: %s\r\n", SERVER_NAME);
		}
	}
	p += sprintf(p, "Accept: */*\r\n");
	p += sprintf(p, "Connection: close\r\n");
	if (http_recv_info_p->range_start_pos) {
		p += sprintf(p, "Range: bytes=");
		p += sprintf(p, "%llu-", http_recv_info_p->range_start_pos);
		if (http_recv_info_p->range_end_pos) {
			p += sprintf(p, "%llu", http_recv_info_p->range_end_pos);
		}
		p += sprintf(p, "\r\n");
	}
	if (p_auth != NULL) {
		p += sprintf(p, "Authorization: Basic %s\r\n", base64(p_auth));
	}
	p += sprintf(p, "\r\n");

	debug_log_output("flag_pc: %d", flag_pc);

	sock = sock_connect(p_target_host_name, port);
	if (sock < 0) {
		debug_log_output("error: %s", strerror(errno));
		debug_log_output("sock: %d", sock);
		return -1;
	}
	set_blocking_mode(sock, 0); /* blocking mode */
	set_blocking_mode(accept_socket, 0); /* blocking mode */

	write(sock, send_http_header_buf, strlen(send_http_header_buf));

	debug_log_output("================= send to proxy\n");
	debug_log_output("%s", send_http_header_buf);
	debug_log_output("=================\n");

	for (line = 0; line < MAX_LINE; line ++) {
		char work_buf[LINE_BUF_SIZE + 10];

		memset(line_buf[line], 0, LINE_BUF_SIZE + 5);
		len = line_receive(sock, line_buf[line], LINE_BUF_SIZE + 1);
		if (len < 0) break;
		debug_log_output("recv html: '%s' len = %d", line_buf[line], len);

		line_buf[line][len++] = '\r';
		line_buf[line][len++] = '\n';
		line_buf[line][len] = '\0';
		if (!strncasecmp(line_buf[line], HTTP_RECV_CONTENT_TYPE, strlen(HTTP_RECV_CONTENT_TYPE))) {
			content_is_html = 1;
			flag_conv_html_code = need_html_encoding_convert(line_buf[line]);
			debug_log_output("%s to convert HTML encoding.", flag_conv_html_code ? "Start" : "Stop");
		}
		if (!strncasecmp(line_buf[line], HTTP_RECV_CONTENT_LENGTH, strlen(HTTP_RECV_CONTENT_LENGTH))) {
			content_length = strtoull(line_buf[line] + strlen(HTTP_RECV_CONTENT_LENGTH), NULL, 0);
		}
		if (!strncasecmp(line_buf[line], HTTP_RECV_LOCATION, strlen(HTTP_RECV_LOCATION))) {
			strcpy(work_buf, line_buf[line]);
			sprintf(line_buf[line], "Location: /-.-%s", work_buf + strlen(HTTP_RECV_LOCATION));
		}
		if (len <= 2) {
			line++;
			break;
		}
	}

	if (len < 0) {
		close(sock);
		return -1;
	}

	if (content_is_html) {
		char *p, *q, *new_p, *r;
		char work_buf[LINE_BUF_SIZE * 2];
		char work_buf2[LINE_BUF_SIZE * 2];
		char *link_pattern = "<A HREF=";

		for (i=0; i<line; i++) {
			if (!strncasecmp(line_buf[i], HTTP_RECV_CONTENT_LENGTH, strlen(HTTP_RECV_CONTENT_LENGTH))) continue;
			write(accept_socket, line_buf[i], strlen(line_buf[i]));
			//debug_log_output("sent html: '%s' len = %d", line_buf[i], strlen(line_buf[i]));
		}
		debug_log_output("sent header");
		//write(accept_socket, "debug:--\n", strlen("debug:--\n"));

		while (1) {
			char rep_str[1024];

			memset(work_buf, 0, LINE_BUF_SIZE);
			len = line_receive(sock, work_buf, LINE_BUF_SIZE);
			if (len < 0) break;
			debug_log_output("recv html: '%s' len = %d", work_buf, len);

			work_buf[len++] = '\r';
			work_buf[len++] = '\n';
			work_buf[len] = '\0';

			if (flag_conv_html_code && my_strcasestr(work_buf, "Content-Type") != NULL) {
				// Content-Type があったら
				// 漢字コードの変換をやめる
				flag_conv_html_code = need_html_encoding_convert(work_buf);
				debug_log_output("%s to convert HTML encoding.", flag_conv_html_code ? "Start" : "Stop");
			}

			if (flag_conv_html_code) {
				convert_language_code(work_buf, work_buf2, LINE_BUF_SIZE
					, CODE_AUTO, global_param.client_language_code);
				strncpy(work_buf, work_buf2, sizeof(work_buf));
		//		debug_log_output("Converted: %s", work_buf);
			}

			p = work_buf;
			q = work_buf2;
			memset(q, 0, sizeof(work_buf2));

			/*
			 * HTML中に <A HREF="..."> があったら、プロクシを経由するように
			 * たとえば <A HREF="/-.-http://www.yahoo.co.jp/"> のように変換する
			 * もし、ファイルがストリーム可能そうであれば、VOD="0" も追加する。
			 *
			 * 問題点:
			 *   タグの途中に改行があると失敗するだろう.
			 *   面倒なのでたいした置換はしていない
			 *   惰性で書いたので汚い。だれか修正して。
			 */
			link_pattern = "<A HREF=";
			while ((new_p = my_strcasestr(p, link_pattern)) != NULL) {
				int l = new_p - p + strlen(link_pattern);
				char *tmp;
				MIME_LIST_T *mlt = NULL;

				strncpy(q, p, l);
				q += l;
				p += l; /* i.e., p = new_p + strlen(link_pattern); */

				r = strchr(p, '>');
				if (r == NULL) continue;
				*r = '\0';

				if (*p == '"') *q++ = *p++;
				if ((tmp = strchr(p, '"')) != NULL || (tmp = strchr(p, ' ')) != NULL) {
					mlt = mime_list;
					while (mlt->mime_name != NULL) {
						if (*(tmp - strlen(mlt->file_extension) - 1) == '.'
						&& !strncasecmp(tmp - strlen(mlt->file_extension), mlt->file_extension, strlen(mlt->file_extension))) {
							break;
						}
						mlt++;
					}
				}

				if (flag_pc && (mlt != NULL) && ((mlt->menu_file_type & TYPE_MASK) == STREAMED_BASE)) {
					q += sprintf(q, "/-.-playlist.pls?http://%s", http_recv_info_p->recv_host);
				}
				if (*p == '/') {
					q += sprintf(q, "%s%s", proxy_pre_string, p);
				} else if (!strncmp(p, "http://", 7)) {
					q += sprintf(q, "/-.-%s", p);
				} else {
					q += sprintf(q, "%s%s", base_url, p);
					//q += sprintf(q, "%s", p);
				}
				if ((mlt != NULL) && ((mlt->menu_file_type & TYPE_MASK) == STREAMED_BASE)) {
					q += sprintf(q, " vod=\"0\"");
				}
				*q++ = '>';
				p = r + 1;
			}
			while (*p) *q++ = *p++;
			*q = '\0';

			/*
			 * HTML中に SRC="..." があったら、プロクシを経由するように変換する
			 *
			 * 問題点:
			 *   タグの途中に改行があると失敗するだろう.
			 *   変数使いまわしたので、融通が効かない。
			 *   だれか修正して。
			 */
			p = work_buf2;
			q = work_buf;
			memset(q, 0, sizeof(work_buf));
			link_pattern = "SRC=";
			while ((new_p = my_strcasestr(p, link_pattern)) != NULL) {
				int l = new_p - p + strlen(link_pattern);
				strncpy(q, p, l);
				q += l;
				p += l; /* i.e., p = new_p + strlen(link_pattern); */

				if (*p == '"') *q++ = *p++;
				if (*p == '/') {
					q += sprintf(q, "%s", proxy_pre_string);
				} else if (!strncmp(p, "http://", 7)) {
					q += sprintf(q, "/-.-");
				} else {
					q += sprintf(q, "%s", base_url);
					//q += sprintf(q, "%s", p);
				}
			}
			while (*p) *q++ = *p++;
			*q = '\0';

			p = work_buf;
			if (p_auth) {
				snprintf(rep_str, sizeof(rep_str), "|http://%s/-.-http://%s@"
					, http_recv_info_p->recv_host, p_auth);
			} else {
				snprintf(rep_str, sizeof(rep_str), "|http://%s/-.-http://"
					, http_recv_info_p->recv_host);
			}
			replase_character(p, LINE_BUF_SIZE, "|http://", rep_str);

			write(accept_socket, p, strlen(p));
			debug_log_output("sent html: %s", p);
		}
	} else {
		for (i=0; i<line; i++) {
			write(accept_socket, line_buf[i], strlen(line_buf[i]));
		}
		copy_descriptors(sock, accept_socket, content_length, NULL);
	}

	close(sock);
	return 0;
}
