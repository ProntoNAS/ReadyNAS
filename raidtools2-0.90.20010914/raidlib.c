
/*
   raidlib.c : Multiple Devices tools for Linux
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

#include <string.h>
#include <popt.h>
#include <asm/page.h>
#include <sys/sysmacros.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE (sizeof(long)*1024)
#endif

struct md_version md_ver;

#define OLD_MDTOOLS ((md_ver.major == 0) && (md_ver.minor < 0.50))

static int save_errno = 0;

md_cfg_entry_t *cfg_head = NULL, *cfg = NULL;
int do_quiet_flag = 0;

int open_or_die (char *file)
{
  int fd;
  
  if ((fd=open (file, O_RDWR))==-1)
  {
    perror (file);
    exit (EXIT_FAILURE);
  }

  return fd;
}

int getpageshift(void)
{
   int i,pagesize,pageshift=0;

   pagesize=getpagesize();
   for (i=1; i <= 8*sizeof(int); i++) {
       if( pagesize & (1UL << i)) {
           pageshift += i;
       }
   }
   return pageshift;
}

/*
 * converts a size in bytes to 'factor' metrics, which is a
 * kernel-internal way of dealing with chunk sizes and stuff.
 * It's the number of pages within the given number.
 */
static int s2f (int bytes)
{
    int factor, c, i;
    int kbytes, pageshift;

    if (bytes % 1024) {
	fprintf (stderr, "chunk_size must be an integral number of k\n");
	return 0;
    }
    kbytes = bytes >> 10;
   
    pageshift=getpageshift();
    factor = kbytes >> (pageshift - 10);
 
    for (i=1, c=0; i<kbytes; i*=2)
	if (factor & i) c++;
    
    if ((kbytes*MD_BLK_SIZ) % PAGE_SIZE || c!=1) {
	fprintf (stderr, "Cannot handle %dk chunks. Defaulting to %dk\n",
		 kbytes, 1 << (pageshift-10));
	return (0);
    }

    for (i=0; !(factor & (1 << i)); i++);
    return (i);
}

md_u32 get_random(void)
{
        md_u32 num;
        int fd;

        fd = open("/dev/urandom", O_RDONLY);
        if (fd != -1 && read(fd, &num, 4) == 4) {
#if DEBUG
                printf("raid set magic: %x\n", num);
#endif
                close(fd);
        } else {
                num = rand();
#if DEBUG
                printf("raid set magic (pseudo-random): %x\n", num);
#endif
        }
        return num;
}

static int do_mdrun (int fd, char *dev, struct md_param *param) {
    int rc;
    int pers;

	/* Old versions of md (< 0.50) used this instead. Thank Ingo
	   for fixing it. */
	if (OLD_MDTOOLS) {
	    pers = param->personality << MD_PERSONALITY_SHIFT;
	    if ((param->personality == LINEAR || param->personality == RAID0)
		&& param->chunk_size)
		    pers |= s2f(param->chunk_size);
	    rc = ioctl (fd, START_MD, (unsigned long)pers);
	    return 0;
	}

    if ((rc = ioctl (fd, RUN_ARRAY, (unsigned long)param))) {
	    save_errno=errno;
	    switch (save_errno) {
	      case EBUSY:
		fprintf(stderr,"%s: already running\n",dev);
		break;
		/* fall through */
	      default:
		perror (dev);
	    }
	    errno=save_errno;
	    return 1;
    }
    return 0;
}
static int do_mdstart (int fd, char *dev, dev_t rdev) {
    int rc;

    if ((rc = ioctl (fd, START_ARRAY, (unsigned long)rdev))) {
	    save_errno=errno;
	    switch (save_errno) {
	      case EBUSY:
		fprintf(stderr,"%s: already running\n",dev);
		break;
		/* fall through */
	      default:
		perror (dev);
	    }
	    errno=save_errno;
	    return 1;
    }
    return 0;
}

