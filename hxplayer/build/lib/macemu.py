# 
# ***** BEGIN LICENSE BLOCK *****
# Source last modified: $Id: macemu.py,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
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

##
## This file allows a posix-style system to emulate macpython
## Only import this on darwin when running codewarrior builds
##
## Relies heavily on:
##   /Developer/Tools/SetFile
##   /Developer/Tools/GetFileInfo
##   /usr/bin/open
##   /usr/bin/osascript
##   /usr/bin/osacompile
##   StuffIt
##   Mactools (patched) (esp. the binhex command)
##

import string
import os
import sys

if os.environ.get("PYTHON_FULL_PATH_KLUGE","NO") != "YES":
    os.environ["PYTHON_FULL_PATH_KLUGE"]="YES"
    os.execvp(sys.executable, [ sys.executable ] + sys.argv )


print "Initiating MacPython emulation mode, expecting one import error."

import MacOS
import macfs
import macpath
import time
import posixpath
import types
import __builtin__


## Copied from archive.py
_mac_second_diff = 2082816000.0 


os.name='mac'
sys.platform='mac'

def __volume():
    return os.environ.get("BUILD_VOL","MacOS X") 

## Convert a MAC path to a posix path
def _path_mtop(path):
    path = string.replace(path, "\xb5","0xb5")
    path = string.replace(path, "\xc4","0xc4")

    if path == ":":
        return "."
    
    # print "_path_mtop: %s" % path
    tmp=string.split(path,":")

    if(macpath.isabs(path)):
        if tmp[0] == __volume():
            tmp[0]=""
            return string.join(tmp,"/")

        ## Kluge for mixed posix/mac path
        if string.count(tmp[0],"/"):
            return apply(os_path_join,tmp)

        if(_real_isdir("/Volumes/"+tmp[0])):
            return "/Volumes/"+string.join(tmp,"/")

        ## Another Kluge, path lacks volume name
        return "/" + string.join(tmp,"/")
    
    if len(tmp) == 1:
        return tmp

    up=1
    while up < len(tmp)-1 and tmp[up] == '':
        up = up + 1

    if up == 1:
        return "./" + string.join(tmp[up:],"/")

    return "../"*(up-1) + string.join(tmp[up:],"/")

def _is_mac_path(path):
    if ":" in path:
        return 1
    return 0

## Convert a mac path to a posix path, but only if it is
## (probably) a mac path
def _path_dtop(path):
    if type(path) != types.StringType:
        return path

    path = string.replace(path,"\\","/")
    if(_is_mac_path(path)):
        return _path_mtop(path)
    return path


_real_normpath=posixpath.normpath
_real_isabs=posixpath.isabs

## Convert a posix path to a mac path
def _path_ptom(path):
    if path == ".":
        return ":"
    orig = path
    path=_real_normpath(path)
    tmp=string.split(path,"/")
    
    if _real_isabs(path):
        if tmp[1] == "Volumes":
            path=string.join(tmp[2:],":")
        else:
            path=__volume() + string.join(tmp,":")
    else:
        path=":"
        p = 0
        while p < len(tmp) and tmp[p]=="..":
            path=path+":"
            p = p + 1
        path=path+string.join(tmp[p:],":")

    if orig[-1] == '/' and path[-1]!=':':
        path = path + ":"

    if orig[:2] == "./" and path[0]!=':':
        path = ":" + path

    return path

def _is_posix_path(path):
    if "/" in path:
        return 1
    if "\\" in path:
        return 1
    if path in [".", ".."]:
        return 1
    return 0

## DWIM to mac
def _path_dtom(path):
    if type(path) != types.StringType:
        return path
    
    if(_is_posix_path(path)):
        return _path_ptom(path)
    return path



##
## These functions try to batch update notifications for the Finder
##
to_update = {}
def queue_dir_update(filename):
    global to_update
    if string.count(filename,"..namedfork"):
        return
    filename = os.path.join(getcwd(), filename)
    filename = os.path.normpath(filename)
    filename = _path_dtom(filename)
        
    if filename[-1:] != ':':
        filename = filename + ':'

    to_update[filename] = 1
    filename = macpath.dirname(macpath.dirname(filename))+":"
    to_update[filename] = 1
    filename = macpath.dirname(macpath.dirname(filename))+":"
    to_update[filename] = 1

def queue_file_update(filename):
    global to_update
    filename = os.path.join(getcwd(), filename)
    filename = os.path.normpath(filename)
    filename = _path_dtom(filename)
    to_update[filename] = 1
    filename = macpath.dirname(filename)+":"
    to_update[filename] = 1
    filename = macpath.dirname(macpath.dirname(filename))+":"
    to_update[filename] = 1

