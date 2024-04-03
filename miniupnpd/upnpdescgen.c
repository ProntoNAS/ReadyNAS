/* project: miniupnp
 * webpage: http://miniupnp.free.fr/
 * (c) 2006 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided with this distribution */
/* $Id: upnpdescgen.c,v 1.4 2008-09-23 00:25:11 jason Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "upnpdescgen.h"
#include "miniupnpdpath.h"
#include "upnpglobalvars.h"
#include "upnpdescstrings.h"

static const char * const upnptypes[] =
{
	"string",
	"boolean",
	"ui2",
	"ui4"
};

static const char * const upnpdefaultvalues[] =
{
	0,
	"Unconfigured"
};

static const char * const upnpallowedvalues[] =
{
	0,		/* 0 */
	"DSL",	/* 1 */
	"POTS",
	"Cable",
	"Ethernet",
	0,
	"Up",	/* 6 */
	"Down",
	"Initializing",
	"Unavailable",
	0,
	"TCP",	/* 11 */
	"UDP",
	0,
	"Unconfigured",	/* 14 */
	"IP_Routed",
	"IP_Bridged",
	0,
	"Unconfigured",	/* 18 */
	"Connecting",
	"Connected",
	"PendingDisconnect",
	"Disconnecting",
	"Disconnected",
	0,
	"ERROR_NONE",	/* 25 */
	0,
	"",		/* 27 */
	0
};

static const char xmlver[] = "<?xml version=\"1.0\"?>\n";
static const char root_service[] =
	"scpd xmlns=\"urn:schemas-upnp-org:service-1-0\"";
static const char root_device[] = 
	"root xmlns=\"urn:schemas-upnp-org:device-1-0\"";

/* For PnP-X */
static const char x_device[] = 
	"device xmlns:pnpx=\"http://schemas.microsoft.com/windows/pnpx/2005/11\"";

/* root Description of the UPnP Device 
 * fixed to match UPnP_IGD_InternetGatewayDevice 1.0.pdf */
static const struct XMLElt rootDesc[] =
{
/* 0 */
	{root_device, INITHELPER(1,2)},
	{"specVersion", INITHELPER(3,2)},
	//{"device", INITHELPER(5,16)},
	{x_device, INITHELPER(5,14)},
	{"/major", "1"},
	{"/minor", "0"},
	//{"/pnpx:X_hardwareId", "PnPX_StorageDevice_HWID"},
	//{"/pnpx:X_compatibleId", "PnPX_StorageDevice_CPID"},
	{"/pnpx:X_deviceCategory", "Storage.NAS"},
	{"/deviceType", "urn:schemas-upnp-org:device:networkstoragedevice:1"},
	{"/friendlyName", hostname},	/* required */
	{"/manufacturer", vendor},		/* required */
/* 8 */
	{"/manufacturerURL", ROOTDEV_MANUFACTURERURL},	/* optional */
	{"/modelDescription", ROOTDEV_MODEDESCRIPTION},
	{"/modelName", model_name},	/* required */
	{"/modelNumber", model_num},
	{"/modelURL", ROOTDEV_MODELURL},
	{"/serialNumber", serial},
	{"/UDN", uuidvalue},	/* required */
	{"/UPC", ROOTDEV_UPC},
	{"/presentationURL", presentationurl},	/* Recommended */
	{"serviceList", INITHELPER(19,1)},
	{"service", INITHELPER(20,5)},
	{"/serviceType", "urn:schemas-upnp-org:service:nascontrol:1"},
	{"/serviceId", "urn:upnp-org:serviceId:nascontrol1"},
	{"/controlURL", "/upnp/control/nascontrol1"},
	{"/eventSubURL", "/upnp/event/nascontrol1"},
	{"/SCPDURL", DUMMY_PATH},
	{0, 0}
};

#if 0

static const struct argument AddPortMappingArgs[] =
{
	{"NewRemoteHost", 1, 11},
	{"NewExternalPort", 1, 12},
	{"NewProtocol", 1, 14},
	{"NewInternalPort", 1, 13},
	{"NewInternalClient", 1, 15},
	{"NewEnabled", 1, 9},
	{"NewPortMappingDescription", 1, 16},
	{"NewLeaseDuration", 1, 10},
	{0, 0, 0}
};

static const struct argument GetExternalIPAddressArgs[] =
{
	{"NewExternalIPAddress", 2, 7},
	{0, 0, 0}
};