int do_raidstart_rw (int fd, char *dev)
{
    int func = RESTART_ARRAY_RW;
    int rc;
    struct stat s;

    fstat (fd, &s);
    
    if (major (s.st_rdev) != MD_MAJOR) {
	fprintf(stderr,"%s: not an MD device!\n",dev);
        exit(EXIT_FAILURE);
    }

    if (OLD_MDTOOLS)
        exit(EXIT_FAILURE);

    if ((rc = ioctl (fd, func, 0UL))) {
	    save_errno = errno;
	    switch (save_errno) {
	      case EBUSY:
		fprintf(stderr,"%s: device already read-write!\n",dev);
		break;
	      case ENXIO:
		fprintf(stderr,"%s: not running!\n",dev);
		break;
	      case EINVAL:
		fprintf(stderr,"%s: old kernel?\n",dev);
		/* fall through */
	      default:
		perror (dev);
	    }
	    errno = save_errno;
	    return 1;
    }
    return 0;
}

int do_raidstop (int fd, char *dev, int ro)
{
    int func = ro ? STOP_ARRAY_RO : STOP_ARRAY;
    int rc;
    struct stat s;


    fstat (fd, &s);
    
    if (major (s.st_rdev) != MD_MAJOR) {
	fprintf(stderr,"%s: not an MD device!\n",dev);
        exit(EXIT_FAILURE);
    }

    if (OLD_MDTOOLS)
	if (func == STOP_ARRAY) {
		rc = ioctl(fd, STOP_MD, 0UL);
    		return 0;
	}

    if ((rc = ioctl (fd, func, 0UL))) {
	    save_errno = errno;
	    switch (save_errno) {
	      case ENXIO:
		fprintf(stderr,"%s: already stopped\n",dev);
		break;
	      case EINVAL:
		if (func == STOP_ARRAY_RO) {
			fprintf(stderr,"%s: old kernel!\n",dev);
			break;
		}
		/* fall through */
	      default:
		perror (dev);
	    }
	    errno = save_errno;
	    return 1;
    }
    return 0;
}

int do_raidhotadd (int md_fd, char * disk_name, char * md_name)
{
    int rc;
    struct stat s;

    if (OLD_MDTOOLS) {
	fprintf(stderr,"kernel does not support hot-add!\n");
        exit(EXIT_FAILURE);
    }
	
    fstat (md_fd, &s);
    if (major (s.st_rdev) != MD_MAJOR) {
	fprintf(stderr,"%s: not an MD device!\n",md_name);
        exit(EXIT_FAILURE);
    }

    stat (disk_name, &s);

    rc = ioctl (md_fd, HOT_ADD_DISK, (unsigned long)s.st_rdev);
    if (rc) {
	    save_errno = errno;
  	    fprintf(stderr,"%s: can not hot-add disk: ", md_name);
	    switch (save_errno) {
	      case ENXIO:
		fprintf(stderr,"disk does not exist!\n");
		break;
	      case EBUSY:
		fprintf(stderr,"disk busy!\n");
		break;
	      case ENOSPC:
		fprintf(stderr,"too small disk!\n");
		break;
	      case ENODEV:
		fprintf(stderr,"array not running!\n");
		break;
	      case EINVAL:
		fprintf(stderr,"invalid argument.\n");
		break;
	      default:
		perror (md_name);
	    }
	    errno = save_errno;
	    return 1;
    }
    return 0;
}

int do_raidhotgenerateerror (int md_fd, char * disk_name, char * md_name)
{
    int rc;
    struct stat s;

    if (OLD_MDTOOLS) {
	fprintf(stderr,"kernel does not support hot-generate-error!\n");
        exit(EXIT_FAILURE);
    }
	
    fstat (md_fd, &s);
    if (major (s.st_rdev) != MD_MAJOR) {
	fprintf(stderr,"%s: not an MD device!\n",md_name);
        exit(EXIT_FAILURE);
    }

    stat (disk_name, &s);

    rc = ioctl (md_fd, HOT_GENERATE_ERROR, (unsigned long)s.st_rdev);
    if (rc) {
	    save_errno = errno;
  	    fprintf(stderr,"%s: can not hot-generate-error disk: ", md_name);
	    switch (save_errno) {
	      case ENXIO:
		fprintf(stderr,"disk not in array!\n");
		break;
	      case EBUSY:
		fprintf(stderr,"disk busy!\n");
		break;
	      case ENOSPC:
		fprintf(stderr,"too small disk!\n");
		break;
	      case ENODEV:
		fprintf(stderr,"array not running!\n");
		break;
	      case EINVAL:
		fprintf(stderr,"invalid argument.\n");
		break;
	      default:
		perror (md_name);
	    }
	    errno = save_errno;
	    return 1;
    }
    return 0;
}

