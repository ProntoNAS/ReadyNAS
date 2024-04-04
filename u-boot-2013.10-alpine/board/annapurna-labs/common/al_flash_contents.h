/*******************************************************************************
Copyright (C) 2014 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 or V3 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __FLASH_CONTENTS_H__
#define __FLASH_CONTENTS_H__

#include "al_hal_common.h"

/** Flash TOC magic number */
#define AL_FLASH_TOC_MAGIC_NUM			0x070c070c

/** Flash TOC minimal format revision ID supported */
#define AL_FLASH_TOC_FORMAT_REV_ID_MIN		0

/** Flash TOC maximal number of entries */
#define AL_FLASH_TOC_MAX_NUM_ENTRIES		64

/** Flash TOC entries max description length */
#define AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN	8

/** Flash object magic number */
#define AL_FLASH_OBJ_MAGIC_NUM			0x000b9ec7

/** Flash object minimal format revision ID supported */
#define AL_FLASH_OBJ_FORMAT_REV_ID_MIN		0

/** Flash object description length */
#define AL_FLASH_OBJ_DESC_LEN			16

/******************************************************************************
 * Flash objects IDs
 ******************************************************************************/
/** Macro for getting flash object ID of specific instance */
#define AL_FLASH_OBJ_ID(obj_id, instance_num)	\
	((obj_id) | ((instance_num) << AL_FLASH_OBJ_ID_INSTANCE_NUM_SHIFT))
/** Macro for getting flash object actual ID (without instance) */
#define AL_FLASH_OBJ_ID_ID(obj_id)	\
	AL_REG_FIELD_GET(obj_id, AL_FLASH_OBJ_ID_ID_MASK,\
		AL_FLASH_OBJ_ID_ID_SHIFT)
/** Macro for getting flash object instance number */
#define AL_FLASH_OBJ_ID_INSTANCE_NUM(obj_id)	\
	AL_REG_FIELD_GET(obj_id, AL_FLASH_OBJ_ID_INSTANCE_NUM_MASK,\
		AL_FLASH_OBJ_ID_INSTANCE_NUM_SHIFT)
#define AL_FLASH_OBJ_ID_ID_MASK			AL_FIELD_MASK(27, 0)
#define AL_FLASH_OBJ_ID_ID_SHIFT		0
#define AL_FLASH_OBJ_ID_INSTANCE_NUM_MASK	AL_FIELD_MASK(31, 28)
#define AL_FLASH_OBJ_ID_INSTANCE_NUM_SHIFT	28

#define AL_FLASH_OBJ_ID_BOOT_MODE		0x0
#define AL_FLASH_OBJ_ID_STG2			0x1
#define AL_FLASH_OBJ_ID_DT			0x2
#define AL_FLASH_OBJ_ID_STG2_5			0x3
#define AL_FLASH_OBJ_ID_STG3			0x4
#define AL_FLASH_OBJ_ID_UBOOT			0x5
#define AL_FLASH_OBJ_ID_UBOOT_SCRIPT		0x6
#define AL_FLASH_OBJ_ID_UBOOT_ENV		0x7
#define AL_FLASH_OBJ_ID_UBOOT_ENV_RED		0x8
#define AL_FLASH_OBJ_ID_KERNEL			0x9
#define AL_FLASH_OBJ_ID_ROOT_FS			0xa
#define AL_FLASH_OBJ_ID_2ND_TOC			0xb
#define AL_FLASH_OBJ_ID_APCEA			0x100
#define AL_FLASH_OBJ_ID_APCEA_CFG		0x101
#define AL_FLASH_OBJ_ID_IPXE			0x110
#define AL_FLASH_OBJ_ID_BOOT_APP		0x200
#define AL_FLASH_OBJ_ID_BOOT_APP_CFG		0x201
#define AL_FLASH_OBJ_ID_APP_0			0x210
#define AL_FLASH_OBJ_ID_APP_0_CFG		0x211
#define AL_FLASH_OBJ_ID_APP_1			0x220
#define AL_FLASH_OBJ_ID_APP_1_CFG		0x221
#define AL_FLASH_OBJ_ID_APP_2			0x230
#define AL_FLASH_OBJ_ID_APP_2_CFG		0x231
#define AL_FLASH_OBJ_ID_APP_3			0x240
#define AL_FLASH_OBJ_ID_APP_3_CFG		0x241

/******************************************************************************
 * Device IDs
 ******************************************************************************/
#define AL_FLASH_DEV_ID_CURRENT			0
#define AL_FLASH_DEV_ID_SPI			1
#define AL_FLASH_DEV_ID_NAND			2
#define AL_FLASH_DEV_ID_DRAM			3

/** Flash TOC entry */
struct al_flash_toc_entry {
	/** Object ID */
	uint32_t	obj_id;
	/** Object ID string */
	uint8_t		obj_id_str[AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN];
	/** ID of the device on which the object resides */
	uint32_t	dev_id;
	/** Object offset in the device */
	uint32_t	offset;
	/** Object maximal size [bytes] */
	uint32_t	max_size;
	/** Object flags */
	uint32_t	flags;
	/** Reserved */
	uint32_t	reserved;
};

