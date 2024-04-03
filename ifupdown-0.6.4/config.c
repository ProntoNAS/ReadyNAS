#line 1012 "ifupdown.nw"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#line 1022 "ifupdown.nw"
#include "header.h"
#line 1225 "ifupdown.nw"
#include <errno.h>
#line 1359 "ifupdown.nw"
#include <ctype.h>
#line 1202 "ifupdown.nw"
static int get_line(char **result, size_t *result_len, FILE *f, int *line);
#line 1429 "ifupdown.nw"
static char *next_word(char *buf, char *word, int maxlen);
#line 1661 "ifupdown.nw"
static address_family *get_address_family(address_family *af[], char *name);
#line 1696 "ifupdown.nw"
static method *get_method(address_family *af, char *name);
#line 1757 "ifupdown.nw"
static int duplicate_if(interface_defn *ifa, interface_defn *ifb);
#line 1135 "ifupdown.nw"
interfaces_file *read_interfaces(char *filename) {
	
#line 1172 "ifupdown.nw"
FILE *f;
int line;
#line 1209 "ifupdown.nw"
char *buf = NULL;
size_t buf_len = 0;
#line 1409 "ifupdown.nw"
interface_defn *currif = NULL;
mapping_defn *currmap = NULL;
enum { NONE, IFACE, MAPPING } currently_processing = NONE;
#line 1420 "ifupdown.nw"
char firstword[80];
char *rest;
#line 1137 "ifupdown.nw"
	interfaces_file *defn;

	
#line 1156 "ifupdown.nw"
defn = malloc(sizeof(interfaces_file));
if (defn == NULL) {
	return NULL;
}
defn->max_autointerfaces = defn->n_autointerfaces = 0;
defn->autointerfaces = NULL;
defn->mappings = NULL;
defn->ifaces = NULL;
#line 1140 "ifupdown.nw"
	
#line 1177 "ifupdown.nw"
f = fopen(filename, "r");
if ( f == NULL ) return NULL;
line = 0;

#line 1142 "ifupdown.nw"
	while (
#line 1217 "ifupdown.nw"
get_line(&buf,&buf_len,f,&line)
#line 1142 "ifupdown.nw"
                                             ) {
		
#line 1454 "ifupdown.nw"
rest = next_word(buf, firstword, 80);
if (rest == NULL) continue; /* blank line */

if (strcmp(firstword, "mapping") == 0) {
	
#line 1503 "ifupdown.nw"
currmap = malloc(sizeof(mapping_defn));
if (currmap == NULL) {
	
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1506 "ifupdown.nw"
}
#line 1510 "ifupdown.nw"
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
			
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1523 "ifupdown.nw"
		}
		currmap->match = tmp;
	}

	currmap->match[currmap->n_matches++] = strdup(firstword);
}
#line 1532 "ifupdown.nw"
currmap->script = NULL;

