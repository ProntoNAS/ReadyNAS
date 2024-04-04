/*
 * mdcsrepair - repair of checksum-mismatching block for RAID-1/5/6/10
 *
 * This program code was greatly inspired by:
 * raid6check - extended consistency check for RAID-6
 * http://git.neil.brown.name/git?p=mdadm.git;a=blob_plain;f=raid6check.c;h=587ac3a8106335fd130ace7a56d74b62e32fb0d0;hb=3b9c96032c35515bda415dea54654cdb8af539eb
 *
 * Copyright (C) 2014 NETGEAR
 * Copyright (C) 2014 Hiro Sugawara
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    Author: Piergiorgio Sartor
 *    Based on "restripe.c" from "mdadm" codebase
 */

#include "mdadm.h"
#include <stdint.h>
#include <signal.h>
#include <sys/mman.h>
#include <getopt.h>
#include <err.h>

const char Name[] = "mdcsrepair";
static int debug, do_modify = 1, verbose, forceQ, lopt;

#define debug_printf(...)	\
	{	\
		if (debug) printf(__VA_ARGS__);	\
	} while (0)

#define verbose_printf(...)	\
	{	\
		if (verbose) printf(__VA_ARGS__);	\
	} while (0)

#define verbose_fprintf(...)	\
	{	\
		if (verbose) fprintf(__VA_ARGS__);	\
	} while (0)

#define THISORNUL(s) (s ? s : "")

static void __warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	if (!lopt || debug)
		vwarn(fmt, ap);
	va_end(ap);
	va_start(ap, fmt);
	if (lopt) {
		char *msg;

		vasprintf(&msg, fmt, ap);
		syslog(LOG_WARNING, "%s: %s", msg, strerror(errno));
		free(msg);
	}
	va_end(ap);
}

static void __warnx(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	if (!lopt || debug)
		vwarnx(fmt, ap);
	va_end(ap);
	va_start(ap, fmt);
	if (lopt)
		vsyslog(LOG_WARNING, fmt, ap);
	va_end(ap);
}

#define __err(ec, ...)	do {__warn(__VA_ARGS__); exit((ec)); } while (0)
#define __errx(ec, ...)	do {__warnx(__VA_ARGS__); exit((ec)); } while (0)

#define	CHKSUM_INVERT	1
typedef unsigned long u32;
extern u32 crc32c_le(u32 seed, const uint8_t *data, size_t length);
static void __crc32c(const uint8_t *data, size_t len, uint8_t *result,
		unsigned int flags)
{
	u32 res = crc32c_le(~0, data, len);
	if (flags & CHKSUM_INVERT)
		res = ~res;
	memcpy(result, &res, sizeof res);
}

extern u32 crc32(u32 seed, const uint8_t *data, size_t length);
static void __crc32(const uint8_t *data, size_t len, uint8_t *result,
		unsigned int flags)
{
	u32 res = crc32(~0, data, len);
	if (flags & CHKSUM_INVERT)
		res = ~res;
	memcpy(result, &res, sizeof res);
}

static struct chksum {
	const char *name;
	void (*summer)(const uint8_t *, size_t, uint8_t *, unsigned int);
	size_t len;
	unsigned int flags;
	uint8_t *value;
} chksummer[] = {
	{/* Default at top */
		"crc32c_le",
		__crc32c,
		4,
		0,
		NULL,
	},
	{
		"crc32_le",
		__crc32,
		4,
		0,
		NULL,
	},
	{ NULL, NULL, 0, 0, NULL, },
};

/*
 * Convention:
 *	slot = Physical disk layout indexed by md
 *	disk = same as slot
 *	cinx = chunk index. Logical chunk index 0...N-1,P,Q
 */
enum {BAD, GOOD};
struct mdrepair {
	unsigned long long mdstart;
	ssize_t length;
	int (*repairer)(struct mdinfo *info, struct mdrepair *rep);
	struct chksum chksum[2];
	unsigned long long tot_stripes;
	unsigned long long start_stripe;
	int dslots_per_stripe;
	int near_copies;
	int far_copies;	/* negative for "offset" */
	int max_failed_disks;
	const char *mddev;
	int mdfd;

	int data_disks;
	int bad_cinx, bad_disk;

	uint8_t **buf;
	char **disk_name;
	int *fd;
	unsigned long long *disk_start;

	uint8_t **buf_by_cinx;
	int *cinx2disk;
};

#define	BUF(rep, n)	(&(rep)->buf[(rep)->length * (n)])

static void set_chksummer(struct chksum *cs, const char *name)
{
	struct chksum *cst = &chksummer[0];
	while (cst->name) {
		if (!strcmp(cst->name, name)) {
			*cs = *cst;
			return;
		}
		cst++;
	}
	__errx(1, "invalid chksummer name '%s'", name);
}

void make_tables(void);
int geo_map(int block, unsigned long long stripe, int raid_disks,
	    int level, int layout);
void qsyndrome(uint8_t *p, uint8_t *q, uint8_t **sources, int disks, int size);
void make_tables(void);
void ensure_zero_has_size(int chunk_size);
extern uint8_t *zero;
void raid6_datap_recov(int disks, size_t bytes, int faila, uint8_t **ptrs);
void raid6_2data_recov(int disks, size_t bytes, int faila, int failb,
		       uint8_t **ptrs);
void xor_blocks(char *target, char **sources, int disks, int size);

static void flush_stripe_cache(struct mdinfo *info, struct mdrepair *rep)
{
	switch (info->array.level) {
	case 5:
	case 6:
		break;
	default:
		return;
	}
	while (sysfs_set_num(info, NULL,
			"stripe_cache_invalidate", rep->mdstart))
		if (errno == EBUSY)
			usleep(1000);
		else {
			__warn("failed to flush stripe cache %s @ %llu",
				rep->mddev, rep->mdstart);
			break;
		}
}

