/* Originally from Ted's losetup.c */

#define LOOPMAJOR	7

/*
 * losetup.c - setup and control loop devices
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sysmacros.h>

#include "loop.h"
#include "lomount.h"
#include "rmd160.h"
#include "xstrncpy.h"
#include "nls.h"

extern int verbose;
extern char *progname;
extern char *xstrdup (const char *s);	/* not: #include "sundries.h" */
extern void error (const char *fmt, ...);	/* idem */

#define SIZE(a) (sizeof(a)/sizeof(a[0]))

#ifdef LOOP_SET_FD

static int
loop_info64_to_old(const struct loop_info64 *info64, struct loop_info *info)
{
        memset(info, 0, sizeof(*info));
        info->lo_number = info64->lo_number;
        info->lo_device = info64->lo_device;
        info->lo_inode = info64->lo_inode;
        info->lo_rdevice = info64->lo_rdevice;
        info->lo_offset = info64->lo_offset;
        info->lo_encrypt_type = info64->lo_encrypt_type;
        info->lo_encrypt_key_size = info64->lo_encrypt_key_size;
        info->lo_flags = info64->lo_flags;
        info->lo_init[0] = info64->lo_init[0];
        info->lo_init[1] = info64->lo_init[1];
        if (info->lo_encrypt_type == LO_CRYPT_CRYPTOAPI)
                memcpy(info->lo_name, info64->lo_crypt_name, LO_NAME_SIZE);
        else
                memcpy(info->lo_name, info64->lo_file_name, LO_NAME_SIZE);
        memcpy(info->lo_encrypt_key, info64->lo_encrypt_key, LO_KEY_SIZE);

        /* error in case values were truncated */
        if (info->lo_device != info64->lo_device ||
            info->lo_rdevice != info64->lo_rdevice ||
            info->lo_inode != info64->lo_inode ||
            info->lo_offset != info64->lo_offset)
                return -EOVERFLOW;

        return 0;
}

#ifdef MAIN

static int
show_loop(char *device) {
	struct loop_info loopinfo;
	struct loop_info64 loopinfo64;
	int fd, errsv;

	if ((fd = open(device, O_RDONLY)) < 0) {
		int errsv = errno;
		fprintf(stderr, _("loop: can't open device %s: %s\n"),
			device, strerror (errsv));
		return 2;
	}

	if (ioctl(fd, LOOP_GET_STATUS64, &loopinfo64) == 0) {

		loopinfo64.lo_file_name[LO_NAME_SIZE-2] = '*';
		loopinfo64.lo_file_name[LO_NAME_SIZE-1] = 0;
		loopinfo64.lo_crypt_name[LO_NAME_SIZE-1] = 0;

		printf("%s: [%04llx]:%llu (%s)",
		       device, loopinfo64.lo_device, loopinfo64.lo_inode,
		       loopinfo64.lo_file_name);

		if (loopinfo64.lo_offset)
			printf(_(", offset %lld"), loopinfo64.lo_offset);

		if (loopinfo64.lo_sizelimit)
			printf(_(", sizelimit %lld"), loopinfo64.lo_sizelimit);

		if (loopinfo64.lo_encrypt_type ||
		    loopinfo64.lo_crypt_name[0]) {
			char *e = loopinfo64.lo_crypt_name;

			if (*e == 0 && loopinfo64.lo_encrypt_type == 1)
				e = "XOR";
			printf(_(", encryption %s (type %d)"),
			       e, loopinfo64.lo_encrypt_type);
		}
		printf("\n");
		close (fd);
		return 0;
	}

	if (ioctl(fd, LOOP_GET_STATUS, &loopinfo) == 0) {
		printf ("%s: [%04x]:%ld (%s)",
			device, loopinfo.lo_device, loopinfo.lo_inode,
			loopinfo.lo_name);

		if (loopinfo.lo_offset)
			printf(_(", offset %d"), loopinfo.lo_offset);

		if (loopinfo.lo_encrypt_type)
			printf(_(", encryption type %d\n"),
			       loopinfo.lo_encrypt_type);

		printf("\n");
		close (fd);
		return 0;
	}

	errsv = errno;
	fprintf(stderr, _("loop: can't get info on device %s: %s\n"),
		device, strerror (errsv));
	close (fd);
	return 1;
}