def update_finder():
    global to_update
    if len(to_update):
        script = ['tell Application "Finder"' ]
        tmp = []
        for path in to_update.keys():
            tmp.append( (len(path), path) )
        tmp.sort()
        for path in tmp:
            script.append('  update "%s"' % path[1])
        script.append('end tell')
        to_update = {}
        tmpfilename = "atmp-%d-%f.ascript" % (os.getpid(), time.time())
        s = string.join(script,"\n")
        # print s
        open(tmpfilename,"w").write(s)
        err=os.popen("/usr/bin/osascript -s o %s" % tmpfilename).read()
        if len(err):
            print "Warning, update failed: %s" % err
            print "script =\n%s" % s
            print "-------------------------------------------"

        os.remove(tmpfilename)
        

## Patch os.*
__old_getcwd = os.getcwd

def getcwd():
    return _path_ptom(__old_getcwd() + "/")

os.getcwd = getcwd

## This is a very very ugly kluge to avoid
## Using the Mac-specific SOURCE_ROOT variable
if os.environ.get("SOURCE_ROOT","") == "":
    cmd=os.path.split(sys.argv[0])[1]
    if cmd == "umake":
        os.environ["SOURCE_ROOT"] = _path_ptom(os.path.dirname(__old_getcwd())+"/")
    if cmd == "build":
        os.environ["SOURCE_ROOT"] = getcwd()

_real_isdir = os.path.isdir
_real_join = os.path.join

def os_path_join(base, *args):
    na = [ _path_dtop(base) ]
    for arg in args:
        arg=_path_dtop(arg)
        while arg[:2] == "./":
            arg=arg[2:]
        na.append(arg)

    ret = apply(_real_join, na)
    # print " os.path.join %r => %r " % ( (base,)+args, ret)
    return ret

os.path.join=os_path_join


## Patch os.stat

__old_stat = os.stat

def os_stat(file):
    file = _path_dtop(file)
    s = list(__old_stat(file))
    s[7] = s[7] + _mac_second_diff
    s[8] = s[8] + _mac_second_diff
    s[9] = s[9] + _mac_second_diff
    s=tuple(s)
    try:
        s=os.stat_result(s)
    except:
        pass
    return s

os.stat = os_stat

## Patch os.utime

__old_utime = os.utime
def os_utime(file, times):
    file = _path_dtop(file)
    if type(times[0]) == types.FloatType:
        times = (int(times[0] - _mac_second_diff),
                 int(times[1] - _mac_second_diff) )
    return __old_utime(file, times)

os.utime = os_utime

## Generic wrappers

class WrapCall:
    def __init__(self,module,callname, filter_func):
        self.filter_func=filter_func
        self.module=module
        self.name=callname
        self.fun = module.__dict__[callname]
        module.__dict__[callname] = self.wrapper

    def wrapper(self,*args):
        #print " %s.%s %r " % ( self.module.__name__, self.name, args)
        #print " >>> %s.%s %r " % ( self.module.__name__, self.name, map(self.filter_func, args))
        ret = apply(self.fun, map(self.filter_func, args))
        # print " %s.%s %r  => %s " % ( self.module.__name__, self.name, args, ret)
        return ret

WrapCall(os,"mkdir",_path_dtop)
WrapCall(os,"rmdir",_path_dtop)
WrapCall(os,"chdir",_path_dtop)
WrapCall(os,"remove",_path_dtop)
WrapCall(os,"rename",_path_dtop)
WrapCall(os,"unlink",_path_dtop)
WrapCall(os,"listdir",_path_dtop)

#WrapCall(__builtin__,"open",_path_dtop)

#WrapCall(os.path,"join",_path_dtop)
WrapCall(os.path,"split",_path_dtop)
WrapCall(os.path,"normpath",_path_dtop)
WrapCall(os.path,"isfile",_path_dtop)
WrapCall(os.path,"isdir",_path_dtop)
WrapCall(os.path,"isabs",_path_dtop)

WrapCall(macpath,"join",_path_dtom)
WrapCall(macpath,"split",_path_dtom)
WrapCall(macpath,"normpath",_path_dtom)


def openrf(file, mode):
    file = _path_dtop(file) + "/..namedfork/rsrc"
    mode = string.replace(mode,"*","")
    return open(file, mode)
    
MacOS.openrf = openrf

try:
    real_fsspec = macfs.FSSpec
except:
    pass