static const struct argument DeletePortMappingArgs[] = 
{
	{"NewRemoteHost", 1, 11},
	{"NewExternalPort", 1, 13},
	{"NewProtocol", 1, 14},
	{0, 0, 0}
};

static const struct argument SetConnectionTypeArgs[] =
{
	{"NewConnectionType", 1, 0},
	{0, 0, 0}
};

static const struct argument GetConnectionTypeInfoArgs[] =
{
	{"NewConnectionType", 2, 0},
	{"NewPossibleConnectionTypes", 2, 1},
	{0, 0, 0}
};

static const struct argument GetStatusInfoArgs[] =
{
	{"NewConnectionStatus", 2, 2},
	{"NewLastConnectionError", 2, 4},
	{"NewUptime", 2, 3},
	{0, 0, 0}
};

static const struct argument GetNATRSIPStatusArgs[] =
{
	{"NewRSIPAvailable", 2, 5},
	{"NewNATEnabled", 2, 6},
	{0, 0, 0}
};

static const struct argument GetGenericPortMappingEntryArgs[] =
{
	{"NewPortMappingIndex", 1, 8},
	{"NewRemoteHost", 2, 11},
	{"NewExternalPort", 2, 12},
	{"NewProtocol", 2, 14},
	{"NewInternalPort", 2, 13},
	{"NewInternalClient", 2, 15},
	{"NewEnabled", 2, 9},
	{"NewPortMappingDescription", 2, 16},
	{"NewLeaseDuration", 2, 10},
	{0, 0, 0}
};

static const struct argument GetSpecificPortMappingEntryArgs[] =
{
	{"NewRemoteHost", 1, 11},
	{"NewExternalPort", 1, 12},
	{"NewProtocol", 1, 14},
	{"NewInternalPort", 2, 13},
	{"NewInternalClient", 2, 15},
	{"NewEnabled", 2, 9},
	{"NewPortMappingDescription", 2, 16},
	{"NewLeaseDuration", 2, 10},
	{0, 0, 0}
};

static const struct action WANIPCnActions[] =
{
	{"AddPortMapping", AddPortMappingArgs}, /* R */
	{"GetExternalIPAddress", GetExternalIPAddressArgs}, /* R */
	{"DeletePortMapping", DeletePortMappingArgs}, /* R */
	{"SetConnectionType", SetConnectionTypeArgs}, /* R */
	{"GetConnectionTypeInfo", GetConnectionTypeInfoArgs}, /* R */
	{"RequestConnection", 0}, /* R */
	{"ForceTermination", 0}, /* R */
	{"GetStatusInfo", GetStatusInfoArgs}, /* R */
	{"GetNATRSIPStatus", GetNATRSIPStatusArgs}, /* R */
	{"GetGenericPortMappingEntry", GetGenericPortMappingEntryArgs}, /* R */
	{"GetSpecificPortMappingEntry", GetSpecificPortMappingEntryArgs}, /* R */
	{0, 0}
};
/* R=Required, O=Optional */

static const struct stateVar WANIPCnVars[] =
{
	{"ConnectionType", 0, 0/*1*/}, /* required */
	{"PossibleConnectionTypes", 0|0x80, 0, 14},
	 /* Required
	  * Allowed values : Unconfigured / IP_Routed / IP_Bridged */
	{"ConnectionStatus", 0|0x80, 0/*1*/, 18}, /* required */
	 /* Allowed Values : Unconfigured / Connecting(opt) / Connected
	  *                  PendingDisconnect(opt) / Disconnecting (opt)
	  *                  Disconnected */
	{"Uptime", 3, 0},	/* Required */
	{"LastConnectionError", 0, 0, 25},	/* required : */
	 /* Allowed Values : ERROR_NONE(req) / ERROR_COMMAND_ABORTED(opt)
	  *                  ERROR_NOT_ENABLED_FOR_INTERNET(opt)
	  *                  ERROR_USER_DISCONNECT(opt)
	  *                  ERROR_ISP_DISCONNECT(opt)
	  *                  ERROR_IDLE_DISCONNECT(opt)
	  *                  ERROR_FORCED_DISCONNECT(opt)
	  *                  ERROR_NO_CARRIER(opt)
	  *                  ERROR_IP_CONFIGURATION(opt)
	  *                  ERROR_UNKNOWN(opt) */
	{"RSIPAvailable", 1, 0}, /* required */
	{"NATEnabled", 1, 0},    /* required */
	{"ExternalIPAddress", 0|0x80, 0}, /* required. Default : empty string */
	{"PortMappingNumberOfEntries", 2|0x80, 0}, /* required >= 0 */
	{"PortMappingEnabled", 1, 0}, /* Required */
	{"PortMappingLeaseDuration", 3, 0}, /* required */
	{"RemoteHost", 0, 0},   /* required. Default : empty string */
	{"ExternalPort", 2, 0}, /* required */
	{"InternalPort", 2, 0}, /* required */
	{"PortMappingProtocol", 0, 0, 11}, /* required allowedValues: TCP/UDP */
	{"InternalClient", 0, 0}, /* required */
	{"PortMappingDescription", 0, 0}, /* required default: empty string */
	{0, 0, 0}
};