static int
show_used_loop_devices (void) {
	char dev[20];
	char *loop_formats[] = { "/dev/loop%d", "/dev/loop/%d" };
	int i, j, fd, permission = 0, somedev = 0;
	struct stat statbuf;
	struct loop_info loopinfo;

	for (j = 0; j < SIZE(loop_formats); j++) {
	    for(i = 0; i < 256; i++) {
		sprintf(dev, loop_formats[j], i);
		if (stat (dev, &statbuf) == 0 && S_ISBLK(statbuf.st_mode)) {
			fd = open (dev, O_RDONLY);
			if (fd >= 0) {
				if(ioctl (fd, LOOP_GET_STATUS, &loopinfo) == 0)
					show_loop(dev);
				close (fd);
				somedev++;
			} else if (errno == EACCES)
				permission++;
			continue; /* continue trying as long as devices exist */
		}
		break;
	    }
	}

	if (somedev==0 && permission) {
		error(_("%s: no permission to look at /dev/loop#"), progname);
		return 1;
	}
	return 0;
}


#endif

int
is_loop_device (const char *device) {
	struct stat statbuf;

	return (stat(device, &statbuf) == 0 &&
		S_ISBLK(statbuf.st_mode) &&
		major(statbuf.st_rdev) == LOOPMAJOR);
}

char *
find_unused_loop_device (void) {
	/* Just creating a device, say in /tmp, is probably a bad idea -
	   people might have problems with backup or so.
	   So, we just try /dev/loop[0-7]. */
	char dev[20];
	char *loop_formats[] = { "/dev/loop%d", "/dev/loop/%d" };
	int i, j, fd, somedev = 0, someloop = 0, permission = 0;
	struct stat statbuf;
	struct loop_info loopinfo;

	for (j = 0; j < SIZE(loop_formats); j++) {
	    for(i = 0; i < 256; i++) {
		sprintf(dev, loop_formats[j], i);
		if (stat (dev, &statbuf) == 0 && S_ISBLK(statbuf.st_mode)) {
			somedev++;
			fd = open (dev, O_RDONLY);
			if (fd >= 0) {
				if(ioctl (fd, LOOP_GET_STATUS, &loopinfo) == 0)
					someloop++;		/* in use */
				else if (errno == ENXIO) {
					close (fd);
					return xstrdup(dev);/* probably free */
				}
				close (fd);
			} else if (errno == EACCES)
				permission++;

			continue;/* continue trying as long as devices exist */
		}
		break;
	    }
	}

	if (!somedev)
		error(_("%s: could not find any device /dev/loop#"), progname);
	else if (!someloop && permission)
		error(_("%s: no permission to look at /dev/loop#"), progname);
	else if (!someloop)
		error(_(
		    "%s: Could not find any loop device. Maybe this kernel "
		    "does not know\n"
		    "       about the loop device? (If so, recompile or "
		    "`modprobe loop'.)"), progname);
	else
		error(_("%s: could not find any free loop device"), progname);
	return 0;
}

/*
 * A function to read the passphrase either from the terminal or from
 * an open file descriptor.
 */
static char *
xgetpass(int pfd, const char *prompt) {
	char *pass;
	int buflen, i;

        if (pfd < 0) /* terminal */
		return getpass(prompt);

	pass = NULL;
	buflen = 0;
	for (i=0; ; i++) {
		if (i >= buflen-1) {
				/* we're running out of space in the buffer.
				 * Make it bigger: */
			char *tmppass = pass;
			buflen += 128;
			pass = realloc(tmppass, buflen);
			if (pass == NULL) {
				/* realloc failed. Stop reading. */
				error("Out of memory while reading passphrase");
				pass = tmppass; /* the old buffer hasn't changed */
				break;
			}
		}
		if (read(pfd, pass+i, 1) != 1 ||
		    pass[i] == '\n' || pass[i] == 0)
			break;
	}

	if (pass == NULL)
		return "";

	pass[i] = 0;
	return pass;
}

static int
digits_only(const char *s) {
	while (*s)
		if (!isdigit(*s++))
			return 0;
	return 1;
}

