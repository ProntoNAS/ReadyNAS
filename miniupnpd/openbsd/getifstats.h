#ifndef __GETIFSTATS_H__
#define __GETIFSTATS_H__

struct ifdata {
	unsigned long opackets;
	unsigned long ipackets;
	unsigned long obytes;
	unsigned long ibytes;
	unsigned long baudrate;
};

int getifstats(const char * ifname, struct ifdata * data);

#endif

