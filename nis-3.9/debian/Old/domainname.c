/*
 * domainname	Well it wasn't included in any nis or yp package anymore
 *		so I wrote a replacement..
 *
 * Version:	@(#)domainname.c  1.00  05-Nov-1997  miquels@cistron.nl
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 dated June, 1991.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	char buf[256];
	char *p;

	if (argc <= 1) {
		if (getdomainname(buf, 256) < 0) {
			perror("getdomainname");
			return 1;
		}
		printf("%s\n", buf);
		return 0;
	}
	if (argc != 2) {
		fprintf(stderr, "Usage: domainname [domain]\n");
		return 1;
	}
	strcpy(buf, argv[1]);
	if ((p = strchr(buf, '\n')) != NULL)
		*p = 0;
	if (setdomainname(buf, strlen(buf)) < 0) {
		perror("setdomainname");
		return 1;
	}

	return 0;
}