int
set_loop(const char *device, const char *file, unsigned long long offset,
	 const char *encryption, int pfd, int *loopro, int keysz, int hash_pass) {
	struct loop_info64 loopinfo64;
	int fd, ffd, mode, i;
	char *pass;

	mode = (*loopro ? O_RDONLY : O_RDWR);
	if ((ffd = open(file, mode)) < 0) {
		if (!*loopro && errno == EROFS)
			ffd = open(file, mode = O_RDONLY);
		if (ffd < 0) {
			perror(file);
			return 1;
		}
	}
	if ((fd = open(device, mode)) < 0) {
		perror (device);
		close(ffd);
		return 1;
	}
	*loopro = (mode == O_RDONLY);

	memset(&loopinfo64, 0, sizeof(loopinfo64));

	xstrncpy(loopinfo64.lo_file_name, file, LO_NAME_SIZE);

	if (encryption && *encryption) {
		if (digits_only(encryption)) {
			loopinfo64.lo_encrypt_type = atoi(encryption);
		} else {
			loopinfo64.lo_encrypt_type = LO_CRYPT_CRYPTOAPI;
			snprintf(loopinfo64.lo_crypt_name, LO_NAME_SIZE,
				 "%s", encryption);
		}
	}

	loopinfo64.lo_offset = offset;

#ifdef MCL_FUTURE
	/*
	 * Oh-oh, sensitive data coming up. Better lock into memory to prevent
	 * passwd etc being swapped out and left somewhere on disk.
	 */
	if (loopinfo64.lo_encrypt_type != LO_CRYPT_NONE) {
		if(mlockall(MCL_CURRENT | MCL_FUTURE)) {
			perror("memlock");
			fprintf(stderr, _("Couldn't lock into memory, exiting.\n"));
			exit(1);
		}
	}
#endif


	if (keysz==0)
		keysz=LO_KEY_SIZE*8;
	switch (loopinfo64.lo_encrypt_type) {
	case LO_CRYPT_NONE:
		loopinfo64.lo_encrypt_key_size = 0;
		break;
	case LO_CRYPT_XOR:
		pass = getpass(_("Password: "));
		memset(loopinfo64.lo_encrypt_key, 0, LO_KEY_SIZE);
		xstrncpy(loopinfo64.lo_encrypt_key, pass, LO_KEY_SIZE);
		memset(pass, 0, strlen(pass));
		loopinfo64.lo_encrypt_key_size = LO_KEY_SIZE;
		break;
#define HASHLENGTH 20
#if 0
	case LO_CRYPT_FISH2:
	case LO_CRYPT_BLOW:
	case LO_CRYPT_IDEA:
	case LO_CRYPT_CAST128:
	case LO_CRYPT_SERPENT:
	case LO_CRYPT_MARS:
	case LO_CRYPT_RC6:
	case LO_CRYPT_3DES:
	case LO_CRYPT_DFC:
	case LO_CRYPT_RIJNDAEL:
	    {
		char keybits[2*HASHLENGTH]; 
		char *pass2;
		int passwdlen;
		int keylength;
		int i;

  		pass = xgetpass(pfd, _("Password: "));
		passwdlen=strlen(pass);
		pass2=malloc(passwdlen+2);
		pass2[0]='A';
		strcpy(pass2+1,pass);
		rmd160_hash_buffer(keybits,pass,passwdlen);
		rmd160_hash_buffer(keybits+HASHLENGTH,pass2,passwdlen+1);
		memcpy((char*)loopinfo64.lo_encrypt_key,keybits,2*HASHLENGTH);
		memset(pass, 0, passwdlen);
		memset(pass2, 0, passwdlen+1);
		free(pass2);
		keylength=0;
		for(i=0; crypt_type_tbl[i].id != -1; i++){
		         if(loopinfo64.lo_encrypt_type == crypt_type_tbl[i].id){
			         keylength = crypt_type_tbl[i].keylength;
				 break;
			 }
		}
		loopinfo64.lo_encrypt_key_size=keylength;
		break;
	    }
#endif
	default:
		if (hash_pass) {
		    char keybits[2*HASHLENGTH]; 
		    char *pass2;
		    int passwdlen;

		    pass = xgetpass(pfd, _("Password: "));
		    passwdlen=strlen(pass);
		    pass2=malloc(passwdlen+2);
		    pass2[0]='A';
		    strcpy(pass2+1,pass);
		    rmd160_hash_buffer(keybits,pass,passwdlen);
		    rmd160_hash_buffer(keybits+HASHLENGTH,pass2,passwdlen+1);
		    memset(pass, 0, passwdlen);
		    memset(pass2, 0, passwdlen+1);
		    free(pass2);

		    memcpy((char*)loopinfo64.lo_encrypt_key,keybits,keysz/8);
		    loopinfo64.lo_encrypt_key_size = keysz/8;
		} else {
		    pass = xgetpass(pfd, _("Password: "));
		    memset(loopinfo64.lo_encrypt_key, 0, LO_KEY_SIZE);
		    xstrncpy(loopinfo64.lo_encrypt_key, pass, LO_KEY_SIZE);
		    memset(pass, 0, strlen(pass));
		    loopinfo64.lo_encrypt_key_size = LO_KEY_SIZE;
		}
	}

	if (ioctl(fd, LOOP_SET_FD, ffd) < 0) {
		int rc = 1;

		if (errno == EBUSY) {
			if (verbose)
				printf(_("ioctl LOOP_SET_FD failed: %s\n"),
							strerror(errno));
			rc = 2;
		} else
			perror("ioctl: LOOP_SET_FD");

		close(fd);
		close(ffd);
		return rc;
	}
	close (ffd);

	i = ioctl(fd, LOOP_SET_STATUS64, &loopinfo64);
	if (i) {
		struct loop_info loopinfo;
		int errsv = errno;

		i = loop_info64_to_old(&loopinfo64, &loopinfo);
		if (i) {
			errno = errsv;
			perror("ioctl: LOOP_SET_STATUS64");
		} else {
			i = ioctl(fd, LOOP_SET_STATUS, &loopinfo);
			if (i)
				perror("ioctl: LOOP_SET_STATUS");
		}
		memset(&loopinfo, 0, sizeof(loopinfo));
	}
	memset(&loopinfo64, 0, sizeof(loopinfo64));

	if (i) {
		ioctl (fd, LOOP_CLR_FD, 0);
		close (fd);
		return 1;
	}
	close (fd);

	if (verbose > 1)
		printf(_("set_loop(%s,%s,%llu): success\n"),
		       device, file, offset);
	return 0;
}

