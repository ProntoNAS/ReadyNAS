# 
# ***** BEGIN LICENSE BLOCK *****
# Source last modified: $Id: bldreg.py,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
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
"""Convenance functions for locating, loading, and modifing the build system
registry.  It uses registry.py for manipulating the registry file.  The build
system registry is the "build.reg" file located in the source root of a
source tree built with the build system."""

import os
import registry
import outmsg


## filename for the registry data file
REGISTRY_FILENAME = "build.reg"


def new_registry():
    if not os.path.isfile(REGISTRY_FILENAME):
        open(REGISTRY_FILENAME, "w").write("\n")
        

## start at the current directory and backtrack until one is found

def find_registry_path(filename = None):
    if not filename:
        filename = REGISTRY_FILENAME
        
    path = os.getcwd()

    while 1:
        try:
            file_list = os.listdir(path)
        except OSError:
            file_list = []

        if file_list.count(filename):
            break

        base_path, temp = os.path.split(path)
        if base_path == path or not len(base_path):
            return None
        path = base_path

    return os.path.join(path, filename)
    
        
def find_registry(filename):
    registry_path = find_registry_path(filename)
    if not registry_path:
        new_registry()
        registry_path = os.path.join(os.getcwd(), filename)

    return registry.OpenRegistryFile(registry_path)
    

## global registry instance
_registry=None

def _getreg():
    global _registry
    if not _registry:
        _registry = find_registry(REGISTRY_FILENAME)
    return _registry

def clear():
    _getreg().clear()

def clear_section(section):
    _getreg().clear_section(section)

def clear_value(section, key):
    _getreg().clear_value(section, key)

def section_list():
    return _getreg().section_list()

def section_key_list(section):
    return _getreg().section_key_list(section)

def get_value(section, key):
    return _getreg().get_value(section, key)

def get_value_default(section, key, value):
    return _getreg().get_value_default(section, key, value)

def get(section, key, value):
    return _getreg().get(section, key, value)

def set_value(section, key, value):
    _getreg().set_value(section, key, value)

def close():
    _getreg().close()

def sync_on():
    _getreg().sync_on()

def sync_off():
    _getreg().sync_off()

def uptake():
    _getreg().uptake()