currmap->max_mappings = 0;
currmap->n_mappings = 0;
currmap->mapping = NULL;
#line 1540 "ifupdown.nw"
{
	mapping_defn **where = &defn->mappings;
	while(*where != NULL) {
		where = &(*where)->next;
	}
	*where = currmap;
	currmap->next = NULL;
}
#line 1459 "ifupdown.nw"
	currently_processing = MAPPING;
} else if (strcmp(firstword, "iface") == 0) {
	
#line 1592 "ifupdown.nw"
{
	
#line 1625 "ifupdown.nw"
char iface_name[80];
char address_family_name[80];
char method_name[80];

#line 1595 "ifupdown.nw"
	
#line 1613 "ifupdown.nw"
currif = malloc(sizeof(interface_defn));
if (!currif) {
	
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1616 "ifupdown.nw"
}

#line 1597 "ifupdown.nw"
	
#line 1631 "ifupdown.nw"
rest = next_word(rest, iface_name, 80);
rest = next_word(rest, address_family_name, 80);
rest = next_word(rest, method_name, 80);

if (rest == NULL) {
	
#line 1885 "ifupdown.nw"
fprintf(stderr, "%s:%d: too few parameters for iface line\n", filename, line);
return NULL;
#line 1637 "ifupdown.nw"
}

if (rest[0] != '\0') {
	
#line 1890 "ifupdown.nw"
fprintf(stderr, "%s:%d: too many parameters for iface line\n", filename, line);
return NULL;
#line 1641 "ifupdown.nw"
}

#line 1599 "ifupdown.nw"
	
#line 1647 "ifupdown.nw"
currif->iface = strdup(iface_name);
if (!currif->iface) {
	
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1650 "ifupdown.nw"
}
#line 1600 "ifupdown.nw"
	
#line 1665 "ifupdown.nw"
currif->address_family = get_address_family(addr_fams, address_family_name);
if (!currif->address_family) {
	
#line 1895 "ifupdown.nw"
fprintf(stderr, "%s:%d: unknown address type\n", filename, line);
return NULL;
#line 1668 "ifupdown.nw"
}
#line 1601 "ifupdown.nw"
	
#line 1700 "ifupdown.nw"
currif->method = get_method(currif->address_family, method_name);
if (!currif->method) {
	
#line 1900 "ifupdown.nw"
fprintf(stderr, "%s:%d: unknown method\n", filename, line);
return NULL;
#line 1703 "ifupdown.nw"
	return NULL; /* FIXME */
}
#line 1602 "ifupdown.nw"
	
#line 1723 "ifupdown.nw"
currif->automatic = 1;
currif->max_options = 0;
currif->n_options = 0;
currif->option = NULL;

#line 1604 "ifupdown.nw"
	
#line 1739 "ifupdown.nw"
{
	interface_defn **where = &defn->ifaces; 
	while(*where != NULL) {
		if (duplicate_if(*where, currif)) {
			
#line 1905 "ifupdown.nw"
fprintf(stderr, "%s:%d: duplicate interface\n", filename, line);
return NULL;
#line 1744 "ifupdown.nw"
		}
		where = &(*where)->next;
	}

	*where = currif;
	currif->next = NULL;
}
#line 1605 "ifupdown.nw"
}
#line 1462 "ifupdown.nw"
	currently_processing = IFACE;
} else if (strcmp(firstword, "auto") == 0) {
	
#line 1837 "ifupdown.nw"
while((rest = next_word(rest, firstword, 80))) {
	
#line 1844 "ifupdown.nw"
{
	int i;

	for (i = 0; i < defn->n_autointerfaces; i++) {
		if (strcmp(firstword, defn->autointerfaces[i]) == 0) {
			
#line 1910 "ifupdown.nw"
fprintf(stderr, "%s:%d: interface declared auto twice\n", filename, line);
return NULL;
#line 1850 "ifupdown.nw"
		}
	}
}
#line 1839 "ifupdown.nw"
	
#line 1856 "ifupdown.nw"
if (defn->n_autointerfaces == defn->max_autointerfaces) {
	char **tmp;
	defn->max_autointerfaces *= 2;
	defn->max_autointerfaces++;
	tmp = realloc(defn->autointerfaces, 
		sizeof(*tmp) * defn->max_autointerfaces);
	if (tmp == NULL) {
		
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1864 "ifupdown.nw"
	}
	defn->autointerfaces = tmp;
}

defn->autointerfaces[defn->n_autointerfaces] = strdup(firstword);
defn->n_autointerfaces++;
#line 1840 "ifupdown.nw"
}
#line 1465 "ifupdown.nw"
	currently_processing = NONE;
} else {
	
#line 1472 "ifupdown.nw"
switch(currently_processing) {
	case IFACE:
		
#line 1777 "ifupdown.nw"
{
	int i;
	if (strcmp(firstword, "up") != 0
	    && strcmp(firstword, "down") != 0
	    && strcmp(firstword, "pre-up") != 0
	    && strcmp(firstword, "post-down") != 0)
        {
		for (i = 0; i < currif->n_options; i++) {
			if (strcmp(currif->option[i].name, firstword) == 0) {
				
#line 1915 "ifupdown.nw"
fprintf(stderr, "%s:%d: duplicate option\n", filename, line);
return NULL;
#line 1787 "ifupdown.nw"
			}
		}
	}
}
#line 1798 "ifupdown.nw"
if (currif->n_options >= currif->max_options) {
	
#line 1820 "ifupdown.nw"
{
	variable *opt;
	currif->max_options = currif->max_options + 10;
	opt = realloc(currif->option, sizeof(*opt) * currif->max_options);
	if (opt == NULL) {
		
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1826 "ifupdown.nw"
	}
	currif->option = opt;
}
#line 1800 "ifupdown.nw"
}

currif->option[currif->n_options].name = strdup(firstword);
currif->option[currif->n_options].value = strdup(rest);

if (!currif->option[currif->n_options].name) {
	
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1807 "ifupdown.nw"
}

if (!currif->option[currif->n_options].value) {
	
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1811 "ifupdown.nw"
}

currif->n_options++;	
#line 1475 "ifupdown.nw"
		break;
	case MAPPING:
		
#line 1555 "ifupdown.nw"
if (strcmp(firstword, "script") == 0) {
	
#line 1565 "ifupdown.nw"
if (currmap->script != NULL) {
	
#line 1920 "ifupdown.nw"
fprintf(stderr, "%s:%d: duplicate script in mapping\n", filename, line);
return NULL;
#line 1567 "ifupdown.nw"
} else {
	currmap->script = strdup(rest);
}
#line 1557 "ifupdown.nw"
} else if (strcmp(firstword, "map") == 0) {
	
#line 1573 "ifupdown.nw"
if (currmap->max_mappings == currmap->n_mappings) {
	char **opt;
	currmap->max_mappings = currmap->max_mappings * 2 + 1;
	opt = realloc(currmap->mapping, sizeof(*opt) * currmap->max_mappings);
	if (opt == NULL) {
		
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1579 "ifupdown.nw"
	}
	currmap->mapping = opt;
}
currmap->mapping[currmap->n_mappings] = strdup(rest);
currmap->n_mappings++;
#line 1559 "ifupdown.nw"
} else {
	
#line 1925 "ifupdown.nw"
fprintf(stderr, "%s:%d: misplaced option\n", filename, line);
return NULL;
#line 1561 "ifupdown.nw"
}
#line 1478 "ifupdown.nw"
		break;
	case NONE:
	default:
		
#line 1925 "ifupdown.nw"
fprintf(stderr, "%s:%d: misplaced option\n", filename, line);
return NULL;
#line 1482 "ifupdown.nw"
}
#line 1468 "ifupdown.nw"
}
#line 1144 "ifupdown.nw"
	}
	if (
#line 1229 "ifupdown.nw"
ferror(f) != 0
#line 1145 "ifupdown.nw"
                                           ) {
		
#line 1880 "ifupdown.nw"
perror(filename);
return NULL;
#line 1147 "ifupdown.nw"
	}

	