static void flush_vm_cache(void)
{
	system("echo 3 >/proc/sys/vm/drop_caches");
}

static void send_notification(const struct mdrepair *rep, const char *path, int status)
{
	char vol[64];
	char *p;

	if (path) {
		strncpy(vol, path + 1, sizeof(vol));
		vol[63] = '\0';
		p = strchr(vol, '/');
		if (p)
			*p = '\0';
	} else
		vol[0] = '\0';

	signal(SIGCHLD, SIG_IGN);
	if (!fork()) {
		execl("/usr/bin/rnutil", "bit_rot_event", vol,
			THISORNUL(rep->disk_name[rep->bad_disk]), THISORNUL(path),
			status ? "1" : "0", NULL);
		exit(1);
	}
}

static int lock_stripe(struct mdinfo *info, struct mdrepair *rep,
		unsigned long long mdstart, sighandler_t *sig) {
	int rv;
	if (mlockall(MCL_CURRENT | MCL_FUTURE))
		return 2;

	sig[0] = signal(SIGTERM, SIG_IGN);
	sig[1] = signal(SIGINT, SIG_IGN);
	sig[2] = signal(SIGQUIT, SIG_IGN);

	mdstart /= info->array.chunk_size * rep->dslots_per_stripe;
	mdstart *= info->array.chunk_size * rep->dslots_per_stripe;
	unsigned long long mdend = mdstart +
			info->array.chunk_size * rep->dslots_per_stripe;

	rv = sysfs_set_num(info, NULL, "suspend_lo", mdstart);
	rv |= sysfs_set_num(info, NULL, "suspend_hi", mdend);
	return rv * 256;
}

static int unlock_all_stripes(struct mdinfo *info, sighandler_t *sig) {
	int rv;
	rv = sysfs_set_num(info, NULL, "suspend_lo", 0x7FFFFFFFFFFFFFFFULL);
	rv |= sysfs_set_num(info, NULL, "suspend_hi", 0);
	rv |= sysfs_set_num(info, NULL, "suspend_lo", 0);

	signal(SIGQUIT, sig[2]);
	signal(SIGINT, sig[1]);
	signal(SIGTERM, sig[0]);

	if(munlockall() != 0)
		return 3;
	return rv * 256;
}

#define CHKSUM_BAD_OK(rep, cinx)	\
		(!chksumcmp(&(rep)->chksum[BAD],	\
				(rep)->buf_by_cinx[(cinx)], (rep)->length))
#define CHKSUM_GOOD_OK(rep, cinx)	\
		(!chksumcmp(&(rep)->chksum[GOOD],	\
				(rep)->buf_by_cinx[(cinx)], (rep)->length))

#define	cinxP(info)	((info)->array.raid_disks - 2)
#define	cinxQ(info)	((info)->array.raid_disks - 1)
#define	diskP(info, rep)	((rep)->cinx2disk[cinxP(info)])
#define	diskQ(info, rep)	((rep)->cinx2disk[cinxQ(info)])

enum PQX {P = -1, Q = -2};
#define updateP(info, rep)	updatePQX((info), (rep), P)
#define updateQ(info, rep)	updatePQX((info), (rep), Q)
#define updateX(info, rep, x)	updatePQX((info), (rep), (x))

static int updatePQX(struct mdinfo *info, struct mdrepair *rep, enum PQX pqx)
{
	int cinx = pqx;
	int rv = 0;

	switch (pqx) {
	case P:
		cinx = cinxP(info); break;
	case Q:
		cinx = cinxQ(info); break;
	}

	int disk = rep->cinx2disk[cinx];
	errno = 0;
	if (!do_modify)
		return 0;
	if (pwrite(rep->fd[disk], rep->buf[disk],
		rep->length, rep->disk_start[disk]) != rep->length) {
		rv = errno;
		__warn("failed to write %ld bytes to %s @ %llu",
			rep->length, rep->disk_name[disk],
			rep->disk_start[disk]);
	} else {
		fsync(rep->fd[disk]);
		flush_vm_cache();
	}
	return rv;
}

static void printchksum(const uint8_t *val, size_t len)
{
	while (len--)
		printf("%02x", *val++);
}

static int chksumcmp(struct chksum *cs, uint8_t *data, size_t len)
{
	uint8_t *buffer = xmalloc(cs->len);
	cs->summer(data, len, buffer, cs->flags);
	int rv = memcmp(cs->value, buffer, cs->len);

	if (debug) {
		printf("Expected: ");
		printchksum(cs->value, cs->len);
		printf(" Computed: ");
		printchksum(buffer, cs->len);
		printf("\n");
	}
	free(buffer);
	return rv;
}

static void raid56_map_cinx(struct mdinfo *info, struct mdrepair *rep)
{
	int i;
	int diskP = geo_map(-1, rep->start_stripe, info->array.raid_disks,
				info->array.level, info->array.layout);

	rep->bad_cinx = (rep->mdstart/info->array.chunk_size) %
				rep->dslots_per_stripe;
	rep->bad_disk = geo_map(rep->bad_cinx, rep->start_stripe,
					info->array.raid_disks,
					info->array.level, info->array.layout);
	rep->data_disks = info->array.raid_disks - (info->array.level - 4);

	for (i = 0; i < rep->data_disks; i++) {
		rep->cinx2disk[i] = geo_map(i, rep->start_stripe,
					info->array.raid_disks,
					info->array.level, info->array.layout);
		rep->buf_by_cinx[i] = rep->buf[rep->cinx2disk[i]];
	}

	rep->cinx2disk[rep->data_disks] = diskP;
	rep->buf_by_cinx[rep->data_disks] = rep->buf[diskP];

	if (info->array.level == 6) {
		int diskQ = geo_map(-2, rep->start_stripe,
				info->array.raid_disks,
				info->array.level, info->array.layout);
		rep->cinx2disk[rep->data_disks + 1] = diskQ;
		rep->buf_by_cinx[rep->data_disks + 1] = rep->buf[diskQ];
	}
}

