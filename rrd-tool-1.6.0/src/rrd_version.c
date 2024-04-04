/*****************************************************************************
 * RRDtool 1.6.0, Copyright 1997-2016 by Tobi Oetiker
 *****************************************************************************
 * rrd_version Return
 *****************************************************************************
 * Initial version by Burton Strauss, ntopSupport.com - 5/2005
 *****************************************************************************/

#include "rrd_tool.h"

double rrd_version(
    void)
{
    return NUMVERS;
}

char     *rrd_strversion(
    void)
{
    return PACKAGE_VERSION;
}
