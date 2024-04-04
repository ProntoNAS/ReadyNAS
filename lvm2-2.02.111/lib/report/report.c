/*
 * Copyright (C) 2002-2004 Sistina Software, Inc. All rights reserved.
 * Copyright (C) 2004-2013 Red Hat, Inc. All rights reserved.
 *
 * This file is part of LVM2.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU Lesser General Public License v.2.1.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "lib.h"
#include "metadata.h"
#include "report.h"
#include "toolcontext.h"
#include "lvm-string.h"
#include "display.h"
#include "activate.h"
#include "segtype.h"
#include "lvmcache.h"
#include "device-types.h"
#include "str_list.h"

#include <stddef.h> /* offsetof() */

struct lv_with_info {
	struct logical_volume *lv;
	struct lvinfo *info;
};

struct lvm_report_object {
	struct volume_group *vg;
	struct lv_with_info *lvi;
	struct physical_volume *pv;
	struct lv_segment *seg;
	struct pv_segment *pvseg;
	struct label *label;
};

/*
 *  Enum for field_num index to use in per-field reserved value definition.
 *  Each field is represented by enum value with name "field_<id>" where <id>
 *  is the field_id of the field as registered in columns.h.
 */
#define FIELD(type, strct, sorttype, head, field_name, width, func, id, desc, writeable) field_ ## id,
enum {
#include "columns.h"
};
#undef FIELD

static const uint64_t _zero64 = UINT64_C(0);
static const uint64_t _one64 = UINT64_C(1);
static const char _str_zero[] = "0";
static const char _str_one[] = "1";
static const char _str_no[] = "no";
static const char _str_yes[] = "yes";
static const char _str_unknown[] = "unknown";

/*
 * 32 bit signed is casted to 64 bit unsigned in dm_report_field internally!
 * So when stored in the struct, the _reserved_number_undef_32 is actually
 * equal to _reserved_number_undef_64.
 */
static const int32_t _reserved_number_undef_32 = INT32_C(-1);

/*
 * Reserved values and their assigned names.
 * The first name is the one that is also used for reporting.
 * All names listed are synonyms recognized in selection criteria.
 * For binary-based values we map all reserved names listed onto value 1, blank onto value 0.
 *
 * TYPE_RESERVED_VALUE(type, reserved_value_id, description, value, reserved name, ...)
 * FIELD_RESERVED_VALUE(field_id, reserved_value_id, description, value, reserved name, ...)
 * FIELD_RESERVED_BINARY_VALUE(field_id, reserved_value_id, description, reserved name for 1, ...)
 *
 * Note: FIELD_RESERVED_BINARY_VALUE creates:
 * 		- 'reserved_value_id_y' (for 1)
 * 		- 'reserved_value_id_n' (for 0)
 */
#define RESERVED(id) _reserved_ ## id
#define FIRST_NAME(id) _reserved_ ## id ## _names[0]

#define NUM uint64_t

#define TYPE_RESERVED_VALUE(type, id, desc, value, ...) \
	static const char *_reserved_ ## id ## _names[] = { __VA_ARGS__, NULL}; \
	static const type _reserved_ ## id = value;

#define FIELD_RESERVED_VALUE(field_id, id, desc, value, ...) \
	static const char *_reserved_ ## id ## _names[] = { __VA_ARGS__ , NULL}; \
	static const struct dm_report_field_reserved_value _reserved_ ## id = {field_ ## field_id, &value};

#define FIELD_RESERVED_BINARY_VALUE(field_id, id, desc, ...) \
	FIELD_RESERVED_VALUE(field_id, id ## _y, desc, _one64, __VA_ARGS__, _str_yes) \
	FIELD_RESERVED_VALUE(field_id, id ## _n, desc, _zero64, __VA_ARGS__, _str_no)

#include "values.h"

#undef NUM
#undef TYPE_RESERVED_VALUE
#undef FIELD_RESERVED_VALUE
#undef FIELD_RESERVED_BINARY_VALUE

/*
 * Create array of reserved values to be registered with reporting code via
 * dm_report_init_with_selection function that initializes report with
 * selection criteria. Selection code then recognizes these reserved values
 * when parsing selection criteria.
*/

#define NUM DM_REPORT_FIELD_TYPE_NUMBER

#define TYPE_RESERVED_VALUE(type, id, desc, value, ...) {type, &_reserved_ ## id, _reserved_ ## id ## _names, desc},

#define FIELD_RESERVED_VALUE(field_id, id, desc, value, ...) {DM_REPORT_FIELD_TYPE_NONE, &_reserved_ ## id, _reserved_ ## id ## _names, desc},

#define FIELD_RESERVED_BINARY_VALUE(field_id, id, desc, ...) \
	FIELD_RESERVED_VALUE(field_id, id ## _y, desc, _one64, __VA_ARGS__) \
	FIELD_RESERVED_VALUE(field_id, id ## _n, desc, _zero64, __VA_ARGS__)

static const struct dm_report_reserved_value _report_reserved_values[] = {
	#include "values.h"
	{0, NULL, NULL}
};

#undef NUM
#undef TYPE_RESERVED_VALUE_REG
#undef FIELD_RESERVED_VALUE_REG
#undef FIELD_RESERVED_BINARY_VALUE_REG

static int _field_set_value(struct dm_report_field *field, const void *data, const void *sort)
{
	dm_report_field_set_value(field, data, sort);

	return 1;
}

static int _field_set_string_list(struct dm_report *rh, struct dm_report_field *field,
				  const struct dm_list *list, void *private, int sorted)
{
	struct cmd_context *cmd = (struct cmd_context *) private;
	return sorted ? dm_report_field_string_list(rh, field, list, cmd->report_list_item_separator)
		      : dm_report_field_string_list_unsorted(rh, field, list, cmd->report_list_item_separator);
}

/*
 * Data-munging functions to prepare each data type for display and sorting
 */

/*
 * Display either "0"/"1" or ""/"word" based on bin_value,
 * cmd->report_binary_values_as_numeric selects the mode to use.
*/
static int _binary_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			struct dm_report_field *field, int bin_value, const char *word,
			void *private)
{
	const struct cmd_context *cmd = (const struct cmd_context *) private;

	if (cmd->report_binary_values_as_numeric)
		/* "0"/"1" */
		return _field_set_value(field, bin_value ? _str_one : _str_zero, bin_value ? &_one64 : &_zero64);
	else
		/* blank/"word" */
		return _field_set_value(field, bin_value ? word : "", bin_value ? &_one64 : &_zero64);
}

static int _binary_undef_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			      struct dm_report_field *field, void *private)
{
	const struct cmd_context *cmd = (const struct cmd_context *) private;

	if (cmd->report_binary_values_as_numeric)
		return _field_set_value(field, FIRST_NAME(number_undef_64), &RESERVED(number_undef_64));
	else
		return _field_set_value(field, _str_unknown, &RESERVED(number_undef_64));
}

static int _string_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			struct dm_report_field *field,
			const void *data, void *private __attribute__((unused)))
{
	return dm_report_field_string(rh, field, (const char * const *) data);
}

static int _chars_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
		       struct dm_report_field *field,
		       const void *data, void *private __attribute__((unused)))
{
	return dm_report_field_string(rh, field, (const char * const *) &data);
}

