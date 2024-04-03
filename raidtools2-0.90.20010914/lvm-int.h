#ifndef _LVM_P_H
#define _LVM_P_H

#define LVM_BLOCKSIZE 4096
#define LVM_BLOCKSIZE_WORDS (LVM_BLOCKSIZE/4)
#define PACKED __attribute__ ((packed))

/*
 * Identifies a block in physical space
 */
typedef struct phys_idx_s {
	md_u16 phys_nr;
	md_u32 phys_block;

} PACKED phys_idx_t;

/*
 * Identifies a block in logical space
 */
typedef struct log_idx_s {
	md_u16 log_id;
	md_u32 log_index;

} PACKED log_idx_t;

/*
 * Describes one PV
 */
#define LVM_PV_SB_MAGIC          0xf091ae9fU

#define LVM_PV_SB_GENERIC_WORDS 32
#define LVM_PV_SB_RESERVED_WORDS \
		(LVM_BLOCKSIZE_WORDS - LVM_PV_SB_GENERIC_WORDS)

/*
 * On-disk PV identification data, on block 0 in any PV.
 */
typedef struct pv_sb_s
{
	md_u32 pv_magic;		/*  0 		 			    */

	md_u32 pv_uuid0;		/*  1 					    */
	md_u32 pv_uuid1;		/*  2		 			    */
	md_u32 pv_uuid2;		/*  3 					    */
	md_u32 pv_uuid3;		/*  4 					    */

	md_u32 pv_major;		/*  5  					    */
	md_u32 pv_minor;		/*  6  					    */
	md_u32 pv_patch;		/*  7 					    */

	md_u32 pv_ctime;		/*  8 Creation time			    */

	md_u32 pv_total_size;	/*  9 size of this PV, in blocks	    */
	md_u32 pv_first_free;	/*  10 first free block			    */
	md_u32 pv_first_used;	/*  11 first used block			    */
	md_u32 pv_blocks_left;	/*  12 unallocated blocks		    */
	md_u32 pv_bg_size;	/*  13 size of a block group, in blocks	    */
	md_u32 pv_block_size;	/*  14 size of blocks, in bytes		    */
	md_u32 pv_pptr_size;	/*  15 size of block descriptor, in bytes   */
	md_u32 pv_block_groups;	/*  16 number of block groups		    */

	md_u32 __reserved1[LVM_PV_SB_GENERIC_WORDS - 17];

	/*
	 * Reserved
	 */
	md_u32 __reserved2[LVM_PV_SB_RESERVED_WORDS];

} PACKED pv_sb_t;

/*
 * this is pretty much arbitrary, but has to be less than ~64
 */
#define LVM_MAX_LVS_PER_VG 32

#define LVM_VG_SB_GENERIC_WORDS 32

#define LV_DESCRIPTOR_WORDS 8
#define LVM_VG_SB_RESERVED_WORDS (LVM_BLOCKSIZE_WORDS - \
	LV_DESCRIPTOR_WORDS*LVM_MAX_LVS_PER_VG - LVM_VG_SB_GENERIC_WORDS)

#if (LVM_PV_SB_RESERVED_WORDS < 0)
#error you messed this one up dude ...
#endif

typedef struct lv_descriptor_s
{
	md_u32 lv_id;		/*  0 					    */
	phys_idx_t lv_root_idx; /*  1					    */
	md_u16 __reserved;	/*  2 					    */
	md_u32 lv_max_indices;	/*  3 					    */
	md_u32 lv_free_indices;	/*  4 					    */
	md_u32 md_id;		/*  5 					    */

	md_u32 reserved[LV_DESCRIPTOR_WORDS - 6];

} PACKED lv_descriptor_t;

#define LVM_VG_SB_MAGIC          0x98320d7aU
/*
 * On-disk VG identification data, in block 1 on all PVs
 */
