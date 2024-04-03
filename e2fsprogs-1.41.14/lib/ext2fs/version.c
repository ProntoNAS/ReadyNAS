/*
 * version.c --- Return the version of the ext2 library
 *
 * Copyright (C) 1997 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Library
 * General Public License, version 2.
 * %End-Header%
 */

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "ext2_fs.h"
#include "ext2fs.h"

#include "../../version.h"

static const char *lib_version = E2FSPROGS_VERSION;
static const char *lib_date = E2FSPROGS_DATE;

int ext2fs_parse_version_string(const char *ver_string)
{
	const char *cp;
	int version = 0, dot_count = 0;

	for (cp = ver_string; *cp; cp++) {
		if (*cp == '.') {
			if (dot_count++)
				break;
			else
				continue;
		}
		if (!isdigit(*cp))
			break;
		version = (version * 10) + (*cp - '0');
	}
	return version;
}


int ext2fs_get_library_version(const char **ver_string,
			       const char **date_string)
{
	if (ver_string)
		*ver_string = lib_version;
	if (date_string)
		*date_string = lib_date;

	return ext2fs_parse_version_string(lib_version);
}

#if 1 // NETGEAR
#include <libreadynas_lgpl/readynas.h>
int get_vol(char *volbuf)
{
	char buf[1024];
	int read, i, ret;
	FILE *cmdline;
	char *dir, *vg, *lv = NULL;
	pid_t pid;

	pid = getpid();
	sprintf(buf, "/proc/%d/cmdline", pid);
	cmdline = fopen(buf, "r");
	if (!cmdline)
		return 1;
	read = fread(buf, 1, 1024, cmdline);
        fclose(cmdline);
	if (read < 4)
		return 1;
	for (i=read-2; i>0; i--)
	{
		if (buf[i] != '\0')
			continue;

		if ((dir = strchr(buf+i+1, '/')))
		{
			if ((vg = strchr(++dir, '/')))
			{
				*vg = '\0';
				if ((lv = strchr(++vg, '/')))
				{
					*lv = '\0';
					if ((strlen(++lv) == 1) || (strcmp(vg, "mapper") == 0))
					{
						volbuf[0] = toupper(lv[0]);
						break;
					}
				}
			}
		}
        }

	return (volbuf[0] ? 0 : 1);
}

void readynas_update_lcd(unsigned short new_stage, __u32 val, __u32 max)
{
	static unsigned short int stage = 0;
	static int total;
	static time_t last_update = 0;
	time_t now = time(NULL);
	char msg[17] = { '\0' };
	FILE *enclog;
	char *line = NULL;
	size_t len = 0, slen, flen, last_line = 0, updated = 0;
	int read;
	static char status[32];
	static char vol[3] = { '\0', '\0', '\0' };
	__u64 pct;

	if (!new_stage && val != total && (now - last_update) < 3)
		return;
	last_update = now;

	if( !vol[0] ) {
		if( get_vol(vol) != 0 )
		{
			strcpy(vol, "FS");
		}
	}

	if (max)
		total = max;
	if (new_stage)
		stage = new_stage;

	switch (stage)
	{
		case 11: // mkfs
			pct = (val*3) / total;
			snprintf(msg, sizeof(msg), "Create %s: %3ju%%", vol, pct);
			break;
		case 12:
			pct = (((val*94) / total) + 3);
			snprintf(msg, sizeof(msg), "Create %s: %3ju%%", vol, pct);
			break;
		case 21: // online resize
			pct = (val*100) / total;
			snprintf(msg, sizeof(msg), "Resize %s: %3ju%%", vol, pct);
			break;
		case 101:
		case 102:
			pct = 100;
			snprintf(msg, sizeof(msg), "%s %s: done", (stage == 101 ? "Create" : "Resize"), vol);
			break;
		default:
			return;
	}

	readynas_lcd_disp_line(2, msg);

	slen = snprintf(status, sizeof(status), "Boot!!VOLUME_CREATE!!%3ju%%\n", pct);
	if ((enclog = fopen("/var/log/frontview/enclosure.log", "r+")))
	{
		while ((read = getline(&line, &len, enclog)) != -1)
		{
			if (strncmp(line, "Boot!", 5) == 0)
			{
				if (read == slen)
				{
					fseek(enclog, last_line, SEEK_SET);
					fwrite(status, 1, slen, enclog);
					updated = 1;
				}
			}
			last_line = ftell(enclog);
		}
		if (feof(enclog) && !updated)
		{
			fseek(enclog, 0, SEEK_END);
			flen = ftell(enclog);
			fseek(enclog, slen, SEEK_END);
			fseek(enclog, flen, SEEK_SET);
			fwrite(status, 1, slen, enclog);
		}
		fclose(enclog);
		if (line)
		{
			free(line);
		}
	}
}
#endif