static int _dev_name_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			  struct dm_report_field *field,
			  const void *data, void *private __attribute__((unused)))
{
	const char *name = dev_name(*(const struct device * const *) data);

	return dm_report_field_string(rh, field, &name);
}

static int _devices_disp(struct dm_report *rh __attribute__((unused)), struct dm_pool *mem,
			 struct dm_report_field *field,
			 const void *data, void *private __attribute__((unused)))
{
	char *str;

	if (!(str = lvseg_devices(mem, (const struct lv_segment *) data)))
		return_0;

	return _field_set_value(field, str, NULL);
}

static int _peranges_disp(struct dm_report *rh __attribute__((unused)), struct dm_pool *mem,
			  struct dm_report_field *field,
			  const void *data, void *private __attribute__((unused)))
{
	char *str;

	if (!(str = lvseg_seg_pe_ranges(mem, (const struct lv_segment *) data)))
		return_0;

	return _field_set_value(field, str, NULL);
}

static int _tags_disp(struct dm_report *rh, struct dm_pool *mem,
		      struct dm_report_field *field,
		      const void *data, void *private)
{
	const struct dm_list *tagsl = (const struct dm_list *) data;

	return _field_set_string_list(rh, field, tagsl, private, 1);
}

static int _modules_disp(struct dm_report *rh, struct dm_pool *mem,
			 struct dm_report_field *field,
			 const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	struct dm_list *modules;

	if (!(modules = str_list_create(mem))) {
		log_error("modules str_list allocation failed");
		return 0;
	}

	if (!(list_lv_modules(mem, lv, modules)))
		return_0;

	return _field_set_string_list(rh, field, modules, private, 1);
}

static int _lvprofile_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;

	if (lv->profile)
		return dm_report_field_string(rh, field, &lv->profile->name);

	return _field_set_value(field, "", NULL);
}

static int _vgfmt_disp(struct dm_report *rh, struct dm_pool *mem,
		       struct dm_report_field *field,
		       const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;

	if (vg->fid)
		return _string_disp(rh, mem, field, &vg->fid->fmt->name, private);

	return _field_set_value(field, "", NULL);
}

static int _pvfmt_disp(struct dm_report *rh, struct dm_pool *mem,
		       struct dm_report_field *field,
		       const void *data, void *private)
{
	const struct label *l =
	    (const struct label *) data;

	if (!l->labeller || !l->labeller->fmt) {
		dm_report_field_set_value(field, "", NULL);
		return 1;
	}

	return _string_disp(rh, mem, field, &l->labeller->fmt->name, private);
}

static int _lvkmaj_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			struct dm_report_field *field,
			const void *data, void *private __attribute__((unused)))
{
	const struct lv_with_info *lvi = (const struct lv_with_info *) data;

	if (lvi->info && lvi->info->exists && lvi->info->major >= 0)
		return dm_report_field_int(rh, field, &lvi->info->major);

	return dm_report_field_int32(rh, field, &RESERVED(number_undef_32));
}

static int _lvkmin_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			struct dm_report_field *field,
			const void *data, void *private __attribute__((unused)))
{
	const struct lv_with_info *lvi = (const struct lv_with_info *) data;

	if (lvi->info && lvi->info->exists && lvi->info->minor >= 0)
		return dm_report_field_int(rh, field, &lvi->info->minor);

	return dm_report_field_int32(rh, field, &RESERVED(number_undef_32));
}

static int _lvstatus_disp(struct dm_report *rh __attribute__((unused)), struct dm_pool *mem,
			  struct dm_report_field *field,
			  const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	char *repstr;

	if (!(repstr = lv_attr_dup(mem, lv)))
		return_0;

	return _field_set_value(field, repstr, NULL);
}

static int _pvstatus_disp(struct dm_report *rh __attribute__((unused)), struct dm_pool *mem,
			  struct dm_report_field *field,
			  const void *data, void *private __attribute__((unused)))
{
	const struct physical_volume *pv =
	    (const struct physical_volume *) data;
	char *repstr;

	if (!(repstr = pv_attr_dup(mem, pv)))
		return_0;

	return _field_set_value(field, repstr, NULL);
}

static int _vgstatus_disp(struct dm_report *rh __attribute__((unused)), struct dm_pool *mem,
			  struct dm_report_field *field,
			  const void *data, void *private __attribute__((unused)))
{
	const struct volume_group *vg = (const struct volume_group *) data;
	char *repstr;

	if (!(repstr = vg_attr_dup(mem, vg)))
		return_0;

	return _field_set_value(field, repstr, NULL);
}

static int _segtype_disp(struct dm_report *rh __attribute__((unused)),
			 struct dm_pool *mem __attribute__((unused)),
			 struct dm_report_field *field,
			 const void *data, void *private __attribute__((unused)))
{
	const struct lv_segment *seg = (const struct lv_segment *) data;
	char *name;

	if (!(name = lvseg_segtype_dup(mem, seg))) {
		log_error("Failed to get segtype.");
		return 0;
	}

	return _field_set_value(field, name, NULL);
}

static int _loglv_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
		       struct dm_report_field *field,
		       const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	const char *name;

	if ((name = lv_mirror_log_dup(mem, lv)))
		return dm_report_field_string(rh, field, &name);

	return _field_set_value(field, "", NULL);
}

static int _lvname_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	char *repstr, *lvname;
	size_t len;

	if (lv_is_visible(lv))
		return dm_report_field_string(rh, field, &lv->name);

	len = strlen(lv->name) + 3;
	if (!(repstr = dm_pool_zalloc(mem, len))) {
		log_error("dm_pool_alloc failed");
		return 0;
	}

	if (dm_snprintf(repstr, len, "[%s]", lv->name) < 0) {
		log_error("lvname snprintf failed");
		return 0;
	}

	if (!(lvname = dm_pool_strdup(mem, lv->name))) {
		log_error("dm_pool_strdup failed");
		return 0;
	}

	return _field_set_value(field, repstr, lvname);
}

static int _lvfullname_disp(struct dm_report *rh, struct dm_pool *mem,
			    struct dm_report_field *field,
			    const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	char *repstr;

	if (!(repstr = lv_fullname_dup(mem, lv)))
		return_0;

	return _field_set_value(field, repstr, NULL);
}

static int _lvparent_disp(struct dm_report *rh, struct dm_pool *mem,
			  struct dm_report_field *field,
			  const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	char *repstr;

	if (!(repstr = lv_parent_dup(mem, lv)))
		return_0;

	return _field_set_value(field, repstr, NULL);
}

static int _datalv_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			struct dm_report_field *field,
			const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	const struct lv_segment *seg = (lv_is_thin_pool(lv) || lv_is_cache_pool(lv)) ? first_seg(lv) : NULL;

	if (seg)
		return _lvname_disp(rh, mem, field, seg_lv(seg, 0), private);

	return _field_set_value(field, "", NULL);
}

