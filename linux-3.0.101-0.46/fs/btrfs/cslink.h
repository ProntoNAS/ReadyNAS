#ifndef __BTRFS_CSLINK__
#define __BTRFS_CSLINK__

#include <linux/ctype.h>

#define	CSL_INTRAFILEPOS	(1<<0)	/* Obsolete */

#if defined(CONFIG_BTRFS_MDCSREPAIR)
void mdcsrepair_netlink(const char *filesystem, const char *place,
		const u8 *expected, const u8 *computed,
		size_t cslen, const void *csfunc,
		u64 dpos, size_t len,
		dev_t dev, dev_t adev, u64 inum, u64 fpos, u64 flags);

/*
 * For debugging <file>:<func> is compared.
 * A nil string works as a wild card ao that ":" would match any.
 */
extern char *__btrfs_proc_mdcsrepair_debug;

static bool inline
btrfs_mdcsrepair_debug(const char *__file, const char *__func)
{
	const char *colon = __btrfs_proc_mdcsrepair_debug;

	if (!__btrfs_proc_mdcsrepair_debug || !*__btrfs_proc_mdcsrepair_debug)
		return false;
	if (*__btrfs_proc_mdcsrepair_debug != ':' &&
		(strncmp(__btrfs_proc_mdcsrepair_debug,
				__file, strlen(__file)) ||
			*(colon = __btrfs_proc_mdcsrepair_debug
						+ strlen(__file)) != ':'))
		return false;
	if (*++colon) {
		if (!isdigit(*colon) && strcmp(colon, __func))
			return false;
	}
	return true;
}

/* Definition in inode.c */
u64 btrfs_log2phys(struct inode *inode, u64 logical, u64 len, dev_t *dev);
#else
static
void inline mdcsrepair_netlink(const char *filesystem, const char *place,
		const u8 *expected, const u8 *computed,
		size_t cslen, const void *csfunc,
		u64 dpos, size_t len,
		dev_t dev, u64 inum, u64 fpos, u64 flags)
{
}

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
	{	\
		u8 *_gp = (u8 *)(_good), *_bp = (u8 *)(_bad); int _i;	\
		printk("%s:%d G:", __FILE__, __LINE__);	\
		for (_i = 0; _i < (_size); _i++)	\
			printk("%02x", _gp[_i]);	\
		printk(" B:");	\
		for (_i = 0; _i < (_size); _i++)	\
			printk("%02x", _bp[_i]);	\
		printk(" %lu @ %llu dev=%u:%u ino=%llu\n",	\
			(size_t)(_len), (_pos),		\
			(unsigned int)MAJOR(_dev),	\
			(unsigned int)MINOR(_dev),	\
			(u64)(_ino));	\
	}
#else
#define __MDCS(_good, _bad, _size, _len, _pos, _dev, _ino)	do{}while(0)
#endif

#if defined(DEBUG_CSLINK)
#define __btrfs_cslink(cond, ...)	\
	       mdcsrepair_netlink("btrfs", __func__, __VA_ARGS__)
#elif defined(CONFIG_BTRFS_MDCSREPAIR)
#define __btrfs_cslink(cond, expected, ...)       \
                do {	\
			if (((cond) && *((u32 *)(expected))) ||	\
				btrfs_mdcsrepair_debug(__FILE__, __func__)) \
				mdcsrepair_netlink("btrfs", __func__,	\
							expected, __VA_ARGS__);	\
		} while(0)
#else
#define __btrfs_cslink(cond, ...)       do {} while(0)
#endif

#endif	/* __BTRFS_CSLINK__ */