static void raid56_list_comp_disks(struct mdinfo *info, struct mdrepair *rep)
{
	int i;

	printf("Component disks...\n");
	for (i = 0; i < info->array.raid_disks; i++) {
		const char *xb = "";
		if (i == rep->bad_cinx)
			xb = "*";

		if (i == rep->data_disks)
			printf("[P]");
		else if (i == rep->data_disks + 1)
			printf("[Q]");
		else
			printf("[%d%s]", i, xb);
		printf("(%s) ",
			(rep->fd[rep->cinx2disk[i]] >= 0) ? 
			strrchr(rep->disk_name[rep->cinx2disk[i]], '/') + 1
				: "F");
	}
	printf("\n");
}

static void list_cinx_except_bad_plus_pq(struct mdrepair *rep, int bad_cinx,
					enum PQX pqx)
{
	int i;

	for (i = 0; i < rep->data_disks; i++)
		if (i != bad_cinx)
			printf("[%d]", i);
	switch (pqx) {
	case P:
		printf(" [P]"); break;
	case Q:
		printf(" [Q]"); break;
	}
}

static void raid5_compute(struct mdinfo *info, struct mdrepair *rep,
			int bad_cinx)
{
	/* Use bad chunk buffer for XOR. */
	uint8_t *d = rep->buf_by_cinx[bad_cinx];
	rep->buf_by_cinx[bad_cinx] = (uint8_t *)zero;

	xor_blocks((char *)d, (char **)rep->buf_by_cinx,
		info->array.raid_disks - (info->array.level - 5), rep->length);

	rep->buf_by_cinx[bad_cinx] = d;
}

static int raid5_repair(struct mdinfo *info, struct mdrepair *rep)
{
	raid56_map_cinx(info, rep);
	if (debug) {
		raid56_list_comp_disks(info, rep);
		printf("%s BAD  @ %llu ",
			rep->disk_name[rep->bad_disk],
			rep->disk_start[rep->bad_disk]);
	}

	if (!CHKSUM_BAD_OK(rep, rep->bad_cinx)) {
		__warnx("%s @ %llu data read-out mismatches bad checksum.",
			rep->disk_name[rep->bad_disk],
			rep->disk_start[rep->bad_disk]);
		return 1;
	}

	raid5_compute(info, rep, rep->bad_cinx);

	if (debug) {
		list_cinx_except_bad_plus_pq(rep, rep->bad_cinx, P);
		printf("XORed GOOD @ %llu ", rep->disk_start[rep->bad_disk]);
	}
	if (!CHKSUM_GOOD_OK(rep, rep->bad_cinx)) {
		__warnx("reconstructed data mismatches good checksum %s @ %llu",
			rep->mddev, rep->mdstart);
		return 1;
	}

	verbose_printf("Fixing: XOR => %s [%d]\n",
				rep->disk_name[rep->bad_disk], rep->bad_cinx);
	return updateX(info, rep, rep->bad_cinx);
}

/*
 * RAID6 repair is the most complicated.
 * White paper at https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&ved=0CCQQFjAA&url=https%3A%2F%2Fwww.kernel.org%2Fpub%2Flinux%2Fkernel%2Fpeople%2Fhpa%2Fraid6.pdf&ei=yxn1UpOAKdCNrQHU7oGQBw&usg=AFQjCNEmQUesuWr5kxRdu9nURW0BwbSNdQ describes its mathematical nature and practical implemenation.
 * Here are some digests of the algorithm.
 * First, legend:
 *   We have N (D0, D1, ..., DN-1) data drives + P and Q parity drives on the
 *   stripe we are repairing. x is the suspicious driver letter (Dx).
 *   SUM[Dn] stands for XOR of all data drives with suspicious and failed
 *   (missing, fi any) drives' data all zeroed.
 *   Likewise, Pxy and Qxy are P-parity and Q-syndrome of all Dn's with
 *   suspicious and failed drives' data zeroed, respectively.
 *   g is a Galoi generator represented by a polinominal
 *   X7+X4+x3+x2+1.
 *   g(n) means aplalying this generator n times.
 *
 * There are 6 distinctive case:
 * CASE 1: All disks alive. Dx suspicious.
 *         Match comes from Dn (n!=x) and P
 *         DX=SUM[Dn]+P  ...(A)
 *         Dx and Q corrected.
 * CASE 2: All disks alive. Dx suspicious.
 *         Match comes from Dn (n!=x) and Q
 *         Dx=(SUM[g(n)*Dn]+Q)*g(-x)  ...(B)
 *         Dx and P corrected.
 * CASE 3: One data disk (Dy, not Dx) dead. Dx suspicious.
 *         Match comes from Dn (n!=x && n!=y), P and Q
 *         Dx=A*(P+Pxy)+B*(Q+Qxy)  ...(C)
 *         Dx corrected.
 * CASE 4: One data disk (Dx itself) dead. Dx suspicious.
 *         Match comes from Dn (n!=x) and Q
 *         Dx=(SUM[g(n)*Dn]+Q)*g(-x)  ...(B)
 *         P corrected.
 * CASE 5: Q disk dead. Dx suspicious.
 *         Match comes from Dn (n!=x) and P
 *         DX=SUM[Dn]+P  ...(A)
 *         Dx corrected.
 * CASE 6: P disk dead. Dx suspicious.
 *         Match comes from Dn (n!=x) and Q
 *         Dx=(SUM[g(n)*Dn]+Q)*g(-x)  ...(B)
 *         Dx corrected.
 */

