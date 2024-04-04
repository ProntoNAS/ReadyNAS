#include "al_hal_plat_types.h"
#include "al_hal_plat_services.h"
#include "al_hal_reg_utils.h"
#include "al_flash_contents.h"
#include "al_hal_iomap.h"

#define DEV_READ(buff, size)					\
	do {							\
		err = dev_read_func(offset_on_dev, buff, size);	\
		if (err)					\
			goto read_error;			\
		offset_on_dev += size;				\
	} while (0);

/*******************************************************************************
 ******************************************************************************/
static uint16_t chksum16(void *buff, int len)
{
	uint16_t val = 0;

	for (; len; len--, buff++)
		val += *(uint8_t *)buff;

	return val;
}

/*******************************************************************************
 ******************************************************************************/
static uint32_t chksum32(void *buff, int len)
{
	uint32_t val = 0;

	for (; len; len--, buff++)
		val += *(uint8_t *)buff;

	return val;
}

/*******************************************************************************
 ******************************************************************************/
const char *al_flash_obj_id_to_str(
	unsigned int id)
{
	switch (id) {
	case AL_FLASH_OBJ_ID_BOOT_MODE:		return "BOOT_MODE";
	case AL_FLASH_OBJ_ID_STG2:		return "STG2";
	case AL_FLASH_OBJ_ID_DT:		return "DT";
	case AL_FLASH_OBJ_ID_STG2_5:		return "STG2_5";
	case AL_FLASH_OBJ_ID_STG3:		return "STG3";
	case AL_FLASH_OBJ_ID_UBOOT:		return "UBOOT";
	case AL_FLASH_OBJ_ID_UBOOT_SCRIPT:	return "UBOOT_SCRIPT";
	case AL_FLASH_OBJ_ID_UBOOT_ENV:		return "UBOOT_ENV";
	case AL_FLASH_OBJ_ID_UBOOT_ENV_RED:	return "UBOOT_ENV_RED";
	case AL_FLASH_OBJ_ID_KERNEL:		return "KERNEL";
	case AL_FLASH_OBJ_ID_ROOT_FS:		return "ROOT_FS";
	case AL_FLASH_OBJ_ID_2ND_TOC:		return "2ND_TOC";
	case AL_FLASH_OBJ_ID_APCEA:		return "APCEA";
	case AL_FLASH_OBJ_ID_APCEA_CFG:		return "APCEA_CFG";
	case AL_FLASH_OBJ_ID_IPXE:		return "IPXE";
	case AL_FLASH_OBJ_ID_BOOT_APP:		return "BOOT_APP";
	case AL_FLASH_OBJ_ID_BOOT_APP_CFG:	return "BOOT_APP_CFG";
	case AL_FLASH_OBJ_ID_APP_0:		return "APP_0";
	case AL_FLASH_OBJ_ID_APP_0_CFG:		return "APP_0_CFG";
	case AL_FLASH_OBJ_ID_APP_1:		return "APP_1";
	case AL_FLASH_OBJ_ID_APP_1_CFG:		return "APP_1_CFG";
	case AL_FLASH_OBJ_ID_APP_2:		return "APP_2";
	case AL_FLASH_OBJ_ID_APP_2_CFG:		return "APP_2_CFG";
	case AL_FLASH_OBJ_ID_APP_3:		return "APP_3";
	case AL_FLASH_OBJ_ID_APP_3_CFG:		return "APP_3_CFG";
	}

	return "N/A";
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_id_from_str(
	const char *str)
{
	if (!al_strcmp(str, "BOOT_MODE"))
		return AL_FLASH_OBJ_ID_BOOT_MODE;
	else if (!al_strcmp(str, "STG2"))
		return AL_FLASH_OBJ_ID_STG2;
	else if (!al_strcmp(str, "DT"))
		return AL_FLASH_OBJ_ID_DT;
	else if (!al_strcmp(str, "STG2_5"))
		return AL_FLASH_OBJ_ID_STG2_5;
	else if (!al_strcmp(str, "STG3"))
		return AL_FLASH_OBJ_ID_STG3;
	else if (!al_strcmp(str, "UBOOT"))
		return AL_FLASH_OBJ_ID_UBOOT;
	else if (!al_strcmp(str, "UBOOT_SCRIPT"))
		return AL_FLASH_OBJ_ID_UBOOT_SCRIPT;
	else if (!al_strcmp(str, "UBOOT_ENV"))
		return AL_FLASH_OBJ_ID_UBOOT_ENV;
	else if (!al_strcmp(str, "UBOOT_ENV_RED"))
		return AL_FLASH_OBJ_ID_UBOOT_ENV_RED;
	else if (!al_strcmp(str, "KERNEL"))
		return AL_FLASH_OBJ_ID_KERNEL;
	else if (!al_strcmp(str, "ROOT_FS"))
		return AL_FLASH_OBJ_ID_ROOT_FS;
	else if (!al_strcmp(str, "2ND_TOC"))
		return AL_FLASH_OBJ_ID_2ND_TOC;
	else if (!al_strcmp(str, "APCEA"))
		return AL_FLASH_OBJ_ID_APCEA;
	else if (!al_strcmp(str, "APCEA_CFG"))
		return AL_FLASH_OBJ_ID_APCEA_CFG;
	else if (!al_strcmp(str, "IPXE"))
		return AL_FLASH_OBJ_ID_IPXE;
	else if (!al_strcmp(str, "BOOT_APP"))
		return AL_FLASH_OBJ_ID_BOOT_APP;
	else if (!al_strcmp(str, "BOOT_APP_CFG"))
		return AL_FLASH_OBJ_ID_BOOT_APP_CFG;
	else if (!al_strcmp(str, "APP_0"))
		return AL_FLASH_OBJ_ID_APP_0;
	else if (!al_strcmp(str, "APP_0_CFG"))
		return AL_FLASH_OBJ_ID_APP_0_CFG;
	else if (!al_strcmp(str, "APP_1"))
		return AL_FLASH_OBJ_ID_APP_1;
	else if (!al_strcmp(str, "APP_1_CFG"))
		return AL_FLASH_OBJ_ID_APP_1_CFG;
	else if (!al_strcmp(str, "APP_2"))
		return AL_FLASH_OBJ_ID_APP_2;
	else if (!al_strcmp(str, "APP_2_CFG"))
		return AL_FLASH_OBJ_ID_APP_2_CFG;
	else if (!al_strcmp(str, "APP_3"))
		return AL_FLASH_OBJ_ID_APP_3;
	else if (!al_strcmp(str, "APP_3_CFG"))
		return AL_FLASH_OBJ_ID_APP_3_CFG;

	return -EINVAL;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_search(
	al_flash_dev_read	dev_read_func,
	unsigned int	first_offset_on_dev,
	unsigned int	skip_size,
	unsigned int	max_num_skips,
	unsigned int	*found_offset_on_dev)
{
	unsigned int offset_on_dev;
	unsigned int i;

	for (offset_on_dev = first_offset_on_dev,
		i = 0; i < max_num_skips;
		offset_on_dev += skip_size, i++) {
		int err;
		unsigned int total_size;
		unsigned int num_entries;

		err = al_flash_toc_validate(dev_read_func, offset_on_dev,
				&total_size, &num_entries);
		if (!err)
			break;
	}

	if (i >= max_num_skips)
		return -EINVAL;

	*found_offset_on_dev = offset_on_dev;

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int	offset_on_dev,
	unsigned int	*total_size,
	unsigned int	*num_entries)
{
	uint32_t csum;
	unsigned int i;
	int err;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	uint32_t entries_csum_le;

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	if (chksum32(&hdr, sizeof(struct al_flash_toc_hdr) -
		sizeof(uint32_t)) != swap32_from_le(hdr.checksum)) {
		al_dbg("%s: flash toc header checksum validation failed!\n",
				__func__);
		return -EIO;
	}

	*num_entries = swap32_from_le(hdr.num_entries);
	*total_size = sizeof(struct al_flash_toc_hdr) + (*num_entries) *
		sizeof(struct al_flash_toc_entry) + sizeof(uint32_t);

	for (i = 0, csum = 0; i < (*num_entries); i++) {
		DEV_READ(&entry, sizeof(struct al_flash_toc_entry));
		csum += chksum32(&entry, sizeof(struct al_flash_toc_entry));
	}

	DEV_READ(&entries_csum_le, sizeof(uint32_t));

	if (csum != swap32_from_le(entries_csum_le)) {
		al_err("%s: flash toc entries checksum validation failed!\n", __func__);
		return -EIO;
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_print(
	al_flash_dev_read	dev_read_func,
	unsigned int	offset_on_dev)
{
	unsigned int i;
	int err;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	unsigned int num_entries;
	uint8_t	obj_id_str[AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN + 1];

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	num_entries = swap32_from_le(hdr.num_entries);

	al_print("#  | OBJ ID                    | Instance | Name     | Device  | Offset   | Max Size\n");
	al_print("------------------------------------------------------------------------------------\n");

	for (i = 0; i < num_entries; i++) {
		DEV_READ(&entry, sizeof(struct al_flash_toc_entry));

		al_memcpy(obj_id_str, entry.obj_id_str, AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN);
		obj_id_str[AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN] = 0;

		al_print("%2d | %06lx (%16s) | %8lu | %8s | %7s | %08x | %08x\n",
			i,
			AL_FLASH_OBJ_ID_ID(swap32_from_le(entry.obj_id)),
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID(swap32_from_le(entry.obj_id))),
			AL_FLASH_OBJ_ID_INSTANCE_NUM(swap32_from_le(entry.obj_id)),
			obj_id_str,
			(swap32_from_le(entry.dev_id) == AL_FLASH_DEV_ID_CURRENT) ? "Current" :
			(swap32_from_le(entry.dev_id) == AL_FLASH_DEV_ID_SPI) ? "SPI" :
			(swap32_from_le(entry.dev_id) == AL_FLASH_DEV_ID_NAND) ? "NAND" :
			(swap32_from_le(entry.dev_id) == AL_FLASH_DEV_ID_DRAM) ? "DRAM" :
			"N/A",
			swap32_from_le(entry.offset),
			swap32_from_le(entry.max_size));
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_info_print(
	al_flash_dev_read	dev_read_func,
	unsigned int	offset_on_dev,
	unsigned int	obj_id,
	void		*temp_buff,
	unsigned int	temp_buff_size)
{
	int err;
	struct al_flash_obj_hdr hdr;
	uint8_t	desc[AL_FLASH_OBJ_DESC_LEN + 1];

	if (obj_id == AL_FLASH_OBJ_ID_STG2)
		err = al_flash_stage2_validate(dev_read_func, offset_on_dev,
				temp_buff, temp_buff_size, &hdr);
	else
		err = al_flash_obj_validate(dev_read_func, offset_on_dev,
				temp_buff, temp_buff_size, &hdr);

	if (err) {
		al_print("ID = %06lx (%s) - Object failed validation!\n",
			AL_FLASH_OBJ_ID_ID(obj_id),
			al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID((obj_id))));
		return -EINVAL;
	}

	al_print("ID = %06lx (%s)\n",
		AL_FLASH_OBJ_ID_ID(hdr.id),
		al_flash_obj_id_to_str(AL_FLASH_OBJ_ID_ID((hdr.id))));

	al_memcpy(desc, hdr.desc, AL_FLASH_OBJ_DESC_LEN);
	desc[AL_FLASH_OBJ_DESC_LEN] = 0;
	al_print("Description = %s\n", desc);

	al_print("Version = %u.%u.%u\n", hdr.major_ver, hdr.minor_ver, hdr.fix_ver);

	al_print("Size = %08x\n", hdr.size);

	al_print("Load address = %08x%08x\n", hdr.load_addr_hi, hdr.load_addr_lo);

	al_print("Exec address = %08x%08x\n", hdr.exec_addr_hi, hdr.exec_addr_lo);

	al_print("Flags = %08x\n", hdr.flags);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_entry_get(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		index,
	struct al_flash_toc_entry	*found_entry)
{
	int err;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	unsigned int num_entries;

	al_dbg("%s(%X, %u)\n", __func__, offset_on_dev, index);

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	num_entries = swap32_from_le(hdr.num_entries);

	if (index >= num_entries) {
		al_err("%s: index exceeds the number of entries!\n", __func__);
		return -EINVAL;
	}

	offset_on_dev += index * sizeof(struct al_flash_toc_entry);

	DEV_READ(&entry, sizeof(struct al_flash_toc_entry));

	al_memcpy(found_entry, &entry, sizeof(struct al_flash_toc_entry));
	found_entry->obj_id = swap32_from_le(entry.obj_id);
	found_entry->dev_id = swap32_from_le(entry.dev_id);
	found_entry->offset = swap32_from_le(entry.offset);
	found_entry->max_size = swap32_from_le(entry.max_size);
	found_entry->flags = swap32_from_le(entry.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_find_id(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	uint32_t		obj_id,
	unsigned int		first_index,
	int			*found_index,
	struct al_flash_toc_entry	*found_entry)
{
	int err;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	unsigned int num_entries;

	al_dbg("%s(%X, %u, %u)\n", __func__, offset_on_dev, obj_id, first_index);

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	num_entries = swap32_from_le(hdr.num_entries);

	offset_on_dev += first_index * sizeof(struct al_flash_toc_entry);

	for (; first_index < num_entries; first_index++) {
		DEV_READ(&entry, sizeof(struct al_flash_toc_entry));
		al_dbg("%s: obj_id = %d\n", __func__, swap32_from_le(entry.obj_id));
		if (obj_id == swap32_from_le(entry.obj_id))
			break;
	}

	if (first_index < num_entries) {
		*found_index = first_index;
		al_memcpy(found_entry, &entry, sizeof(struct al_flash_toc_entry));
		found_entry->obj_id = swap32_from_le(entry.obj_id);
		found_entry->dev_id = swap32_from_le(entry.dev_id);
		found_entry->offset = swap32_from_le(entry.offset);
		found_entry->max_size = swap32_from_le(entry.max_size);
		found_entry->flags = swap32_from_le(entry.flags);
	} else {
		*found_index = -1;
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_toc_stage2_active_instance_get(
	void			*pbs_sram_base,
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*instance_num)
{
	int err;
	struct al_flash_toc_hdr hdr;
	struct al_flash_toc_entry entry;
	unsigned int num_entries;
	uint32_t stage2_actual_offset;
	unsigned int i;

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	stage2_actual_offset = al_reg_read32(
		pbs_sram_base + STAGE2_ACTUAL_OFFSET_ADDR_IN_PBS_SRAM);

	DEV_READ(&hdr, sizeof(struct al_flash_toc_hdr));

	num_entries = swap32_from_le(hdr.num_entries);

	for (i = 0; i < num_entries; i++) {
		DEV_READ(&entry, sizeof(struct al_flash_toc_entry));
		if ((AL_FLASH_OBJ_ID_ID(swap32_from_le(entry.obj_id)) == AL_FLASH_OBJ_ID_STG2) &&
			(swap32_from_le(entry.offset) == stage2_actual_offset)) {
			*instance_num = AL_FLASH_OBJ_ID_INSTANCE_NUM(swap32_from_le(entry.obj_id));
			break;
		}
	}

	if (i >= num_entries) {
		al_err("%s: unable to find stage 2 at offset %08x\n", __func__, stage2_actual_offset);
		return -EINVAL;
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_header_read_and_validate(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	struct al_flash_obj_hdr	*read_hdr)
{
	int err;
	struct al_flash_obj_hdr hdr;

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	DEV_READ(&hdr, sizeof(struct al_flash_obj_hdr));

	if (chksum32(&hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(hdr.checksum)) {
		al_err("%s: flash obj header checksum validation failed!\n", __func__);
		return -EIO;
	}

	al_memcpy(read_hdr, &hdr, sizeof(struct al_flash_obj_hdr));
	read_hdr->magic_num = swap32_from_le(hdr.magic_num);
	read_hdr->format_rev_id = swap32_from_le(hdr.format_rev_id);
	read_hdr->id = swap32_from_le(hdr.id);
	read_hdr->major_ver = swap32_from_le(hdr.major_ver);
	read_hdr->minor_ver = swap32_from_le(hdr.minor_ver);
	read_hdr->size = swap32_from_le(hdr.size);
	read_hdr->load_addr_hi = swap32_from_le(hdr.load_addr_hi);
	read_hdr->load_addr_lo = swap32_from_le(hdr.load_addr_lo);
	read_hdr->exec_addr_hi = swap32_from_le(hdr.exec_addr_hi);
	read_hdr->exec_addr_lo = swap32_from_le(hdr.exec_addr_lo);
	read_hdr->flags = swap32_from_le(hdr.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_data_load(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	void			*load_addr)
{
	uint32_t csum;
	int err;
	struct al_flash_obj_hdr hdr;
	unsigned int size;
	uint32_t data_csum_le;

	al_dbg("%s(%X, %p)\n", __func__, offset_on_dev, load_addr);

	DEV_READ(&hdr, sizeof(struct al_flash_obj_hdr));

	size = swap32_from_le(hdr.size);

	DEV_READ(load_addr, size);

	DEV_READ(&data_csum_le, sizeof(uint32_t));

	csum = chksum32(load_addr, size);
	if (csum != swap32_from_le(data_csum_le)) {
		al_err("%s: data checksum validation failed!\n", __func__);
		return -EIO;
	}

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_obj_validate(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	int err;
	struct al_flash_obj_hdr hdr;
	uint32_t csum = 0;
	unsigned int size;
	uint32_t data_csum_le;

	al_dbg("%s(%X)\n", __func__, offset_on_dev);

	DEV_READ(&hdr, sizeof(struct al_flash_obj_hdr));

	if (chksum32(&hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(hdr.checksum)) {
		al_err("%s: flash obj header checksum validation failed!\n", __func__);
		return -EIO;
	}

	size = swap32_from_le(hdr.size);

	while (size) {
		unsigned int size_current =
			(size < temp_buff_size) ? size : temp_buff_size;

		DEV_READ(temp_buff, size_current);
		csum += chksum32(temp_buff, size_current);
		size -= size_current;
	}

	DEV_READ(&data_csum_le, sizeof(uint32_t));

	if (csum != swap32_from_le(data_csum_le)) {
		al_err("%s: data checksum validation failed!\n", __func__);
		return -EIO;
	}

	al_memcpy(read_hdr, &hdr, sizeof(struct al_flash_obj_hdr));
	read_hdr->magic_num = swap32_from_le(hdr.magic_num);
	read_hdr->format_rev_id = swap32_from_le(hdr.format_rev_id);
	read_hdr->id = swap32_from_le(hdr.id);
	read_hdr->major_ver = swap32_from_le(hdr.major_ver);
	read_hdr->minor_ver = swap32_from_le(hdr.minor_ver);
	read_hdr->size = swap32_from_le(hdr.size);
	read_hdr->load_addr_hi = swap32_from_le(hdr.load_addr_hi);
	read_hdr->load_addr_lo = swap32_from_le(hdr.load_addr_lo);
	read_hdr->exec_addr_hi = swap32_from_le(hdr.exec_addr_hi);
	read_hdr->exec_addr_lo = swap32_from_le(hdr.exec_addr_lo);
	read_hdr->flags = swap32_from_le(hdr.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
int al_flash_stage2_validate(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr)
{
	int err;
	struct al_flash_stage2_hdr hdr;
	struct al_flash_obj_hdr new_hdr;
	unsigned int size;
	uint16_t csum = 0;
	uint16_t data_csum_le;

	al_dbg("%s(%X, %p, %u)\n", __func__, offset_on_dev, temp_buff, temp_buff_size);

	DEV_READ(&hdr, sizeof(struct al_flash_stage2_hdr));

	if (chksum16(&hdr, sizeof(struct al_flash_stage2_hdr) - sizeof(uint16_t)) !=
		swap16_from_le(hdr.checksum)) {
		al_err("%s: flash stage2 header checksum validation failed!\n", __func__);
		return -EIO;
	}

	size = swap16_from_le(hdr.size);

	while (size) {
		unsigned int size_current =
			(size < temp_buff_size) ? size : temp_buff_size;

		DEV_READ(temp_buff, size_current);
		csum += chksum16(temp_buff, size_current);
		size -= size_current;
	}

	DEV_READ(&data_csum_le, sizeof(uint16_t));

	if (csum != swap16_from_le(data_csum_le)) {
		al_err("%s: data checksum validation failed!\n", __func__);
		return -EIO;
	}

	DEV_READ(&new_hdr, sizeof(struct al_flash_obj_hdr));

	if (chksum32(&new_hdr, sizeof(struct al_flash_obj_hdr) - sizeof(uint32_t)) !=
		swap32_from_le(new_hdr.checksum)) {
		al_err("%s: flash obj header checksum validation failed!\n", __func__);
		return -EIO;
	}

	al_memcpy(read_hdr, &new_hdr, sizeof(struct al_flash_obj_hdr));
	read_hdr->magic_num = swap32_from_le(new_hdr.magic_num);
	read_hdr->format_rev_id = swap32_from_le(new_hdr.format_rev_id);
	read_hdr->id = swap32_from_le(new_hdr.id);
	read_hdr->major_ver = swap32_from_le(new_hdr.major_ver);
	read_hdr->minor_ver = swap32_from_le(new_hdr.minor_ver);
	read_hdr->size = swap32_from_le(new_hdr.size);
	read_hdr->load_addr_hi = swap32_from_le(new_hdr.load_addr_hi);
	read_hdr->load_addr_lo = swap32_from_le(new_hdr.load_addr_lo);
	read_hdr->exec_addr_hi = swap32_from_le(new_hdr.exec_addr_hi);
	read_hdr->exec_addr_lo = swap32_from_le(new_hdr.exec_addr_lo);
	read_hdr->flags = swap32_from_le(new_hdr.flags);

	return 0;

read_error:
	al_err("%s: device read failed!\n", __func__);

	return err;
}

