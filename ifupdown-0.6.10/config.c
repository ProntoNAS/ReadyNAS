#line 1033 "ifupdown.nw"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#line 1044 "ifupdown.nw"
#include "header.h"
#line 1263 "ifupdown.nw"
#include <errno.h>
#line 1397 "ifupdown.nw"
#include <ctype.h>
#line 1240 "ifupdown.nw"
static int get_line(char **result, size_t *result_len, FILE *f, int *line);
#line 1467 "ifupdown.nw"
static char *next_word(char *buf, char *word, int maxlen);
#line 1704 "ifupdown.nw"
static address_family *get_address_family(address_family *af[], char *name);
#line 1739 "ifupdown.nw"
static method *get_method(address_family *af, char *name);
#line 1800 "ifupdown.nw"
static int duplicate_if(interface_defn *ifa, interface_defn *ifb);
#line 1919 "ifupdown.nw"
allowup_defn *get_allowup(allowup_defn **allowups, char *name);

#line 1957 "ifupdown.nw"
allowup_defn *add_allow_up(char *filename, int line,
	 allowup_defn *allow_up, char *iface_name);
#line 1174 "ifupdown.nw"
interfaces_file *read_interfaces(char *filename) {
	
#line 1210 "ifupdown.nw"
FILE *f;
int line;
#line 1247 "ifupdown.nw"
char *buf = NULL;
size_t buf_len = 0;
#line 1447 "ifupdown.nw"
interface_defn *currif = NULL;
mapping_defn *currmap = NULL;
enum { NONE, IFACE, MAPPING } currently_processing = NONE;
#line 1458 "ifupdown.nw"
char firstword[80];
char *rest;
#line 1176 "ifupdown.nw"
	interfaces_file *defn;

	
#line 1195 "ifupdown.nw"
defn = malloc(sizeof(interfaces_file));
if (defn == NULL) {
	return NULL;
}
defn->allowups = NULL;
defn->mappings = NULL;
defn->ifaces = NULL;
#line 1179 "ifupdown.nw"
	
#line 1215 "ifupdown.nw"
f = fopen(filename, "r");
if ( f == NULL ) return NULL;
line = 0;

#line 1181 "ifupdown.nw"
	while (
#line 1255 "ifupdown.nw"
get_line(&buf,&buf_len,f,&line)
#line 1181 "ifupdown.nw"
                                             ) {
		
#line 1494 "ifupdown.nw"
rest = next_word(buf, firstword, 80);
if (rest == NULL) continue; /* blank line */

if (strcmp(firstword, "mapping") == 0) {
	
#line 1546 "ifupdown.nw"
currmap = malloc(sizeof(mapping_defn));
if (currmap == NULL) {
	
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1549 "ifupdown.nw"
}
#line 1553 "ifupdown.nw"
currmap->max_matches = 0;
currmap->n_matches = 0;
currmap->match = NULL;

while((rest = next_word(rest, firstword, 80))) {
	if (currmap->max_matches == currmap->n_matches) {
		char **tmp;
		currmap->max_matches = currmap->max_matches * 2 + 1;
		tmp = realloc(currmap->match, 
			sizeof(*tmp) * currmap->max_matches);
		if (tmp == NULL) {
			currmap->max_matches = (currmap->max_matches - 1) / 2;
			
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1566 "ifupdown.nw"
		}
		currmap->match = tmp;
	}

	currmap->match[currmap->n_matches++] = strdup(firstword);
}
#line 1575 "ifupdown.nw"
currmap->script = NULL;

currmap->max_mappings = 0;
currmap->n_mappings = 0;
currmap->mapping = NULL;
#line 1583 "ifupdown.nw"
{
	mapping_defn **where = &defn->mappings;
	while(*where != NULL) {
		where = &(*where)->next;
	}
	*where = currmap;
	currmap->next = NULL;
}
#line 1499 "ifupdown.nw"
	currently_processing = MAPPING;
} else if (strcmp(firstword, "iface") == 0) {
	
#line 1635 "ifupdown.nw"
{
	
#line 1668 "ifupdown.nw"
char iface_name[80];
char address_family_name[80];
char method_name[80];

#line 1638 "ifupdown.nw"
	
#line 1656 "ifupdown.nw"
currif = malloc(sizeof(interface_defn));
if (!currif) {
	
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1659 "ifupdown.nw"
}

#line 1640 "ifupdown.nw"
	
#line 1674 "ifupdown.nw"
rest = next_word(rest, iface_name, 80);
rest = next_word(rest, address_family_name, 80);
rest = next_word(rest, method_name, 80);

if (rest == NULL) {
	
#line 2013 "ifupdown.nw"
fprintf(stderr, "%s:%d: too few parameters for iface line\n", filename, line);
return NULL;
#line 1680 "ifupdown.nw"
}

if (rest[0] != '\0') {
	
#line 2018 "ifupdown.nw"
fprintf(stderr, "%s:%d: too many parameters for iface line\n", filename, line);
return NULL;
#line 1684 "ifupdown.nw"
}

#line 1642 "ifupdown.nw"
	
#line 1690 "ifupdown.nw"
currif->logical_iface = strdup(iface_name);
if (!currif->logical_iface) {
	
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1693 "ifupdown.nw"
}
#line 1643 "ifupdown.nw"
	
#line 1708 "ifupdown.nw"
currif->address_family = get_address_family(addr_fams, address_family_name);
if (!currif->address_family) {
	
#line 2023 "ifupdown.nw"
fprintf(stderr, "%s:%d: unknown address type\n", filename, line);
return NULL;
#line 1711 "ifupdown.nw"
}
#line 1644 "ifupdown.nw"
	
#line 1743 "ifupdown.nw"
currif->method = get_method(currif->address_family, method_name);
if (!currif->method) {
	
#line 2028 "ifupdown.nw"
fprintf(stderr, "%s:%d: unknown method\n", filename, line);
return NULL;
#line 1746 "ifupdown.nw"
	return NULL; /* FIXME */
}
#line 1645 "ifupdown.nw"
	
#line 1766 "ifupdown.nw"
currif->automatic = 1;
currif->max_options = 0;
currif->n_options = 0;
currif->option = NULL;

#line 1647 "ifupdown.nw"
	
#line 1782 "ifupdown.nw"
{
	interface_defn **where = &defn->ifaces; 
	while(*where != NULL) {
		if (duplicate_if(*where, currif)) {
			
#line 2033 "ifupdown.nw"
fprintf(stderr, "%s:%d: duplicate interface\n", filename, line);
return NULL;
#line 1787 "ifupdown.nw"
		}
		where = &(*where)->next;
	}

	*where = currif;
	currif->next = NULL;
}
#line 1648 "ifupdown.nw"
}
#line 1502 "ifupdown.nw"
	currently_processing = IFACE;
} else if (strcmp(firstword, "auto") == 0) {
	
#line 1898 "ifupdown.nw"
allowup_defn *auto_ups = get_allowup(&defn->allowups, "auto");
if (!auto_ups) {
	
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1901 "ifupdown.nw"
}
while((rest = next_word(rest, firstword, 80))) {
	if (!add_allow_up(filename, line, auto_ups, firstword))
		return NULL;
}
#line 1505 "ifupdown.nw"
	currently_processing = NONE;
} else if (strncmp(firstword, "allow-", 6) == 0 && strlen(firstword) > 6) {
	
#line 1908 "ifupdown.nw"
allowup_defn *allow_ups = get_allowup(&defn->allowups, firstword + 6);
if (!allow_ups) {
	
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1911 "ifupdown.nw"
}
while((rest = next_word(rest, firstword, 80))) {
	if (!add_allow_up(filename, line, allow_ups, firstword))
		return NULL;
}
#line 1508 "ifupdown.nw"
	currently_processing = NONE;
} else {
	
#line 1515 "ifupdown.nw"
switch(currently_processing) {
	case IFACE:
		
#line 1821 "ifupdown.nw"
if (strcmp(firstword, "post-up") == 0) {
	strcpy(firstword, "up");
}
if (strcmp(firstword, "pre-down") == 0) {
	strcpy(firstword, "down");
} 
#line 1830 "ifupdown.nw"
{
	int i;

	if (strlen (rest) == 0) {
		
#line 2059 "ifupdown.nw"
fprintf(stderr, "%s:%d: option with empty value\n", filename, line);
return NULL;
#line 1835 "ifupdown.nw"
	}

	if (strcmp(firstword, "pre-up") != 0 
	    && strcmp(firstword, "up") != 0
	    && strcmp(firstword, "down") != 0
	    && strcmp(firstword, "post-down") != 0)
        {
		for (i = 0; i < currif->n_options; i++) {
			if (strcmp(currif->option[i].name, firstword) == 0) {
				
#line 2044 "ifupdown.nw"
fprintf(stderr, "%s:%d: duplicate option\n", filename, line);
return NULL;
#line 1845 "ifupdown.nw"
			}
		}
	}
}
#line 1857 "ifupdown.nw"
if (currif->n_options >= currif->max_options) {
	
#line 1879 "ifupdown.nw"
{
	variable *opt;
	currif->max_options = currif->max_options + 10;
	opt = realloc(currif->option, sizeof(*opt) * currif->max_options);
	if (opt == NULL) {
		
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1885 "ifupdown.nw"
	}
	currif->option = opt;
}
#line 1859 "ifupdown.nw"
}

currif->option[currif->n_options].name = strdup(firstword);
currif->option[currif->n_options].value = strdup(rest);

if (!currif->option[currif->n_options].name) {
	
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1866 "ifupdown.nw"
}

if (!currif->option[currif->n_options].value) {
	
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1870 "ifupdown.nw"
}

currif->n_options++;	
#line 1518 "ifupdown.nw"
		break;
	case MAPPING:
		
#line 1598 "ifupdown.nw"
if (strcmp(firstword, "script") == 0) {
	
#line 1608 "ifupdown.nw"
if (currmap->script != NULL) {
	
#line 2049 "ifupdown.nw"
fprintf(stderr, "%s:%d: duplicate script in mapping\n", filename, line);
return NULL;
#line 1610 "ifupdown.nw"
} else {
	currmap->script = strdup(rest);
}
#line 1600 "ifupdown.nw"
} else if (strcmp(firstword, "map") == 0) {
	
#line 1616 "ifupdown.nw"
if (currmap->max_mappings == currmap->n_mappings) {
	char **opt;
	currmap->max_mappings = currmap->max_mappings * 2 + 1;
	opt = realloc(currmap->mapping, sizeof(*opt) * currmap->max_mappings);
	if (opt == NULL) {
		
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1622 "ifupdown.nw"
	}
	currmap->mapping = opt;
}
currmap->mapping[currmap->n_mappings] = strdup(rest);
currmap->n_mappings++;
#line 1602 "ifupdown.nw"
} else {
	
#line 2054 "ifupdown.nw"
fprintf(stderr, "%s:%d: misplaced option\n", filename, line);
return NULL;
#line 1604 "ifupdown.nw"
}
#line 1521 "ifupdown.nw"
		break;
	case NONE:
	default:
		
#line 2054 "ifupdown.nw"
fprintf(stderr, "%s:%d: misplaced option\n", filename, line);
return NULL;
#line 1525 "ifupdown.nw"
}
#line 1511 "ifupdown.nw"
}
#line 1183 "ifupdown.nw"
	}
	if (
#line 1267 "ifupdown.nw"
ferror(f) != 0
#line 1184 "ifupdown.nw"
                                           ) {
		
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1186 "ifupdown.nw"
	}

	
#line 1221 "ifupdown.nw"
fclose(f);
line = -1;

#line 1190 "ifupdown.nw"
	return defn;
}
#line 1280 "ifupdown.nw"
static int get_line(char **result, size_t *result_len, FILE *f, int *line) {
	
#line 1305 "ifupdown.nw"
size_t pos;

#line 1283 "ifupdown.nw"
	do {
		
#line 1312 "ifupdown.nw"
pos = 0;
#line 1285 "ifupdown.nw"
		
#line 1323 "ifupdown.nw"
do {
	
#line 1344 "ifupdown.nw"
if (*result_len - pos < 10) {
	char *newstr = realloc(*result, *result_len * 2 + 80);
	if (newstr == NULL) {
		return 0;
	}
	*result = newstr;
	*result_len = *result_len * 2 + 80;
}
#line 1325 "ifupdown.nw"
	
#line 1373 "ifupdown.nw"
if (!fgets(*result + pos, *result_len - pos, f)) {
	if (ferror(f) == 0 && pos == 0) return 0;
	if (ferror(f) != 0) return 0;
}
pos += strlen(*result + pos);
#line 1326 "ifupdown.nw"
} while(
#line 1364 "ifupdown.nw"
pos == *result_len - 1 && (*result)[pos-1] != '\n'
#line 1326 "ifupdown.nw"
                                   );

#line 1385 "ifupdown.nw"
if (pos != 0 && (*result)[pos-1] == '\n') {
	(*result)[--pos] = '\0';
}

#line 1330 "ifupdown.nw"
(*line)++;

assert( (*result)[pos] == '\0' );
#line 1286 "ifupdown.nw"
		
#line 1401 "ifupdown.nw"
{ 
	int first = 0; 
	while (isspace((*result)[first]) && (*result)[first]) {
		first++;
	}

	memmove(*result, *result + first, pos - first + 1);
	pos -= first;
}
#line 1287 "ifupdown.nw"
	} while (
#line 1425 "ifupdown.nw"
(*result)[0] == '#'
#line 1287 "ifupdown.nw"
                               );

	while (
#line 1429 "ifupdown.nw"
(*result)[pos-1] == '\\'
#line 1289 "ifupdown.nw"
                               ) {
		
#line 1433 "ifupdown.nw"
(*result)[--pos] = '\0';
#line 1291 "ifupdown.nw"
		
#line 1323 "ifupdown.nw"
do {
	
#line 1344 "ifupdown.nw"
if (*result_len - pos < 10) {
	char *newstr = realloc(*result, *result_len * 2 + 80);
	if (newstr == NULL) {
		return 0;
	}
	*result = newstr;
	*result_len = *result_len * 2 + 80;
}
#line 1325 "ifupdown.nw"
	
#line 1373 "ifupdown.nw"
if (!fgets(*result + pos, *result_len - pos, f)) {
	if (ferror(f) == 0 && pos == 0) return 0;
	if (ferror(f) != 0) return 0;
}
pos += strlen(*result + pos);
#line 1326 "ifupdown.nw"
} while(
#line 1364 "ifupdown.nw"
pos == *result_len - 1 && (*result)[pos-1] != '\n'
#line 1326 "ifupdown.nw"
                                   );

#line 1385 "ifupdown.nw"
if (pos != 0 && (*result)[pos-1] == '\n') {
	(*result)[--pos] = '\0';
}

#line 1330 "ifupdown.nw"
(*line)++;

assert( (*result)[pos] == '\0' );
#line 1292 "ifupdown.nw"
	}

	
#line 1413 "ifupdown.nw"
while (isspace((*result)[pos-1])) { /* remove trailing whitespace */
	pos--;
}
(*result)[pos] = '\0';

#line 1296 "ifupdown.nw"
	return 1;
}
#line 1471 "ifupdown.nw"
static char *next_word(char *buf, char *word, int maxlen) {
	if (!buf) return NULL;
	if (!*buf) return NULL;

	while(!isspace(*buf) && *buf) {
		if (maxlen-- > 1) *word++ = *buf;
		buf++;
	}
	if (maxlen > 0) *word = '\0';

	while(isspace(*buf) && *buf) buf++;

	return buf;
}
#line 1720 "ifupdown.nw"
static address_family *get_address_family(address_family *af[], char *name) {
	int i;
	for (i = 0; af[i]; i++) {
		if (strcmp(af[i]->name, name) == 0) {
			return af[i];
		}
	}
	return NULL;
}
#line 1751 "ifupdown.nw"
static method *get_method(address_family *af, char *name) {
	int i;
	for (i = 0; i < af->n_methods; i++) {
		if (strcmp(af->method[i].name, name) == 0) {
			return &af->method[i];
		}
	}
	return NULL;
}
#line 1804 "ifupdown.nw"
static int duplicate_if(interface_defn *ifa, interface_defn *ifb) {
	if (strcmp(ifa->logical_iface, ifb->logical_iface) != 0) return 0;
	if (ifa->address_family != ifb->address_family) return 0;
	return 1;
}
#line 1922 "ifupdown.nw"
allowup_defn *get_allowup(allowup_defn **allowups, char *name) {
	for (; *allowups; allowups = &(*allowups)->next) {
		if (strcmp((*allowups)->when, name) == 0) break;
	}
	if (*allowups == NULL) {
		*allowups = malloc(sizeof(allowup_defn));
		if (*allowups == NULL) return NULL;
		(*allowups)->when = strdup(name);
		(*allowups)->next = NULL;
		(*allowups)->max_interfaces = 0;
		(*allowups)->n_interfaces = 0;
		(*allowups)->interfaces = NULL;
	}
	return *allowups;
}
#line 1947 "ifupdown.nw"
allowup_defn *find_allowup(interfaces_file *defn, char *name) {
	allowup_defn *allowups = defn->allowups;
	for (; allowups; allowups = allowups->next) {
		if (strcmp(allowups->when, name) == 0) break;
	}
	return allowups;
}
#line 1962 "ifupdown.nw"
allowup_defn *add_allow_up(char *filename, int line,
	allowup_defn *allow_up, char *iface_name)
{
	
#line 1972 "ifupdown.nw"
{
	int i;

	for (i = 0; i < allow_up->n_interfaces; i++) {
		if (strcmp(iface_name, allow_up->interfaces[i]) == 0) {
			
#line 2038 "ifupdown.nw"
fprintf(stderr, "%s:%d: interface %s declared allow-%s twice\n", 
	filename, line, iface_name, allow_up->when);
return NULL;
#line 1978 "ifupdown.nw"
		}
	}
}
#line 1966 "ifupdown.nw"
	
#line 1984 "ifupdown.nw"
if (allow_up->n_interfaces == allow_up->max_interfaces) {
	char **tmp;
	allow_up->max_interfaces *= 2;
	allow_up->max_interfaces++;
	tmp = realloc(allow_up->interfaces, 
		sizeof(*tmp) * allow_up->max_interfaces);
	if (tmp == NULL) {
		
#line 2008 "ifupdown.nw"
perror(filename);
return NULL;
#line 1992 "ifupdown.nw"
	}
	allow_up->interfaces = tmp;
}

allow_up->interfaces[allow_up->n_interfaces] = strdup(iface_name);
allow_up->n_interfaces++;
#line 1967 "ifupdown.nw"
	return allow_up;
}
