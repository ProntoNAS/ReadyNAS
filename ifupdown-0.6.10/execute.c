#line 2076 "ifupdown.nw"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "header.h"
#line 2698 "ifupdown.nw"
#include <errno.h>
#line 2884 "ifupdown.nw"
#include <stdarg.h>
#line 2907 "ifupdown.nw"
#include <unistd.h>
#include <sys/wait.h>
#line 2160 "ifupdown.nw"
static char **environ = NULL;
#line 2137 "ifupdown.nw"
static int check(char *str);
#line 2167 "ifupdown.nw"
static void set_environ(interface_defn *iface, char *mode, char *phase);
#line 2245 "ifupdown.nw"
static char *setlocalenv(char *format, char *name, char *value);
#line 2358 "ifupdown.nw"
static int doit(char *str);
#line 2525 "ifupdown.nw"
static char *parse(char *command, interface_defn *ifd);
#line 2572 "ifupdown.nw"
void addstr(char **buf, size_t *len, size_t *pos, char *str, size_t strlen);
#line 2779 "ifupdown.nw"
int strncmpz(char *l, char *r, size_t llen);
#line 2796 "ifupdown.nw"
char *get_var(char *id, size_t idlen, interface_defn *ifd);
#line 2888 "ifupdown.nw"
static int popen2(FILE **in, FILE **out, char *command, ...);
#line 2141 "ifupdown.nw"
static int check(char *str) {
	return str != NULL;
}
#line 2173 "ifupdown.nw"
static void set_environ(interface_defn *iface, char *mode, char *phase) {
	
#line 2202 "ifupdown.nw"
char **environend;
#line 2175 "ifupdown.nw"
	int i;
	const int n_env_entries = iface->n_options + 8;

	
#line 2215 "ifupdown.nw"
if (environ != NULL) {
	char **ppch;
	for (ppch = environ; *ppch; ppch++) {
		free(*ppch);
		*ppch = NULL;
	}
	free(environ);
	environ = NULL;
}
#line 2209 "ifupdown.nw"
environ = malloc(sizeof(char*) * (n_env_entries + 1 /* for final NULL */));
environend = environ; 
*environend = NULL;

#line 2180 "ifupdown.nw"
	for (i = 0; i < iface->n_options; i++) {
		
#line 2229 "ifupdown.nw"
if (strcmp(iface->option[i].name, "pre-up") == 0
    || strcmp(iface->option[i].name, "up") == 0
    || strcmp(iface->option[i].name, "down") == 0
    || strcmp(iface->option[i].name, "post-down") == 0)
{
	continue;
}

#line 2183 "ifupdown.nw"
		
#line 2251 "ifupdown.nw"
*(environend++) = setlocalenv("IF_%s=%s", iface->option[i].name,
                              iface->option[i].value);
*environend = NULL;
#line 2184 "ifupdown.nw"
	}

	
#line 2257 "ifupdown.nw"
*(environend++) = setlocalenv("%s=%s", "IFACE", iface->real_iface);
*environend = NULL;
#line 2187 "ifupdown.nw"
	
#line 2262 "ifupdown.nw"
*(environend++) = setlocalenv("%s=%s", "LOGICAL", iface->logical_iface);
*environend = NULL;
#line 2188 "ifupdown.nw"
	
#line 2287 "ifupdown.nw"
*(environend++) = setlocalenv("%s=%s", "ADDRFAM", iface->address_family->name);
*environend = NULL;
#line 2189 "ifupdown.nw"
	
#line 2292 "ifupdown.nw"
*(environend++) = setlocalenv("%s=%s", "METHOD", iface->method->name);
*environend = NULL;

#line 2191 "ifupdown.nw"
	
#line 2267 "ifupdown.nw"
*(environend++) = setlocalenv("%s=%s", "MODE", mode);
*environend = NULL;
#line 2192 "ifupdown.nw"
	
#line 2272 "ifupdown.nw"
*(environend++) = setlocalenv("%s=%s", "PHASE", phase); 
*environend = NULL;
#line 2193 "ifupdown.nw"
	
#line 2282 "ifupdown.nw"
*(environend++) = setlocalenv("%s=%s", "VERBOSITY", verbose ? "1" : "0");
*environend = NULL;
#line 2194 "ifupdown.nw"
	
#line 2277 "ifupdown.nw"
*(environend++) = setlocalenv("%s=%s", "PATH", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");
*environend = NULL;
#line 2195 "ifupdown.nw"
}
#line 2299 "ifupdown.nw"
static char *setlocalenv(char *format, char *name, char *value) {
	char *result;

	
#line 2317 "ifupdown.nw"
result = malloc(strlen(format)   /* -4 for the two %s's */
                + strlen(name) 
                + strlen(value) 
                + 1);
if (!result) {
	perror("malloc");
	exit(1);
}

#line 2304 "ifupdown.nw"
	sprintf(result, format, name, value);

	
#line 2333 "ifupdown.nw"
{
	char *here, *there;

	for(here = there = result; *there != '=' && *there; there++) {
		if (*there == '-') *there = '_';
		if (isalpha(*there)) *there = toupper(*there);

		if (isalnum(*there) || *there == '_') {
			*here = *there;
			here++;
		}
	}
	memmove(here, there, strlen(there) + 1);
}

#line 2308 "ifupdown.nw"
	return result;
}
#line 2362 "ifupdown.nw"
static int doit(char *str) {
	assert(str);

	if (verbose || no_act) {
		fprintf(stderr, "%s\n", str);
	}
	if (!no_act) {
		pid_t child;
		int status;

		fflush(NULL);
		switch(child = fork()) {
		    case -1: /* failure */
			return 0;
		    case 0: /* child */
			execle("/bin/sh", "/bin/sh", "-c", str, NULL, environ);
			exit(127);
		    default: /* parent */
		    	break;
		}
		waitpid(child, &status, 0);
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			return 0;
	}
	return 1;
}
#line 2408 "ifupdown.nw"
int execute_all(interface_defn *ifd, execfn *exec, char *opt) {
	int i;
	char buf[100];
	for (i = 0; i < ifd->n_options; i++) {
		if (strcmp(ifd->option[i].name, opt) == 0) {
			if (!(*exec)(ifd->option[i].value)) {
				return 0;
			}
		}
	}

	snprintf(buf, sizeof(buf), "run-parts %s /etc/network/if-%s.d",
		verbose ? "--verbose" : "", opt);

	(*exec)(buf); 

	return 1;
}
#line 2438 "ifupdown.nw"
int iface_up(interface_defn *iface) {
	if (!iface->method->up(iface,check)) return -1;

	set_environ(iface, "start", "pre-up");
	if (!execute_all(iface,doit,"pre-up")) return 0;

	if (!iface->method->up(iface,doit)) return 0;

	set_environ(iface, "start", "post-up");
	if (!execute_all(iface,doit,"up")) return 0;

	return 1;
}
#line 2454 "ifupdown.nw"
int iface_down(interface_defn *iface) {
	if (!iface->method->down(iface,check)) return -1;

	set_environ(iface, "stop", "pre-down");
	if (!execute_all(iface,doit,"down")) return 0;

	if (!iface->method->down(iface,doit)) return 0;

	set_environ(iface, "stop", "post-down");
	if (!execute_all(iface,doit,"post-down")) return 0;

	return 1;
}
#line 2483 "ifupdown.nw"
int execute(char *command, interface_defn *ifd, execfn *exec) { 
	char *out;
	int ret;

	out = parse(command, ifd);
	if (!out) { return 0; }

	ret = (*exec)(out);

	free(out);
	return ret;
}
#line 2529 "ifupdown.nw"
static char *parse(char *command, interface_defn *ifd) {
	
#line 2554 "ifupdown.nw"
char *result = NULL;
size_t pos = 0, len = 0;
#line 2648 "ifupdown.nw"
size_t old_pos[MAX_OPT_DEPTH] = {0};
int okay[MAX_OPT_DEPTH] = {1};
int opt_depth = 1;

#line 2532 "ifupdown.nw"
	while(*command) {
		switch(*command) {
			
#line 2602 "ifupdown.nw"
default:
	addstr(&result, &len, &pos, command, 1);
	command++;
	break;
#line 2615 "ifupdown.nw"
case '\\':
	if (command[1]) {
		addstr(&result, &len, &pos, command+1, 1);
		command += 2;
	} else {
		addstr(&result, &len, &pos, command, 1);
		command++;
	}
	break;
#line 2663 "ifupdown.nw"
case '[':
	if (command[1] == '[' && opt_depth < MAX_OPT_DEPTH) {
		old_pos[opt_depth] = pos;
		okay[opt_depth] = 1;
		opt_depth++;
		command += 2;
	} else {
		addstr(&result, &len, &pos, "[", 1);
		command++;
	}
	break;
#line 2677 "ifupdown.nw"
case ']':
	if (command[1] == ']' && opt_depth > 1) {
		opt_depth--;
		if (!okay[opt_depth]) {
			pos = old_pos[opt_depth];
			result[pos] = '\0';
		}
		command += 2;
	} else {
		addstr(&result, &len, &pos, "]", 1);
		command++;
	}
	break;
#line 2732 "ifupdown.nw"
case '%':
{
	
#line 2757 "ifupdown.nw"
char *nextpercent;
#line 2735 "ifupdown.nw"
	char *varvalue;

	
#line 2761 "ifupdown.nw"
command++;
nextpercent = strchr(command, '%');
if (!nextpercent) {
	errno = EUNBALPER;
	free(result);
	return NULL;
}

#line 2739 "ifupdown.nw"
	
#line 2820 "ifupdown.nw"
varvalue = get_var(command, nextpercent - command, ifd);

#line 2741 "ifupdown.nw"
	if (varvalue) {
		addstr(&result, &len, &pos, varvalue, strlen(varvalue));
	} else {
		okay[opt_depth - 1] = 0;
	}

	
#line 2771 "ifupdown.nw"
command = nextpercent + 1;

#line 2749 "ifupdown.nw"
	break;
}
#line 2535 "ifupdown.nw"
		}
	}

	
#line 2707 "ifupdown.nw"
if (opt_depth > 1) {
	errno = EUNBALBRACK;
	free(result);
	return NULL;
}

if (!okay[0]) {
	errno = EUNDEFVAR;
	free(result);
	return NULL;
}

#line 2540 "ifupdown.nw"
	
#line 2561 "ifupdown.nw"
return result;
#line 2541 "ifupdown.nw"
}
#line 2576 "ifupdown.nw"
void addstr(char **buf, size_t *len, size_t *pos, char *str, size_t strlen) {
	assert(*len >= *pos);
	assert(*len == 0 || (*buf)[*pos] == '\0');

	if (*pos + strlen >= *len) {
		char *newbuf;
		newbuf = realloc(*buf, *len * 2 + strlen + 1);
		if (!newbuf) {
			perror("realloc");
			exit(1); /* a little ugly */
		}
		*buf = newbuf;
		*len = *len * 2 + strlen + 1;
	}

	while (strlen-- >= 1) {
		(*buf)[(*pos)++] = *str;
		str++;
	}
	(*buf)[*pos] = '\0';
}
#line 2783 "ifupdown.nw"
int strncmpz(char *l, char *r, size_t llen) {
	int i = strncmp(l, r, llen);
	if (i == 0)
		return -r[llen];
	else
		return i;
}
#line 2800 "ifupdown.nw"
char *get_var(char *id, size_t idlen, interface_defn *ifd) {
	int i;

	if (strncmpz(id, "iface", idlen) == 0) {
		return ifd->real_iface;
	} else {
		for (i = 0; i < ifd->n_options; i++) {
			if (strncmpz(id, ifd->option[i].name, idlen) == 0) {
				return ifd->option[i].value;
			}
		}
	}

	return NULL;
}
#line 2839 "ifupdown.nw"
int run_mapping(char *physical, char *logical, int len, mapping_defn *map) {
	FILE *in, *out;
	int i, status;
	pid_t pid;

	
#line 2898 "ifupdown.nw"
pid = popen2(&in, &out, map->script, physical, NULL);
if (pid == 0) {
	return 0;
}
#line 2845 "ifupdown.nw"
	
#line 2857 "ifupdown.nw"
for (i = 0; i < map->n_mappings; i++) {
	fprintf(in, "%s\n", map->mapping[i]);
}
fclose(in);
#line 2846 "ifupdown.nw"
	
#line 2864 "ifupdown.nw"
waitpid(pid, &status, 0);
#line 2847 "ifupdown.nw"
	
#line 2868 "ifupdown.nw"
if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
	if (fgets(logical, len, out)) {
		char *pch = logical + strlen(logical) - 1;
		while (pch >= logical && isspace(*pch)) 
			*(pch--) = '\0';
	}
}
fclose(out);	

#line 2849 "ifupdown.nw"
	return 1;
}
#line 2912 "ifupdown.nw"
static int popen2(FILE **in, FILE **out, char *command, ...) {
	va_list ap;
	char *argv[11] = {command};
	int argc;
	int infd[2], outfd[2];
	pid_t pid;

	argc = 1;
	va_start(ap, command);
	while((argc < 10) && (argv[argc] = va_arg(ap, char*))) {
		argc++;
	}
	argv[argc] = NULL; /* make sure */
	va_end(ap);

	if (pipe(infd) != 0) return 0;
	if (pipe(outfd) != 0) {
		close(infd[0]); close(infd[1]);
		return 0;
	}

	fflush(NULL);
	switch(pid = fork()) {
		case -1: /* failure */
			close(infd[0]); close(infd[1]);
			close(outfd[0]); close(outfd[1]);
			return 0;
		case 0: /* child */
			dup2(infd[0], 0);
			dup2(outfd[1], 1);
			close(infd[0]); close(infd[1]);
			close(outfd[0]); close(outfd[1]);
			execvp(command, argv);
			exit(127);
		default: /* parent */
			*in = fdopen(infd[1], "w");
			*out = fdopen(outfd[0], "r");
			close(infd[0]);	close(outfd[1]);
			return pid;
	}
	/* unreached */
}
