#ifndef _COMPAT_H_
#define _COMPAT_H_

#define btrfs_drop_nlink(inode) drop_nlink(inode)
#define btrfs_inc_nlink(inode)	inc_nlink(inode)

static inline int printk_get_level(const char *buffer)
{
	if (buffer[0] == '<' && buffer[2] == '>') {
		switch (buffer[1]) {
		case '0' ... '7':
		case 'd':
			return buffer[1];
		}
	}
	return 0;
}

static inline const char *printk_skip_level(const char *buffer)
{
	if (printk_get_level(buffer)) {
		switch (buffer[1]) {
		case '0' ... '7':
		case 'd':       /* KERN_DEFAULT */
			return buffer + 3;
		}
	}
	return buffer;
}

/* stubs, not used */
static inline void sb_start_intwrite(struct super_block *sb) { }
static inline void sb_end_intwrite(struct super_block *sb) { }

#endif /* _COMPAT_H_ */