static void raid6_comp_from_n_Q(struct mdinfo *info, struct mdrepair *rep,
				int bad_cinx)
{
	raid6_datap_recov(info->array.raid_disks, rep->length,
			bad_cinx, (uint8_t **)rep->buf_by_cinx);
}

static void raid6_comp_from_P_Q(struct mdinfo *info, struct mdrepair *rep,
				int bad_cinx, int failed_cinx)
{
	/* Failed disks must be in order. */
	int faila = min(bad_cinx, failed_cinx);
	int failb = max(bad_cinx, failed_cinx);
	raid6_2data_recov(info->array.raid_disks, rep->length,
			faila, failb, (uint8_t **)rep->buf_by_cinx);
}

static int raid6_repair(struct mdinfo *info, struct mdrepair *rep)
{
	int rv = 0;

	raid56_map_cinx(info, rep);
	if (debug) {
		raid56_list_comp_disks(info, rep);
		if (rep->fd[rep->bad_disk] >= 0) {
			printf("Designated data on a live disk.\n");
			printf("%s BAD  @ %llu ",
				rep->disk_name[rep->bad_disk],
				rep->disk_start[rep->bad_disk]);
		}
	}
	if (rep->fd[rep->bad_disk] >= 0 &&
		!CHKSUM_BAD_OK(rep, rep->bad_cinx) &&
		!debug)
		goto nobad;

	debug_printf("Failed disks: %d\n", info->array.failed_disks);
	switch (info->array.failed_disks) {
	case 0:
		raid5_compute(info, rep, rep->bad_cinx);
		if (debug) {
			printf(
			"Reconstruct from data disks except [%d] with P\n",
				rep->bad_cinx);
			list_cinx_except_bad_plus_pq(rep, rep->bad_cinx, P);
			printf(" %s GOOD @ %llu ",
				rep->disk_name[diskP(info, rep)],
				rep->disk_start[diskP(info, rep)]);
		}
		if (CHKSUM_GOOD_OK(rep, rep->bad_cinx) && !forceQ) {
			verbose_printf("Fixing: XOR => %s [%d]\n",
					rep->disk_name[rep->bad_disk],
					rep->bad_cinx);
			if ((rv = updateX(info, rep, rep->bad_cinx)))
				break;
			if (debug)
				list_cinx_except_bad_plus_pq(rep, -1, Q);
			uint8_t *q = xmalloc(rep->length);
			qsyndrome(rep->buf_by_cinx[cinxP(info)], q,
					rep->buf_by_cinx,
					rep->data_disks, rep->length);
			int qmismatch = memcmp(q,
					rep->buf_by_cinx[cinxQ(info)],
					rep->length);
			memcpy(rep->buf_by_cinx[cinxQ(info)], q, rep->length);
			free(q);
			debug_printf(" %s Q-syn @ %llu %smatch\n",
				rep->disk_name[diskQ(info, rep)],
				rep->disk_start[diskQ(info, rep)],
				qmismatch ? "mis" : "");
			if (!qmismatch)
				break;
			verbose_printf("Fixing: XOR^Q-syn => %s [Q]\n",
					rep->disk_name[diskQ(info, rep)]);
			if ((rv = updateQ(info, rep)))
				break;
		} else {/* P failed to restore. Try Q. */
			raid6_comp_from_n_Q(info, rep, rep->bad_cinx);
			if (debug) {
				printf(
			"Reconstruct from data disks except [%d] with Q\n",
				rep->bad_cinx);
				list_cinx_except_bad_plus_pq(rep,
							rep->bad_cinx, Q);
				printf(" %s GOOD @ %llu ",
					rep->disk_name[diskQ(info, rep)],
					rep->disk_start[diskQ(info, rep)]);
			}
			if (!CHKSUM_GOOD_OK(rep, rep->bad_cinx))
				goto nogood;
			verbose_printf("Fixing: Q-syn => %s [%d]\n",
					rep->disk_name[rep->bad_disk],
					rep->bad_cinx);
			if ((rv = updateX(info, rep, rep->bad_cinx)))
				break;
			verbose_printf("Fixing: Q-syn => %s [P]\n",
					rep->disk_name[diskP(info, rep)]);
			if ((rv = updateP(info, rep)))
				break;
		}
		break;
	case 1:
		if (rep->fd[diskQ(info, rep)] < 0) {
			raid5_compute(info, rep, rep->bad_cinx);
			if (debug) {
				list_cinx_except_bad_plus_pq(rep,
							rep->bad_cinx, P);
				printf(" Recon'ed from P-par GOOD ");
			}
			if (!CHKSUM_GOOD_OK(rep, rep->bad_cinx))
				goto nogood;
			verbose_printf("Fixing: XOR => %s [%d]\n",
					rep->disk_name[rep->bad_disk],
					rep->bad_cinx);
			rv = updateX(info, rep, rep->bad_cinx);
		} else if (rep->fd[diskP(info, rep)] < 0) {
			raid6_comp_from_n_Q(info, rep, rep->bad_cinx);
			if (debug) {
				list_cinx_except_bad_plus_pq(rep,
							rep->bad_cinx, P);
				printf(" Recon'ed from Q-syn GOOD ");
			}
			if (!CHKSUM_GOOD_OK(rep, rep->bad_cinx))
				goto nogood;
			verbose_printf("Fixing: Q-syn => %s [%d]\n",
					rep->disk_name[rep->bad_disk],
					rep->bad_cinx);
			rv = updateX(info, rep, rep->bad_cinx);
		} else if (rep->fd[rep->bad_disk] < 0) {
			/* Data is reconstructed with P.
			   First, reconstruct Dx from P. */
			raid5_compute(info, rep, rep->bad_cinx);
			if (debug) {
				list_cinx_except_bad_plus_pq(rep,
							rep->bad_cinx, Q);
				printf(" %s BAD  @ %llu ",
					rep->disk_name[diskP(info, rep)],
					rep->disk_start[diskP(info, rep)]);
			}
			if (!CHKSUM_BAD_OK(rep, rep->bad_cinx))
				goto nobad;
			/* Then reconstruct Dx from Q. */
			raid6_comp_from_n_Q(info, rep, rep->bad_cinx);
			if (debug) {
				list_cinx_except_bad_plus_pq(rep,
							rep->bad_cinx, Q);
				printf(" Recon'ed from Q-syn GOOD ");
			}
			if (!CHKSUM_GOOD_OK(rep, rep->bad_cinx))
				goto nogood;
			verbose_printf("Fixing: Q-syn => %s [P]\n",
					rep->disk_name[diskP(info, rep)]);
			updateP(info, rep);
		} else {/* Non-suspicious data disk is missing.
			   Good data to be rebuilt from remaining w/ P & Q. */
			int f_cinx;

			for (f_cinx = 0;
				f_cinx < rep->data_disks; f_cinx++)
				if (f_cinx != rep->bad_cinx &&
					rep->fd[rep->cinx2disk[f_cinx]] < 0)
					break;
			if (f_cinx >= rep->data_disks) {
				rv = 1;
				__warnx("cannot find failed drive %s @ %llu",
					rep->mddev, rep->mdstart);
				break;
			}
			raid6_comp_from_P_Q(info, rep, rep->bad_cinx, f_cinx);
			if (debug) {
				int i;

				for (i = 0; i < rep->data_disks; i++)
					if (i != rep->bad_cinx &&
						i != f_cinx)
						printf("[%d]", i);
				printf("[P][Q] Recon'ed from P&Q GOOD ");
			}
			if (!CHKSUM_GOOD_OK(rep, rep->bad_cinx))
				goto nogood;
			verbose_printf("Fixing: XOR & Q-syn => %s [%d]\n",
					rep->disk_name[rep->bad_disk],
					rep->bad_cinx);
			updateX(info, rep, rep->bad_cinx);
		}
		break;
	}

	return rv;

nobad:
	__warnx("component read-out mismatches bad checksum %s @ %llu",
		rep->mddev, rep->mdstart);
	return 1;

nogood:
	__warnx("reconstructed data mismatches good checksum %s @ %llu",
		rep->mddev, rep->mdstart);
	return 1;
}