int
del_loop (const char *device) {
	int fd;

	if ((fd = open (device, O_RDONLY)) < 0) {
		int errsv = errno;
		fprintf(stderr, _("loop: can't delete device %s: %s\n"),
			device, strerror (errsv));
		return 1;
	}
	if (ioctl (fd, LOOP_CLR_FD, 0) < 0) {
		perror ("ioctl: LOOP_CLR_FD");
		close(fd);
		return 1;
	}
	close (fd);
	if (verbose > 1)
		printf(_("del_loop(%s): success\n"), device);
	return 0;
}

#else /* no LOOP_SET_FD defined */
static void
mutter(void) {
	fprintf(stderr,
		_("This mount was compiled without loop support. "
		  "Please recompile.\n"));
}  

int
set_loop (const char *device, const char *file, unsigned long long offset,
	  const char *encryption, int *loopro) {
	mutter();
	return 1;
}

int
del_loop (const char *device) {
	mutter();
	return 1;
}

char *
find_unused_loop_device (void) {
	mutter();
	return 0;
}

#endif

#ifdef MAIN

#ifdef LOOP_SET_FD

#include <getopt.h>
#include <stdarg.h>

int verbose = 0;
char *progname;

static void
usage(void) {
	fprintf(stderr, _("\nUsage:\n"
  " %1$s loop_device                                  # give info\n"
  " %1$s -a | --all                                   # list all used\n"
  " %1$s -d | --detach loop_device                    # delete\n"
  " %1$s -f | --find                                  # find unused\n"
  " %1$s [ options ] {-f|--find|loop_device} file     # setup\n"
  "\nOptions:\n"
  " -e | --encryption <type> enable data encryption with specified <name/num>\n"
  " -h | --help              this help\n"
  " -o | --offset <num>      start at offset <num> into file\n"
  " -p | --pass-fd <num>     read passphrase from file descriptor <num>\n"
  " -r | --read-only         setup read-only loop device\n"
  " -s | --show              print device name (with -f <file>)\n"
  " -N | --nohashpass        Do not hash the given password (Debian hashes)\n"
  " -k | --keybits <num>     specify number of bits in the hashed key given\n"
  "                          to the cipher.  Some ciphers support several key\n"
  "                          sizes and might be more efficient with a smaller\n"
  "                          key size.  Key sizes < 128 are generally not\n"
  "                          recommended\n"
  " -v | --verbose           verbose mode\n\n"),
		progname);
	exit(1);
 }

