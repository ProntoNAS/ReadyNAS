#line 91 "ifupdown.nw"
#ifndef HEADER_H
#define HEADER_H

#line 372 "ifupdown.nw"
typedef struct address_family address_family;
#line 405 "ifupdown.nw"
typedef struct method method;
#line 1047 "ifupdown.nw"
typedef struct interfaces_file interfaces_file;
#line 1067 "ifupdown.nw"
typedef struct interface_defn interface_defn;
#line 1090 "ifupdown.nw"
typedef struct variable variable;
#line 1106 "ifupdown.nw"
typedef struct mapping_defn mapping_defn;
#line 421 "ifupdown.nw"
typedef int (execfn)(char *command);
typedef int (command_set)(interface_defn *ifd, execfn *e);
#line 376 "ifupdown.nw"
struct address_family {
	char *name;
	int n_methods;
	method *method;
};
#line 409 "ifupdown.nw"
struct method {
	char *name;
	command_set *up, *down;
};
#line 1051 "ifupdown.nw"
struct interfaces_file {
	int max_autointerfaces;
	int n_autointerfaces;
	char **autointerfaces;

	interface_defn *ifaces;
	mapping_defn *mappings;
};
#line 1071 "ifupdown.nw"
struct interface_defn {
	interface_defn *next;

	char *iface;
	address_family *address_family;
	method *method;

	int automatic;

	int max_options;
	int n_options;
	variable *option;
};
#line 1094 "ifupdown.nw"
struct variable {
	char *name;
	char *value;
};
#line 1110 "ifupdown.nw"
struct mapping_defn {
	mapping_defn *next;

	int max_matches;
	int n_matches;
	char **match;

	char *script;

	int max_mappings;
	int n_mappings;
	char **mapping;
};
#line 2470 "ifupdown.nw"
#define MAX_OPT_DEPTH 10
#line 2528 "ifupdown.nw"
#define EUNBALBRACK 10001
#define EUNDEFVAR   10002
#line 2553 "ifupdown.nw"
#define MAX_VARNAME    32
#define EUNBALPER   10000
#line 388 "ifupdown.nw"
extern address_family *addr_fams[];
#line 1131 "ifupdown.nw"
interfaces_file *read_interfaces(char *filename);
#line 2230 "ifupdown.nw"
int execute_all(interface_defn *ifd, execfn *exec, char *opt);
#line 2265 "ifupdown.nw"
int iface_up(interface_defn *iface);
int iface_down(interface_defn *iface);
#line 2302 "ifupdown.nw"
int execute(char *command, interface_defn *ifd, execfn *exec);
#line 2661 "ifupdown.nw"
int run_mapping(char *physical, char *logical, int len, mapping_defn *map);
#line 2950 "ifupdown.nw"
extern int no_act;
extern int verbose;

#line 100 "ifupdown.nw"
#endif /* HEADER_H */