static int raid1_repair(struct mdinfo *info, struct mdrepair *rep)
{
	int bad_disk = -1, good_disk = -1, i;

	if (debug) {
		for (i = 0; i < info->array.raid_disks; i++) {
			if (rep->disk_start[i] == ~0ULL)
				continue;
			printf("[%d](%s) ", i, 
			(rep->fd[i] >= 0) ?  strrchr(rep->disk_name[i], '/') + 1
					  : "F");
		}
		printf("\n");
	}

	for (i = 0; i < info->array.raid_disks; i++) {
		if (rep->fd[i] < 0)
			continue;
		debug_printf("%s BAD  @ %llu ",
				rep->disk_name[i], rep->disk_start[i]);
		if (!chksumcmp(&rep->chksum[BAD], rep->buf[i], rep->length))
			bad_disk = i;

		debug_printf("%s GOOD @ %llu ",
				rep->disk_name[i], rep->disk_start[i]);
		if (!chksumcmp(&rep->chksum[GOOD], rep->buf[i], rep->length))
			good_disk = i;
		else
			rep->buf_by_cinx[i] = (uint8_t *)1;
	}

	if (bad_disk < 0) {
		__warnx("cannot find component with bad checksum %s @ %llu",
			rep->mddev, rep->mdstart);
		return 1;
	}

	if (good_disk < 0) {
		__warnx("cannot find component with good checksum %s @ %llu",
			rep->mddev, rep->mdstart);
		return 1;
	}

	int rv = 0;
	for (i = 0; i < info->array.raid_disks; i++)
		if (rep->buf_by_cinx[i]) {
			verbose_printf("Fixing: %s => %s [%d]\n",
					rep->disk_name[good_disk],
					rep->disk_name[i], i);
			if (do_modify) {
				if (pwrite64(rep->fd[i],
					rep->buf[good_disk], rep->length,
					rep->disk_start[i]) != rep->length) {
					rv |= errno;
					__warn(
				"failed to write %ld bytes to %s @ %llu",
						rep->length, rep->disk_name[i],
						rep->disk_start[i]);
				} else {
					fsync(rep->fd[i]);
					flush_vm_cache();
				}
			} 
		}

	return rv;
}