static const struct serviceDesc scpdWANIPCn =
{ WANIPCnActions, WANIPCnVars };

/* WANCfg.xml */
/* See UPnP_IGD_WANCommonInterfaceConfig 1.0.pdf */

static const struct argument GetCommonLinkPropertiesArgs[] =
{
	{"NewWANAccessType", 2, 0},
	{"NewLayer1UpstreamMaxBitRate", 2, 1},
	{"NewLayer1DownstreamMaxBitRate", 2, 2},
	{"NewPhysicalLinkStatus", 2, 3},
	{0, 0, 0}
};

static const struct argument GetTotalBytesSentArgs[] =
{
	{"NewTotalBytesSent", 2, 4},
	{0, 0, 0}
};

static const struct argument GetTotalBytesReceivedArgs[] =
{
	{"NewTotalBytesReceived", 2, 5},
	{0, 0, 0}
};

static const struct argument GetTotalPacketsSentArgs[] =
{
	{"NewTotalPacketsSent", 2, 6},
	{0, 0, 0}
};

static const struct argument GetTotalPacketsReceivedArgs[] =
{
	{"NewTotalPacketsReceived", 2, 7},
	{0, 0, 0}
};

static const struct action WANCfgActions[] =
{
	{"GetCommonLinkProperties", GetCommonLinkPropertiesArgs}, /* Required */
	{"GetTotalBytesSent", GetTotalBytesSentArgs},             /* optional */
	{"GetTotalBytesReceived", GetTotalBytesReceivedArgs},     /* optional */
	{"GetTotalPacketsSent", GetTotalPacketsSentArgs},         /* optional */
	{"GetTotalPacketsReceived", GetTotalPacketsReceivedArgs}, /* optional */
	{0, 0}
};

/* See UPnP_IGD_WANCommonInterfaceConfig 1.0.pdf */
static const struct stateVar WANCfgVars[] =
{
	{"WANAccessType", 0, 0, 1},
	/* Allowed Values : DSL / POTS / Cable / Ethernet 
	 * Default value : empty string */
	{"Layer1UpstreamMaxBitRate", 3, 0},
	{"Layer1DownstreamMaxBitRate", 3, 0},
	{"PhysicalLinkStatus", 0|0x80, 0, 6},
	/*  allowed values : 
	 *      Up / Down / Initializing (optional) / Unavailable (optionnal)
	 *  no Default value 
	 *  Evented */
	{"TotalBytesSent", 3, 0},	   /* Optional */
	{"TotalBytesReceived", 3, 0},  /* Optional */
	{"TotalPacketsSent", 3, 0},    /* Optional */
	{"TotalPacketsReceived", 3, 0},/* Optional */
	/*{"MaximumActiveConnections", 2, 0},	// allowed Range value // OPTIONAL */
	{0, 0, 0}
};

static const struct serviceDesc scpdWANCfg =
{ WANCfgActions, WANCfgVars };

#endif

/* recursive sub routine */
#if 0
void DisplayXML(const struct XMLElt * p, int i)
{
	int j, k;
	const char * eltname;

	eltname = p[i].eltname;
	if(eltname[0] == '/')
	{
		printf("<%s>%s<%s>\n", eltname+1,
				p[i].data, eltname);
	}
	else
	{
		/*j = p[i].index; */
		/*k = j + p[i].nchild; */
		j = (unsigned)p[i].data & 0xffff;
		k = j + ((unsigned)p[i].data >> 16);
		if(j == k)
		{
			printf("<%s/>\n", eltname);
		}
		else
		{
			printf("<%s>\n", eltname);
			while(j < k)
				DisplayXML(p, j++);
			printf("</%s>\n", eltname);
		}
	}
}

