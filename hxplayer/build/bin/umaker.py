#!/usr/bin/env python
# 
# ***** BEGIN LICENSE BLOCK *****
# Source last modified: $Id: umaker.py,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
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
## $Id: umaker.py,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
## UMAKER -- Find all Umakefil(s) recursivly, run umake on them, and 
##           form a list of modified makefiles for inspection before
##           committing.
import os, sys, string, getopt, stat, re


# really commit?
REALLY_COMMIT = 0

# modify this list to add/remove platforms
PLATFORMS = [
    'linux-2.0-libc6-i386-gcc2.95',
    'linux-2.0-libc6-i386',
    'freebsd-2.2-i386',
    'freebsd-3.0-i386',
    'sunos-5.6-sparc',
    'sunos-5.7-sparc',
]

# output file
COMMIT_FILENAME = 'commit.txt'

# commit message
COMMIT_MESSAGE = 'committing umake-generated makefiles for SDK samples'


def get_target_list(path_list):
    origional_directory_list = []
    directory_list = []
    file_list = []

    # get all directories in the path list
    for path in path_list:
        if os.path.isdir(path):
            origional_directory_list.append(path)
            directory_list = directory_list + get_directories(path)
        else:
            if os.path.isfile(path):
                file_list.append(path)

    # get all files in given paths 
    for directory in directory_list:
        try:
            path_list = os.listdir(directory)
        except:
            continue
        
        for path in path_list:
            path = os.path.join(directory, path)
            if os.path.isfile(path):
                file_list.append(path)

    # decompose origional paths so the archive system
    # can make directory records for them that will set the
    # correct access/modification dates and permissions
    for directory in origional_directory_list:
        while 1:
            base, junk = os.path.split(directory)
            if base == '' or \
               base == os.curdir or \
               base == os.pardir or \
               base == os.sep:
                break

            directory_list.append(base)
            directory = base

    return file_list, directory_list


def get_directories(directory):
    directory_list = [directory]

    try:
        path_list = os.listdir(directory)
    except:
        return directory_list

    for path in path_list:
        path = os.path.join(directory, path)
        
        if os.path.isdir(path) and not os.path.islink(path):
            directory_list = directory_list + get_directories(path)

    return directory_list


def run_umake(umake_path, platform_list):
    old_dir = os.getcwd()
    (path, umakefil) = os.path.split(umake_path)
    if path != '':
        os.chdir(path)

    makefile_list = []
    for platform in platform_list:
        makefile = '%s.mak' % (platform)

        os.environ['SYSTEM_ID'] = platform
        
        cmd = 'umake -t release -m %s' % (makefile)
        status = os.system(cmd)
        
        if not os.path.isfile(makefile):
            print 'ERROR: %s not generated in subdir %s' % (makefile, path)
            
        makefile = os.path.join(path, makefile)
        print 'generated %s' % (makefile)
        makefile_list.append(makefile)
            
    os.chdir(old_dir)
    return makefile_list


def generate_makefiles(platforms):
     file_list, directory_list = get_target_list(os.listdir(os.curdir))

     # form a list of all umakefil(s)
     umake_file_list = []
     for file in file_list:
         if string.lower(os.path.basename(file)) == 'umakefil':
             umake_file_list.append(file)

     # generate Makefiles
     commit_list = []
     for umake_file in umake_file_list:
         commit_list = commit_list + run_umake(umake_file, platforms)

     # write a file with the filenames of all generated
     # makefiles for inspection
     print 'writing %s' % (COMMIT_FILENAME)
     fil = open(COMMIT_FILENAME, 'w')
     for commit_file in commit_list:
         fil.write('%s\n' % (commit_file))


def commit():
    commit_file_list = []
    
    # retrieve files out of commit file list
    for commit_file in open(COMMIT_FILENAME, 'r').readlines():
        commit_file_list.append(string.strip(commit_file))

    # form a list subset of commit_file_list which
    # we need to do a 'cvs add' on
    add_file_list = []
    for commit_file in commit_file_list:
        print 'checking %s' % (commit_file)
        
        cmd = '{ cvs status %s; } 2>&1' % (commit_file)
        
        # get ouput text and status from the cvs status
        # command to decide if we need to preform a cvs add
        cmd_fil = os.popen(cmd, 'r')
        text = cmd_fil.read()
        status = cmd_fil.close()

        # use the output to detect if we need to run
        # cvs add on this file before committing
        pattern = '.*File:\s+%s\s+Status:\s+Unknown.*' % (
            re.escape(os.path.basename(commit_file)))
        if re.match(pattern, text, re.M|re.S):
            print 'adding %s to cvs add list' % (commit_file)
            add_file_list.append(commit_file)

    # run cvs add & commit
    for commit_file in commit_file_list:

        # cvs add ...
        if add_file_list.count(commit_file):
            print 'adding %s' % (commit_file)
            
            cmd = 'cvs add %s' % (commit_file)

            if REALLY_COMMIT:
                cmd_fil = os.popen(cmd, 'r')
                text = cmd_fil.read()
                status = cmd_fil.close()

                if status:
                    print 'ERROR: command %s' % (cmd)
                    print text
                    sys.exit(1)
            else:
                print 'CMD: %s' % (cmd)

        # cvs commit ...
        print 'committing %s' % (commit_file)

        cmd = 'cvs commit -m \'%s\' %s' % (COMMIT_MESSAGE, commit_file)
        if REALLY_COMMIT:
            cmd_fil = os.popen(cmd, 'r')
            text = cmd_fil.read()
            status = cmd_fil.close()

            if status:
                print 'ERROR: command %s' % (cmd)
                print text
                sys.exit(1)
        else:
            print 'CMD: %s' % (cmd)


def usage():
    print 'USAGE: %s OPTION [PLATFORM list]' % (os.path.basename(sys.argv[0]))
    print '^^^^^^'
    print '   Recursivly generate makefiles (<platform-name>.mak) for '
    print '   each Umakefil found. Generate a %s file containing' % (COMMIT_FILENAME)
    print '   the relative path of each makefile generated so it can be'
    print '   inspected and modified before committing.'
    print 'OPTION'
    print '^^^^^^'
    print '   generate\tgenerate makefiles and %s commit file' % (COMMIT_FILENAME)
    print '   commit\tadd/commit files in %s' % (COMMIT_FILENAME)
    print 'PLATFORM list'
    print '^^^^^^^^^^^^^'
    print '   It consists of a list of platform names seperated by SPACES'
    for platform in PLATFORMS:
        print '\t%s' % (platform)
    print 'Example usage'
    print '^^^^^^^^^^^^^'
    print '   %s generate -- generate makefiles for all supported platforms' % (os.path.basename(sys.argv[0]))
    print '   %s generate linux2 freebsd3 -- generate for linux2 and freebsd3' % (os.path.basename(sys.argv[0]))
    print ''


# MAIN
if __name__ == '__main__':
    option_list = ['generate', 'commit']
    
    if len(sys.argv) < 2 or not option_list.count(sys.argv[1]):
        usage()
        sys.exit(1)
    elif len(sys.argv) > 2:
        all_platforms = 0
        platforms = []
        for platform in sys.argv[2:]:
            platforms.append(platform)
    else: 
        all_platforms = 1

    if sys.argv[1] == 'generate':
        if all_platforms == 1:
            generate_makefiles(PLATFORMS)
        else:
            generate_makefiles(platforms)
    elif sys.argv[1] == 'commit':
        commit()

    sys.exit(0)
