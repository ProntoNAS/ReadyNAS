#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

#include "wizd.h"

static int clear_environment()
{
	extern char **environ;
	char **newenv;

	newenv = calloc(20, sizeof(char*));
	if (newenv == NULL) {
		debug_log_output("calloc failed. (err = %s)", strerror(errno));
		return -1;
	}
	newenv[0] = NULL;
	environ = newenv;

	return 0;
}

static int set_environment(const char *name, const char *value)
{
	debug_log_output("set_environment: '%s' = '%s'", name, value);
	return setenv(name, value, 1);
}

int http_cgi_response(int accept_socket, HTTP_RECV_INFO *http_recv_info_p)
{
	char *query_string;
	char *request_uri;
	char *request_method;
	char script_filename[WIZD_FILENAME_MAX];
	char script_name[WIZD_FILENAME_MAX];
	char *script_exec_name;
	char cwd[WIZD_FILENAME_MAX];
	int pfd[2];
	int pfd_post[2];
	int pid;
	int nullfd;
	long post_length;

	request_method = http_recv_info_p->request_method;
	request_uri = http_recv_info_p->request_uri;
	post_length = http_recv_info_p->recv_content_length;
	strncpy(script_name, request_uri, sizeof(script_name));

# if 0
	if (http_recv_info_p->send_filename[0] != '/') {
		debug_log_output("WARNING: send_filename[0] != '/', send_filanem = '%s'", http_recv_info_p->send_filename);
		if (getcwd(cwd, sizeof(cwd)) == NULL) {
			debug_log_output("getcwd() failed. err = %s", strerror(errno));
			return -1;
		}
		snprintf(script_filename, sizeof(script_filename), "%s/%s"
			, cwd, http_recv_info_p->send_filename);
		path_sanitize(script_filename, sizeof(script_filename));
	} else
# endif
	{
		strncpy(script_filename, http_recv_info_p->send_filename, sizeof(script_filename));
	}

	query_string = strchr(script_name, '?');
	if (query_string == NULL) {
		query_string = "";
	} else {
		*query_string++ = '\0';
	}
	script_exec_name = strrchr(script_name, '/');
	if (script_exec_name == NULL) {
		script_exec_name = script_name;
	} else {
		script_exec_name++;
	}
	if (script_exec_name == NULL) {
		debug_log_output("script_exec_name and script_name == NULL");
		return -1;
	}

	if (pipe(pfd) < 0) {
		debug_log_output("pipe() failed!!");
		return -1;
	}
	//debug_log_output("pipe opened. pfd[0] = %d, pfd[1] = %d", pfd[0], pfd[1]);
	if (pipe(pfd_post) < 0) {
		debug_log_output("pipe() failed!!");
		return -1;
	}

	if ((pid = fork()) < 0) {
		debug_log_output("fork() failed!!");
		close(pfd[0]);
		close(pfd[1]);
		close(pfd_post[0]);
		close(pfd_post[1]);
		return -1;
	}
	if (pid == 0) { // child
		char server_port[100];
		char remote_port[100];
		char post_length_str[100];
		struct sockaddr_in saddr;
		socklen_t socklen;
		char next_cwd[WIZD_FILENAME_MAX];

		close(pfd[0]);
		close(pfd_post[1]);

		clear_environment();

		set_environment("PATH", DEFAULT_PATH);

		set_environment("GATEWAY_INTERFACE", "CGI/1.1");
		set_environment("SERVER_PROTOCOL", "HTTP/1.0");

		socklen = sizeof(saddr);
		getsockname(accept_socket, (struct sockaddr*)&saddr, &socklen);
		set_environment("SERVER_ADDR", inet_ntoa(saddr.sin_addr));
		snprintf(server_port, sizeof(server_port), "%u"
			, ntohs(saddr.sin_port));
		set_environment("SERVER_PORT", server_port);
		set_environment("SERVER_NAME", global_param.server_name);
		set_environment("SERVER_SOFTWARE", SERVER_NAME);
		set_environment("DOCUMENT_ROOT", global_param.document_root);

		socklen = sizeof(saddr);
		getpeername(accept_socket, (struct sockaddr*)&saddr, &socklen);
		set_environment("REMOTE_ADDR", inet_ntoa(saddr.sin_addr));
		snprintf(remote_port, sizeof(remote_port), "%u"
			, ntohs(saddr.sin_port));
		set_environment("REMOTE_PORT", remote_port);

		set_environment("REQUEST_METHOD", request_method);
		set_environment("REQUEST_URI", request_uri);
		set_environment("SCRIPT_FILENAME", script_filename);
		set_environment("SCRIPT_NAME", script_name);
		set_environment("QUERY_STRING", query_string);

		if (!strcasecmp(request_method, "POST") && post_length > 0) {
			snprintf(post_length_str, sizeof(post_length_str), "%ld", post_length);
			set_environment("CONTENT_LENGTH", post_length_str);
		}
		if (http_recv_info_p->recv_host[0]) {
			set_environment("HTTP_HOST", http_recv_info_p->recv_host);
		}
		if (http_recv_info_p->user_agent[0]) {
			set_environment("HTTP_USER_AGENT", http_recv_info_p->user_agent);
		}
		if (http_recv_info_p->recv_range[0]) {
			set_environment("HTTP_RANGE", http_recv_info_p->recv_range);
		}

		/* let's make sure that STDOUT_FILENO points output pipe desc. */
		if (pfd[1] != STDOUT_FILENO) {
			if (dup2(pfd[1], STDOUT_FILENO) == -1) {
				debug_log_output("dup2(pfd[1], stdout) failed.");
				exit(0);
			}
			close(pfd[1]);
		}

		if (!strcasecmp(request_method, "POST")) {
			/* STDIN_FILENO <= pfd_post[0] */
			debug_log_output("CGI: assign STDIN <= POST input");
			if (pfd_post[0] != STDIN_FILENO) {
				if (dup2(pfd_post[0], STDIN_FILENO) == -1) {
					debug_log_output("dup2(pfd_post[0], stdin) failed.");
					exit(0);
				}
				close(pfd_post[0]);
			} /* else, already STDIN_FILENO == accept_socket */
		} else {
			close(pfd_post[0]);

			debug_log_output("CGI: assign STDIN <= NULL input");
			/* STDIN_FILENO <= "/dev/null", otherwise close STDIN_FILENO */
			nullfd = open("/dev/null", O_RDONLY);
			if (nullfd < 0) {
				debug_log_output("WARNING: failed to open /dev/null");
				close(STDIN_FILENO);
			} else if (nullfd != STDIN_FILENO) {
				if (dup2(nullfd, STDIN_FILENO) == -1) {
					debug_log_output("dup2(nullfd, stdin) failed");
					close(STDIN_FILENO);
				}
				close(nullfd);
			}
		}

		nullfd = open(global_param.debug_cgi_output, O_WRONLY);
		if (nullfd < 0 && strcmp(global_param.debug_cgi_output, "/dev/null")) {
			debug_log_output("WARNING: failed to open log file, \"%s\""
				, global_param.debug_cgi_output);
			debug_log_output(" -- trying to open /dev/null instead...");
			nullfd = open("/dev/null", O_WRONLY);
		}
		if (nullfd < 0) {
			debug_log_output("WARNING: failed to open /dev/null");
			close(STDERR_FILENO);
		} else if (nullfd != STDERR_FILENO) {
			if (dup2(nullfd, STDERR_FILENO) == -1) {
				debug_log_output("dup2(logfd, stderr) failed");
				close(STDERR_FILENO);
			}
			close(nullfd);
		}

		send_printf(STDOUT_FILENO, "%s", HTTP_OK);
		send_printf(STDOUT_FILENO, "%s", HTTP_CONNECTION);
		send_printf(STDOUT_FILENO, HTTP_SERVER_NAME, SERVER_NAME);

		strncpy(next_cwd, script_filename, sizeof(next_cwd));
		cut_after_last_character(next_cwd, '/');
		if (chdir(next_cwd) != 0) {
			debug_log_output("chdir failed. err = %s", strerror(errno));
		}

		if (execl(script_filename, script_exec_name, NULL) < 0) {
			debug_log_output("CGI EXEC ERROR. "
				"script = '%s', argv[0] = '%s', err = %s"
				, script_filename
				, script_exec_name
				, strerror(errno)
			);
			send_printf(STDOUT_FILENO, "\nCGI EXEC ERROR");
		}
		exit(0);
	}

	// parent
	close(pfd[1]);
	close(pfd_post[0]);

	debug_log_output("POST length: %ld", post_length);
	if (!strcasecmp(request_method, "POST") && post_length > 0) {
		char postbuf[1024];
		ssize_t len, remain = post_length;

		debug_log_output("POST sinking start...");

		while (remain > 0) {
			len = read(accept_socket, postbuf, MIN(remain, sizeof(postbuf)));
			if (len <= 0) break;
			remain -= len;
			debug_log_output("post_read: %d bytes., remain = %d", len, remain);

			/*
			 * たぶんこの部分、大量のデータが来たら
			 * パイプが詰まってデッドロックするよね…。違うかな??
			 */
			len = write(pfd_post[1], postbuf, len);
			debug_log_output("post_write: %d bytes.", len);
		}
		debug_log_output("POST sinking finished.");
		if (remain > 0) {
			debug_log_output("Warning: Premature EOF was POST...");
		}
	}
	close(pfd_post[1]);

	copy_descriptors(pfd[0], accept_socket, 0, NULL);

	return 0;
}
