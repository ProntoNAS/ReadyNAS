/* $Id: upnphttp.h,v 1.1.1.1 2007-01-12 02:38:46 jmaggard Exp $ */ 
/* miniupnp project
 * http://miniupnp.free.fr/
 * http://miniupnp.tuxfamily.org/
 * (c) 2006 Thomas Bernard
 * This software is subject to the conditions detailed in
 * the LICENCE file included in the distribution */
#ifndef __UPNPHTTP_H__
#define __UPNPHTTP_H__
#include <sys/queue.h>

#include "config.h"

/* Server: HTTP header returned in all HTTP responses : */
#define MINIUPNPD_SERVER_STRING	OS_VERSION " UPnP/1.0 upnpd/1.0"

/*
 States :
  0 - Waiting for data to read
  1 - Waiting for HTTP Post Content.
 ...
 >= 100 - to be deleted
*/
enum httpCommands {
	EUnknown = 0,
	EGet,
	EPost
};

struct upnphttp {
	int socket;
	int state;
	char HttpVer[16];
	/* request */
	char * req_buf;
	int req_buflen;
	int req_contentlen;
	int req_contentoff;     /* header length */
	enum httpCommands req_command;
	char * req_soapAction;
	int req_soapActionLen;
	/* response */
	char * res_buf;
	int res_buflen;
	int res_buf_alloclen;
	/*int res_contentlen;*/
	/*int res_contentoff;*/		/* header length */
	LIST_ENTRY(upnphttp) entries;
};

struct upnphttp * New_upnphttp(int);

void CloseSocket_upnphttp(struct upnphttp *);

void Delete_upnphttp(struct upnphttp *);

void Process_upnphttp(struct upnphttp *);

/* Build the header for the HTTP Response
 * Also allocate the buffer for body data */
void
BuildHeader_upnphttp(struct upnphttp * h, int respcode,
                     const char * respmsg,
                     int bodylen);
/* BuildResp_upnphttp() fill the res_buf buffer with the complete
 * HTTP 200 OK response from the body passed as argument */
void BuildResp_upnphttp(struct upnphttp *, const char *, int);

/* same but with given response code/message */
void
BuildResp2_upnphttp(struct upnphttp * h, int respcode,
                    const char * respmsg,
                    const char * body, int bodylen);

void SendResp_upnphttp(struct upnphttp *);

#endif