#line 1183 "ifupdown.nw"
fclose(f);
line = -1;

#line 1151 "ifupdown.nw"
	return defn;
}
#line 1242 "ifupdown.nw"
static int get_line(char **result, size_t *result_len, FILE *f, int *line) {
	
#line 1267 "ifupdown.nw"
size_t pos;

#line 1245 "ifupdown.nw"
	do {
		
#line 1274 "ifupdown.nw"
pos = 0;
#line 1247 "ifupdown.nw"
		
#line 1285 "ifupdown.nw"
do {
	
#line 1306 "ifupdown.nw"
if (*result_len - pos < 10) {
	char *newstr = realloc(*result, *result_len * 2 + 80);
	if (newstr == NULL) {
		return 0;
	}
	*result = newstr;
	*result_len = *result_len * 2 + 80;
}
#line 1287 "ifupdown.nw"
	
#line 1335 "ifupdown.nw"
if (!fgets(*result + pos, *result_len - pos, f)) {
	if (ferror(f) == 0 && pos == 0) return 0;
	if (ferror(f) != 0) return 0;
}
pos += strlen(*result + pos);
#line 1288 "ifupdown.nw"
} while(
#line 1326 "ifupdown.nw"
pos == *result_len - 1 && (*result)[pos-1] != '\n'
#line 1288 "ifupdown.nw"
                                   );

#line 1347 "ifupdown.nw"
if (pos != 0 && (*result)[pos-1] == '\n') {
	(*result)[--pos] = '\0';
}

#line 1292 "ifupdown.nw"
(*line)++;

assert( (*result)[pos] == '\0' );
#line 1248 "ifupdown.nw"
		
#line 1363 "ifupdown.nw"
{ 
	int first = 0; 
	while (isspace((*result)[first]) && (*result)[first]) {
		first++;
	}

	memmove(*result, *result + first, pos - first + 1);
	pos -= first;
}
#line 1249 "ifupdown.nw"
	} while (
#line 1387 "ifupdown.nw"
(*result)[0] == '#'
#line 1249 "ifupdown.nw"
                               );

	while (
#line 1391 "ifupdown.nw"
(*result)[pos-1] == '\\'
#line 1251 "ifupdown.nw"
                               ) {
		
#line 1395 "ifupdown.nw"
(*result)[--pos] = '\0';
#line 1253 "ifupdown.nw"
		
#line 1285 "ifupdown.nw"
do {
	
#line 1306 "ifupdown.nw"
if (*result_len - pos < 10) {
	char *newstr = realloc(*result, *result_len * 2 + 80);
	if (newstr == NULL) {
		return 0;
	}
	*result = newstr;
	*result_len = *result_len * 2 + 80;
}
#line 1287 "ifupdown.nw"
	
#line 1335 "ifupdown.nw"
if (!fgets(*result + pos, *result_len - pos, f)) {
	if (ferror(f) == 0 && pos == 0) return 0;
	if (ferror(f) != 0) return 0;
}
pos += strlen(*result + pos);
#line 1288 "ifupdown.nw"
} while(
#line 1326 "ifupdown.nw"
pos == *result_len - 1 && (*result)[pos-1] != '\n'
#line 1288 "ifupdown.nw"
                                   );

#line 1347 "ifupdown.nw"
if (pos != 0 && (*result)[pos-1] == '\n') {
	(*result)[--pos] = '\0';
}

#line 1292 "ifupdown.nw"
(*line)++;

assert( (*result)[pos] == '\0' );
#line 1254 "ifupdown.nw"
	}

	
#line 1375 "ifupdown.nw"
while (isspace((*result)[pos-1])) { /* remove trailing whitespace */
	pos--;
}
(*result)[pos] = '\0';

#line 1258 "ifupdown.nw"
	return 1;
}
#line 1433 "ifupdown.nw"
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
#line 1677 "ifupdown.nw"
static address_family *get_address_family(address_family *af[], char *name) {
	int i;
	for (i = 0; af[i]; i++) {
		if (strcmp(af[i]->name, name) == 0) {
			return af[i];
		}
	}
	return NULL;
}
#line 1708 "ifupdown.nw"
static method *get_method(address_family *af, char *name) {
	int i;
	for (i = 0; i < af->n_methods; i++) {
		if (strcmp(af->method[i].name, name) == 0) {
			return &af->method[i];
		}
	}
	return NULL;
}
#line 1761 "ifupdown.nw"
static int duplicate_if(interface_defn *ifa, interface_defn *ifb) {
	if (strcmp(ifa->iface, ifb->iface) != 0) return 0;
	if (ifa->address_family != ifb->address_family) return 0;
	return 1;
}
