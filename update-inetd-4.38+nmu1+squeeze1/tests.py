#!/usr/bin/python

# basic command-line testing of update-inetd(8)
# Copyright (C) 2009 Serafeim Zanikolas <serzan@hellug.gr>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


from commands import getstatusoutput
from tempfile import mkstemp
import unittest
import os
import sys
import shutil
import posix

quiet = True
disabled_prefix = "#<off># "

# We expect this string to appear in update-inetd's stdout (in verbose mode)
# every time it starts, restarts, or sends SIGHUP to (x)inetd.
inetd_wakeup_string = "About to "

GREP_NO_MATCH_EXIT_STATUS = 256 # 1 + 255

inetd_conf =\
"""
# /etc/inetd.conf:  see inetd(8) for further informations.
#
# Internet superserver configuration database
#
#
# Lines starting with "#:LABEL:" or "#<off>#" should not
# be changed unless you know what you are doing!
#
# If you want to disable an entry so it isn't touched during
# package updates just comment it out with a single '#' character.
#
# Packages should modify this file by using update-inetd(8)
#
# <service_name> <sock_type> <proto> <flags> <user> <server_path> <args>
#
#:INTERNAL: Internal services
#discard		stream	tcp	nowait	root	internal
#discard		dgram	udp	wait	root	internal
%sdaytime		stream	tcp	nowait	root	internal
time		stream	tcp	nowait	root	internal
#time2		stream	tcp	nowait	root	internal

#:STANDARD: These are standard services.

#:BSD: Shell, login, exec and talk are BSD protocols.

#:MAIL: Mail, news and uucp services.

#:INFO: Info services

#:BOOT: TFTP service is provided primarily for booting.  Most sites
#       run this only on machines acting as "boot servers."

#:RPC: RPC based services

#:HAM-RADIO: amateur-radio services

#:OTHER: Other services
""" % disabled_prefix

def run(cmd, ok_exit_status=0):
    if not quiet:
        print 'running "%s"' % cmd
    status, output = getstatusoutput(cmd)
    if status != ok_exit_status:
        raise AssertionError(("the command \"%s\" failed with exit status %d "
            + "\nand printed this output:\n\"%s\"") % (cmd, status, output))
    return output

class TempFileManager(object):
    files = []
    def getTempFilename(prefix=""):
        _, filename = mkstemp(prefix)
        TempFileManager.files.append(filename)
        return filename
    def cleanup():
        sticky_files = []
        for f in TempFileManager.files:
            if os.path.exists(f):
                try:
                    os.remove(f)
                except IOError, e:
                    sys.stderr.write(("failed to remove temporary file " +
                                      "\"%s\": %s") % (f, e))
                    sticky_files.append(f)
        TempFileManager.files = sticky_files
    getTempFilename = staticmethod(getTempFilename)
    cleanup = staticmethod(cleanup)

orig_conffile = TempFileManager.getTempFilename(".orig")
conffile = TempFileManager.getTempFilename(".modified")

cmdline = "./update-inetd --file %s --verbose" % conffile

tmp_fc = TempFileManager.getTempFilename()
def fcomparator(f1, f2):
    run("sort %s >%s; mv %s %s" % (f1, tmp_fc, tmp_fc, f1))
    run("sort %s >%s; mv %s %s" % (f2, tmp_fc, tmp_fc, f2))
    comm_cmd = "comm -3 --nocheck-order %s %s" % (f1, f2)
    return run(comm_cmd)

class FileStat(object):
    """encapsulates a subset of posix stat values"""
    def __init__(self, filename):
        stat = posix.stat_result(os.stat(filename))
        self.ctime = stat.st_ctime
        self.mtime = stat.st_mtime
        self.size = stat.st_size
    def __str__(self):
        return "%s %s %s" % (self.ctime, self.mtime, self.size)

