/*
 * Rendezvous support with avahi
 *
 * Copyright (C) 2006 Justin Maggard <jmaggard@infrant.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>

#include "err.h"

int rend_init(char *user) {
	/* Clean up after a potentially left-over service file. */
	unlink("/etc/avahi/services/mt-daapd.service");
	return 0;
}

int rend_running(void) {
	/* Unimplemented */
	return 0;
}

int rend_stop(void) {
        FILE *a_pidfile;
	unsigned int avahi_pid=0;

	DPRINTF(E_DBG, L_REND, "Stopping avahi polling\n");

	unlink("/etc/avahi/services/mt-daapd.service");

	a_pidfile=fopen("/var/run/avahi-daemon/pid","r");
	if(!a_pidfile)
		return 1;
	fscanf(a_pidfile, "%u", &avahi_pid);
	fclose(a_pidfile);
	if (avahi_pid) {
		DPRINTF(E_DBG, L_REND, "Sending SIGHUP to avahi PID %u\n", avahi_pid);
		kill(avahi_pid, SIGHUP);
	}
	return 0;
}

int rend_unregister(char *name, char *type, int port) {
	/* Unimplemented */
	return 0;
}

int rend_register(char *name, char *type, int port, char *iface, char *txt) {
	FILE *service_in;
	FILE *service_out;
	FILE *a_pidfile;
	char *service_in_file = "/etc/avahi/services/mt-daapd.service";
	char *service_out_file = "/tmp/mt-daapd.temp";
	char * line = NULL;
	size_t len = 0;
	int done = 0;

	char txt_buff[256];
	int txt_size=0;
	int txt_begin=0;

	unsigned int avahi_pid=0;

	assert(name != NULL);
	assert(port != 0);

	
	DPRINTF(E_DBG, L_REND, "Updating avahi service file\n");

	service_out = fopen(service_out_file, "w");
	if (service_out == NULL) {
		DPRINTF(E_DBG, L_REND, "rend-avahi.c: Could not open tmp service file.\n");
		return 1;
	}

	service_in = fopen(service_in_file, "r");
	if (service_in == NULL) {
		fputs("<?xml version='1.0' standalone='no'?>\n", service_out);
		fputs("<service-group>\n", service_out);
		fputs("  <name replace-wildcards='yes'>", service_out);
		fprintf(service_out, "%s", name);
		fputs(" on %h</name>\n", service_out);
		fputs("  <service>\n", service_out);
		fputs("    <type>", service_out);
		fprintf(service_out, "%s", type);
		fputs("</type>\n", service_out);
		fputs("    <port>", service_out);
		fprintf(service_out, "%d", port);
		fputs("</port>\n", service_out);
		while( txt_begin < strlen(txt) ) {
			txt_size = txt[txt_begin];
			txt_begin++;
			memset(txt_buff,0x00,sizeof(txt_buff));
			memcpy(&txt_buff,txt+txt_begin,txt_size);
			fputs("    <txt-record>", service_out);
			fprintf(service_out, "%s", txt_buff);
			fputs("</txt-record>\n", service_out);
			txt_begin = (txt_begin+txt_size);
		}
		fputs("  </service>\n", service_out);
		fputs("</service-group>\n", service_out);
	}
	else {
		while ((getline(&line, &len, service_in)) != -1) {
			if ( !done && strstr(line, "<service>") != NULL ) {
				fputs("  <service>\n", service_out);
				fputs("    <type>", service_out);
				fprintf(service_out, "%s", type);
				fputs("</type>\n", service_out);
				fputs("    <port>", service_out);
				fprintf(service_out, "%d", port);
				fputs("</port>\n", service_out);
				while( txt_begin < strlen(txt) ) {
					txt_size = txt[txt_begin];
					txt_begin++;
					memset(txt_buff,0x00,sizeof(txt_buff));
					memcpy(&txt_buff,txt+txt_begin,txt_size);
					fputs("    <txt-record>", service_out);
					fprintf(service_out, "%s", txt_buff);
					fputs("</txt-record>\n", service_out);
					txt_begin = (txt_begin+txt_size);
				}
				fputs("  </service>\n", service_out);
				done++;
			}
			fprintf(service_out, "%s", line);
		}
		if (line)
			free(line);
		fclose(service_in);
	}
	fclose(service_out);

	if ( rename(service_out_file, service_in_file) ) {
		DPRINTF(E_DBG, L_REND, "rend-avahi.c: Could not rename service file.\n");
		return 1;
	}


	DPRINTF(E_DBG, L_REND, "Starting avahi polling\n");
	a_pidfile=fopen("/var/run/avahi-daemon/pid","r");
	if(!a_pidfile)
		return 1;
	fscanf(a_pidfile, "%u", &avahi_pid);
	fclose(a_pidfile);
	if (avahi_pid) {
		DPRINTF(E_DBG, L_REND, "Sending SIGHUP to avahi PID %u\n", avahi_pid);
		kill(avahi_pid, SIGHUP);
		sleep(2);
	}

	return 0;
}