/** Flash TOC header */
struct al_flash_toc_hdr {
	/** TOC magic number */
	uint32_t	magic_num;
	/** TOC format revision ID */
	uint32_t	format_rev_id;
	/** TOC num entries */
	uint32_t	num_entries;
	/** Reserved */
	uint32_t	reserved;
	/** TOC header 32 bit checksum */
	uint32_t	checksum;
};

/** Flash TOC footer */
struct al_flash_toc_footer {
	/** TOC entries 32 bit checksum */
	uint32_t	checksum;
};

/** Flash object header */
struct al_flash_obj_hdr {
	/** Object magic number */
	uint32_t	magic_num;
	/** Object format revision ID */
	uint32_t	format_rev_id;
	/** Object ID */
	uint32_t	id;
	/** Object major version */
	uint32_t	major_ver;
	/** Object minor version */
	uint32_t	minor_ver;
	/** Object fix version */
	uint32_t	fix_ver;
	/** Object description */
	uint8_t		desc[AL_FLASH_OBJ_DESC_LEN];
	/** Object size */
	uint32_t	size;
	/** Object loading address - high 32 bits */
	uint32_t	load_addr_hi;
	/** Object loading address - low 32 bits */
	uint32_t	load_addr_lo;
	/** Object execution address - high 32 bits */
	uint32_t	exec_addr_hi;
	/** Object execution address - low 32 bits */
	uint32_t	exec_addr_lo;
	/** Object flags */
	uint32_t	flags;
	/** Reserved */
	uint32_t	reserved;
	/** Object header 32 bit checksum */
	uint32_t	checksum;
};

/** Flash object footer */
struct al_flash_obj_footer {
	/** Object data 32 bit checksum */
	uint32_t	data_checksum;
};

/** Flash stage2 header */
struct al_flash_stage2_hdr {
	/** Reserved */
	uint8_t		reserved[12];
	/** Size */
	uint16_t	size;
	/** Header 16 bit checksum */
	uint16_t	checksum;
};

/**
 * Flash device read function pointer
 *
 * @param	offset inside the device
 * @param	address of memory buffer that will store read data
 * @param	size of read (in Bytes)
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
typedef int (*al_flash_dev_read)(unsigned int, void *, unsigned int);

/**
 * Flash object ID conversion to string
 *
 * @param	id
 *		Object ID
 *
 * @returns	Pointer to a const string representing the object ID
 */
const char *al_flash_obj_id_to_str(
	unsigned int id);

/**
 * Flash object ID string conversion to object ID enumeration
 *
 * @param	str
 *		Object ID string
 *
 * @returns	Object ID enumeration
 */
int al_flash_obj_id_from_str(
	const char *str);

/**
 * Flash TOC searching
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	first_offset_on_dev
 *		First offset on device to begin searching for TOC
 * @param	skip_size
 *		Searching skip size
 * @param	max_num_skips
 *		MAximal number of skips
 * @param	found_offset_on_dev
 *		Offset on device in which TOC has been found
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_search(
	al_flash_dev_read	dev_read_func,
	unsigned int		first_offset_on_dev,
	unsigned int		skip_size,
	unsigned int		max_num_skips,
	unsigned int		*found_offset_on_dev);

/**
 * Flash TOC validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	total_size
 *		TOC total size
 * @param	num_entries
 *		TOC number of entries
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*total_size,
	unsigned int		*num_entries);

/**
 * Flash TOC printout
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_print(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev);

/**
 * Flash object information printout
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	obj_id
 *		Object ID
 * @param	temp_buff
 *		Temporary buffer for object data processing
 * @param	temp_buff_size
 *		Temporary buffer size
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_obj_info_print(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		obj_id,
	void			*temp_buff,
	unsigned int		temp_buff_size);

/**
 * Flash TOC entry get
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	index
 *		TOC entry index
 * @param	found_entry
 *		Found entry
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_entry_get(
	al_flash_dev_read		dev_read_func,
	unsigned int			offset_on_dev,
	unsigned int			index,
	struct al_flash_toc_entry	*found_entry);

/**
 * Flash TOC object finding by ID
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	obj_id
 *		Object ID
 * @param	first_index
 *		First TOC entry to begin the search with
 * @param	found_index
 *		Index of the found entry
 * @param	found_entry
 *		Found entry
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_find_id(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	uint32_t		obj_id,
	unsigned int		first_index,
	int			*found_index,
	struct al_flash_toc_entry	*found_entry);

/**
 * Flash TOC active stage 2 finding
 *
 * @param	pbs_sram_base
 *		Pointer to the PBS SRAM
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	instance_num
 *		Stage 2 instance number
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_stage2_active_instance_get(
	void			*pbs_sram_base,
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*instance_num);

/**
 * Flash object header validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	hdr
 *		Flash object header
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_obj_header_read_and_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	struct al_flash_obj_hdr	*read_hdr);

/**
 * Flash object data loading
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	load_addr
 *		Address to load the data to
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_obj_data_load(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*load_addr);

/**
 * Flash object validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	temp_buff
 *		Temporary buffer for object data processing
 * @param	temp_buff_size
 *		Temporary buffer size
 * @param	read_hdr
 *		The object read header
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_obj_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr);

/**
 * Flash stage2 validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	temp_buff
 *		Temporary buffer for object data processing
 * @param	temp_buff_size
 *		Temporary buffer size
 * @param	read_hdr
 *		The object read header
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_stage2_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr);

#endif

