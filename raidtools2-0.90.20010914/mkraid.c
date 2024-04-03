/*
 * mkraid.c : Utility for the Linux Multiple Devices driver
 *            Copyright (C) 1997 Ingo Molnar, Miguel de Icaza, Gadi Oxman
 *            Copyright (C) 1998 Erik Troan
 *
 * This utility reads a Linux MD raid1/4/5 configuration file, writes a
 * raid superblock on the devices in the raid set, and initializes the
 * set for first time use, depending on command line switches and
 * configuration file settings.
 *
 * This source is covered by the GNU GPL, the same as all Linux kernel
 * sources.
 */

#include "common.h"
#include "parser.h"
#include "raid_io.h"
#include "raidlib.h"
#include "version.h"
#include <popt.h>

void printcfg (md_cfg_entry_t * cfg);

void usage (void) {
    printf("usage: mkraid [--configfile] [--version] [--force] [--upgrade]\n");
    printf("       [--dangerous-no-resync] [-acfhuv] </dev/md?>*\n");
}

static int makeOneRaid(struct md_version * ver, enum mkraidFunc func,
	 md_cfg_entry_t * cfg, int force, int upgrade, int resync) {
int i, ret, file;

    if (check_active(cfg)) 
	return 1;

    if (analyze_sb(ver, func, cfg, force, upgrade, resync))
	return 1;

    if (upgrade)
	return 0;
#if 0
    printcfg(cfg);
#endif

    file = open(cfg->md_name,O_RDONLY);
    ret = ioctl(file, SET_ARRAY_INFO, (unsigned long)&cfg->array.param);
    if (ret)
        return 1;

    for (i = 0; i < cfg->array.param.nr_disks; i++) {
            ret = ioctl(file, ADD_NEW_DISK,(unsigned long)(cfg->array.disks+i));
            if (ret)
                 return 1;
    }
//    close(file);

    handleOneConfig(raidrun, cfg);

    return 0;
}