static int _metadatalv_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			    struct dm_report_field *field,
			    const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	const struct lv_segment *seg = (lv_is_thin_pool(lv) || lv_is_cache_pool(lv)) ? first_seg(lv) : NULL;

	if (seg)
		return _lvname_disp(rh, mem, field, seg->metadata_lv, private);

	return _field_set_value(field, "", NULL);
}

static int _poollv_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			struct dm_report_field *field,
			const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	struct lv_segment *seg = (lv_is_thin_volume(lv) || lv_is_cache(lv)) ?
				  first_seg(lv) : NULL;

	if (seg)
		return _lvname_disp(rh, mem, field, seg->pool_lv, private);

	return _field_set_value(field, "", NULL);
}

static int _lvpath_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	char *repstr;

	if (!(repstr = lv_path_dup(mem, lv)))
		return_0;

	return _field_set_value(field, repstr, NULL);
}

static int _lvdmpath_disp(struct dm_report *rh, struct dm_pool *mem,
			  struct dm_report_field *field,
			  const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	char *repstr;

	if (!(repstr = lv_dmpath_dup(mem, lv)))
		return_0;

	return _field_set_value(field, repstr, NULL);
}

static int _origin_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	const struct lv_segment *seg = first_seg(lv);

	if (lv_is_cow(lv))
		return _lvname_disp(rh, mem, field, origin_from_cow(lv), private);

	if (lv_is_cache(lv))
		return _lvname_disp(rh, mem, field, seg_lv(seg, 0), private);

	if (lv_is_thin_volume(lv) && first_seg(lv)->origin)
		return _lvname_disp(rh, mem, field, first_seg(lv)->origin, private);

	if (lv_is_thin_volume(lv) && first_seg(lv)->external_lv)
		return _lvname_disp(rh, mem, field, first_seg(lv)->external_lv, private);

	return _field_set_value(field, "", NULL);
}

static int _movepv_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			struct dm_report_field *field,
			const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	const char *name;

	if ((name = lv_move_pv_dup(mem, lv)))
		return dm_report_field_string(rh, field, &name);

	return _field_set_value(field, "", NULL);
}

static int _convertlv_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			   struct dm_report_field *field,
			   const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	const char *name;

	if ((name = lv_convert_lv_dup(mem, lv)))
		return dm_report_field_string(rh, field, &name);

	return _field_set_value(field, "", NULL);
}

static int _size32_disp(struct dm_report *rh __attribute__((unused)), struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const uint32_t size = *(const uint32_t *) data;
	const char *disp, *repstr;
	uint64_t *sortval;

	if (!*(disp = display_size_units(private, (uint64_t) size)))
		return_0;

	if (!(repstr = dm_pool_strdup(mem, disp))) {
		log_error("dm_pool_strdup failed");
		return 0;
	}

	if (!(sortval = dm_pool_alloc(mem, sizeof(uint64_t)))) {
		log_error("dm_pool_alloc failed");
		return 0;
	}

	*sortval = (uint64_t) size;

	return _field_set_value(field, repstr, sortval);
}

static int _size64_disp(struct dm_report *rh __attribute__((unused)),
			struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const uint64_t size = *(const uint64_t *) data;
	const char *disp, *repstr;
	uint64_t *sortval;

	if (!*(disp = display_size_units(private, size)))
		return_0;

	if (!(repstr = dm_pool_strdup(mem, disp))) {
		log_error("dm_pool_strdup failed");
		return 0;
	}

	if (!(sortval = dm_pool_alloc(mem, sizeof(uint64_t)))) {
		log_error("dm_pool_alloc failed");
		return 0;
	}

	*sortval = size;

	return _field_set_value(field, repstr, sortval);
}

static int _uint32_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			struct dm_report_field *field,
			const void *data, void *private __attribute__((unused)))
{
	return dm_report_field_uint32(rh, field, data);
}

static int _int8_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
		       struct dm_report_field *field,
		       const void *data, void *private __attribute__((unused)))
{
	const int32_t val = *(const int8_t *)data;

	return dm_report_field_int32(rh, field, &val);
}

static int _int32_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
		       struct dm_report_field *field,
		       const void *data, void *private __attribute__((unused)))
{
	return dm_report_field_int32(rh, field, data);
}

static int _lvreadahead_disp(struct dm_report *rh, struct dm_pool *mem,
			     struct dm_report_field *field,
			     const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;

	if (lv->read_ahead == DM_READ_AHEAD_AUTO)
		return _field_set_value(field, "auto", &RESERVED(number_undef_64));

	return _size32_disp(rh, mem, field, &lv->read_ahead, private);
}

static int _lvkreadahead_disp(struct dm_report *rh, struct dm_pool *mem,
			      struct dm_report_field *field,
			      const void *data,
			      void *private)
{
	const struct lv_with_info *lvi = (const struct lv_with_info *) data;

	if (!lvi->info || !lvi->info->exists)
		return dm_report_field_int32(rh, field, &RESERVED(number_undef_32));

	return _size32_disp(rh, mem, field, &lvi->info->read_ahead, private);
}

static int _vgsize_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;
	uint64_t size = vg_size(vg);

	return _size64_disp(rh, mem, field, &size, private);
}

static int _segmonitor_disp(struct dm_report *rh, struct dm_pool *mem,
			    struct dm_report_field *field,
			    const void *data, void *private)
{
	char *str;

	if (!(str = lvseg_monitor_dup(mem, (const struct lv_segment *)data)))
		return_0;

	return _field_set_value(field, str, NULL);
}

static int _segstart_disp(struct dm_report *rh, struct dm_pool *mem,
			  struct dm_report_field *field,
			  const void *data, void *private)
{
	const struct lv_segment *seg = (const struct lv_segment *) data;
	uint64_t start = lvseg_start(seg);

	return _size64_disp(rh, mem, field, &start, private);
}

static int _segstartpe_disp(struct dm_report *rh,
			    struct dm_pool *mem __attribute__((unused)),
			    struct dm_report_field *field,
			    const void *data,
			    void *private __attribute__((unused)))
{
	const struct lv_segment *seg = (const struct lv_segment *) data;

	return dm_report_field_uint32(rh, field, &seg->le);
}

static int _segsize_disp(struct dm_report *rh, struct dm_pool *mem,
			 struct dm_report_field *field,
			 const void *data, void *private)
{
	const struct lv_segment *seg = (const struct lv_segment *) data;
	uint64_t size = lvseg_size(seg);

	return _size64_disp(rh, mem, field, &size, private);
}

static int _segsizepe_disp(struct dm_report *rh,
			   struct dm_pool *mem __attribute__((unused)),
			   struct dm_report_field *field,
			   const void *data,
			   void *private __attribute__((unused)))
{
	const struct lv_segment *seg = (const struct lv_segment *) data;

	return dm_report_field_uint32(rh, field, &seg->len);
}

static int _chunksize_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct lv_segment *seg = (const struct lv_segment *) data;
	uint64_t size = lvseg_chunksize(seg);

	return _size64_disp(rh, mem, field, &size, private);
}

static int _transactionid_disp(struct dm_report *rh, struct dm_pool *mem,
				struct dm_report_field *field,
				const void *data, void *private)
{
	const struct lv_segment *seg = (const struct lv_segment *) data;

	if (seg_is_thin_pool(seg))
		return dm_report_field_uint64(rh, field, &seg->transaction_id);

	return _field_set_value(field, "", &RESERVED(number_undef_64));
}

