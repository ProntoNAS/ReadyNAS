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

static int _pvdisplay_single(struct cmd_context *cmd,
			     struct volume_group *vg __attribute((unused)),
			     struct physical_volume *pv, void *handle)
{
	uint64_t size;

	const char *pv_name = dev_name(pv->dev);

	if (!*pv->vg_name)
		size = pv->size;
	else
		size = (pv->pe_count - pv->pe_alloc_count) * pv->pe_size;

	if (arg_count(cmd, short_ARG)) {
		log_print("Device \"%s\" has a capacity of %s", pv_name,
			  display_size(cmd, size));
		return ECMD_PROCESSED;
	}

	if (pv->status & EXPORTED_VG)
		log_print("Physical volume \"%s\" of volume group \"%s\" "
			  "is exported", pv_name, pv->vg_name);

	if (!pv->vg_name)
		log_print("\"%s\" is a new physical volume of \"%s\"",
			  pv_name, display_size(cmd, size));

	if (arg_count(cmd, colon_ARG)) {
		pvdisplay_colons(pv);
		return ECMD_PROCESSED;
	}

	pvdisplay_full(cmd, pv, handle);

	if (!arg_count(cmd, maps_ARG))
		return ECMD_PROCESSED;

	return ECMD_PROCESSED;
}

int pvdisplay(struct cmd_context *cmd, int argc, char **argv)
{
	if (arg_count(cmd, columns_ARG)) {
		if (arg_count(cmd, colon_ARG) || arg_count(cmd, maps_ARG) ||
		    arg_count(cmd, short_ARG)) {
			log_error("Incompatible options selected");
			return EINVALID_CMD_LINE;
		}
		return pvs(cmd, argc, argv);
	} else if (arg_count(cmd, aligned_ARG) ||
		   arg_count(cmd, all_ARG) ||
		   arg_count(cmd, noheadings_ARG) ||
		   arg_count(cmd, options_ARG) ||
		   arg_count(cmd, separator_ARG) ||
		   arg_count(cmd, sort_ARG) || arg_count(cmd, unbuffered_ARG)) {
		log_error("Incompatible options selected");
		return EINVALID_CMD_LINE;
	}

	if (arg_count(cmd, colon_ARG) && arg_count(cmd, maps_ARG)) {
		log_error("Option -v not allowed with option -c");
		return EINVALID_CMD_LINE;
	}

	return process_each_pv(cmd, argc, argv, NULL, NULL, _pvdisplay_single);
}