void DisplayRootDesc(void)
{
	printf("%s", xmlver);
	DisplayXML(rootDesc, 0);
}
#endif

static char * strcat_str(char * str, int * len, int * tmplen, const char * s2)
{
	int s2len;
	s2len = (int)strlen(s2);
	if(*tmplen <= (*len + s2len))
	{
		if(s2len < 256)
			*tmplen += 256;
		else
			*tmplen += s2len;
		str = (char *)realloc(str, *tmplen);
	}
	/*strcpy(str + *len, s2); */
	memcpy(str + *len, s2, s2len + 1);
	*len += s2len;
	return str;
}

static char * strcat_char(char * str, int * len, int * tmplen, char c)
{
	if(*tmplen <= (*len + 1))
	{
		*tmplen += 256;
		str = (char *)realloc(str, *tmplen);
	}
	str[*len] = c;
	(*len)++;
	return str;
}

/* iterative subroutine using a stack*/
static char * genXML(char * str, int * len, int * tmplen,
                   const struct XMLElt * p)
{
	unsigned short i, j, k;
	int top;
	const char * eltname, *s;
	char c;
	struct {
		unsigned short i;
		unsigned short j;
		const char * eltname;
	} pile[16]; /* stack */
	top = -1;
	i = 0;	/* current node */
	j = 1;	/* i + number of nodes*/
	for(;;)
	{
		eltname = p[i].eltname;
		if(!eltname)
			return str;
		if(eltname[0] == '/')
		{
			/*printf("<%s>%s<%s>\n", eltname+1, p[i].data, eltname); */
			str = strcat_char(str, len, tmplen, '<');
			str = strcat_str(str, len, tmplen, eltname+1);
			str = strcat_char(str, len, tmplen, '>');
			str = strcat_str(str, len, tmplen, p[i].data);
			str = strcat_char(str, len, tmplen, '<');
			str = strcat_str(str, len, tmplen, eltname);
			str = strcat_char(str, len, tmplen, '>');
			for(;;)
			{
				if(top < 0)
					return str;
				i = ++(pile[top].i);
				j = pile[top].j;
				/*printf("  pile[%d]\t%d %d\n", top, i, j); */
				if(i==j)
				{
					/*printf("</%s>\n", pile[top].eltname); */
					str = strcat_char(str, len, tmplen, '<');
					str = strcat_char(str, len, tmplen, '/');
					s = pile[top].eltname;
					for(c = *s; c > ' '; c = *(++s))
						str = strcat_char(str, len, tmplen, c);
					str = strcat_char(str, len, tmplen, '>');
					top--;
				}
				else
					break;
			}
		}
		else
		{
			/*printf("<%s>\n", eltname); */
			str = strcat_char(str, len, tmplen, '<');
			str = strcat_str(str, len, tmplen, eltname);
			str = strcat_char(str, len, tmplen, '>');
			k = i;
			/*i = p[k].index; */
			/*j = i + p[k].nchild; */
			i = (unsigned)p[k].data & 0xffff;
			j = i + ((unsigned)p[k].data >> 16);
			top++;
			/*printf(" +pile[%d]\t%d %d\n", top, i, j); */
			pile[top].i = i;
			pile[top].j = j;
			pile[top].eltname = eltname;
		}
	}
}

/* genRootDesc() :
 * - Generate the root description of the UPnP device.
 * - the len argument is used to return the length of
 *   the returned string. 
 * - tmp_uuid argument is used to build the uuid string */
char * genRootDesc(int * len)
{
	char * str;
	int tmplen;
	str = (char *)malloc(4000);
	tmplen = 4000;
	* len = strlen(xmlver);
	/*strcpy(str, xmlver); */
	memcpy(str, xmlver, *len + 1);
	str = genXML(str, len, &tmplen, rootDesc);
	str[*len] = '\0';
	return str;
}

#if 0