class UpdateInetdTest(unittest.TestCase):
    """
    Try 2 test cases per mode: one that is actually effective (eg, adds,
    modifies or removes a line) and one that isn't (because the matching entry
    is commented out with '#'). Test success by matching expected messages in
    verbose mode, and patterns in the supplied inetd.conf.
    """
    def setUp(self):
        try:
            f = open(conffile, "w")
            f.write(inetd_conf)
            f.close()
            shutil.copy(conffile, orig_conffile)
        except IOError:
            sys.stderr.write("failed to create tempfile %s\n" % conffile)
            sys.exit(1)
    def assertOutputMatches(self, string, output, n=1):
        """the given string must appear n times in output"""
        nmatches = sum([1 for line in output.split("\n") if string in line])
        if nmatches != n:
            raise AssertionError(("Expected string \"%s\"\n to appear %s " +
                    "time(s) in update-inet's output:\n\"%s\"\n but it " +
                    "appears %d time(s) instead") % (string, n, output,
                                                      nmatches))
    def assertConffileMatches(self, pattern, n=1, ok_run_status=0):
        """the given pattern must appear exactly n times in the conffile"""
        assert os.path.exists(conffile)
        output = run("grep -c '%s' %s" % (pattern, conffile), ok_run_status)
        self.assertEqual(output, str(n))
    def assertConffileDiffs(self, nlines_diff):
        """orig and modified conffiles must differ in exactly nlines"""
        comm_output = fcomparator(orig_conffile, conffile)
        actual_nlines_diff = len(comm_output.split("\n"))
        if nlines_diff == 0 and comm_output == "":
            return
        if actual_nlines_diff != nlines_diff:
            raise AssertionError(("original and modified config files " +
                "differ in %d lines instead of %d:\n" +
                "\n\"%s\"") % (actual_nlines_diff, nlines_diff,
                               comm_output))
    def assertNoTempFile(self, output):
        """test that no stale file is left behind"""
        # output should have a line: Using tempfile $new_inetdcf
        tmpfilename = ""
        try:
            tmpfilename = [l.split()[2] for l in output.split("\n")
                           if l.startswith("Using tempfile")][0]
        except Exception:
            pass
        if tmpfilename == "":
            raise AssertionError(("failed to extract temp filename from output; " +
                    "here's the output:\n%s") % output)
        if os.path.exists(tmpfilename):
            raise AssertionError(("stale temp file \"%s\" left behind; " +
                "here's the output:\n%s") % (tmpfilename, output))
    def update_inetd(self, mode, srv, other_opts=""):
        return run("%s --%s %s %s" % (cmdline, mode, srv, other_opts))
    def testEffectiveEnable(self):
        srv = "daytime"
        output = self.update_inetd("enable", srv)
        self.assertOutputMatches("Processing service `%s' ... enabled" % srv, output)
        self.assertOutputMatches("Number of service entries enabled: 1", output)
        self.assertOutputMatches(inetd_wakeup_string, output)
        self.assertConffileMatches("^%s\t" % srv)
        self.assertConffileDiffs(2)
        self.assertNoTempFile(output)
    def testIneffectiveEnable(self):
        srv = "time2"
        conffile_stat_before = FileStat(conffile)
        output = self.update_inetd("enable", srv)
        conffile_stat_after = FileStat(conffile)
        self.assertEqual(str(conffile_stat_before), str(conffile_stat_after))
        self.assertOutputMatches("No service entries were enabled", output)
        self.assertOutputMatches(inetd_wakeup_string, output, 0)
        self.assertConffileMatches("^#%s\t" % srv)
        self.assertConffileDiffs(0)
        self.assertNoTempFile(output)
    def testEffectiveDisable(self):
        srv = "time"
        output = self.update_inetd("disable", srv)
        self.assertOutputMatches("Processing service `%s' ... disabled" % srv, output)
        self.assertOutputMatches("Number of service entries disabled: 1", output)
        self.assertOutputMatches(inetd_wakeup_string, output)
        self.assertConffileMatches("^%s%s\t" % (disabled_prefix, srv))
        self.assertConffileDiffs(2)
        self.assertNoTempFile(output)
    def testIneffectiveDisable(self):
        srv = "time2"
        conffile_stat_before = FileStat(conffile)
        output = self.update_inetd("disable", srv)
        conffile_stat_after = FileStat(conffile)
        self.assertEqual(str(conffile_stat_before), str(conffile_stat_after))
        self.assertOutputMatches("No service entries were disabled", output)
        self.assertOutputMatches(inetd_wakeup_string, output, 0)
        self.assertConffileMatches("^#%s\t" % srv)
        self.assertConffileDiffs(0)
        self.assertNoTempFile(output)
    def testEffectiveAdd(self):
        srv = "pop-3"
        srv_entry = ("%s\t\tstream\ttcp\tnowait\troot\t/usr/sbin/tcpd\t" +
                     "/usr/sbin/in.pop3d") % srv
        output = self.update_inetd("add", "'%s'" % srv_entry)
        self.assertOutputMatches("Processing service `%s' ... added" % srv,
                                 output)
        self.assertOutputMatches("New service(s) added", output)
        self.assertOutputMatches(inetd_wakeup_string, output)
        self.assertConffileMatches("^%s\t" % srv)
        self.assertConffileDiffs(1)
        self.assertNoTempFile(output)
        self.assertOutputMatches("Number of currently enabled services: 1", output);
    def testIneffectiveAdd(self):
        srv = "time2"
        srv_entry = ("%s\t\tstream\ttcp\tnowait\troot\t/usr/sbin/tcpd\t" +
                     "/usr/sbin/in.pop3d") % srv
        conffile_stat_before = FileStat(conffile)
        output = self.update_inetd("add", "'%s'" % srv_entry)
        conffile_stat_after = FileStat(conffile)
        self.assertEqual(str(conffile_stat_before), str(conffile_stat_after))
        self.assertOutputMatches("Processing service `%s' ... not enabled"
                                 % srv, output)
        self.assertOutputMatches("No service(s) added", output)
        self.assertOutputMatches(inetd_wakeup_string, output, 0)
        self.assertConffileMatches("^#%s\t" % srv)
        self.assertConffileDiffs(1)
        #self.assertNoTempFile(output) # doesn't create a temp file
    def testEffectiveRemove(self):
        srv = "time"
        output = self.update_inetd("remove", "'%s'" % srv)
        self.assertOutputMatches("Removing line: `%s\t" % srv, output)
        self.assertOutputMatches("Number of service entries removed: 1", output)
        self.assertOutputMatches(inetd_wakeup_string, output)
        self.assertConffileMatches("^%s\t" % srv, 0, GREP_NO_MATCH_EXIT_STATUS)
        self.assertConffileDiffs(1)
        self.assertNoTempFile(output)
    def testIneffectiveRemove(self):
        srv = "time2"
        conffile_stat_before = FileStat(conffile)
        output = self.update_inetd("remove", "'%s'" % srv)
        conffile_stat_after = FileStat(conffile)
        self.assertEqual(str(conffile_stat_before), str(conffile_stat_after))
        self.assertOutputMatches("No service entries were removed", output)
        self.assertOutputMatches(inetd_wakeup_string, output, 0)
        self.assertConffileDiffs(0)
        self.assertNoTempFile(output)
    def testAddDisableEnableRemove(self):
        srv = "pop-3"
        # add
        srv_entry = ("%s\t\tstream\ttcp\tnowait\troot\t/usr/sbin/tcpd\t" +
                     "/usr/sbin/in.pop3d") % srv
        output = self.update_inetd("add", "'%s'" % srv_entry)
        self.assertOutputMatches("Processing service `%s' ... added" % srv,
                                 output)
        self.assertOutputMatches("New service(s) added", output)
        self.assertOutputMatches(inetd_wakeup_string, output)
        self.assertConffileMatches("^%s\t" % srv)
        self.assertConffileDiffs(1)
        self.assertOutputMatches("Number of currently enabled services: 1", output);
        # disable
        output = self.update_inetd("disable", srv)
        self.assertOutputMatches("Processing service `%s' ... disabled" % srv, output)
        self.assertOutputMatches("Number of service entries disabled: 1", output)
        self.assertOutputMatches(inetd_wakeup_string, output)
        self.assertConffileMatches("^%s%s\t" % (disabled_prefix, srv))
        # assertConffileDiff calls compares against the original conffile
        # (before the "add" operation)
        self.assertConffileDiffs(1)
        # enable
        output = self.update_inetd("enable", srv)
        self.assertOutputMatches("Processing service `%s' ... enabled" % srv, output)
        self.assertOutputMatches("Number of service entries enabled: 1", output)
        self.assertOutputMatches(inetd_wakeup_string, output)
        self.assertConffileMatches("^%s\t" % srv)
        self.assertConffileDiffs(1)
        # remove
        output = self.update_inetd("remove", "'%s'" % srv)
        self.assertOutputMatches("Removing line: `%s\t" % srv, output)
        self.assertOutputMatches("Number of service entries removed: 1", output)
        self.assertOutputMatches(inetd_wakeup_string, output)
        self.assertConffileMatches("^%s\t" % srv, 0, GREP_NO_MATCH_EXIT_STATUS)
        self.assertConffileDiffs(1)
        #
        self.assertNoTempFile(output)

if __name__ == "__main__":
    run("chmod 755 update-inetd")
    # set this env var so that DebianNet.pm won't actually run update_inetd-rc.d
    os.environ["UPDATE_INETD_FAKE_IT"] = "."
    os.environ["UPDATE_INETD_NOXINETD"] = "."
    # set current dir first in PERLLIB (last by default), so that we test
    # ./DebianNet.pm, instead of whatever might be installed system-wide
    default_perllib = run("perl -e 'print @INC'")
    os.environ["PERLLIB"] = ".:%s" % default_perllib.rstrip(":.")

    try:
        unittest.main()
    except Exception, e:
        print e
    finally:
        TempFileManager.cleanup()
