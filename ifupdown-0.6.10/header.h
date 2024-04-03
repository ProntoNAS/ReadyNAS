#line 91 "ifupdown.nw"
#ifndef HEADER_H
#define HEADER_H

#line 392 "ifupdown.nw"
typedef struct address_family address_family;
#line 425 "ifupdown.nw"
typedef struct method method;
#line 1069 "ifupdown.nw"
typedef struct interfaces_file interfaces_file;
#line 1085 "ifupdown.nw"
typedef struct allowup_defn allowup_defn;
#line 1104 "ifupdown.nw"
typedef struct interface_defn interface_defn;
#line 1129 "ifupdown.nw"
typedef struct variable variable;
#line 1145 "ifupdown.nw"
typedef struct mapping_defn mapping_defn;
#line 441 "ifupdown.nw"
typedef int (execfn)(char *command);
typedef int (command_set)(interface_defn *ifd, execfn *e);
#line 396 "ifupdown.nw"
struct address_family {
	char *name;
	int n_methods;
	method *method;
};
#line 429 "ifupdown.nw"
struct method {
	char *name;
	command_set *up, *down;
};
#line 1073 "ifupdown.nw"
struct interfaces_file {
	allowup_defn *allowups;
	interface_defn *ifaces;
	mapping_defn *mappings;
};
#line 1089 "ifupdown.nw"
struct allowup_defn {
	allowup_defn *next;

	char *when;
	int max_interfaces;
	int n_interfaces;
	char **interfaces;
};
#line 1108 "ifupdown.nw"
struct interface_defn {
	interface_defn *next;

	char *logical_iface;
	char *real_iface;

	address_family *address_family;
	method *method;

	int automatic;

	int max_options;
	int n_options;
	variable *option;
};
#line 1133 "ifupdown.nw"
struct variable {
	char *name;
	char *value;
};
#line 1149 "ifupdown.nw"
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
#line 2644 "ifupdown.nw"
#define MAX_OPT_DEPTH 10
#line 2702 "ifupdown.nw"
#define EUNBALBRACK 10001
#define EUNDEFVAR   10002
#line 2727 "ifupdown.nw"
#define MAX_VARNAME    32
#define EUNBALPER   10000
#line 408 "ifupdown.nw"
extern address_family *addr_fams[];
#line 1170 "ifupdown.nw"
interfaces_file *read_interfaces(char *filename);
#line 1943 "ifupdown.nw"
allowup_defn *find_allowup(interfaces_file *defn, char *name);
#line 2396 "ifupdown.nw"
int execute_all(interface_defn *ifd, execfn *exec, char *opt);
#line 2433 "ifupdown.nw"
int iface_up(interface_defn *iface);
int iface_down(interface_defn *iface);
#line 2476 "ifupdown.nw"
int execute(char *command, interface_defn *ifd, execfn *exec);
#line 2835 "ifupdown.nw"
int run_mapping(char *physical, char *logical, int len, mapping_defn *map);
#line 3161 "ifupdown.nw"
extern int no_act;
extern int verbose;

#line 100 "ifupdown.nw"
#endif /* HEADER_H */