typedef struct vg_sb_s
{
	md_u32 vg_magic;		/*  0 		 			    */
	md_u32 nr_lvs;		/*  1					    */

	md_u32 __reserved1[LVM_VG_SB_GENERIC_WORDS - 2];

	lv_descriptor_t lv_array [LVM_MAX_LVS_PER_VG];
	/*
	 * Reserved
	 */
	md_u32 __reserved2[LVM_VG_SB_RESERVED_WORDS];

} PACKED vg_sb_t;

/*
 * Describes one LV
 */

#define LVM_LV_SB_MAGIC          0xe182bd8aU

/* FIXME: do we need lv_sb_t? */

typedef struct lv_sb_s
{
	/*
	 * On-disk LV identifier
	 */
	md_u32 lv_magic;		/*  0 LV identifier 			    */
	md_u32 lv_uuid0;		/*  1 					    */
	md_u32 lv_uuid1;		/*  2		 			    */
	md_u32 lv_uuid2;		/*  3 					    */
	md_u32 lv_uuid3;		/*  4 					    */

	md_u32 lv_major;		/*  5 PV identifier 			    */
	md_u32 lv_minor;		/*  6 PV identifier 			    */
	md_u32 lv_patch;		/*  7 PV identifier 			    */

	md_u32 ctime;		/*  8 Creation time			    */
	md_u32 size;		/*  9 size of this LV, in blocks	    */
	phys_idx_t start;	/*  10 position of root index block	    */
	log_idx_t first_free;	/*  11-12 first free index		    */

	/*
	 * Reserved
	 */
	md_u32 reserved[LVM_BLOCKSIZE_WORDS-13];

} PACKED lv_sb_t;

/*
 * Pointer pointing from the physical space, points to
 * the LV owning this block. It also contains various
 * statistics about the physical block.
 */
typedef struct pv_pptr_s
{
	union {
	/* case 1 */
		struct {
			log_idx_t owner;
			log_idx_t predicted;
			md_u32 last_referenced;
		} used;
	/* case 2 */
		struct {
			md_u16 log_id;
			md_u16 md_unused1;
			md_u32 next_free;
			md_u32 md_unused2;
			md_u32 md_unused3;
		} free;
	} u;
} PACKED pv_pptr_t;

static __inline__ int pv_pptr_free (const pv_pptr_t * pptr)
{
	return !pptr->u.free.log_id;
}


#define DATA_BLOCKS_PER_BG ((LVM_BLOCKSIZE*8)/(8*sizeof(pv_pptr_t)+1))

#define TOTAL_BLOCKS_PER_BG (DATA_BLOCKS_PER_BG+1)
/*
 * A table of pointers filling up a single block, managing
 * the next DATA_BLOCKS_PER_BG physical blocks. Such block
 * groups form the physical space of blocks.
 */
typedef struct pv_block_group_s
{
	md_u8 used_bitmap[(DATA_BLOCKS_PER_BG+7)/8];

	pv_pptr_t blocks[DATA_BLOCKS_PER_BG];

} PACKED pv_block_group_t;

/*
 * Pointer from the logical space, points to
 * the (PV,block) containing this logical block
 */
typedef struct lv_lptr_s
{
	phys_idx_t data;
	md_u16 __reserved;
	md_u32 cpu_addr;
	md_u32 __reserved2;

} PACKED lv_lptr_t;

static __inline__ int index_free (const lv_lptr_t * index)
{
	return !index->data.phys_block;
}

static __inline__ int index_present (const lv_lptr_t * index)
{
	return index->cpu_addr;
}


#define LVM_LPTRS_PER_BLOCK (LVM_BLOCKSIZE/sizeof(lv_lptr_t))
/*
 * A table of pointers filling up a single block, managing
 * LVM_LPTRS_PER_BLOCK logical blocks. Such block groups form
 * the logical space of blocks.
 */
typedef struct lv_index_block_s
{
	lv_lptr_t blocks[LVM_LPTRS_PER_BLOCK];

} PACKED lv_index_block_t;

#endif