static int _thinid_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct lv_segment *seg = (const struct lv_segment *) data;

	if (seg_is_thin_volume(seg))
		return dm_report_field_uint32(rh, field, &seg->device_id);

	return _field_set_value(field, "", &RESERVED(number_undef_64));
}

static int _discards_disp(struct dm_report *rh, struct dm_pool *mem,
			  struct dm_report_field *field,
			  const void *data, void *private)
{
	const struct lv_segment *seg = (const struct lv_segment *) data;
	const char *discards_str;

	if (seg_is_thin_volume(seg))
		seg = first_seg(seg->pool_lv);

	if (seg_is_thin_pool(seg)) {
		discards_str = get_pool_discards_name(seg->discards);
		return dm_report_field_string(rh, field, &discards_str);
	}

	return _field_set_value(field, "", NULL);
}

static int _originsize_disp(struct dm_report *rh, struct dm_pool *mem,
			    struct dm_report_field *field,
			    const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	uint64_t size = lv_origin_size(lv);

	if (size)
		return _size64_disp(rh, mem, field, &size, private);

	return _field_set_value(field, "", &_zero64);
}

static int _pvused_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct physical_volume *pv =
	    (const struct physical_volume *) data;
	uint64_t used = pv_used(pv);

	return _size64_disp(rh, mem, field, &used, private);
}

static int _pvfree_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct physical_volume *pv =
	    (const struct physical_volume *) data;
	uint64_t freespace = pv_free(pv);

	return _size64_disp(rh, mem, field, &freespace, private);
}

static int _pvsize_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct physical_volume *pv =
	    (const struct physical_volume *) data;
	uint64_t size = pv_size_field(pv);

	return _size64_disp(rh, mem, field, &size, private);
}

static int _devsize_disp(struct dm_report *rh, struct dm_pool *mem,
			 struct dm_report_field *field,
			 const void *data, void *private)
{
	const struct device *dev = *(const struct device * const *) data;
	uint64_t size;

	if (!dev || !dev->dev || !dev_get_size(dev, &size))
		size = _zero64;

	return _size64_disp(rh, mem, field, &size, private);
}

static int _vgfree_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;
	uint64_t freespace = vg_free(vg);

	return _size64_disp(rh, mem, field, &freespace, private);
}

static int _uuid_disp(struct dm_report *rh __attribute__((unused)), struct dm_pool *mem,
		      struct dm_report_field *field,
		      const void *data, void *private __attribute__((unused)))
{
	char *repstr;

	if (!(repstr = id_format_and_copy(mem, data)))
		return_0;

	return _field_set_value(field, repstr, NULL);
}

static int _pvuuid_disp(struct dm_report *rh __attribute__((unused)), struct dm_pool *mem,
		        struct dm_report_field *field,
		        const void *data, void *private __attribute__((unused)))
{
	const struct label *label = (const struct label *) data;
	const char *repstr = "";

	if (label->dev &&
	    !(repstr = id_format_and_copy(mem, (const struct id *) label->dev->pvid)))
		return_0;

	return _field_set_value(field, repstr, NULL);
}

static int _pvmdas_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct physical_volume *pv =
	    (const struct physical_volume *) data;
	uint32_t count = pv_mda_count(pv);

	return _uint32_disp(rh, mem, field, &count, private);
}

static int _pvmdasused_disp(struct dm_report *rh, struct dm_pool *mem,
			     struct dm_report_field *field,
			     const void *data, void *private)
{
	const struct physical_volume *pv =
	    (const struct physical_volume *) data;
	uint32_t count = pv_mda_used_count(pv);

	return _uint32_disp(rh, mem, field, &count, private);
}

static int _vgmdas_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;
	uint32_t count = vg_mda_count(vg);

	return _uint32_disp(rh, mem, field, &count, private);
}

static int _vgmdasused_disp(struct dm_report *rh, struct dm_pool *mem,
			     struct dm_report_field *field,
			     const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;
	uint32_t count = vg_mda_used_count(vg);

	return _uint32_disp(rh, mem, field, &count, private);
}

static int _vgmdacopies_disp(struct dm_report *rh, struct dm_pool *mem,
				   struct dm_report_field *field,
				   const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;
	uint32_t count = vg_mda_copies(vg);

	if (count == VGMETADATACOPIES_UNMANAGED)
		return _field_set_value(field, "unmanaged", &RESERVED(number_undef_64));

	return _uint32_disp(rh, mem, field, &count, private);
}

static int _vgprofile_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;

	if (vg->profile)
		return dm_report_field_string(rh, field, &vg->profile->name);

	return _field_set_value(field, "", NULL);
}

static int _pvmdafree_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct label *label = (const struct label *) data;
	uint64_t freespace = lvmcache_info_mda_free(label->info);

	return _size64_disp(rh, mem, field, &freespace, private);
}

static int _pvmdasize_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct label *label = (const struct label *) data;
	uint64_t min_mda_size = lvmcache_smallest_mda_size(label->info);

	return _size64_disp(rh, mem, field, &min_mda_size, private);
}

static int _vgmdasize_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;
	uint64_t min_mda_size = vg_mda_size(vg);

	return _size64_disp(rh, mem, field, &min_mda_size, private);
}

static int _vgmdafree_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;
	uint64_t freespace = vg_mda_free(vg);

	return _size64_disp(rh, mem, field, &freespace, private);
}

static int _lvcount_disp(struct dm_report *rh, struct dm_pool *mem,
			 struct dm_report_field *field,
			 const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;
	uint32_t count = vg_visible_lvs(vg);

	return _uint32_disp(rh, mem, field, &count, private);
}

static int _lvsegcount_disp(struct dm_report *rh, struct dm_pool *mem,
			    struct dm_report_field *field,
			    const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	uint32_t count = dm_list_size(&lv->segments);

	return _uint32_disp(rh, mem, field, &count, private);
}

static int _snapcount_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct volume_group *vg = (const struct volume_group *) data;
	uint32_t count = snapshot_count(vg);

	return _uint32_disp(rh, mem, field, &count, private);
}

static int _snpercent_disp(struct dm_report *rh, struct dm_pool *mem __attribute__((unused)),
			   struct dm_report_field *field,
			   const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	dm_percent_t snap_percent;

	if ((lv_is_cow(lv) || lv_is_merging_origin(lv)) &&
	    lv_snapshot_percent(lv, &snap_percent)) {
		if ((snap_percent != DM_PERCENT_INVALID) &&
		    (snap_percent != LVM_PERCENT_MERGE_FAILED))
			return dm_report_field_percent(rh, field, &snap_percent);

		if (!lv_is_merging_origin(lv)) {
			snap_percent = DM_PERCENT_100;
			return dm_report_field_percent(rh, field, &snap_percent);
		}

		/*
		 * on activate merge that hasn't started yet would
		 * otherwise display incorrect snap% in origin
		 */
	}

	snap_percent = DM_PERCENT_INVALID;
	return dm_report_field_percent(rh, field, &snap_percent);
}

