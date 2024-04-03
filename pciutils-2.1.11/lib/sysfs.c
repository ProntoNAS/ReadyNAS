/*
 *	$Id: $
 *
 *	The PCI Library -- Configuration Access via /sys/bus/pci
 *
 * 	Copyrigh  (c) 2003 Matthew Wilcox <willy@fc.hp.com>
 *	Copyright (c) 1997--1999 Martin Mares <mj@ucw.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>

#include "internal.h"

#define SYSFS_PATH "/sys/bus/pci/devices"

#include <sys/utsname.h> // to get the kernel version
       
/*
 *  We'd like to use pread/pwrite for configuration space accesses, but
 *  unfortunately it isn't simple at all since all libc's until glibc 2.1
 *  don't define it.
 */

#if defined(__GLIBC__) && __GLIBC__ == 2 && __GLIBC_MINOR__ > 0
/* glibc 2.1 or newer -> pread/pwrite supported automatically */

#elif defined(i386) && defined(__GLIBC__)
/* glibc 2.0 on i386 -> call syscalls directly */
#include <asm/unistd.h>
#include <syscall-list.h>
#ifndef SYS_pread
#define SYS_pread 180
#endif
static int pread(unsigned int fd, void *buf, size_t size, loff_t where)
{ return syscall(SYS_pread, fd, buf, size, where); }
#ifndef SYS_pwrite
#define SYS_pwrite 181
#endif
static int pwrite(unsigned int fd, void *buf, size_t size, loff_t where)
{ return syscall(SYS_pwrite, fd, buf, size, where); }

#elif defined(i386)
/* old libc on i386 -> call syscalls directly the old way */
#include <asm/unistd.h>
static _syscall5(int, pread, unsigned int, fd, void *, buf, size_t, size, u32, where_lo, u32, where_hi);
static _syscall5(int, pwrite, unsigned int, fd, void *, buf, size_t, size, u32, where_lo, u32, where_hi);
static int do_read(struct pci_dev *d UNUSED, int fd, void *buf, size_t size, int where) { return pread(fd, buf, size, where, 0); }
static int do_write(struct pci_dev *d UNUSED, int fd, void *buf, size_t size, int where) { return pwrite(fd, buf, size, where, 0); }
#define HAVE_DO_READ

#else
/* In all other cases we use lseek/read/write instead to be safe */
#define make_rw_glue(op) \
	static int do_##op(struct pci_dev *d, int fd, void *buf, size_t size, int where)	\
	{											\
	  struct pci_access *a = d->access;							\
	  int r;										\
	  if (a->fd_pos != where && lseek(fd, where, SEEK_SET) < 0)				\
	    return -1;										\
	  r = op(fd, buf, size);								\
	  if (r < 0)										\
	    a->fd_pos = -1;									\
	  else											\
	    a->fd_pos = where + r;								\
	  return r;										\
	}
make_rw_glue(read)
make_rw_glue(write)
#define HAVE_DO_READ
#endif

#ifndef HAVE_DO_READ
#define do_read(d,f,b,l,p) pread(f,b,l,p)
#define do_write(d,f,b,l,p) pwrite(f,b,l,p)
#endif

static void
sysfs_config(struct pci_access *a)
{
#if 0
	a->method_params[PCI_ACCESS_PROC_BUS_PCI] = PATH_PROC_BUS_PCI;
#endif
}

static int sysfs_detect(struct pci_access *a)
{

	if (access(SYSFS_PATH, R_OK)) {
                /* do not warn on no 2.6 kernels */
                struct utsname *buf;
                buf = (struct utsname *) malloc(sizeof(struct utsname));
                if(buf) {
                  int res;
                  uname(buf);
                  res=strncmp("2.6", buf->release, 3);
                  if(res==0) {
                     //a->warning("kernel version %s\n", buf->release);
                     a->warning("Cannot open %s", SYSFS_PATH);
                  }
                }  
		return 0;
	}
	a->debug("...using %s", SYSFS_PATH);
	return 1;
}

static void sysfs_init(struct pci_access *a)
{
	a->fd = -1;
}

static void sysfs_cleanup(struct pci_access *a)
{
	if (a->fd >= 0) {
		close(a->fd);
		a->fd = -1;
	}
}

static int sysfs_get_value(char *name, char *object)
{
	int fd, len;
	char buf[256];
	sprintf(buf, "%s/%s/%s", SYSFS_PATH, name, object);
	fd = open(buf, O_RDONLY);
	if (fd < 0)
		return 0;
	len = read(fd, buf, 256);
	close(fd);
	buf[len] = '\0';
	return strtol(buf, NULL, 0);
}

