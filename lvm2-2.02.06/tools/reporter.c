/*
 * Copyright (C) 2001-2004 Sistina Software, Inc. All rights reserved. 
 * Copyright (C) 2004 Red Hat, Inc. All rights reserved.
 *
 * This file is part of LVM2.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "tools.h"
#include "report.h"

static int _vgs_single(struct cmd_context *cmd __attribute((unused)),
		       const char *vg_name, struct volume_group *vg,
		       int consistent __attribute((unused)), void *handle)
{
	if (!vg) {
		log_error("Volume group %s not found", vg_name);
		return ECMD_FAILED;
	}

	if (!report_object(handle, vg, NULL, NULL, NULL, NULL))
		return ECMD_FAILED;

	check_current_backup(vg);

	return ECMD_PROCESSED;
}

static int _lvs_single(struct cmd_context *cmd, struct logical_volume *lv,
		       void *handle)
{
	if (!arg_count(cmd, all_ARG) && !lv_is_visible(lv))
		return ECMD_PROCESSED;

	if (!report_object(handle, lv->vg, lv, NULL, NULL, NULL))
		return ECMD_FAILED;

	return ECMD_PROCESSED;
}

static int _segs_single(struct cmd_context *cmd __attribute((unused)),
			struct lv_segment *seg, void *handle)
{
	if (!report_object(handle, seg->lv->vg, seg->lv, NULL, seg, NULL))
		return ECMD_FAILED;

	return ECMD_PROCESSED;
}

static int _pvsegs_sub_single(struct cmd_context *cmd, struct volume_group *vg,
			      struct pv_segment *pvseg, void *handle)
{
	int consistent = 0;
	struct physical_volume *pv = pvseg->pv;
	int ret = ECMD_PROCESSED;

	if (!lock_vol(cmd, pv->vg_name, LCK_VG_READ)) {
		log_error("Can't lock %s: skipping", pv->vg_name);
		return ECMD_FAILED;
	}

	if (!(vg = vg_read(cmd, pv->vg_name, NULL, &consistent))) {
		log_error("Can't read %s: skipping", pv->vg_name);
		unlock_vg(cmd, pv->vg_name);
		return ECMD_FAILED;
	}

	if (!report_object(handle, vg, NULL, pv, NULL, pvseg))
		ret = ECMD_FAILED;

	unlock_vg(cmd, pv->vg_name);
	return ret;
}

static int _lvsegs_single(struct cmd_context *cmd, struct logical_volume *lv,
			  void *handle)
{
	if (!arg_count(cmd, all_ARG) && !lv_is_visible(lv))
		return ECMD_PROCESSED;

	return process_each_segment_in_lv(cmd, lv, handle, _segs_single);
}

static int _pvsegs_single(struct cmd_context *cmd, struct volume_group *vg,
			  struct physical_volume *pv, void *handle)
{
	return process_each_segment_in_pv(cmd, vg, pv, handle,
					  _pvsegs_sub_single);
}

static int _pvs_single(struct cmd_context *cmd, struct volume_group *vg,
		       struct physical_volume *pv, void *handle)
{
	int consistent = 0;
	int ret = ECMD_PROCESSED;

	if (pv->vg_name) {
		if (!lock_vol(cmd, pv->vg_name, LCK_VG_READ)) {
			log_error("Can't lock %s: skipping", pv->vg_name);
			return ECMD_FAILED;
		}

		if (!(vg = vg_read(cmd, pv->vg_name, (char *)&pv->vgid, &consistent))) {
			log_error("Can't read %s: skipping", pv->vg_name);
			unlock_vg(cmd, pv->vg_name);
			return ECMD_FAILED;
		}
	}

	if (!report_object(handle, vg, NULL, pv, NULL, NULL))
		ret = ECMD_FAILED;

	if (pv->vg_name)
		unlock_vg(cmd, pv->vg_name);

	return ret;
}

static int _report(struct cmd_context *cmd, int argc, char **argv,
		   report_type_t report_type)
{
	void *report_handle;
	const char *opts;
	char *str;
	const char *keys = NULL, *options = NULL, *separator;
	int r = ECMD_PROCESSED;

	int aligned, buffered, headings;

	aligned = find_config_tree_int(cmd, "report/aligned",
				  DEFAULT_REP_ALIGNED);
	buffered = find_config_tree_int(cmd, "report/buffered",
				   DEFAULT_REP_BUFFERED);
	headings = find_config_tree_int(cmd, "report/headings",
				   DEFAULT_REP_HEADINGS);
	separator = find_config_tree_str(cmd, "report/separator",
				    DEFAULT_REP_SEPARATOR);

	switch (report_type) {
	case LVS:
		keys = find_config_tree_str(cmd, "report/lvs_sort",
				       DEFAULT_LVS_SORT);
		if (!arg_count(cmd, verbose_ARG))
			options = find_config_tree_str(cmd,
						  "report/lvs_cols",
						  DEFAULT_LVS_COLS);
		else
			options = find_config_tree_str(cmd,
						  "report/lvs_cols_verbose",
						  DEFAULT_LVS_COLS_VERB);
		break;
	case VGS:
		keys = find_config_tree_str(cmd, "report/vgs_sort",
				       DEFAULT_VGS_SORT);
		if (!arg_count(cmd, verbose_ARG))
			options = find_config_tree_str(cmd,
						  "report/vgs_cols",
						  DEFAULT_VGS_COLS);
		else
			options = find_config_tree_str(cmd,
						  "report/vgs_cols_verbose",
						  DEFAULT_VGS_COLS_VERB);
		break;
	case PVS:
		keys = find_config_tree_str(cmd, "report/pvs_sort",
				       DEFAULT_PVS_SORT);
		if (!arg_count(cmd, verbose_ARG))
			options = find_config_tree_str(cmd,
						  "report/pvs_cols",
						  DEFAULT_PVS_COLS);
		else
			options = find_config_tree_str(cmd,
						  "report/pvs_cols_verbose",
						  DEFAULT_PVS_COLS_VERB);
		break;
	case SEGS:
		keys = find_config_tree_str(cmd, "report/segs_sort",
				       DEFAULT_SEGS_SORT);
		if (!arg_count(cmd, verbose_ARG))
			options = find_config_tree_str(cmd,
						  "report/segs_cols",
						  DEFAULT_SEGS_COLS);
		else
			options = find_config_tree_str(cmd,
						  "report/segs_cols_verbose",
						  DEFAULT_SEGS_COLS_VERB);
		break;
	case PVSEGS:
		keys = find_config_tree_str(cmd, "report/pvsegs_sort",
				       DEFAULT_PVSEGS_SORT);
		if (!arg_count(cmd, verbose_ARG))
			options = find_config_tree_str(cmd,
						  "report/pvsegs_cols",
						  DEFAULT_PVSEGS_COLS);
		else
			options = find_config_tree_str(cmd,
						  "report/pvsegs_cols_verbose",
						  DEFAULT_PVSEGS_COLS_VERB);
		break;
	}

	/* If -o supplied use it, else use default for report_type */
	if (arg_count(cmd, options_ARG)) {
		opts = arg_str_value(cmd, options_ARG, "");
		if (!opts || !*opts) {
			log_error("Invalid options string: %s", opts);
			return 0;
		}
		if (*opts == '+') {
			if (!(str = dm_pool_alloc(cmd->mem,
					 strlen(options) + strlen(opts) + 1))) {
				log_error("options string allocation failed");
				return 0;
			}
			strcpy(str, options);
			strcat(str, ",");
			strcat(str, opts + 1);
			options = str;
		} else
			options = opts;
	}

	/* -O overrides default sort settings */
	if (arg_count(cmd, sort_ARG))
		keys = arg_str_value(cmd, sort_ARG, "");

	if (arg_count(cmd, separator_ARG))
		separator = arg_str_value(cmd, separator_ARG, " ");
	if (arg_count(cmd, separator_ARG))
		aligned = 0;
	if (arg_count(cmd, aligned_ARG))
		aligned = 1;
	if (arg_count(cmd, unbuffered_ARG) && !arg_count(cmd, sort_ARG))
		buffered = 0;
	if (arg_count(cmd, noheadings_ARG))
		headings = 0;

	if (!(report_handle = report_init(cmd, options, keys, &report_type,
					  separator, aligned, buffered,
					  headings)))
		return 0;

	switch (report_type) {
	case LVS:
		r = process_each_lv(cmd, argc, argv, LCK_VG_READ, report_handle,
				    &_lvs_single);
		break;
	case VGS:
		r = process_each_vg(cmd, argc, argv, LCK_VG_READ, 0,
				    report_handle, &_vgs_single);
		break;
	case PVS:
		r = process_each_pv(cmd, argc, argv, NULL, report_handle,
				    &_pvs_single);
		break;
	case SEGS:
		r = process_each_lv(cmd, argc, argv, LCK_VG_READ, report_handle,
				    &_lvsegs_single);
		break;
	case PVSEGS:
		r = process_each_pv(cmd, argc, argv, NULL, report_handle,
				    &_pvsegs_single);
		break;
	}

	report_output(report_handle);

	report_free(report_handle);
	return r;
}

int lvs(struct cmd_context *cmd, int argc, char **argv)
{
	report_type_t type;

	if (arg_count(cmd, segments_ARG))
		type = SEGS;
	else
		type = LVS;

	return _report(cmd, argc, argv, type);
}

int vgs(struct cmd_context *cmd, int argc, char **argv)
{
	return _report(cmd, argc, argv, VGS);
}

int pvs(struct cmd_context *cmd, int argc, char **argv)
{
	report_type_t type;

	if (arg_count(cmd, segments_ARG))
		type = PVSEGS;
	else
		type = PVS;

	return _report(cmd, argc, argv, type);
}