static int _copypercent_disp(struct dm_report *rh,
			     struct dm_pool *mem __attribute__((unused)),
			     struct dm_report_field *field,
			     const void *data, void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	dm_percent_t percent = DM_PERCENT_INVALID;

	if (((lv_is_raid(lv) && lv_raid_percent(lv, &percent)) ||

	    ((lv->status & (PVMOVE | MIRRORED)) &&
	     lv_mirror_percent(lv->vg->cmd, lv, 0, &percent, NULL))) &&
	    (percent != DM_PERCENT_INVALID)) {
		percent = copy_percent(lv);
		return dm_report_field_percent(rh, field, &percent);
	}

	return dm_report_field_percent(rh, field, &percent);
}

static int _raidsyncaction_disp(struct dm_report *rh __attribute__((unused)),
			     struct dm_pool *mem,
			     struct dm_report_field *field,
			     const void *data,
			     void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	char *sync_action;

	if (lv_is_raid(lv) && lv_raid_sync_action(lv, &sync_action))
		return _string_disp(rh, mem, field, &sync_action, private);

	return _field_set_value(field, "", NULL);
}

static int _raidmismatchcount_disp(struct dm_report *rh __attribute__((unused)),
				struct dm_pool *mem,
				struct dm_report_field *field,
				const void *data,
				void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	uint64_t mismatch_count;

	if (lv_is_raid(lv) && lv_raid_mismatch_count(lv, &mismatch_count))
		return dm_report_field_uint64(rh, field, &mismatch_count);

	return _field_set_value(field, "", &RESERVED(number_undef_64));
}

static int _raidwritebehind_disp(struct dm_report *rh __attribute__((unused)),
			      struct dm_pool *mem,
			      struct dm_report_field *field,
			      const void *data,
			      void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;

	if (lv_is_raid_type(lv) && first_seg(lv)->writebehind)
		return dm_report_field_uint32(rh, field, &first_seg(lv)->writebehind);

	return _field_set_value(field, "", &RESERVED(number_undef_64));
}

static int _raidminrecoveryrate_disp(struct dm_report *rh __attribute__((unused)),
				   struct dm_pool *mem,
				   struct dm_report_field *field,
				   const void *data,
				   void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;

	if (lv_is_raid_type(lv) && first_seg(lv)->min_recovery_rate)
		return dm_report_field_uint32(rh, field,
					      &first_seg(lv)->min_recovery_rate);

	return _field_set_value(field, "", &RESERVED(number_undef_64));
}

static int _raidmaxrecoveryrate_disp(struct dm_report *rh __attribute__((unused)),
				   struct dm_pool *mem,
				   struct dm_report_field *field,
				   const void *data,
				   void *private __attribute__((unused)))
{
	const struct logical_volume *lv = (const struct logical_volume *) data;

	if (lv_is_raid_type(lv) && first_seg(lv)->max_recovery_rate)
		return dm_report_field_uint32(rh, field,
					      &first_seg(lv)->max_recovery_rate);

	return _field_set_value(field, "", &RESERVED(number_undef_64));
}

/* Called only with lv_is_thin_pool/volume */
static int _dtpercent_disp(int metadata, struct dm_report *rh,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	dm_percent_t percent = DM_PERCENT_INVALID;

	/* Suppress data percent if not using driver */
	/* cannot use lv_is_active_locally - need to check for layer -tpool */
	if (!lv_info(lv->vg->cmd, lv, 1, NULL, 0, 0))
		return dm_report_field_percent(rh, field, &percent);

	if (lv_is_thin_pool(lv)) {
		if (!lv_thin_pool_percent(lv, metadata, &percent))
			return_0;
	} else { /* thin_volume */
		if (!lv_thin_percent(lv, 0, &percent))
			return_0;
	}

	return dm_report_field_percent(rh, field, &percent);
}

static int _datapercent_disp(struct dm_report *rh, struct dm_pool *mem,
			     struct dm_report_field *field,
			     const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	dm_percent_t percent = DM_PERCENT_INVALID;

	if (lv_is_cow(lv))
		return _snpercent_disp(rh, mem, field, data, private);

	if (lv_is_thin_pool(lv) || lv_is_thin_volume(lv))
		return _dtpercent_disp(0, rh, field, data, private);

	return dm_report_field_percent(rh, field, &percent);
}

static int _metadatapercent_disp(struct dm_report *rh,
				 struct dm_pool *mem __attribute__((unused)),
				 struct dm_report_field *field,
				 const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;

	if (lv_is_thin_pool(lv))
		return _dtpercent_disp(1, rh, field, data, private);

	return _field_set_value(field, "", &RESERVED(number_undef_64));
}

static int _lvmetadatasize_disp(struct dm_report *rh, struct dm_pool *mem,
				struct dm_report_field *field,
				const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	uint64_t size;

	if (lv_is_thin_pool(lv) || lv_is_cache_pool(lv)) {
		size = lv_metadata_size(lv);
		return _size64_disp(rh, mem, field, &size, private);
	}

	return _field_set_value(field, "", &RESERVED(number_undef_64));
}

static int _thincount_disp(struct dm_report *rh, struct dm_pool *mem,
                         struct dm_report_field *field,
                         const void *data, void *private)
{
	const struct lv_segment *seg = (const struct lv_segment *) data;
	uint32_t count;

	if (seg_is_thin_pool(seg)) {
		count = dm_list_size(&seg->lv->segs_using_this_lv);
		return _uint32_disp(rh, mem, field, &count, private);
	}

	return _field_set_value(field, "", &RESERVED(number_undef_64));
}

static int _lvtime_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	char *repstr;
	uint64_t *sortval;

	if (!(repstr = lv_time_dup(mem, lv)) ||
	    !(sortval = dm_pool_alloc(mem, sizeof(uint64_t)))) {
		log_error("Failed to allocate buffer for time.");
		return 0;
	}

	*sortval = lv->timestamp;

	return _field_set_value(field, repstr, sortval);
}

static int _lvhost_disp(struct dm_report *rh, struct dm_pool *mem,
			struct dm_report_field *field,
			const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	char *repstr;

	if (!(repstr = lv_host_dup(mem, lv))) {
		log_error("Failed to allocate buffer for host.");
		return 0;
	}

	return _field_set_value(field, repstr, NULL);
}

/* PV/VG/LV Attributes */

static int _pvallocatable_disp(struct dm_report *rh, struct dm_pool *mem,
			       struct dm_report_field *field,
			       const void *data, void *private)
{
	int allocatable = (((const struct physical_volume *) data)->status & ALLOCATABLE_PV) != 0;
	return _binary_disp(rh, mem, field, allocatable, FIRST_NAME(pv_allocatable_y), private);
}

static int _pvexported_disp(struct dm_report *rh, struct dm_pool *mem,
			    struct dm_report_field *field,
			    const void *data, void *private)
{
	int exported = (((const struct physical_volume *) data)->status & EXPORTED_VG) != 0;
	return _binary_disp(rh, mem, field, exported, FIRST_NAME(pv_exported_y), private);
}

static int _pvmissing_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	int missing = (((const struct physical_volume *) data)->status & MISSING_PV) != 0;
	return _binary_disp(rh, mem, field, missing, FIRST_NAME(pv_missing_y), private);
}