static int sysfs_get_resources(char *name, struct pci_dev *d)
{
	char buf[256];
	FILE *file;
	int i;
	sprintf(buf, "%s/%s/%s", SYSFS_PATH, name, "resource");
	file = fopen(buf, "r");
	if (!file)
		return errno;
	for (i = 0; i < 7; i++) {
		unsigned long long start, end, size = 0;
		if (!fgets(buf, 256, file))
			break;
		sscanf(buf, "%llx %llx", &start, &end);
		if (start)
			size = end - start + 1;
		if (i < 6) {
			d->base_addr[i] = start;
			d->size[i] = size;
		} else {
			d->rom_base_addr = start;
			d->rom_size = size;
		}
	}
	fclose(file);
	return 0;
}

static void sysfs_scan(struct pci_access *a)
{
	DIR *dir;
	struct dirent *entry;

	dir = opendir(SYSFS_PATH);
	if (!dir)
		a->error("Cannot open %s", SYSFS_PATH);
	while ((entry = readdir(dir))) {
		struct pci_dev *d;
		unsigned int dev, func;

		/* ".", ".." or a special non-device perhaps */
		if (entry->d_name[0] == '.')
			continue;

		d = pci_alloc_dev(a);
		if (sscanf(entry->d_name, "%04x:%02hx:%02x.%d",
				&d->domain, &d->bus, &dev, &func) < 4) {
			a->error("Couldn't parse %s", entry->d_name);
			pci_free_dev(d);
			continue;
		}
		d->dev = dev;
		d->func = func;
		d->vendor_id = sysfs_get_value(entry->d_name, "vendor");
		d->device_id = sysfs_get_value(entry->d_name, "device");
		d->irq = sysfs_get_value(entry->d_name, "irq");
		d->hdrtype = pci_read_byte(d, PCI_HEADER_TYPE);

		if (sysfs_get_resources(entry->d_name, d) != 0)
			a->error("when reading resources");
		d->known_fields = PCI_FILL_IDENT;
		if (!a->buscentric) {
			d->known_fields |= PCI_FILL_IRQ | PCI_FILL_BASES |
					PCI_FILL_ROM_BASE | PCI_FILL_SIZES;
		}
		pci_link_dev(a, d);
	}
	closedir(dir);
}

static int
sysfs_setup(struct pci_dev *d, int rw)
{
	struct pci_access *a = d->access;

	if (a->cached_dev != d || a->fd_rw < rw) {
		char buf[256];
		if (a->fd >= 0)
			close(a->fd);
		if (snprintf(buf, sizeof(buf), "%s/%04x:%02x:%02x.%d/config",
					SYSFS_PATH, d->domain, d->bus, d->dev,
					d->func) == sizeof(buf))
			a->error("File name too long");
		a->fd_rw = a->writeable || rw;
		a->fd = open(buf, a->fd_rw ? O_RDWR : O_RDONLY);
		if (a->fd < 0)
			a->warning("Cannot open %s", buf);
		a->cached_dev = d;
		a->fd_pos = 0;
	}
	return a->fd;
}

static int sysfs_read(struct pci_dev *d, int pos, byte *buf, int len)
{
	int fd = sysfs_setup(d, 0);
	int res;

	if (fd < 0)
		return 0;
	res = do_read(d, fd, buf, len, pos);
	if (res < 0) {
		d->access->warning("sysfs_read: read failed: %s", strerror(errno));
		return 0;
	} else if (res != len) {
		d->access->warning("sysfs_read: tried to read %d bytes at %d, but got only %d", len, pos, res);
		return 0;
	}
	return 1;
}

static int sysfs_write(struct pci_dev *d, int pos, byte *buf, int len)
{
	int fd = sysfs_setup(d, 1);
	int res;

	if (fd < 0)
		return 0;
	res = do_write(d, fd, buf, len, pos);
	if (res < 0) {
		d->access->warning("sysfs_write: write failed: %s", strerror(errno));
		return 0;
	} else if (res != len) {
		d->access->warning("sysfs_write: tried to write %d bytes at %d, but got only %d", len, pos, res);
		return 0;
	}
	return 1;
}

static void sysfs_cleanup_dev(struct pci_dev *d)
{
	if (d->access->cached_dev == d)
		d->access->cached_dev = NULL;
}

struct pci_methods pm_linux_sysfs = {
	SYSFS_PATH,
	sysfs_config,
	sysfs_detect,
	sysfs_init,
	sysfs_cleanup,
	sysfs_scan,
	pci_generic_fill_info,
	sysfs_read,
	sysfs_write,
	NULL,					/* init_dev */
	sysfs_cleanup_dev
};