int do_raidhotremove (int md_fd, char * disk_name, char * md_name)
{
    int rc;
    struct stat s;

    if (OLD_MDTOOLS) {
	fprintf(stderr,"kernel does not support hot-remove!\n");
        exit(EXIT_FAILURE);
    }
	
    fstat (md_fd, &s);
    if (major (s.st_rdev) != MD_MAJOR) {
	fprintf(stderr,"%s: not an MD device!\n",md_name);
        exit(EXIT_FAILURE);
    }

    stat (disk_name, &s);

    rc = ioctl (md_fd, HOT_REMOVE_DISK, (unsigned long)s.st_rdev);
    if (rc) {
	    save_errno = errno;
  	    fprintf(stderr,"%s: can not hot-remove disk: ", md_name);
	    switch (save_errno) {
	      case ENXIO:
		fprintf(stderr,"disk not in array!\n");
		break;
	      case EBUSY:
		fprintf(stderr,"disk busy!\n");
		break;
	      case ENOSPC:
		fprintf(stderr,"too small disk!\n");
		break;
	      case ENODEV:
		fprintf(stderr,"array not running!\n");
		break;
	      case EINVAL:
		fprintf(stderr,"invalid argument.\n");
		break;
	      default:
		perror (md_name);
	    }
	    errno = save_errno;
	    return 1;
    }
    return 0;
}

int do_raidsetfaulty (int md_fd, char * disk_name, char * md_name)
{
	int rc;
	struct stat s;

	if (OLD_MDTOOLS) {
		fprintf(stderr,"kernel does not support hot-add!\n");
		exit(EXIT_FAILURE);
	}

	fstat (md_fd, &s);
	if (major (s.st_rdev) != MD_MAJOR) {
		fprintf(stderr,"%s: not an MD device!\n",md_name);
		exit(EXIT_FAILURE);
	}

	stat (disk_name, &s);

	rc = ioctl (md_fd, SET_DISK_FAULTY, (unsigned long)s.st_rdev);
	if (rc) {
		save_errno = errno;
		fprintf(stderr,"%s: can not set disk faulty: ", md_name);
		switch (save_errno) {
			case ENXIO:
				fprintf(stderr,"disk does not exist!\n");
				break;
			case EBUSY:
				fprintf(stderr,"disk busy!\n");
				break;
			case ENOSPC:
				fprintf(stderr,"too small disk!\n");
				break;
			case ENODEV:
				fprintf(stderr,"array not running!\n");
				break;
			case EINVAL:
				fprintf(stderr,"invalid argument.\n");
				break;
			default:
				perror (md_name);
		}
		errno = save_errno;
		return 1;
	}
	return 0;
}


int handleOneConfig(enum raidFunc func, md_cfg_entry_t * cfg) {
    int rc = 0;
    int fd = -1;
    struct md_param param;

    switch (func) {
      case raidstart:
      {
	struct stat s;

	stat (cfg->device_name[0], &s);

	fd = open_or_die(cfg->md_name);
	if (do_mdstart (fd, cfg->md_name, s.st_rdev)) rc++;
	break;
      }

      case raidrun:
	memset(&param,0,sizeof(param));
	switch (cfg->array.param.level) {
	  case -4: param.personality = MULTIPATH; break;
	  case -3: param.personality = LVM; break;
	  case -2: param.personality = TRANSLUCENT; break;
	  case -1: param.personality = LINEAR; break;
	  case 0:  param.personality = RAID0; break;
	  case 1:  param.personality = RAID1; break;
	  case 4:
	  case 5:  param.personality = RAID5; break;
	  default: exit (EXIT_FAILURE);
	}

	param.chunk_size = cfg->array.param.chunk_size;

	fd = open_or_die(cfg->md_name);
	if (do_mdrun (fd, cfg->md_name, &param)) rc++;
	break;

      case raidstop:
	return 1;

      case raidstop_ro:
	return 1;

      case raidhotremove:
	return 1;

      case raidhotadd:
	return 1;

	  case raidsetfaulty:
	return 1;

      case raidhotgenerateerror:
	return 1;
    }
//    close (fd);

    return rc;
}

int prepare_raidlib (void)
{
    if (getMdVersion(&md_ver)) {
        fprintf(stderr, "cannot determine md version: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    return 0;
}