static int _vgpermissions_disp(struct dm_report *rh, struct dm_pool *mem,
			       struct dm_report_field *field,
			       const void *data, void *private)
{
	const char *perms = ((const struct volume_group *) data)->status & LVM_WRITE ? FIRST_NAME(vg_permissions_rw)
										     : FIRST_NAME(vg_permissions_r);
	return _string_disp(rh, mem, field, &perms, private);
}

static int _vgextendable_disp(struct dm_report *rh, struct dm_pool *mem,
			      struct dm_report_field *field,
			      const void *data, void *private)
{
	int extendable = (vg_is_resizeable((const struct volume_group *) data)) != 0;
	return _binary_disp(rh, mem, field, extendable, FIRST_NAME(vg_extendable_y),private);
}

static int _vgexported_disp(struct dm_report *rh, struct dm_pool *mem,
			    struct dm_report_field *field,
			    const void *data, void *private)
{
	int exported = (vg_is_exported((const struct volume_group *) data)) != 0;
	return _binary_disp(rh, mem, field, exported, FIRST_NAME(vg_exported_y), private);
}

static int _vgpartial_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	int partial = (vg_missing_pv_count((const struct volume_group *) data)) != 0;
	return _binary_disp(rh, mem, field, partial, FIRST_NAME(vg_partial_y), private);
}

static int _vgallocationpolicy_disp(struct dm_report *rh, struct dm_pool *mem,
				    struct dm_report_field *field,
				    const void *data, void *private)
{
	const char *alloc_policy = get_alloc_string(((const struct volume_group *) data)->alloc) ? : _str_unknown;
	return _string_disp(rh, mem, field, &alloc_policy, private);
}

static int _vgclustered_disp(struct dm_report *rh, struct dm_pool *mem,
			     struct dm_report_field *field,
			     const void *data, void *private)
{
	int clustered = (vg_is_clustered((const struct volume_group *) data)) != 0;
	return _binary_disp(rh, mem, field, clustered, FIRST_NAME(vg_clustered_y), private);
}

static int _lvlayout_disp(struct dm_report *rh, struct dm_pool *mem,
				struct dm_report_field *field,
				const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	struct dm_list *lv_layout;
	struct dm_list *lv_role;

	if (!lv_layout_and_role(mem, lv, &lv_layout, &lv_role)) {
		log_error("Failed to display layout for LV %s/%s.", lv->vg->name, lv->name);
		return 0;
	}

	return _field_set_string_list(rh, field, lv_layout, private, 0);
}

static int _lvrole_disp(struct dm_report *rh, struct dm_pool *mem,
			      struct dm_report_field *field,
			      const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	struct dm_list *lv_layout;
	struct dm_list *lv_role;

	if (!lv_layout_and_role(mem, lv, &lv_layout, &lv_role)) {
		log_error("Failed to display role for LV %s/%s.", lv->vg->name, lv->name);
		return 0;
	}

	return _field_set_string_list(rh, field, lv_role, private, 0);
}

static int _lvinitialimagesync_disp(struct dm_report *rh, struct dm_pool *mem,
				    struct dm_report_field *field,
				    const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	int initial_image_sync;

	if (lv_is_raid(lv) || lv_is_mirrored(lv))
		initial_image_sync = (lv->status & LV_NOTSYNCED) == 0;
	else
		initial_image_sync = 0;

	return _binary_disp(rh, mem, field, initial_image_sync, FIRST_NAME(lv_initial_image_sync_y), private);
}

static int _lvimagesynced_disp(struct dm_report *rh, struct dm_pool *mem,
			       struct dm_report_field *field,
			       const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	int image_synced;

	if (lv_is_raid_image(lv))
		image_synced = !lv_is_visible(lv) && lv_raid_image_in_sync(lv);
	else if (lv_is_mirror_image(lv))
		image_synced = lv_mirror_image_in_sync(lv);
	else
		image_synced = 0;

	return _binary_disp(rh, mem, field, image_synced, FIRST_NAME(lv_image_synced_y), private);
}

static int _lvmerging_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	int merging;

	if (lv_is_origin(lv) || lv_is_external_origin(lv))
		merging = lv_is_merging_origin(lv);
	else if (lv_is_cow(lv))
		merging = lv_is_merging_cow(lv);
	else if (lv_is_thin_volume(lv))
		merging = lv_is_merging_thin_snapshot(lv);
	else
		merging = 0;

	return _binary_disp(rh, mem, field, merging, FIRST_NAME(lv_merging_y), private);
}

static int _lvconverting_disp(struct dm_report *rh, struct dm_pool *mem,
			      struct dm_report_field *field,
			      const void *data, void *private)
{
	int converting = (((const struct logical_volume *) data)->status & CONVERTING) != 0;
	return _binary_disp(rh, mem, field, converting, "converting", private);
}

static int _lvpermissions_disp(struct dm_report *rh, struct dm_pool *mem,
			       struct dm_report_field *field,
			       const void *data, void *private)
{
	const struct lv_with_info *lvi = (const struct lv_with_info *) data;
	const char *perms = "";

	if (!(lvi->lv->status & PVMOVE)) {
		if (lvi->lv->status & LVM_WRITE) {
			if (!lvi->info->exists)
				perms = _str_unknown;
			else if (lvi->info->read_only)
				perms = FIRST_NAME(lv_permissions_r_override);
			else
				perms = FIRST_NAME(lv_permissions_rw);
		} else if (lvi->lv->status & LVM_READ)
			perms = FIRST_NAME(lv_permissions_r);
		else
			perms = _str_unknown;
	}

	return _string_disp(rh, mem, field, &perms, private);
}

static int _lvallocationpolicy_disp(struct dm_report *rh, struct dm_pool *mem,
				    struct dm_report_field *field,
				    const void *data, void *private)
{
	const char *alloc_policy = get_alloc_string(((const struct logical_volume *) data)->alloc) ? : _str_unknown;
	return _string_disp(rh, mem, field, &alloc_policy, private);
}

static int _lvallocationlocked_disp(struct dm_report *rh, struct dm_pool *mem,
				    struct dm_report_field *field,
				    const void *data, void *private)
{
	int alloc_locked = (((const struct logical_volume *) data)->status & LOCKED) != 0;
	return _binary_disp(rh, mem, field, alloc_locked, FIRST_NAME(lv_allocation_locked_y), private);
}

static int _lvfixedminor_disp(struct dm_report *rh, struct dm_pool *mem,
			      struct dm_report_field *field,
			      const void *data, void *private)
{
	int fixed_minor = (((const struct logical_volume *) data)->status & FIXED_MINOR) != 0;
	return _binary_disp(rh, mem, field, fixed_minor, FIRST_NAME(lv_fixed_minor_y), private);
}

static int _lvactive_disp(struct dm_report *rh, struct dm_pool *mem,
			     struct dm_report_field *field,
			     const void *data, void *private)
{
	char *repstr;

	if (!(repstr = lv_active_dup(mem, (const struct logical_volume *) data))) {
		log_error("Failed to allocate buffer for active.");
		return 0;
	}

	return _field_set_value(field, repstr, NULL);
}