class FSSpec:
    def __init__(self, p):
        if type(p) == types.TupleType:
            global real_fsspec
            p = real_fsspec(p).as_pathname()
            
        self.path = os.path.normpath(os.path.join(os.getcwd(),p))

    def as_pathname(self):
        return self.path

    def SetCreatorType(self, crea, type):
        if os.environ.get("NO_SETFILE", "NO") == "YES":
            return
        cmd = "/Developer/Tools/Setfile"
        if crea != "\x00\x00\x00\x00":
            cmd = cmd + ' -c "%s"' % crea

        if type != "\x00\x00\x00\x00":
            cmd = cmd + ' -t "%s"' % type

        cmd = cmd + ' "%s"' % self.path

        try:
            err = os.popen(cmd).read()
        except:
            print "SetCreatorType%s ..." % repr( (self.path, crea, type) )
            raise
            
        if string.count(err, "ERROR"):
            print "SetCreatorType%s: %s" % ( repr( (self.path, crea, type) ), err )

    def GetCreatorType(self):
        o1 = os.popen("/Developer/Tools/GetFileInfo -c '%s'" % self.path).read()
        o2 = os.popen("/Developer/Tools/GetFileInfo -t '%s'" % self.path).read()
        o1 = string.split(o1,'"')
        o2 = string.split(o2,'"')
        if len(o1) < 2 or len(o2) < 2:
            raise "Failed to get creator type for file '%s' (File does not exist?)" % self.path
        return (o1[1], o2[1])

    def SetDates(self, ctime, mtime, backup):
        os.utime(self.path, (
            int(backup - _mac_second_diff),
            int(mtime - _mac_second_diff) ))

macfs.FSSpec = FSSpec

class OSAm:
    def __fix_posix_paths(self, data):
        # print "Magic glue in: %r" % (data,)
        data=string.replace(data,"\r","\n")

        tmpdata=string.split(data,'"/')
        x=1
        while x < len(tmpdata):
            tmpdata[x] = "/"+tmpdata[x]
            try:
                eq = string.index(tmpdata[x], '"')
                path=tmpdata[x][:eq]
                if string.split(path,"/")[1] != "Developer":
                    tmpdata[x] = _path_ptom(path)+tmpdata[x][eq:]
            except:
                pass
            x = x + 1
        data=string.join(tmpdata, '"')

        #tmpdata=string.split(data,',../')
        #x=1
        #while x < len(tmpdata):
        #    try:
        #        eq = string.index(tmpdata[x], '"')
        #        try:
        #            eq2 = string.index(tmpdata[x], ',')
        #            if eq2 < eq:
        #                eq = eq2
        #        except:
        #            pass
        #
        #        tmpdata[x] = _path_ptom("../"+tmpdata[x][:eq])+tmpdata[x][eq:]
        #    except:
        #        pass
        #    x = x + 1
        #data=string.join(tmpdata, ',')
        # print "Magic glue out: %r" % (data,)
        return data
        
    def CompileAndExecute(self, text):
        import time
        import os
        update_finder()
        tmpfilename = "atmp-%d-%f.ascript" % (os.getpid(), time.time())
        text=self.__fix_posix_paths(text)
        # print "COMPILEANDEXECUTE: %r" % (text,)
        # print "COMPILEANDEXECUTE:\n%s\n\n" % (text,)
        open(tmpfilename,"w").write(text)
        err = os.popen("/usr/bin/osascript -s o %s" % tmpfilename).read()
        #print "COMPILEANDEXECUTE: %r" % (err,)
        os.remove(tmpfilename)
        return err
        
    def CompileAndSave(self, text, path):
        path=_path_dtop(path)
        text=self.__fix_posix_paths(text)
        # print "COMPILEANDSAVE: %r" % (text,)
        open(path+".ascript","w").write(text)
        err = os.popen("/usr/bin/osacompile -o %s %s.ascript" % (path,path)).read()
        # print "COMPILEANDSAVE: %r" % (err,)
        return err
        
sys.modules["OSAm"]=OSAm()

import socket

## NOTA BENE
## DropStuff must be configured to generate both sit and hqx files,
## and it should be configured to drop the .sit extention when
## making hqx files!   /Hubbe

