#ifndef __BTRFS_CSLINK__
#define __BTRFS_CSLINK__

#include <linux/ctype.h>

#define	CSL_INTRAFILEPOS	(1<<0)	/* Obsolete */

#if defined(CONFIG_BTRFS_MDCSREPAIR)
void mdcsrepair_procfs(const char *filesystem, const char *place,
		u64 inum,
		const u8 *expected, const u8 *computed,
		size_t cslen, const void *csfunc,
		u64 dpos, size_t len,
		dev_t dev, dev_t adev, u64 fpos, u64 flags);

/*
 * For debugging, <file>:<func> or :<inum> is compared to simulate
 * a CRC error for each code execution.
 * A nil string before and after ":" works as a wild card,
 * so that ":" alone matches any (i.e. will always generate fake error).
 */
extern char *__btrfs_proc_mdcsrepair_debug;

#if defined(DEBUG_CSLINK)
#define btrfs_mdcsrepair_hit(...)	true
#else
static bool inline
btrfs_mdcsrepair_hit(bool cond, const void *expected,
			const char *__file, const char *__func, u64 inum)
{
	const char *colon = __btrfs_proc_mdcsrepair_debug;

	if (cond && *((u32 *)expected))
		return true;

	if (!colon || !*colon)
		return false;

	/* Check for <file> field before ":" */
	if (*colon != ':' &&
		(strncmp(colon, __file, strlen(__file)) ||
			*(colon += strlen(__file)) != ':'))
		return false;

	/* Now colon points to ':'. */
	colon++;
	if (isdigit(*colon)) {
		char *end;
		u64 restrict_inum = simple_strtoull(colon, &end, 10);

		return !*end && inum == restrict_inum;
	}
	return !*colon || !strcmp(colon, __func);
}
#endif
/* Definition in inode.c */
u64 btrfs_log2phys(struct inode *inode, u64 logical, u64 len, dev_t *dev);
#else
static
void inline mdcsrepair_procfs(const char *filesystem, const char *place,
		const u8 *expected, const u8 *computed,
		size_t cslen, const void *csfunc,
		u64 dpos, size_t len,
		dev_t dev, u64 inum, u64 fpos, u64 flags) {}

static u64
inline btrfs_log2phys(struct inode *inode, u64 logical, u64 len, dev_t *dev)
{
	*dev = 0;
	return (u64)~0;
}
#endif

/* Debug macros */
#if defined(DEBUG_MDCS)
#define __MDCS(_good, _bad, _size, _len, _pos, _dev, _ino)	\
	do {	\
		u8 *_gp = (u8 *)(_good), *_bp = (u8 *)(_bad); int _i;	\
		pr_emerg("%s:%d G:", __FILE__, __LINE__);	\
		for (_i = 0; _i < (_size); _i++)	\
			pr_emerg("%02x", _gp[_i]);	\
		pr_emerg(" B:");	\
		for (_i = 0; _i < (_size); _i++)	\
			pr_emerg("%02x", _bp[_i]);	\
		pr_emerg(" %lu @ %llu dev=%u:%u ino=%llu\n",	\
			(size_t)(_len), (_pos),		\
			(unsigned int)MAJOR(_dev),	\
			(unsigned int)MINOR(_dev),	\
			(u64)(_ino));	\
	} while (0)
#else
#define __MDCS(...)	do {} while(0)
#endif

#if defined(CONFIG_BTRFS_MDCSREPAIR)
#define __btrfs_cslink(cond, inum, expected, ...)       \
	do {	\
		if (btrfs_mdcsrepair_hit(cond, expected,	\
			__FILE__, __func__, (inum)))		\
			mdcsrepair_procfs("btrfs", __func__,	\
					inum, expected, __VA_ARGS__);	\
	} while (0)
#else
#define __btrfs_cslink(...)	do {} while(0)
#endif

#endif	/* __BTRFS_CSLINK__ */
