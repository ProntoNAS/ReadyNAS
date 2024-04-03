
/*
   raidstart.c : Multiple Devices tools for Linux
               Copyright (C) 1994-96 Marc ZYNGIER
	       <zyngier@ufr-info-p7.ibp.fr> or
	       <maz@gloups.fdn.fr>
	       Copyright (C) 1998 Erik Troan
	       <ewt@redhat.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   
   You should have received a copy of the GNU General Public License
   (for example /usr/src/linux/COPYING); if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  
*/

#include "common.h"
#include "parser.h"
#include "raidlib.h"
#include "version.h"

#include <popt.h>
#include <string.h>
#include <asm/page.h>
#include <sys/sysmacros.h>

#define RAIDADD_VERSION "v0.3d"


void usage(unsigned char *arg) {
    printf("usage: %s [--all] [--configfile] [--help] [--version] [-achv] </dev/md?>\n", arg);
}

int main (int argc, char *argv[]) {
    int i, all=0, readonly=0, readwrite=0, exit_status=0, version = 0, help = 0;
    char *namestart=argv[0];
    md_cfg_entry_t *p;
    char ** args;
    enum raidFunc func;
    FILE * fp;
    poptContext optCon;
    char * configFile = RAID_CONFIG;
    struct poptOption optionsTable[] = {
	{ "all", 'a', 0, &all, 0 },
	{ "configfile", 'c', POPT_ARG_STRING, &configFile, 0 },
	{ "readonly", 'r', 0, &readonly, 0 },
	{ "readwrite", 'w', 0, &readwrite, 0 },
	{ "help", 'h', 0, &help, 0 },
	{ "version", 'V', 0, &version, 0 },
	{ NULL, 0, 0, NULL, 0 }
    } ;

    namestart = strrchr(argv[0], '/');
    if (!namestart)
	namestart = argv[0];
    else
	namestart++;

    if (!strcmp (namestart, "raidstart"))
	func = raidstart;
    else if (!strcmp (namestart, "raidstop"))
	func = raidstop;
    else if (!strcmp (namestart, "raidhotadd"))
	func = raidhotadd;
    else if (!strcmp (namestart, "raidhotremove"))
	func = raidhotremove;
    else if (!strcmp (namestart, "raidsetfaulty"))
	func = raidsetfaulty;
    else if (!strcmp (namestart, "raidhotgenerateerror"))
	func = raidhotgenerateerror;
    else {
	fprintf (stderr, "Unknown command %s\n", argv[0]);
	usage(namestart);
	return (EXIT_FAILURE);
    }

    optCon = poptGetContext(namestart, argc, (const char **)argv,
			    optionsTable, 0);
    if ((i = poptGetNextOpt(optCon)) < -1) {
	fprintf(stderr, "%s: %s\n", 
		poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
		poptStrerror(i));
	usage(namestart);
	return EXIT_FAILURE;
    }

    if (help) {
	usage(namestart);
	return(EXIT_USAGE);
    } else if (version) {
	printf ("%s " RAIDADD_VERSION " compiled for md " MD_VERSION "\n",
		argv[0]);
	return (EXIT_VERSION);
    }

    fp = fopen(configFile, "r");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open %s -- %s\n", configFile, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (prepare_raidlib())
         return EXIT_FAILURE;

    if (readwrite) {
        int fd;
        if (func != raidstart) {
	   fprintf(stderr, "Can do --readwrite only with raidstart!\n");
	   return EXIT_FAILURE;
        }
	args = (char **)poptGetArgs(optCon);
	if (!args) {
	    fprintf(stderr, "nothing to do!\n");
            usage(namestart);
	    exit(EXIT_FAILURE);
	}

	fd = open_or_die(*args);

        if (do_raidstart_rw (fd, *args))
	    exit(EXIT_FAILURE);
        exit(0);
    }

    if (readonly) {
         if (func != raidstop) {
	    fprintf(stderr, "Can do --readonly only with raidstop!\n");
	    return EXIT_FAILURE;
         }
         func = raidstop_ro;
    }

    /*
     * stop is special, we want to get it done
     * without parsing the config
     */
    if ((func == raidstop) || (func == raidstop_ro)) {
        int fd;

	args = (char **)poptGetArgs(optCon);
	if (!args) {
	    fprintf(stderr, "nothing to do!\n");
            usage(namestart);
	    exit(EXIT_FAILURE);
	}

	fd = open_or_die(*args);

        if (do_raidstop (fd, *args, func == raidstop ? 0:1))
	    exit(EXIT_FAILURE);
        exit(0);
    }

    /*
     * we want to fast-handle hot-remove and hot-add too.
     */
    if ((func == raidhotremove) || (func == raidhotadd) ||
			 (func == raidhotgenerateerror) || (func == raidsetfaulty)) {
        int md_fd;
	char * md_name;

	md_name = (char *)poptGetArg(optCon);
	if (!md_name) {
	    fprintf(stderr, "nothing to do!\n");
            usage(namestart);
	    exit(EXIT_FAILURE);
	}

	
	md_fd = open_or_die(md_name);

	args = (char **)poptGetArgs(optCon);
	if (!args) {
	    fprintf(stderr, "no target disk specified!\n");
            usage(namestart);
	    exit(EXIT_FAILURE);
	}

	if (func == raidhotremove) {
	        if (do_raidhotremove (md_fd, *args, md_name))
		    exit(EXIT_FAILURE);
	} else {
		if (func == raidhotadd) {
		        if (do_raidhotadd (md_fd, *args, md_name))
			    exit(EXIT_FAILURE);
		} else if (func == raidhotgenerateerror) {
		        if (do_raidhotgenerateerror (md_fd, *args, md_name))
			    exit(EXIT_FAILURE);
		} else if (func == raidsetfaulty) {
			    if (do_raidsetfaulty (md_fd, *args, md_name))
			    exit(EXIT_FAILURE);
		}
	}
        exit(0);
    }


    if (parse_config(fp))
	exit(EXIT_FAILURE);

    if (all) {
	for (p = cfg_head; p; p = p->next)
	    exit_status += handleOneConfig(func, p);
    } else {
	args = (char **)poptGetArgs(optCon);
	if (!args) {
	    fprintf(stderr, "nothing to do!\n");
            usage(namestart);
	    return EXIT_FAILURE;
	}

	while (*args) {
	    for (p = cfg_head; p; p = p->next) {
		if (strcmp(p->md_name, *args)) continue;
		exit_status += handleOneConfig(func, p);
		break;
	    }
	    if (!p) {
		fprintf(stderr, "device %s is not described in config file\n", *args);
		exit_status++;
	    }
	    args++;
	}
    }

    poptFreeContext(optCon);

    return (exit_status);
}