class StuffIt:

    def run(self, app, file, newfile):
        cmd='/usr/bin/open -a "%s" "%s"' % (app, _path_dtop(file));
        retry = 1
        while retry < 100:
            try:
                os.remove(newfile)
            except OSError:
                pass
            
            print "Attempt %d: %s" % (retry, cmd)
            os.popen(cmd).read()
            time.sleep(1)

            w=0
            while 1:
                num=string.count(os.popen("ps -auxwwwww").read(),app)
                if not num or w > 900:
                    break

                time.sleep(2)
                w = w + 2

                if num > 1:
                    os.popen("""/usr/bin/osascript -e 'tell application "%s" to quit'"""  % app).read()
                    
                
            retry = retry + 1
            s=os.stat(newfile)

            if s and s[6]:
                return

            print "DropStuff failed, retrying...."


    def RunStuffitDeluxe(self, op, file, to, script):
        stuffit = os.environ.get("STUFFIT_DELUXE")
        if stuffit:
            print "%s:ing %s into %s using %s" %(op, file, to, stuffit)
            m_to= _path_dtom(to)
            m_file = _path_dtom(file)

            script = [
                'tell application %s' % stuffit,  ## Create new stuffit file
                '   activate',
                '   with timeout of 9999 seconds',
                ] + script + [
                '   end timeout',
                '   quit',
                'end tell' ]

            if os.path.exists(to):
                script =  [
                    'tell application %s' % stuffit, ## quit stuffit deluxe
                    '   quit',
                    'end tell',
                    'tell application "Finder"',
                    '   delete file "%s"' % m_to,
                    'end tell',
                    ] + script

            script.append('return "noerror"')
            script = string.join(script, "\n")
            script = string.replace(script,"$file$",m_file)
            script = string.replace(script,"$to$",m_to)
            ret=OSAm().CompileAndExecute(script)

            if not string.count(ret,"noerror"):
                raise "Failed to %s %s => %s\nApplescript Error: %s" % (op, file, to, ret)
            return 1

        return None
        
    def Stuff(self, file, to = None):
        print "STUFF STUFF STUFF"

        cmd="stuff -l 16"
        if to:
            cmd = "%s -n \"%s\"" %( cmd, _path_dtop(to))
            
        if not os.system("%s \"%s\"" % (cmd, _path_dtop(file))):
            return
            
        if not os.system("/usr/local/bin/%s \"%s\"" % (cmd, _path_dtop(file))):
            return
            
        if self.RunStuffitDeluxe("Stuff",file, to, [
            '     make new archive with properties {location:file "$to$"}',
            '     stuff {alias "$file$"} into archive "%s" compression level maximum ' % os.path.basename(to) ]):
            return
        
        self.run("DropStuff", file, file + ".sit")
        if to and to != file + ".sit":
            os.rename(file+".sit", to)

    def HQXEncode(self, file, to = None):
        if not to:
            to = file+".hqx"

        if not os.system("stuff -f hqx -n \"%s\" \"%s\"" % (_path_dtop(to), _path_dtop(file))):
            return
            
        if not os.system("/usr/local/bin/stuff -f hqx -n \"%s\" \"%s\"" % (_path_dtop(to), _path_dtop(file))):
            return
            

        if self.RunStuffitDeluxe("HQX", file, to, [
            '     encode {alias "$file$"} with binhex into file "$to$"']):
            return
            
        # This must be the special modified MacOS X binhex
        if not os.system('binhex -d "%s" >"%s"' % (_path_dtop(file), _path_dtop(to))):
            return
        
        sitfile=file+".sit"
        hqxfile=file+".hqx"
        self.Stuff(file, sitfile);
        s=os.stat(hqxfile)
        if not (s and s[6]):
            self.run("DropStuff", sitfile, hqxfile)
        if to and to != hqxfile:
            os.rename(hqxfile, to)

    def MacBinEncode(self,file, to = None):
        if not to:
            to = file+".bin"

        if not os.system("stuff -f bin -n \"%s\" \"%s\"" % (_path_dtop(to), _path_dtop(file))):
            return
            
        if not os.system("/usr/local/bin/stuff -f bin -n \"%s\" \"%s\"" % (_path_dtop(to), _path_dtop(file))):
            return
            

        if self.RunStuffitDeluxe("macbinencode", file, to, [
            '     encode {alias "$file$"} with macbinary into file "$to$"']):
            return
            
        # This must be the special modified MacOS X binhex
        if not os.system('macstream -d "%s" >"%s"' % (_path_dtop(file), _path_dtop(to))):
            return

    def Expand(self, *args):
        ##FIXME: run /usr/local/bin/unstuff here
        
        ### This does not work, does it?
        self.run("StuffIt Expander", args, args)


sys.modules["StuffIt"]=StuffIt()


## open()
real_open=open

def my_open(file, mode = "r"):
    file = _path_dtop(file);

    ## Tell finder to update it's cache before running
    ## Next applescript...
    if string.count(mode,"w") or string.count(mode,"c"):
        queue_dir_update(os.path.dirname(file))
    return real_open(file,mode)

__builtin__.open=my_open


## Trap this to make sure updates are done after cvs has run
real_fork = os.fork
def os_fork():
    queue_dir_update(getcwd())
    return real_fork()

os.environ["FAKE_MAC"]="YES"

class myMACFS:
    kOnAppropriateDisk=-32767
    kPreferencesFolderType='pref'

sys.modules["MACFS"]=myMACFS()

