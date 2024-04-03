#!/usr/bin/env python
# 
# ***** BEGIN LICENSE BLOCK *****
# Source last modified: $Id: datelib.py,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
# 
# Portions Copyright (c) 1995-2004 RealNetworks, Inc. All Rights Reserved.
# 
# The contents of this file, and the files included with this file,
# are subject to the current version of the RealNetworks Public
# Source License (the "RPSL") available at
# http://www.helixcommunity.org/content/rpsl unless you have licensed
# the file under the current version of the RealNetworks Community
# Source License (the "RCSL") available at
# http://www.helixcommunity.org/content/rcsl, in which case the RCSL
# will apply. You may also obtain the license terms directly from
# RealNetworks.  You may not use this file except in compliance with
# the RPSL or, if you have a valid RCSL with RealNetworks applicable
# to this file, the RCSL.  Please see the applicable RPSL or RCSL for
# the rights, obligations and limitations governing use of the
# contents of the file.
# 
# Alternatively, the contents of this file may be used under the
# terms of the GNU General Public License Version 2 or later (the
# "GPL") in which case the provisions of the GPL are applicable
# instead of those above. If you wish to allow use of your version of
# this file only under the terms of the GPL, and not to allow others
# to use your version of this file under the terms of either the RPSL
# or RCSL, indicate your decision by deleting the provisions above
# and replace them with the notice and other provisions required by
# the GPL. If you do not delete the provisions above, a recipient may
# use your version of this file under the terms of any one of the
# RPSL, the RCSL or the GPL.
# 
# This file is part of the Helix DNA Technology. RealNetworks is the
# developer of the Original Code and owns the copyrights in the
# portions it created.
# 
# This file, and the files included with this file, is distributed
# and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS
# ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
# ENJOYMENT OR NON-INFRINGEMENT.
# 
# Technology Compatibility Kit Test Suite(s) Location:
#    http://www.helixcommunity.org/content/tck
# 
# Contributor(s):
# 
# ***** END LICENSE BLOCK *****
# 
import string
import re
import time

_re_rev_date1 = re.compile(
    "([0-9]{4})-([0-9][0-9])-([0-9][0-9]) "\
    "([0-9][0-9]):([0-9][0-9]):([0-9][0-9])")

_re_rev_date12 = re.compile(
    "([0-9]{4})/([0-9][0-9])/([0-9][0-9]) "\
    "([0-9][0-9]):([0-9][0-9]):([0-9][0-9])")

_re_rev_date2 = re.compile(
    "([0-9]{4})-([0-9][0-9])-([0-9][0-9])")

def date_to_ticks(date):
    date = string.strip(date)

    temp = None

    match = _re_rev_date2.match(date)
    if match:
        temp = tuple(map(int, match.groups())) + (0, 0, 0)
    else:
        match = _re_rev_date1.match(date)
        if not match:
            match = _re_rev_date12.match(date)

        if match:
            temp = tuple(map(int, match.groups()))

    if temp:
        dst=time.localtime(time.mktime(temp + (0,1,0)))[8]
        return time.mktime(temp + (0,1,dst))

    return None

def date_to_gmticks(date):
    t=date_to_ticks(date)
    if not t:
        return t
    return t - time.timezone

def asctime(ticks):
    return time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(ticks))

def cvstime(ticks):
    return time.strftime("%Y/%m/%d %H:%M:%S", time.localtime(ticks))

def cvs_server_time(ticks):
    return time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(ticks))


def get_day(ticks):
    (t, t, day, t, t, t, t, t, t) = time.localtime(ticks)
    return day


def asctime_from_seconds(sec):
    seconds = sec % 60
    minutes = (sec / 60) % 60
    hours   = sec / 3600

    return "%d:%02d.%02d" % (hours, minutes, seconds)


## This may not technically belong here....
def fix_tag(str):
    """Return (tag, timestamp) given a tag or timestamp"""
    if " " in str:
        return ( "", str )
    if "/" in str:
        return ( "", str +" 00:00:00" )

    return (str, "")
        

def timestamp():
    t = time.localtime(time.time())
    return "%04d-%02d-%02d %02d:%02d:%02d" % t[:6]