int main (int argc, char *argv[])
{
    char *namestart=argv[0];
    FILE *fp = NULL;
    md_cfg_entry_t *p;
    int exit_status=0;
    int version = 0, help = 0, debug = 0;
    char * configFile = RAID_CONFIG;
    int force_flag = 0;
    int old_force_flag = 0;
    int upgrade_flag = 0;
    int no_resync_flag = 0;
    int all_flag = 0;
    char ** args;
    struct md_version ver;
    poptContext optCon;
    int i;
    enum mkraidFunc func;
    struct poptOption optionsTable[] = {
	{ "configfile", 'c', POPT_ARG_STRING, &configFile, 0 },
	{ "force", 'f', 0, &old_force_flag, 0 },
	{ "really-force", 'R', 0, &force_flag, 0 },
	{ "upgrade", 'u', 0, &upgrade_flag, 0 },
	{ "dangerous-no-resync", 'r', 0, &no_resync_flag, 0 },
	{ "help", 'h', 0, &help, 0 },
	{ "version", 'V', 0, &version, 0 },
	{ "debug", 0, 0, &debug, 0 },
	{ "all", 'a', 0, &all_flag, 0 },
	{ NULL, 0, 0, NULL, 0 }
    } ;

    optCon = poptGetContext("mkraid", argc, (const char **)argv,
			    optionsTable, 0);
    if ((i = poptGetNextOpt(optCon)) < -1) {
	fprintf(stderr, "%s: %s\n", 
		poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
		poptStrerror(i));
	usage();
	return EXIT_FAILURE;
    }

    if (prepare_raidlib())
         return EXIT_FAILURE;

    namestart = strrchr(argv[0], '/');
    if (!namestart)
        namestart = argv[0];
    else
        namestart++;

    if (!strcmp (namestart, "mkraid")) {
        func = mkraid;
	if (all_flag) {
        	fprintf (stderr, "--all not possible for mkraid!\n");
        	return (EXIT_FAILURE);
	}
    } else if (!strcmp (namestart, "raid0run")) {
        func = raid0run;
	if (old_force_flag) {
        	fprintf (stderr, "--force not possible for raid0run!\n");
        	return (EXIT_FAILURE);
	}
	if (force_flag) {
        	fprintf (stderr, "--really-force not possible for raid0run!\n");
        	return (EXIT_FAILURE);
	}
	if (upgrade_flag) {
        	fprintf (stderr, "--upgrade not possible for raid0run!\n");
        	return (EXIT_FAILURE);
	}
	if (no_resync_flag) {
        	fprintf (stderr, "--dangerous-no-resync not possible for raid0run!\n");
        	return (EXIT_FAILURE);
	}
    } else {
        fprintf (stderr, "Unknown command %s\n", argv[0]);
        usage();
        return (EXIT_FAILURE);
    }

    if (help) {
	usage();
	return EXIT_FAILURE;
    } else if (version) {
	printf("mkraid version %d.%d.%d\n", MKRAID_MAJOR_VERSION,
			MKRAID_MINOR_VERSION, MKRAID_PATCHLEVEL_VERSION);
	return EXIT_VERSION;
    }

    fp = fopen(configFile, "r");
    if (fp == NULL) {
	fprintf(stderr, "Couldn't open %s -- %s\n", configFile, 
	        strerror(errno));
	goto abort;
    }

    srand((unsigned int) time(NULL));
    if (parse_config(fp))
	goto abort;

    args = (char **)poptGetArgs(optCon);
    if (!all_flag && !args) {
	fprintf(stderr, "nothing to do!\n");
	usage();
	return EXIT_FAILURE;
    }

    if (getMdVersion(&ver)) {
	fprintf(stderr, "cannot determine md version: %s\n", strerror(errno));
	return EXIT_FAILURE;
    }

    if (old_force_flag && (func == mkraid)) {
	fprintf(stderr, 

"--force and the new RAID 0.90 hot-add/hot-remove functionality should be\n"
" used with extreme care! If /etc/raidtab is not in sync with the real array\n"
" configuration, then a --force will DESTROY ALL YOUR DATA. It's especially\n"
" dangerous to use -f if the array is in degraded mode. \n\n"
" PLEASE dont mention the --really-force flag in any email, documentation or\n"
" HOWTO, just suggest the --force flag instead. Thus everybody will read\n"
" this warning at least once :) It really sucks to LOSE DATA. If you are\n"
" confident that everything will go ok then you can use the --really-force\n"
" flag. Also, if you are unsure what this is all about, dont hesitate to\n"
" ask questions on linux-raid@vger.kernel.org\n");

	return EXIT_FAILURE;
    }

    if (debug) {
    	int file, ret;

	file = open(cfg_head->md_name, O_RDONLY);
	if (file == -1) {
		perror("could not open file");
		return EXIT_FAILURE;
	}
		
	ret = ioctl(file, PRINT_RAID_DEBUG, 0UL);
	if (ret) {
		perror("debug ioctl failed");
		return EXIT_FAILURE;
	}

	printf("dumped RAID status into the syslog.\n");
	return 0;
    }

    if (all_flag) {
	for (p = cfg_head; p; p = p->next) {
	    printf("considering %s...\n", p->md_name);
	    if ((p->array.param.level != 0) &&
			(p->array.param.level != -1)) {
		fprintf(stderr, "%s is not a RAID0 or LINEAR array, skipping.\n", p->md_name);
		continue;
	    }
	    if (!p->array.param.not_persistent) {
		fprintf(stderr, "%s is persistent, skipping.\n", p->md_name);
		return EXIT_FAILURE;
	    }
	    /*
	     * Invert the meaning of no_resync_flag from this point on
	     * to avoid confusion.
	     */
	    if (makeOneRaid(&ver, func, p, force_flag, upgrade_flag,
			!no_resync_flag))
		goto abort;
	}
    } else {

    while (*args) {
	if (force_flag) {
		fprintf(stderr, 
		"DESTROYING the contents of %s in 5 seconds, Ctrl-C if unsure!\n", *args);
		sleep(5);
	}
	for (p = cfg_head; p; p = p->next) {
	    if (strcmp(p->md_name, *args)) continue;
	    if (func == raid0run) {
		if ((p->array.param.level != 0) &&
			(p->array.param.level != -1)) {
			fprintf(stderr, "%s is not a RAID0 or LINEAR array!\n",
					p->md_name);
			return EXIT_FAILURE;
		}
		if (!p->array.param.not_persistent) {
			fprintf(stderr, "%s must be a nonpersistent RAID0 or LINEAR array!\n", p->md_name);
			return EXIT_FAILURE;
		}
	    }
	    if (makeOneRaid(&ver, func, p, force_flag, upgrade_flag, !no_resync_flag))
		goto abort;
	    break;
	}
	if (!p) {
	    fprintf(stderr, "device %s is not described in config file\n", *args);
	    exit_status++;
	}
	args++;
    }
    }

    fclose(fp);
    return 0;

abort:
    fprintf(stderr, "mkraid: aborted, see the syslog and /proc/mdstat for potential clues.\n");
    exit_status = 1;
    if (fp)
	fclose(fp);
    return exit_status;
}


#define P(x) printf("%18s: \t %d\n",#x,cfg->array.param.x)
#define DP(x) printf("%18s: \t %d\n",#x,cfg->array.disks[i].x)

void printcfg (md_cfg_entry_t * cfg)
{
	int i;

	P(major_version);
	P(minor_version);
	P(patch_version);
        P(ctime);
        P(level);
        P(size);
        P(nr_disks);
        P(raid_disks);
        P(md_minor);

        P(utime);
        P(state);
        P(active_disks);
        P(working_disks);
        P(failed_disks);
        P(spare_disks);

        P(layout);
        P(chunk_size);

	for (i = 0; i < cfg->array.param.nr_disks; i++) {
		printf("\n");

		DP(number);
		DP(major);
		DP(minor);
		DP(raid_disk);
		DP(state);
	}
}