static char * genServiceDesc(int * len, const struct serviceDesc * s)
{
	int i, j;
	const struct action * acts;
	const struct stateVar * vars;
	const struct argument * args;
	char * str;
	int tmplen;
	tmplen = 2048;
	str = (char *)malloc(tmplen);
	/*strcpy(str, xmlver); */
	*len = strlen(xmlver);
	memcpy(str, xmlver, *len + 1);
	
	acts = s->actionList;
	vars = s->serviceStateTable;

	str = strcat_char(str, len, &tmplen, '<');
	str = strcat_str(str, len, &tmplen, root_service);
	str = strcat_char(str, len, &tmplen, '>');

	str = strcat_str(str, len, &tmplen,
		"<specVersion><major>1</major><minor>0</minor></specVersion>");

	i = 0;
	str = strcat_str(str, len, &tmplen, "<actionList>");
	while(acts[i].name)
	{
		str = strcat_str(str, len, &tmplen, "<action><name>");
		str = strcat_str(str, len, &tmplen, acts[i].name);
		str = strcat_str(str, len, &tmplen, "</name>");
		/* argument List */
		args = acts[i].args;
		if(args)
		{
			str = strcat_str(str, len, &tmplen, "<argumentList>");
			j = 0;
			while(args[j].name)
			{
				str = strcat_str(str, len, &tmplen, "<argument><name>");
				str = strcat_str(str, len, &tmplen, args[j].name);
				str = strcat_str(str, len, &tmplen, "</name><direction>");
				str = strcat_str(str, len, &tmplen, args[j].dir==1?"in":"out");
				str = strcat_str(str, len, &tmplen,
						"</direction><relatedStateVariable>");
				str = strcat_str(str, len, &tmplen,
						vars[args[j].relatedVar].name);
				str = strcat_str(str, len, &tmplen,
						"</relatedStateVariable></argument>");
				j++;
			}
			str = strcat_str(str, len, &tmplen,"</argumentList>");
		}
		str = strcat_str(str, len, &tmplen, "</action>");
		/*str = strcat_char(str, len, &tmplen, '\n'); // TEMP ! */
		i++;
	}
	str = strcat_str(str, len, &tmplen, "</actionList><serviceStateTable>");
	i = 0;
	while(vars[i].name)
	{
		str = strcat_str(str, len, &tmplen,
				"<stateVariable sendEvents=\"");
		/* for the moment allways send no. Wait for SUBSCRIBE implementation
		 * before setting it to yes */
		/*str = strcat_str(str, len, &tmplen, (vars[i].itype & 0x80)?"yes":"no"); */
		str = strcat_str(str, len, &tmplen, "no");
		str = strcat_str(str, len, &tmplen, "\"><name>");
		str = strcat_str(str, len, &tmplen, vars[i].name);
		str = strcat_str(str, len, &tmplen, "</name><dataType>");
		str = strcat_str(str, len, &tmplen, upnptypes[vars[i].itype & 0x0f]);
		str = strcat_str(str, len, &tmplen, "</dataType>");
		if(vars[i].iallowedlist)
		{
		  str = strcat_str(str, len, &tmplen, "<allowedValueList>");
		  for(j=vars[i].iallowedlist; upnpallowedvalues[j]; j++)
		  {
		    str = strcat_str(str, len, &tmplen, "<allowedValue>");
		    str = strcat_str(str, len, &tmplen, upnpallowedvalues[j]);
		    str = strcat_str(str, len, &tmplen, "</allowedValue>");
		  }
		  str = strcat_str(str, len, &tmplen, "</allowedValueList>");
		}
		/*if(vars[i].defaultValue) */
		if(vars[i].idefault)
		{
		  str = strcat_str(str, len, &tmplen, "<defaultValue>");
		  /*str = strcat_str(str, len, &tmplen, vars[i].defaultValue); */
		  str = strcat_str(str, len, &tmplen, upnpdefaultvalues[vars[i].idefault]);
		  str = strcat_str(str, len, &tmplen, "</defaultValue>");
		}
		str = strcat_str(str, len, &tmplen, "</stateVariable>");
		/*str = strcat_char(str, len, &tmplen, '\n'); // TEMP ! */
		i++;
	}
	str = strcat_str(str, len, &tmplen, "</serviceStateTable></scpd>");
	str[*len] = '\0';
	return str;
}

char * genWANIPCn(int * len)
{
	return genServiceDesc(len, &scpdWANIPCn);
}

char * genWANCfg(int * len)
{
	return genServiceDesc(len, &scpdWANCfg);
}

#endif