static int _lvactivelocally_disp(struct dm_report *rh, struct dm_pool *mem,
				 struct dm_report_field *field,
				 const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	int active_locally;

	if (!activation())
		return _binary_undef_disp(rh, mem, field, private);

	if (vg_is_clustered(lv->vg)) {
		lv = lv_lock_holder(lv);
		active_locally = lv_is_active_locally(lv);
	} else
		active_locally = lv_is_active(lv);

	return _binary_disp(rh, mem, field, active_locally, FIRST_NAME(lv_active_locally_y), private);
}

static int _lvactiveremotely_disp(struct dm_report *rh, struct dm_pool *mem,
				  struct dm_report_field *field,
				  const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	int active_remotely;

	if (!activation())
		return _binary_undef_disp(rh, mem, field, private);

	if (vg_is_clustered(lv->vg)) {
		lv = lv_lock_holder(lv);
		/* FIXME: It seems we have no way to get this info correctly
		 * 	  with current interface - we'd need to check number
		 * 	  of responses from the cluster:
		 * 	    - if number of nodes that responded == 1
		 * 	    - and LV is active on local node
		 * 	  ..then we may say that LV is *not* active remotely.
		 *
		 * 	  Otherwise ((responses > 1 && LV active locally) ||
		 * 	  (responses == 1 && LV not active locally)), it's
		 * 	  active remotely.
		 *
		 * 	  We have this info, but hidden underneath the
		 * 	  locking interface (locking_type.query_resource fn).
		 *
		 * 	  For now, let's use 'unknown' for remote status if
		 * 	  the LV is found active locally until we find a way to
		 * 	  smuggle the proper information out of the interface.
		 */
		if (lv_is_active_locally(lv))
			return _binary_undef_disp(rh, mem, field, private);
		else
			active_remotely = lv_is_active_but_not_locally(lv);
	} else
		active_remotely = 0;

	return _binary_disp(rh, mem, field, active_remotely, FIRST_NAME(lv_active_remotely_y), private);
}

static int _lvactiveexclusively_disp(struct dm_report *rh, struct dm_pool *mem,
				     struct dm_report_field *field,
				     const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	int active_exclusively;

	if (!activation())
		return _binary_undef_disp(rh, mem, field, private);

	if (vg_is_clustered(lv->vg)) {
		lv = lv_lock_holder(lv);
		active_exclusively = lv_is_active_exclusive(lv);
	} else
		active_exclusively = lv_is_active(lv);

	return _binary_disp(rh, mem, field, active_exclusively, FIRST_NAME(lv_active_exclusively_y), private);
}

static int _lvmergefailed_disp(struct dm_report *rh, struct dm_pool *mem,
			       struct dm_report_field *field,
			       const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	dm_percent_t snap_percent;
	int merge_failed;

	if (!lv_is_cow(lv) || !lv_snapshot_percent(lv, &snap_percent))
		return _field_set_value(field, _str_unknown, &RESERVED(number_undef_64));

	merge_failed = snap_percent == LVM_PERCENT_MERGE_FAILED;
	return _binary_disp(rh, mem, field, merge_failed, FIRST_NAME(lv_merge_failed_y), private);
}

static int _lvsnapshotinvalid_disp(struct dm_report *rh, struct dm_pool *mem,
				   struct dm_report_field *field,
				   const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	dm_percent_t snap_percent;
	int snap_invalid;

	if (!lv_is_cow(lv))
		return _field_set_value(field, _str_unknown, &RESERVED(number_undef_64));

	snap_invalid = !lv_snapshot_percent(lv, &snap_percent) || snap_percent == DM_PERCENT_INVALID;
	return _binary_disp(rh, mem, field, snap_invalid, FIRST_NAME(lv_snapshot_invalid_y), private);
}

static int _lvsuspended_disp(struct dm_report *rh, struct dm_pool *mem,
			     struct dm_report_field *field,
			     const void *data, void *private)
{
	const struct lv_with_info *lvi = (const struct lv_with_info *) data;

	if (lvi->info->exists)
		return _binary_disp(rh, mem, field, lvi->info->suspended, FIRST_NAME(lv_suspended_y), private);

	return _binary_undef_disp(rh, mem, field, private);
}

static int _lvlivetable_disp(struct dm_report *rh, struct dm_pool *mem,
			     struct dm_report_field *field,
			     const void *data, void *private)
{
	const struct lv_with_info *lvi = (const struct lv_with_info *) data;

	if (lvi->info->exists)
		return _binary_disp(rh, mem, field, lvi->info->live_table, FIRST_NAME(lv_live_table_y), private);

	return _binary_undef_disp(rh, mem, field, private);
}

static int _lvinactivetable_disp(struct dm_report *rh, struct dm_pool *mem,
				 struct dm_report_field *field,
				 const void *data, void *private)
{
	const struct lv_with_info *lvi = (const struct lv_with_info *) data;

	if (lvi->info->exists)
		return _binary_disp(rh, mem, field, lvi->info->inactive_table, FIRST_NAME(lv_inactive_table_y), private);

	return _binary_undef_disp(rh, mem, field, private);
}

static int _lvdeviceopen_disp(struct dm_report *rh, struct dm_pool *mem,
			      struct dm_report_field *field,
			      const void *data, void *private)
{
	const struct lv_with_info *lvi = (const struct lv_with_info *) data;

	if (lvi->info->exists)
		return _binary_disp(rh, mem, field, lvi->info->open_count, FIRST_NAME(lv_device_open_y), private);

	return _binary_undef_disp(rh, mem, field, private);
}

static int _thinzero_disp(struct dm_report *rh, struct dm_pool *mem,
			   struct dm_report_field *field,
			   const void *data, void *private)
{
	const struct lv_segment *seg = (const struct lv_segment *) data;

	if (seg_is_thin_pool(seg))
		return _binary_disp(rh, mem, field, seg->zero_new_blocks, FIRST_NAME(zero_y), private);

	return _binary_undef_disp(rh, mem, field, private);
}

static int _lvhealthstatus_disp(struct dm_report *rh, struct dm_pool *mem,
				struct dm_report_field *field,
				const void *data, void *private)
{
	const struct logical_volume *lv = (const struct logical_volume *) data;
	const char *health = "";
	uint64_t n;

	if (lv->status & PARTIAL_LV)
		health = "partial";
	else if (lv_is_raid_type(lv)) {
		if (!activation())
			health = "unknown";
		else if (!lv_raid_healthy(lv))
			health = "refresh needed";
		else if (lv_is_raid(lv)) {
			if (lv_raid_mismatch_count(lv, &n) && n)
				health = "mismatches exist";
		} else if (lv->status & LV_WRITEMOSTLY)
			health = "writemostly";
	}

	return _string_disp(rh, mem, field, &health, private);
}

static int _lvskipactivation_disp(struct dm_report *rh, struct dm_pool *mem,
				  struct dm_report_field *field,
				  const void *data, void *private)
{
	int skip_activation = (((const struct logical_volume *) data)->status & LV_ACTIVATION_SKIP) != 0;
	return _binary_disp(rh, mem, field, skip_activation, "skip activation", private);
}