static unsigned long long
raid10_disk_start(unsigned int disk, struct mdinfo *info, struct mdrepair *rep)
{
	unsigned long long chunk = ~0ULL;
	unsigned long long ts = rep->tot_stripes;
	unsigned long long ss = rep->start_stripe;
	int near = rep->near_copies;
	int far = rep->far_copies;
	int disks = info->array.raid_disks;

	if (far > 0) {/* NEAR (plus FAR) layout */
		/* n3f1 on 5 drives example */
		/* A0  A0  A0  A1  A1
		   A1  A2  A2  A2  A3
		   A3  A3  A4  A4  A4
		   A5  A5  A5  A6  A6
		   A6  A7  A7  A7  A8
		   ...  */
		/* n1f3 on 5 drives example (x: number of stripes) */
		/* A0  A1  A2  A3  A4
		   A5  A6  A7  A8  A9
		   ...             Ax-1
		   A4  A0  A1  A2  A3
		   A9  A5  A5  A7  A8
		   ...             Ax-2
		   A3  A4  A0  A1  A2
		   A8  A9  A5  A5  A7
		   ...             Ax-3 */
		/* n2f2 on 5 drives example */
		/* A0  A0  A1  A1  A2
		   A2  A3  A3  A4  A4
		   ...
		   A1  A2  A0  A0  A1
		   A4  A4  A2  A3  A3
		   ... */
		int f, n;
		for (f = 0; f < far; f++)
			for (n = 0; n < near; n++)
				if (((ss + ts*f) * near + near*f + n)%disks
					== disk) {
						chunk =
						((ss + ts*f) * near + n)/disks;
					goto done;
				}
	} else {/* OFFSET layout */
		/* o3 on 5 drives example */
		/* A0  A1  A2  A3  A4
		   A4  A0  A1  A2  A3
		   A3  A4  A0  A1  A2
		   A5  A6  A7  A8  A9
		   A9  A5  A6  A7  A8
		   A8  A9  A5  A6  A7
		   A10 A11 A12 A13 A14
		   A14 A10 A11 A12 A13
		   A13 A14 A10 A11 A12
		   ...  */
		int f;
		far = abs(far);
		for (f = 0; f < far; f++)
			if ((ss + f) % disks == disk) {
				chunk = (ss/disks) * far + f;
				goto done;
			}
	}
	return ~0ULL;

done:
	return chunk * info->array.chunk_size +
			(rep->mdstart % info->array.chunk_size);
}

static int raid10_repair(struct mdinfo *info, struct mdrepair *rep)
{
	return raid1_repair(info, rep);
}

static void usage(FILE *fp, const char *name, int ec)
{
	fprintf(fp,
"Usage: %s [options] /dev/mdX <start> <length> <badchksum> <goodchksum>\n"
"       <start> and <length> are in decimal. <chksums> are in hexadecimal\n"
"       -B|--invert-bad-chksum  Invert bad (computed) checksum bits\n"
"       -G|--invert-good-chksum  Invert good (expected) checksum bits\n"
"       -Q|--force-Q  Force to use Q when both P and Q are usable\n"
"       -X|--xdebug   Verify bad (computed) checksum and exit\n"
"       -d|--debug    Print out debug messages\n"
"       -b|--bad-chksum=<name>  Bad (computed) checksum algorithm\n"
"       -g|--good-chksum=<name> Good (expected) checksum algorithm\n"
"       -l|--log      Log to system log\n"
"       -n|--dry-run  Do not modify disk\n"
"       -v|--verbose\n",
			name);
	exit(ec);
}

void parsechksum(const char *str, uint8_t *crc, size_t len)
{
	errno = EINVAL;
	if (strlen(str) != len * 2)
		__err(1, "Illegal checksum length (%s)", str);
	errno = 0;
	const char *p = str;
	while (len--) {
		char xdigits[3];

		memcpy(xdigits, p, 2);
		xdigits[2] = '\0';
		p += 2;
		*crc++ = strtoul(xdigits, NULL, 16);
		if (errno)
			__err(1, "Illegal checksum string (%s)", str);
	}
}

static void exit_failed_disks(const char *mddev, int level, int avail, int all)
{
	__errx(3, "%s degraded - too few good drives for level %d (%d/%d)",
		mddev, level, avail, all);
}

