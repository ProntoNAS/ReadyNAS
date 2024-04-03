#line 3466 "ifupdown.nw"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>

#include "archlinux.h"

unsigned int mylinuxver() {
	static int maj = -1, rev, min;

	if (maj == -1) {
		struct utsname u;
		char *pch;
		uname(&u);
		maj = atoi(u.release);
		pch = strchr(u.release, '.');
		rev = atoi(pch+1);
		pch = strchr(pch+1, '.');
		min = atoi(pch+1);
	}

	return mylinux(maj,rev,min);
}

unsigned int mylinux(int maj, int rev, int min) { 
	return min | rev << 10 | maj << 13;
}

int execable(char *program) {
	struct stat buf;

	if (0 == stat(program, &buf)) {
		if (S_ISREG(buf.st_mode) && (S_IXUSR & buf.st_mode)) return 1;
	}
	return 0;
}