/* Report object types */

/* necessary for displaying something for PVs not belonging to VG */
static struct format_instance _dummy_fid = {
	.metadata_areas_in_use = { &(_dummy_fid.metadata_areas_in_use), &(_dummy_fid.metadata_areas_in_use) },
	.metadata_areas_ignored = { &(_dummy_fid.metadata_areas_ignored), &(_dummy_fid.metadata_areas_ignored) },
};

static struct volume_group _dummy_vg = {
	.fid = &_dummy_fid,
	.name = "",
	.system_id = (char *) "",
	.pvs = { &(_dummy_vg.pvs), &(_dummy_vg.pvs) },
	.lvs = { &(_dummy_vg.lvs), &(_dummy_vg.lvs) },
	.tags = { &(_dummy_vg.tags), &(_dummy_vg.tags) },
};

static void *_obj_get_vg(void *obj)
{
	struct volume_group *vg = ((struct lvm_report_object *)obj)->vg;

	return vg ? vg : &_dummy_vg;
}

static void *_obj_get_lv(void *obj)
{
	return ((struct lvm_report_object *)obj)->lvi->lv;
}

static void *_obj_get_lv_with_info(void *obj)
{
	return ((struct lvm_report_object *)obj)->lvi;
}

static void *_obj_get_pv(void *obj)
{
	return ((struct lvm_report_object *)obj)->pv;
}

static void *_obj_get_label(void *obj)
{
	return ((struct lvm_report_object *)obj)->label;
}

static void *_obj_get_seg(void *obj)
{
	return ((struct lvm_report_object *)obj)->seg;
}

static void *_obj_get_pvseg(void *obj)
{
	return ((struct lvm_report_object *)obj)->pvseg;
}

static void *_obj_get_devtypes(void *obj)
{
	return obj;
}

static const struct dm_report_object_type _report_types[] = {
	{ VGS, "Volume Group", "vg_", _obj_get_vg },
	{ LVS, "Logical Volume", "lv_", _obj_get_lv },
	{ LVSINFO, "Logical Volume Device", "lv_", _obj_get_lv_with_info },
	{ PVS, "Physical Volume", "pv_", _obj_get_pv },
	{ LABEL, "Physical Volume Label", "pv_", _obj_get_label },
	{ SEGS, "Logical Volume Segment", "seg_", _obj_get_seg },
	{ PVSEGS, "Physical Volume Segment", "pvseg_", _obj_get_pvseg },
	{ 0, "", "", NULL },
};

static const struct dm_report_object_type _devtypes_report_types[] = {
	{ DEVTYPES, "Device Types", "devtype_", _obj_get_devtypes },
	{ 0, "", "", NULL },
};

/*
 * Import column definitions
 */

#define STR DM_REPORT_FIELD_TYPE_STRING
#define NUM DM_REPORT_FIELD_TYPE_NUMBER
#define BIN DM_REPORT_FIELD_TYPE_NUMBER
#define SIZ DM_REPORT_FIELD_TYPE_SIZE
#define PCT DM_REPORT_FIELD_TYPE_PERCENT
#define STR_LIST DM_REPORT_FIELD_TYPE_STRING_LIST
#define FIELD(type, strct, sorttype, head, field, width, func, id, desc, writeable) \
	{type, sorttype, offsetof(type_ ## strct, field), width, \
	 #id, head, &_ ## func ## _disp, desc},

typedef struct physical_volume type_pv;
typedef struct logical_volume type_lv;
typedef struct volume_group type_vg;
typedef struct lv_segment type_seg;
typedef struct pv_segment type_pvseg;
typedef struct label type_label;

typedef dev_known_type_t type_devtype;

static const struct dm_report_field_type _fields[] = {
#include "columns.h"
{0, 0, 0, 0, "", "", NULL, NULL},
};

static const struct dm_report_field_type _devtypes_fields[] = {
#include "columns-devtypes.h"
{0, 0, 0, 0, "", "", NULL, NULL},
};

#undef STR
#undef NUM
#undef BIN
#undef SIZ
#undef STR_LIST
#undef FIELD

void *report_init(struct cmd_context *cmd, const char *format, const char *keys,
		  report_type_t *report_type, const char *separator,
		  int aligned, int buffered, int headings, int field_prefixes,
		  int quoted, int columns_as_rows, const char *selection)
{
	uint32_t report_flags = 0;
	int devtypes_report = *report_type & DEVTYPES ? 1 : 0;
	void *rh;

	if (aligned)
		report_flags |= DM_REPORT_OUTPUT_ALIGNED;

	if (buffered)
		report_flags |= DM_REPORT_OUTPUT_BUFFERED;

	if (headings)
		report_flags |= DM_REPORT_OUTPUT_HEADINGS;

	if (field_prefixes)
		report_flags |= DM_REPORT_OUTPUT_FIELD_NAME_PREFIX;

	if (!quoted)
		report_flags |= DM_REPORT_OUTPUT_FIELD_UNQUOTED;

	if (columns_as_rows)
		report_flags |= DM_REPORT_OUTPUT_COLUMNS_AS_ROWS;

	rh = dm_report_init_with_selection(report_type,
		devtypes_report ? _devtypes_report_types : _report_types,
		devtypes_report ? _devtypes_fields : _fields,
		format, separator, report_flags, keys,
		selection, _report_reserved_values, cmd);

	if (rh && field_prefixes)
		dm_report_set_output_field_name_prefix(rh, "lvm2_");

	return rh;
}

/*
 * Create a row of data for an object
 */
int report_object(void *handle, struct volume_group *vg,
		  struct logical_volume *lv, struct physical_volume *pv,
		  struct lv_segment *seg, struct pv_segment *pvseg,
		  struct lvinfo *lvinfo, struct label *label)
{
	struct device dummy_device = { .dev = 0 };
	struct label dummy_label = { .dev = &dummy_device };
	struct lv_with_info lvi = { .lv = lv, .info = lvinfo };
	struct lvm_report_object obj = {
		.vg = vg,
		.lvi = &lvi,
		.pv = pv,
		.seg = seg,
		.pvseg = pvseg,
		.label = label ? : (pv ? pv_label(pv) : NULL)
	};

	/* FIXME workaround for pv_label going through cache; remove once struct
	 * physical_volume gains a proper "label" pointer */
	if (!obj.label) {
		if (pv) {
			if (pv->fmt)
				dummy_label.labeller = pv->fmt->labeller;
			if (pv->dev)
				dummy_label.dev = pv->dev;
			else
				memcpy(dummy_device.pvid, &pv->id, ID_LEN);
		}
		obj.label = &dummy_label;
	}

	/* The two format fields might as well match. */
	if (!vg && pv)
		_dummy_fid.fmt = pv->fmt;

	return dm_report_object(handle, &obj);
}

static int _report_devtype_single(void *handle, const dev_known_type_t *devtype)
{
	return dm_report_object(handle, (void *)devtype);
}

int report_devtypes(void *handle)
{
	int devtypeind = 0;

	while (_dev_known_types[devtypeind].name[0])
		if (!_report_devtype_single(handle, &_dev_known_types[devtypeind++]))
			return 0;

	return 1;
}