int main(int argc, char **argv)
{
	int raid_disks = 0;
	int active_disks;
	int chunk_size = 0;
	unsigned long long start;
	size_t length;
	int i;
	struct mdinfo *info = NULL, *comp = NULL;
	char *prg = strrchr(argv[0], '/');
	uint8_t *buffer = NULL;
	uint8_t *lbuf = NULL;
	unsigned long long comp_sizeB;
	int xdebug = 0;
	int invert_bad_chksum = 0, invert_good_chksum = 0;

	struct mdrepair rep;

	memset(&rep, sizeof rep, 0);
	rep.chksum[BAD]  = chksummer[0];
	rep.chksum[GOOD] = chksummer[0];

	if (prg == NULL)
		prg = argv[0];
	else
		prg++;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"Q", 0, 0, 'Q'},
			{"invert-bad-chksum", 0, 0, 'B'},
			{"invert-good-chksum", 0, 0, 'G'},
			{"force-Q", 1, 0, 'Q'},
			{"xdebug", 0, 0, 'X'},
			{"bad-chksum", 1, 0, 'b'},
			{"good-chksum", 1, 0, 'g'},
			{"debug", 0, 0, 'd'},
			{"dry-run", 0, 0, 'n'},
			{"help", 0, 0, 'h'},
			{"log", 0, 0, 'l'},
			{"verbose", 0, 0, 'v'},
			{0, 0, 0, 0},
		};

		int c = getopt_long(argc, argv, "BGQbdghlnvX",
				long_options, &option_index);
		if (c < 0)
			break;
		switch (c) {
		case 'B':
			invert_bad_chksum = 1;
			break;
		case 'G':
			invert_good_chksum = 1;
			break;
		case 'Q':
			forceQ = 1;
			break;
		case 'b':
			set_chksummer(&rep.chksum[BAD], optarg);
			break;
		case 'g':
			set_chksummer(&rep.chksum[GOOD], optarg);
			break;
		case 'd':
			debug = 1;
			break;
		case 'l':
			lopt = 1;
			break;
		case 'n':
			do_modify = 0;
			break;
		case 'v':
			verbose++;
			break;
		case 'X':
			xdebug++;
			break;
		case '?':
		case 'h':
			usage(stdout, prg, 0);
			break;
		default:
			usage(stderr, prg, 1);
			break;
		}
	}

	argv += optind;
	argc -= optind;
	if (argc != 5 && argc != 6)
		usage(stderr, prg, argc);

	if (lopt)
		openlog(basename(prg), LOG_PID, LOG_USER);

	errno = 0;
	start = strtoull(argv[1], NULL, 10);
	if (errno)
		__err(1, "Bad start position (%s)", argv[2]);
	length = strtoull(argv[2], NULL, 10);
	if (errno)
		__err(1, "Bad length (%s)", argv[2]);
	rep.length = length;

	rep.chksum[BAD].value = malloc(rep.chksum[BAD].len);
	rep.chksum[GOOD].value = malloc(rep.chksum[GOOD].len);

	parsechksum(argv[3], rep.chksum[BAD].value, rep.chksum[BAD].len);
	parsechksum(argv[4], rep.chksum[GOOD].value, rep.chksum[GOOD].len);
	if (invert_bad_chksum)
		rep.chksum[BAD].flags |= CHKSUM_INVERT;
	if (invert_good_chksum)
		rep.chksum[GOOD].flags |= CHKSUM_INVERT;

	errno = EAGAIN;
	if (rep.chksum[BAD].len == rep.chksum[GOOD].len &&
		!memcmp(rep.chksum[BAD].value, rep.chksum[GOOD].value,
			rep.chksum[BAD].len))
		__errx(1, "Good and bad checksums are identical");

	rep.mddev = argv[0];
	rep.mdfd = open(rep.mddev, O_RDONLY);
	if (rep.mdfd < 0)
		__err(2, "cannot open %s", rep.mddev);
	lbuf = xmalloc(length);

	if (xdebug) {
		if (pread64(rep.mdfd, lbuf, length, start) != (ssize_t)length)
			__warn("cannot read %lu bytes from %s @ %llu",
				length, rep.mddev, start);
		else
			chksumcmp(&rep.chksum[BAD], lbuf, length);
		close(rep.mdfd);
		free(lbuf);
		return 0;
	}

	info = sysfs_read(rep.mdfd, NULL,
			  GET_CHUNK|
			  GET_LEVEL|
			  GET_LAYOUT|
			  GET_DISKS|
			  GET_DEGRADED |
			  GET_COMPONENT|
			  /*GET_CH*/
			  GET_DEVS|
			  GET_MISMATCH|
			  GET_OFFSET|
			  GET_SIZE);

	if (!info)
		__err(9, "Error reading sysfs information of %s", rep.mddev);

	raid_disks = info->array.raid_disks;
	chunk_size = info->array.chunk_size;
	if (!chunk_size)
		info->array.chunk_size = chunk_size = 4096;
	rep.repairer = raid6_repair;
	comp_sizeB = info->component_size * 512;
	rep.tot_stripes = comp_sizeB/chunk_size;

	switch (info->array.level) {
	case 1:
		rep.dslots_per_stripe = 1;
		rep.max_failed_disks = raid_disks - 2;
		rep.repairer = raid1_repair;
		break;
	case 5:
		rep.repairer = raid5_repair;
	case 6:
		rep.dslots_per_stripe = raid_disks - (info->array.level - 4);
		rep.max_failed_disks = info->array.level - 5;
		break;
	case 10:
		rep.near_copies = info->array.layout & 0xff;
		rep.far_copies = (info->array.layout >> 8) & 0xff;
		if (info->array.layout & 0x10000)
			rep.far_copies *= -1;
		rep.dslots_per_stripe = 1;
		rep.tot_stripes *= info->array.raid_disks;
		rep.tot_stripes /= rep.near_copies * abs(rep.far_copies);
		rep.repairer = raid10_repair;
		break;
	default:
		errno = EINVAL;
		err(3, "%s is not a suppoted RAID device (level=%d)",
			rep.mddev, info->array.level);
	}

	
	unsigned long long mdsize =
			chunk_size * rep.tot_stripes * rep.dslots_per_stripe;
	if (verbose) {
		printf("        device: %s\n", rep.mddev);
		printf("         level: %d\n", info->array.level);
		printf("        layout: %d", info->array.layout);
		if (info->array.level == 10) {
			int more = 0;
			printf(" (");
			if (rep.far_copies < 0)
				more += printf("%sOFFSETx%d",
						more ? " " : "",
						-rep.far_copies);
			if (rep.far_copies > 1)
				more += printf("%sFARx%d",
						more ? " " : "",
						rep.far_copies);
			if (rep.near_copies > 1)
				more += printf("%sNEARx%d",
						more ? " " : "",
						rep.near_copies);
			printf(")");
		}
		printf("\n");
		printf("         disks: %d\n", raid_disks);
		/* component_size: sctrs (1KiB blks in /sys) used for stripes */
		printf("component size: %llu (%lluMiB)\n",
			comp_sizeB, comp_sizeB/(1024 * 1024));
		printf(" total stripes: %llu\n", rep.tot_stripes);
		printf("    chunk size: %d (%uKiB)\n",
			chunk_size, chunk_size/1024);
		printf("    total size: %llu (%lluMiB)\n",
			mdsize, mdsize/(1024 * 1024));
		printf("mismatch count: %d\n", info->mismatch_cnt);
		printf("\n");
	}

	if (!debug && info->array.failed_disks > rep.max_failed_disks) {
		exit_failed_disks(rep.mddev, 
			info->array.level,
			raid_disks - info->array.failed_disks,
			raid_disks);
	}

	errno = EIO;
	if ((start & ~(chunk_size - 1)) !=
		((start + length - 1) & ~(chunk_size - 1)))
		__err(1, "data cannot cross chunk (%dB) boundary", chunk_size);
	if (start + length >= mdsize)
		__err(1, "start + length out of range");

	rep.mdstart = start;
	if (verbose) {
		for (i = 0, active_disks = 0, comp = info->devs;
			comp && i < raid_disks; i++, comp = comp->next) {
			printf(
		"disk: %d - offset: %llu - size: %llu - name: %s - slot: %d\n",
				i, comp->data_offset * 512,
				comp->component_size * 512,
				map_dev(comp->disk.major, comp->disk.minor, 0),
				comp->disk.raid_disk);
			if (comp->disk.raid_disk >= 0)
				active_disks++;
		}
		printf("\n");
	}

	rep.disk_name = xmalloc(raid_disks * sizeof(*rep.disk_name));
	rep.disk_start = xmalloc(raid_disks * sizeof(*rep.disk_start));
	rep.fd = xmalloc(raid_disks * sizeof(*rep.fd));
	buffer = xmalloc(raid_disks * length);
	rep.buf = xmalloc(raid_disks * sizeof *rep.buf);

	for (i = 0; i < raid_disks; i++) {
		rep.fd[i] = -1;
		rep.buf[i] = buffer + length * i;
	}

	size_t stripe_size = chunk_size * rep.dslots_per_stripe;
	rep.start_stripe = start/stripe_size;
	int failed_disks = 0;

	for (i = 0, active_disks = 0, comp = info->devs;
		comp && i < raid_disks; i++, comp = comp->next) {
		int disk = comp->disk.raid_disk;
		if (disk < 0)
			continue;

		rep.disk_name[disk] =
			map_dev(comp->disk.major, comp->disk.minor, 0);
		switch (info->array.level) {
		case 1:
		case 5:
		case 6:
			rep.disk_start[disk] =
					(start/stripe_size) * chunk_size +
					start % chunk_size;
			break;
		case 10:
			rep.disk_start[disk] =
					raid10_disk_start(disk, info, &rep);
			if (rep.disk_start[disk] == ~0ULL)
				continue;
			break;
		default:
			__errx(1, "Unsupported RAID level (%d)",
				info->array.level);
		}

		rep.disk_start[disk] += comp->data_offset * 512;
		rep.fd[disk] = open(rep.disk_name[disk], O_RDWR);
		if (rep.fd[disk] < 0) {
			__warn("cannot open %s", rep.disk_name[disk]);
			failed_disks++;
		}

		if (pread64(rep.fd[disk], rep.buf[disk], length,
				rep.disk_start[disk]) != (ssize_t)length) {
				__warn(
			"failed to read %lu bytes from %s @ %llu",
					length,
					rep.disk_name[disk],
					rep.disk_start[disk]);
			close(rep.fd[disk]);
			rep.fd[disk] = -1;
			/* Reading failed. */
			memset(rep.buf[disk], 0, length);
		} else {
			active_disks++;
			fsync(rep.fd[disk]);
		}
	}

	if (!debug && failed_disks > rep.max_failed_disks)
		exit_failed_disks(rep.mddev, 
			info->array.level,
			info->array.raid_disks - failed_disks,
			info->array.raid_disks);

	make_tables();
	ensure_zero_has_size(length);
	rep.buf_by_cinx = xcalloc(info->array.raid_disks *
					sizeof *rep.buf_by_cinx, 1);
	rep.cinx2disk = xmalloc(info->array.raid_disks *
					sizeof *rep.cinx2disk);

	sighandler_t sig[3];
	int rv = lock_stripe(info, &rep, start, sig);
	if (rv) {
		__warn("failed to lock stripe %s @ %llu", rep.mddev, start);
		goto unlockall;
	}

	flush_vm_cache();
	flush_stripe_cache(info, &rep);
	flush_vm_cache();

	if (pread64(rep.mdfd, lbuf, length, start) != (ssize_t)length) {
		__warn("cannot read %lu bytes from %s @ %llu.",
			length, rep.mddev, start);
		goto unlockall;
	}

	debug_printf("%s BAD  @ %llu ", rep.mddev, start);
	if (chksumcmp(&rep.chksum[BAD], lbuf, length)) {
		__warnx("designated data mismatches bad checksum %s @ %llu",
			rep.mddev, start);
		if (!debug)
			goto unlockall;
	}

	rv = rep.repairer(info, &rep);
	if (rv) {
		__warnx("repairing %s @ %llu [%s] failed.",
			rep.mddev, start, THISORNUL(argv[5]));
		send_notification(&rep, argv[5], 0);
	} else {
		flush_stripe_cache(info, &rep);
		__warnx("repairing %s @ %llu [%s] succeeded.\n",
			rep.mddev, start, THISORNUL(argv[5]));
		send_notification(&rep, argv[5], 1);
	}
unlockall:
	unlock_all_stripes(info, sig);

	for (i = 0; i < raid_disks; i++)
		if (rep.fd[i] >= 0)
			close(rep.fd[i]);

	free(lbuf);
	free(rep.cinx2disk);
	free(rep.buf_by_cinx);
	close(rep.mdfd);
	free(rep.buf);
	free(buffer);
	free(rep.fd);
	free(rep.disk_name);
	free(rep.disk_start);
	free(rep.chksum[GOOD].value);
	free(rep.chksum[BAD].value);

	sysfs_free(info);
	closelog();

	return rv;
}
