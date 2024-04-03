/*
 * parser.c : Utility for the Linux Multiple Devices driver
 *            Copyright (C) 1997 Ingo Molnar, Miguel de Icaza, Gadi Oxman
 *            Copyright (C) 1998 Erik Troan
 *
 * This is the config file parser. Lets rewrite it in Yacc+Lex if it gets
 * too complex.
 *
 * This source is covered by the GNU GPL, the same as all Linux kernel
 * sources.
 */

#include "common.h"
#include "parser.h"

char *parity_algorithm_table[] = {
	"left-asymmetric",
	"right-asymmetric",
	"left-symmetric",
	"right-symmetric",
	NULL
};

static int parity_algorithm_to_num(char *val)
{
	int i = 0;
	char **s = parity_algorithm_table;

	while (*s) {
		if (strcmp(*s, val) == 0)
			return i;
		s++; i++;
	}
	return -1;
}

static int process_entry (char *par, char *val_s)
{
	int val, i;
	md_raid_info_t *array;
	md_cfg_entry_t *last;

	if (!par[0] && !val_s[0]) {
#if DEBUG
		printf("Ignoring line\n");
#endif /* DEBUG */
		return 0;
	}
	if (par[0] == '#') {
#if DEBUG
		printf("Ignoring comment\n");
#endif /* DEBUG */
		return 0;
	}
	val = isdigit(val_s[0]) ? atoi(val_s) : -1;
	if (strcmp(par, "raiddev") == 0) {
		if ((cfg = malloc(sizeof(md_cfg_entry_t))) == NULL) {
			fprintf(stderr, "couldn't allocate configuration entry\n");
			return 1;
		}
#if DEBUG
		printf("Processing MD device %s\n", val_s);
#endif
		memset(cfg, 0, sizeof(*cfg));
		if ((cfg->md_name = malloc(strlen(val_s) + 1)) == NULL) {
			fprintf(stderr, "out of memory\n");
			return 1;
		}
		strcpy(cfg->md_name, val_s);
		cfg->array.param.nr_disks = 0;

		last = cfg_head;
		while (last && last->next) last = last->next;

		if (last) {
		    last->next = cfg;
		} else {
		    cfg_head = cfg;
		    cfg->next = NULL;
		}

		last = cfg;
		return 0;
	}
	if (!cfg)
		return 1;
	array = &cfg->array;
	if (strcmp(par, "raid-level") == 0) {
		if (val == -1) {
			if (!strcmp(val_s, "linear"))
		    		val = -1;
			if (!strcmp(val_s, "translucent"))
		    		val = -2;
			if (!strcmp(val_s, "lvm"))
		    		val = -3;
			if (!strcmp(val_s, "multipath"))
		    		val = -4;
		}
		if (val != 4 && val != 5 && val != 1 && val != -1 &&
			val != -2 && val != -3 && val != -4 && val != 0) {
			fprintf(stderr, "raid level %d not supported\n", val);
			return 1;
		}
		array->param.level = val;
		if ((val == 0) || (val == -1))
			array->param.not_persistent = 1;
		else
			array->param.not_persistent = 0;
		return 0;
	} else if (strcmp(par, "nr-raid-disks") == 0) {
		if (val < 0)
			return 1;
		array->param.raid_disks = val;
		return 0;
	} else if (strcmp(par, "persistent-superblock") == 0) {
		if ((val < 0) || (val > 1))
			return 1;
		array->param.not_persistent = 1-val;
		return 0;
	} else if (strcmp(par, "nr-spare-disks") == 0) {
		if ((array->param.level < 1) && (array->param.level != -4) && val) {
			fprintf(stderr, "nr-spare-disks must be zero for raid level %d\n", array->param.level);
			return 1;
		}

		if (val < 0)
			return 1;
		array->param.spare_disks = val;
		return 0;
	} else if (strcmp(par, "parity-algorithm") == 0) {
		if (array->param.level != 5) {
			fprintf(stderr, "parity-algorithm undefined for raid level %d\n", array->param.level);
			return 1;
		}
		if (val < 0)
			val = parity_algorithm_to_num(val_s);
		if (val < 0)
			return 1;
		array->param.layout = val;
		return 0;
	} else if (strcmp(par, "chunk-size") == 0) {
		if (!val || val % 4 || ((1 << (ffs(val)-1)) != val)) {
			fprintf(stderr, "invalid chunk-size (%dkB)\n", val);
			return 1;
		}
		array->param.chunk_size = val * MD_BLK_SIZ;
		return 0;
	} else if (strcmp(par, "device") == 0) {
		if (array->param.nr_disks == MD_SB_DISKS) {
			fprintf(stderr, "a maximum of %d devices in a set is supported\n", MD_SB_DISKS);
			return 1;
		}
		i = array->param.nr_disks++;
		if ((cfg->device_name[i] = malloc(strlen(val_s) + 1)) == NULL) {
			fprintf(stderr, "out of memory\n");
			return 1;
		}
		strcpy(cfg->device_name[i], val_s);
		array->disks[i].raid_disk = i;
		return 0;
	} else if (strcmp(par, "raid-disk") == 0) {
		if (!array->param.nr_disks) {
			fprintf(stderr, "\"device\" line expected\n");
			return 1;
		}
		if (val >= array->param.raid_disks) {
			fprintf(stderr, "raid-disk should be smaller than raid_disks\n");
			return 1;
		}
		i = array->param.nr_disks - 1;
		array->disks[i].raid_disk = val;
		return 0;
	} else if (strcmp(par, "spare-disk") == 0) {
		if ((array->param.level != 5) && (array->param.level != 4) && (array->param.level != 1) && (array->param.level != -4)) {
			fprintf(stderr, "spare-disk not supported for raid level %d\n", array->param.level);
			return 1;
		}
		if (!array->param.nr_disks) {
			fprintf(stderr, "\"device\" line expected\n");
			return 1;
		}
		i = array->param.nr_disks - 1;
		array->disks[i].raid_disk = i;
		return 0;
	} else if (strcmp(par, "parity-disk") == 0) {
		if (!array->param.nr_disks) {
			fprintf(stderr, "\"device\" line expected\n");
			return 1;
		}
		if (array->param.level != 4) {
			fprintf(stderr, "parity_disk only supported for raid level 4\n");
			return 1;
		}
		i = array->param.nr_disks - 1;
		array->disks[i].raid_disk = array->param.raid_disks - 1;
		return 0;
	} else if (strcmp(par, "failed-disk") == 0) {
		if ((array->param.level != 5) && (array->param.level != 4) && (array->param.level != 1)) {
			fprintf(stderr, "failed-disk not supported for raid level %d\n", array->param.level);
			return 1;
		}
		if (val >= array->param.raid_disks) {
			fprintf(stderr, "failed-disk should be smaller than raid_disks\n");
			return 1;
		}
		i = array->param.nr_disks - 1;
		array->disks[i].raid_disk = val;
		array->disks[i].state |= (1 << MD_DISK_FAULTY);
		array->param.failed_disks++;
		return 0;
	
	}
	fprintf(stderr, "unrecognized option %s\n", par);
	return 1;
}

int parse_config (FILE *fp)
{
	int nr = 0;
	char line[MAX_LINE_LENGTH], par[MAX_LINE_LENGTH], val[MAX_LINE_LENGTH];

#if DEBUG
	printf("parsing configuration file\n");
#endif

	while (1) {
		if ((fgets(line, MAX_LINE_LENGTH, fp)) == NULL)
			break;
		nr++;
#if DEBUG
		printf("\n");
		printf("parsing %s", line);
#endif /* DEBUG */
		par[0] = 0; val[0] = 0;
		sscanf(line, "\t%s\t%s\n", par, val);
#if DEBUG
		printf("par == %s, val == %s\n", par, val);
#endif /* DEBUG */
		if (process_entry(par, val)) {
			printf("detected error on line %d:\n\t%s", nr, line);
			return 1;
		}
	}
#if DEBUG
	printf("finished to parse configuration file\n");
#endif /* DEBUG */
	return 0;
}