char *
xstrdup (const char *s) {
	char *t;

	if (s == NULL)
		return NULL;

	t = strdup (s);

	if (t == NULL) {
		fprintf(stderr, _("not enough memory"));
		exit(1);
	}

	return t;
}

void
error (const char *fmt, ...) {
	va_list args;

	va_start (args, fmt);
	vfprintf (stderr, fmt, args);
	va_end (args);
	fprintf (stderr, "\n");
}


int
main(int argc, char **argv) {
	char *p, *offset, *encryption, *passfd, *device, *file;
	char *keysize;
	int delete, find, c, all;
	int res = 0;
	int showdev = 0;
	int ro = 0;
	int pfd = -1;
	int keysz = 0;
	int hash_pass = 1;
	unsigned long long off;
	struct option longopts[] = {
		{ "all", 0, 0, 'a' },
		{ "detach", 0, 0, 'd' },
		{ "encryption", 1, 0, 'e' },
		{ "find", 0, 0, 'f' },
		{ "help", 0, 0, 'h' },
		{ "keybits", 1, 0, 'k' },
		{ "nopasshash", 0, 0, 'N' },
		{ "nohashpass", 0, 0, 'N' },
		{ "offset", 1, 0, 'o' },
		{ "pass-fd", 1, 0, 'p' },
		{ "read-only", 0, 0, 'r' },
	        { "show", 0, 0, 's' },
		{ "verbose", 0, 0, 'v' },
		{ NULL, 0, 0, 0 }
	};

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	delete = find = all = 0;
	off = 0;
	offset = encryption = passfd = NULL;
	keysize = NULL;

	progname = argv[0];
	if ((p = strrchr(progname, '/')) != NULL)
		progname = p+1;

	while ((c = getopt_long(argc, argv, "ade:E:fhk:No:p:rsv",
				longopts, NULL)) != -1) {
		switch (c) {
		case 'a':
			all = 1;
			break;
		case 'r':
			ro = 1;
			break;
		case 'd':
			delete = 1;
			break;
		case 'E':
		case 'e':
			encryption = optarg;
			break;
		case 'f':
			find = 1;
			break;
		case 'k':
			keysize = optarg;
			break;
		case 'N':
			hash_pass = 0;
			break;
		case 'o':
			offset = optarg;
			break;
		case 'p':
			passfd = optarg;
			break;
		case 's':
			showdev = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			usage();
		}
	}

	if (argc == 1) {
		usage();
	} else if (delete) {
		if (argc != optind+1 || encryption || offset || find || all || showdev)
			usage();
	} else if (find) {
		if (all || argc < optind || argc > optind+1)
			usage();
	} else if (all) {
		if (argc > 2)
			usage();
	} else {
		if (argc < optind+1 || argc > optind+2)
			usage();
	}

	if (all)
		return show_used_loop_devices();
	else if (find) {
		device = find_unused_loop_device();
		if (device == NULL)
			return -1;
		if (argc == optind) {
			if (verbose)
				printf("Loop device is %s\n", device);
			printf("%s\n", device);
			return 0;
		}
		file = argv[optind];
	} else {
		device = argv[optind];
		if (argc == optind+1)
			file = NULL;
		else
			file = argv[optind+1];
	}

	if (delete)
		res = del_loop(device);
	else if (file == NULL)
		res = show_loop(device);
	else {
		if (offset && sscanf(offset, "%llu", &off) != 1)
			usage();
		if (passfd && sscanf(passfd, "%d", &pfd) != 1)
			usage();
		if (keysize && sscanf(keysize,"%d",&keysz) != 1)
			usage();
		do {
			res = set_loop(device, file, off, encryption, pfd, &ro, keysz, hash_pass);
			if (res == 2 && find) {
				if (verbose)
					printf("stolen loop=%s...trying again\n",
						device);
				free(device);
				if (!(device = find_unused_loop_device()))
					return -1;
			}
		} while (find && res == 2);

		if (verbose && res == 0)
			printf("Loop device is %s\n", device);

		if (res == 0 && showdev && find)
			printf("%s\n", device);
	}
	return res;
}

#else /* LOOP_SET_FD not defined */

int
main(int argc, char **argv) {
	fprintf(stderr,
		_("No loop support was available at compile time. "
		  "Please recompile.\n"));
	return -1;
}
#endif
#endif
