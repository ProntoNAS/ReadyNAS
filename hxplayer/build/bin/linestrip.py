#!/usr/bin/env python
# 
# ***** BEGIN LICENSE BLOCK *****
# Source last modified: $Id: linestrip.py,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
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
## $Id: linestrip.py,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
## out-of-date stripping program to remove private portions
## of header files
import sys, regex, getopt, os


#
# Classes
#

class Parameters:
    debug_mode          = 0
    start_pattern       = ''
    end_pattern         = ''

parameters = Parameters()

class MatchRangeFinder:
    match_ranges                = []
    debug_mode                  = 0

    def find_ranges(self, lines, start_pattern, end_pattern):
        start_pattern_expression        = regex.compile(start_pattern)
        end_pattern_expression          = regex.compile(end_pattern)
        current_line_num                = 0
        start_pattern_match_line        = -1
        state                           = 'looking for start pattern'
        for current_line in lines:
            if (state == 'looking for start pattern'):
                if (start_pattern_expression.match(current_line) != -1):
                    if self.debug_mode:
                            sys.stderr.write('*** start match found at line ' \
                                             '%d\n' % current_line_num)
                    state = 'looking for end pattern'
                    start_pattern_match_line = current_line_num
            if (state == 'looking for end pattern'):
                if (end_pattern_expression.match(current_line) != -1):
                    if self.debug_mode:
                            sys.stderr.write('*** end match found at line ' \
                                             '%d\n' % current_line_num) 
                    state = 'looking for start pattern'
                    self.match_ranges.append((start_pattern_match_line, 
                                             current_line_num))
            current_line_num = current_line_num + 1

def strip_lines(lines, start_pattern, end_pattern):
    range_finder = MatchRangeFinder()

    #
    # Find Ranges
    #
    range_finder.debug_mode = parameters.debug_mode
    range_finder.find_ranges(lines, parameters.start_pattern, 
                             parameters.end_pattern)

    removal_groups = range_finder.match_ranges
    line_offset = 0
    for removal_group in removal_groups:
            lines = lines[:removal_group[0]-line_offset] + \
                    lines[removal_group[1]-line_offset+1:]
            line_offset = line_offset + removal_group[1] - removal_group[0] + 1

    return lines

def strip_file(file_name, start_pattern, end_pattern):
    file = open(parameters.file_name, 'r')
    lines = file.readlines()
    lines = strip_lines(lines, start_pattern, end_pattern)
    file.close()
    os.remove(parameters.file_name)
    file = open(parameters.file_name, 'w')
    file.writelines(lines)
    file.close()

if __name__ == '__main__':
    #
    # Parse Options
    #
    argv = sys.argv
    option_list, argv = getopt.getopt(argv[1:], 'df:')

    for current_option in option_list:
            if (current_option[0] == '-d'):
                    parameters.debug_mode = 1
            if (current_option[0] == '-f'):
                    parameters.file_name = current_option[1]

    parameters.start_pattern = argv[0]
    parameters.end_pattern   = argv[1]

    if parameters.file_name != '':
        strip_file(parameters.file_name, parameters.start_pattern,
                   parameters.end_pattern)
    else:
        lines = sys.stdin.readlines()
        lines = strip_lines(lines, parameters.start_pattern, 
                            parameters.end_pattern)
        sys.stdout.writelines(lines)
