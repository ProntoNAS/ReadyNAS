# ----------------------------------------------------------------------
#    Copyright (C) 2013 Kshitij Gupta <kgupta8592@gmail.com>
#    Copyright (C) 2014-2015 Christian Boltz <apparmor@cboltz.de>
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of version 2 of the GNU General Public
#    License as published by the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
# ----------------------------------------------------------------------
# No old version logs, only 2.6 + supported
from __future__ import division, with_statement
import inspect
import os
import re
import shutil
import subprocess
import sys
import time
import traceback
import atexit
import tempfile

import apparmor.config
import apparmor.logparser
import apparmor.severity

from copy import deepcopy

from apparmor.common import (AppArmorException, AppArmorBug, open_file_read, valid_path, hasher,
                             open_file_write, convert_regexp, DebugLogger)

import apparmor.ui as aaui

from apparmor.aamode import (str_to_mode, mode_to_str, contains, split_mode,
                             mode_to_str_user, mode_contains, AA_OTHER,
                             flatten_mode, owner_flatten_mode)

from apparmor.regex import (RE_PROFILE_START, RE_PROFILE_END, RE_PROFILE_LINK,
                            RE_PROFILE_ALIAS,
                            RE_PROFILE_BOOLEAN, RE_PROFILE_VARIABLE, RE_PROFILE_CONDITIONAL,
                            RE_PROFILE_CONDITIONAL_VARIABLE, RE_PROFILE_CONDITIONAL_BOOLEAN,
                            RE_PROFILE_BARE_FILE_ENTRY, RE_PROFILE_PATH_ENTRY,
                            RE_PROFILE_CHANGE_HAT,
                            RE_PROFILE_HAT_DEF, RE_PROFILE_DBUS, RE_PROFILE_MOUNT,
                            RE_PROFILE_PIVOT_ROOT,
                            RE_PROFILE_UNIX, RE_RULE_HAS_COMMA, RE_HAS_COMMENT_SPLIT,
                            strip_quotes, parse_profile_start_line, re_match_include )

import apparmor.rules as aarules

from apparmor.rule.capability import CapabilityRuleset, CapabilityRule
from apparmor.rule.change_profile import ChangeProfileRuleset, ChangeProfileRule
from apparmor.rule.network    import NetworkRuleset,    NetworkRule
from apparmor.rule.ptrace     import PtraceRuleset,    PtraceRule
from apparmor.rule.rlimit     import RlimitRuleset,    RlimitRule
from apparmor.rule.signal     import SignalRuleset,    SignalRule
from apparmor.rule import parse_modifiers, quote_if_needed

ruletypes = ['capability', 'change_profile', 'network', 'ptrace', 'rlimit', 'signal']

from apparmor.yasti import SendDataToYast, GetDataFromYast, shutdown_yast

# setup module translations
from apparmor.translations import init_translation
_ = init_translation()

# Setup logging incase of debugging is enabled
debug_logger = DebugLogger('aa')

CONFDIR = '/etc/apparmor'
running_under_genprof = False
unimplemented_warning = False

# The database for severity
sev_db = None
# The file to read log messages from
### Was our
logfile = None

cfg = None
repo_cfg = None

parser = None
profile_dir = None
extra_profile_dir = None
### end our
# To keep track of previously included profile fragments
include = dict()

existing_profiles = dict()

seen_events = 0  # was our
# To store the globs entered by users so they can be provided again
user_globs = []

# The key for representing bare "file," rules
ALL = '\0ALL'

## Variables used under logprof
### Were our
t = hasher()  # dict()
transitions = hasher()

aa = hasher()  # Profiles originally in sd, replace by aa
original_aa = hasher()
extras = hasher()  # Inactive profiles from extras
### end our
log = []
pid = dict()

seen = hasher()  # dir()
profile_changes = hasher()
prelog = hasher()
log_dict = hasher()  # dict()
changed = dict()
created = []
skip = hasher()
helpers = dict()  # Preserve this between passes # was our
### logprof ends

filelist = hasher()    # File level variables and stuff in config files

def on_exit():
    """Shutdowns the logger and records exit if debugging enabled"""
    debug_logger.debug('Exiting..')
    debug_logger.shutdown()

# Register the on_exit method with atexit
atexit.register(on_exit)

def check_for_LD_XXX(file):
    """Returns True if specified program contains references to LD_PRELOAD or
    LD_LIBRARY_PATH to give the Px/Ux code better suggestions"""
    if not os.path.isfile(file):
        return False
    size = os.stat(file).st_size
    # Limit to checking files under 100k for the sake of speed
    if size > 100000:
        return False
    with open(file, 'rb') as f_in:
        for line in f_in:
            if b'LD_PRELOAD' in line or b'LD_LIBRARY_PATH' in line:
                return True
    return False

def fatal_error(message):
    # Get the traceback to the message
    tb_stack = traceback.format_list(traceback.extract_stack())
    tb_stack = ''.join(tb_stack)
    # Add the traceback to message
    message = tb_stack + '\n\n' + message
    debug_logger.error(message)
    caller = inspect.stack()[1][3]

    # If caller is SendDataToYast or GetDatFromYast simply exit
    if caller == 'SendDataToYast' or caller == 'GetDatFromYast':
        sys.exit(1)

    # Else tell user what happened
    aaui.UI_Important(message)
    shutdown_yast()
    sys.exit(1)

def check_for_apparmor(filesystem='/proc/filesystems', mounts='/proc/mounts'):
    """Finds and returns the mountpoint for apparmor None otherwise"""
    support_securityfs = False
    aa_mountpoint = None
    if valid_path(filesystem):
        with open_file_read(filesystem) as f_in:
            for line in f_in:
                if 'securityfs' in line:
                    support_securityfs = True
                    break
    if valid_path(mounts) and support_securityfs:
        with open_file_read(mounts) as f_in:
            for line in f_in:
                split = line.split()
                if len(split) > 2 and split[2] == 'securityfs':
                    mountpoint = split[1] + '/apparmor'
                    # Check if apparmor is actually mounted there
                    # XXX valid_path() only checks the syntax, but not if the directory exists!
                    if valid_path(mountpoint) and valid_path(mountpoint + '/profiles'):
                        aa_mountpoint = mountpoint
                        break
    return aa_mountpoint

def which(file):
    """Returns the executable fullpath for the file, None otherwise"""
    if sys.version_info >= (3, 3):
        return shutil.which(file)
    env_dirs = os.getenv('PATH').split(':')
    for env_dir in env_dirs:
        env_path = env_dir + '/' + file
        # Test if the path is executable or not
        if os.access(env_path, os.X_OK):
            return env_path
    return None

def get_full_path(original_path):
    """Return the full path after resolving any symlinks"""
    path = original_path
    link_count = 0
    if not path.startswith('/'):
        path = os.getcwd() + '/' + path
    while os.path.islink(path):
        link_count += 1
        if link_count > 64:
            fatal_error(_("Followed too many links while resolving %s") % (original_path))
        direc, file = os.path.split(path)
        link = os.readlink(path)
        # If the link an absolute path
        if link.startswith('/'):
            path = link
        else:
            # Link is relative path
            path = direc + '/' + link
    return os.path.realpath(path)

def find_executable(bin_path):
    """Returns the full executable path for the given executable, None otherwise"""
    full_bin = None
    if os.path.exists(bin_path):
        full_bin = get_full_path(bin_path)
    else:
        if '/' not in bin_path:
            env_bin = which(bin_path)
            if env_bin:
                full_bin = get_full_path(env_bin)
    if full_bin and os.path.exists(full_bin):
        return full_bin
    return None

def get_profile_filename(profile):
    """Returns the full profile name"""
    if existing_profiles.get(profile, False):
        return existing_profiles[profile]
    elif profile.startswith('/'):
        # Remove leading /
        profile = profile[1:]
    else:
        profile = "profile_" + profile
    profile = profile.replace('/', '.')
    full_profilename = profile_dir + '/' + profile
    return full_profilename

def name_to_prof_filename(prof_filename):
    """Returns the profile"""
    if prof_filename.startswith(profile_dir):
        profile = prof_filename.split(profile_dir, 1)[1]
        return (prof_filename, profile)
    else:
        bin_path = find_executable(prof_filename)
        if bin_path:
            prof_filename = get_profile_filename(bin_path)
            if os.path.isfile(prof_filename):
                return (prof_filename, bin_path)

    return None, None

def complain(path):
    """Sets the profile to complain mode if it exists"""
    prof_filename, name = name_to_prof_filename(path)
    if not prof_filename:
        fatal_error(_("Can't find %s") % path)
    set_complain(prof_filename, name)

def enforce(path):
    """Sets the profile to enforce mode if it exists"""
    prof_filename, name = name_to_prof_filename(path)
    if not prof_filename:
        fatal_error(_("Can't find %s") % path)
    set_enforce(prof_filename, name)

def set_complain(filename, program):
    """Sets the profile to complain mode"""
    aaui.UI_Info(_('Setting %s to complain mode.') % (filename if program is None else program))
    # a force-complain symlink is more packaging-friendly, but breaks caching
    # create_symlink('force-complain', filename)
    delete_symlink('disable', filename)
    change_profile_flags(filename, program, 'complain', True)

def set_enforce(filename, program):
    """Sets the profile to enforce mode"""
    aaui.UI_Info(_('Setting %s to enforce mode.') % (filename if program is None else program))
    delete_symlink('force-complain', filename)
    delete_symlink('disable', filename)
    change_profile_flags(filename, program, 'complain', False)

def delete_symlink(subdir, filename):
    path = filename
    link = re.sub('^%s' % profile_dir, '%s/%s' % (profile_dir, subdir), path)
    if link != path and os.path.islink(link):
        os.remove(link)

def create_symlink(subdir, filename):
    path = filename
    bname = os.path.basename(filename)
    if not bname:
        raise AppArmorException(_('Unable to find basename for %s.') % filename)
    #print(filename)
    link = re.sub('^%s' % profile_dir, '%s/%s' % (profile_dir, subdir), path)
    #print(link)
    #link = link + '/%s'%bname
    #print(link)
    symlink_dir = os.path.dirname(link)
    if not os.path.exists(symlink_dir):
        # If the symlink directory does not exist create it
        os.makedirs(symlink_dir)

    if not os.path.exists(link):
        try:
            os.symlink(filename, link)
        except:
            raise AppArmorException(_('Could not create %(link)s symlink to %(file)s.') % { 'link': link, 'file': filename })

def head(file):
    """Returns the first/head line of the file"""
    first = ''
    if os.path.isfile(file):
        with open_file_read(file) as f_in:
            try:
                first = f_in.readline().rstrip()
            except UnicodeDecodeError:
                pass
            return first
    else:
        raise AppArmorException(_('Unable to read first line from %s: File Not Found') % file)

def get_output(params):
    '''Runs the program with the given args and returns the return code and stdout (as list of lines)'''
    try:
        # Get the output of the program
        output = subprocess.check_output(params)
        ret = 0
    except OSError as e:
        raise AppArmorException(_("Unable to fork: %(program)s\n\t%(error)s") % { 'program': params[0], 'error': str(e) })
    except subprocess.CalledProcessError as e:  # If exit code != 0
        output = e.output
        ret = e.returncode

    output = output.decode('utf-8').split('\n')

    # Remove the extra empty string caused due to \n if present
    if output[len(output) - 1] == '':
        output.pop()

    return (ret, output)

def get_reqs(file):
    """Returns a list of paths from ldd output"""
    pattern1 = re.compile('^\s*\S+ => (\/\S+)')
    pattern2 = re.compile('^\s*(\/\S+)')
    reqs = []

    ldd = conf.find_first_file(cfg['settings'].get('ldd')) or '/usr/bin/ldd'
    if not os.path.isfile(ldd) or not os.access(ldd, os.EX_OK):
        raise AppArmorException('Can\'t find ldd')

    ret, ldd_out = get_output([ldd, file])
    if ret == 0:
        for line in ldd_out:
            if 'not a dynamic executable' in line:
                break
            if 'cannot read header' in line:
                break
            if 'statically linked' in line:
                break
            match = pattern1.search(line)
            if match:
                reqs.append(match.groups()[0])
            else:
                match = pattern2.search(line)
                if match:
                    reqs.append(match.groups()[0])
    return reqs

def handle_binfmt(profile, path):
    """Modifies the profile to add the requirements"""
    reqs_processed = dict()
    reqs = get_reqs(path)
    while reqs:
        library = reqs.pop()
        library = get_full_path(library)  # resolve symlinks
        if not reqs_processed.get(library, False):
            if get_reqs(library):
                reqs += get_reqs(library)
            reqs_processed[library] = True
        combined_mode = match_prof_incs_to_path(profile, 'allow', library)
        if combined_mode:
            continue
        library = glob_common(library)
        if not library:
            continue
        profile['allow']['path'][library]['mode'] = profile['allow']['path'][library].get('mode', set()) | str_to_mode('mr')
        profile['allow']['path'][library]['audit'] |= profile['allow']['path'][library].get('audit', set())

def get_interpreter_and_abstraction(exec_target):
    '''Check if exec_target is a script.
       If a hashbang is found, check if we have an abstraction for it.

       Returns (interpreter_path, abstraction)
       - interpreter_path is none if exec_target is not a script or doesn't have a hashbang line
       - abstraction is None if no matching abstraction exists'''

    if not os.path.exists(exec_target):
        aaui.UI_Important(_('Execute target %s does not exist!') % exec_target)
        return None, None

    if not os.path.isfile(exec_target):
        aaui.UI_Important(_('Execute target %s is not a file!') % exec_target)
        return None, None

    hashbang = head(exec_target)
    if not hashbang.startswith('#!'):
        return None, None

    # get the interpreter (without parameters)
    interpreter = hashbang[2:].strip().split()[0]
    interpreter_path = get_full_path(interpreter)
    interpreter = re.sub('^(/usr)?/bin/', '', interpreter_path)

    if interpreter in ['bash', 'dash', 'sh']:
        abstraction = 'abstractions/bash'
    elif interpreter == 'perl':
        abstraction = 'abstractions/perl'
    elif re.search('^python([23]|[23]\.[0-9]+)?$', interpreter):
        abstraction = 'abstractions/python'
    elif re.search('^ruby([0-9]+(\.[0-9]+)*)?$', interpreter):
        abstraction = 'abstractions/ruby'
    else:
        abstraction = None

    return interpreter_path, abstraction

def get_inactive_profile(local_profile):
    if extras.get(local_profile, False):
        return {local_profile: extras[local_profile]}
    return dict()

def profile_storage(profilename, hat, calledby):
    # keys used in aa[profile][hat]:
    # a) rules (as dict): alias, include, lvar
    # b) rules (as hasher): allow, deny
    # c) one for each rule class
    # d) other: external, flags, name, profile, attachment, initial_comment, filename, info,
    #           profile_keyword, header_comment (these two are currently only set by set_profile_flags())

    # Note that this function doesn't explicitely init all those keys (yet).
    # It will be extended over time, with the final goal to get rid of hasher().

    profile = hasher()

    # profile['info'] isn't used anywhere, but can be helpful in debugging.
    profile['info'] = {'profile': profilename, 'hat': hat, 'calledby': calledby}

    profile['capability']       = CapabilityRuleset()
    profile['change_profile']   = ChangeProfileRuleset()
    profile['network']          = NetworkRuleset()
    profile['ptrace']           = PtraceRuleset()
    profile['rlimit']           = RlimitRuleset()
    profile['signal']           = SignalRuleset()

    profile['allow']['path'] = hasher()
    profile['allow']['dbus'] = list()
    profile['allow']['mount'] = list()
    profile['allow']['pivot_root'] = list()

    return profile

def create_new_profile(localfile, is_stub=False):
    local_profile = hasher()
    local_profile[localfile] = profile_storage('NEW', localfile, 'create_new_profile()')
    local_profile[localfile]['flags'] = 'complain'
    local_profile[localfile]['include']['abstractions/base'] = 1

    if os.path.exists(localfile) and os.path.isfile(localfile):
        interpreter_path, abstraction = get_interpreter_and_abstraction(localfile)

        if interpreter_path:
            local_profile[localfile]['allow']['path'][localfile]['mode'] = local_profile[localfile]['allow']['path'][localfile].get('mode', str_to_mode('r')) | str_to_mode('r')

            local_profile[localfile]['allow']['path'][localfile]['audit'] = local_profile[localfile]['allow']['path'][localfile].get('audit', set())

            local_profile[localfile]['allow']['path'][interpreter_path]['mode'] = local_profile[localfile]['allow']['path'][interpreter_path].get('mode', str_to_mode('ix')) | str_to_mode('ix')

            local_profile[localfile]['allow']['path'][interpreter_path]['audit'] = local_profile[localfile]['allow']['path'][interpreter_path].get('audit', set())

            if abstraction:
                local_profile[localfile]['include'][abstraction] = True

            handle_binfmt(local_profile[localfile], interpreter_path)
        else:

            local_profile[localfile]['allow']['path'][localfile]['mode'] = local_profile[localfile]['allow']['path'][localfile].get('mode', str_to_mode('mr')) | str_to_mode('mr')

            local_profile[localfile]['allow']['path'][localfile]['audit'] = local_profile[localfile]['allow']['path'][localfile].get('audit', set())

            handle_binfmt(local_profile[localfile], localfile)
    # Add required hats to the profile if they match the localfile
    for hatglob in cfg['required_hats'].keys():
        if re.search(hatglob, localfile):
            for hat in sorted(cfg['required_hats'][hatglob].split()):
                if not local_profile.get(hat, False):
                    local_profile[hat] = profile_storage('NEW', hat, 'create_new_profile() required_hats')
                local_profile[hat]['flags'] = 'complain'

    if not is_stub:
        created.append(localfile)
        changed[localfile] = True

    debug_logger.debug("Profile for %s:\n\t%s" % (localfile, local_profile.__str__()))
    return {localfile: local_profile}

def delete_profile(local_prof):
    """Deletes the specified file from the disk and remove it from our list"""
    profile_file = get_profile_filename(local_prof)
    if os.path.isfile(profile_file):
        os.remove(profile_file)
    if aa.get(local_prof, False):
        aa.pop(local_prof)

    #prof_unload(local_prof)

def confirm_and_abort():
    ans = aaui.UI_YesNo(_('Are you sure you want to abandon this set of profile changes and exit?'), 'n')
    if ans == 'y':
        aaui.UI_Info(_('Abandoning all changes.'))
        shutdown_yast()
        for prof in created:
            delete_profile(prof)
        sys.exit(0)

def get_profile(prof_name):
    profile_data = None
    distro = cfg['repository']['distro']
    repo_url = cfg['repository']['url']
    # local_profiles = []
    profile_hash = hasher()
    if repo_is_enabled():
        aaui.UI_BusyStart(_('Connecting to repository...'))
        status_ok, ret = fetch_profiles_by_name(repo_url, distro, prof_name)
        aaui.UI_BusyStop()
        if status_ok:
            profile_hash = ret
        else:
            aaui.UI_Important(_('WARNING: Error fetching profiles from the repository'))
    inactive_profile = get_inactive_profile(prof_name)
    if inactive_profile:
        uname = 'Inactive local profile for %s' % prof_name
        inactive_profile[prof_name][prof_name]['flags'] = 'complain'
        inactive_profile[prof_name][prof_name].pop('filename')
        profile_hash[uname]['username'] = uname
        profile_hash[uname]['profile_type'] = 'INACTIVE_LOCAL'
        profile_hash[uname]['profile'] = serialize_profile(inactive_profile[prof_name], prof_name)
        profile_hash[uname]['profile_data'] = inactive_profile
    # If no profiles in repo and no inactive profiles
    if not profile_hash.keys():
        return None
    options = []
    tmp_list = []
    preferred_present = False
    preferred_user = cfg['repository'].get('preferred_user', 'NOVELL')

    for p in profile_hash.keys():
        if profile_hash[p]['username'] == preferred_user:
            preferred_present = True
        else:
            tmp_list.append(profile_hash[p]['username'])

    if preferred_present:
        options.append(preferred_user)
    options += tmp_list

    q = aaui.PromptQuestion()
    q.headers = ['Profile', prof_name]
    q.functions = ['CMD_VIEW_PROFILE', 'CMD_USE_PROFILE', 'CMD_CREATE_PROFILE',
                      'CMD_ABORT', 'CMD_FINISHED']
    q.default = "CMD_VIEW_PROFILE"
    q.options = options
    q.selected = 0

    ans = ''
    while 'CMD_USE_PROFILE' not in ans and 'CMD_CREATE_PROFILE' not in ans:
        if ans == 'CMD_FINISHED':
            save_profiles()
            return

        ans, arg = q.promptUser()
        p = profile_hash[options[arg]]
        q.selected = options.index(options[arg])
        if ans == 'CMD_VIEW_PROFILE':
            if aaui.UI_mode == 'yast':
                SendDataToYast({'type': 'dialogue-view-profile',
                                'user': options[arg],
                                'profile': p['profile'],
                                'profile_type': p['profile_type']
                                })
                ypath, yarg = GetDataFromYast()
            #else:
            #    pager = get_pager()
            #    proc = subprocess.Popen(pager, stdin=subprocess.PIPE)
            #    proc.communicate('Profile submitted by %s:\n\n%s\n\n' %
            #                     (options[arg], p['profile']))
            #    proc.kill()
        elif ans == 'CMD_USE_PROFILE':
            if p['profile_type'] == 'INACTIVE_LOCAL':
                profile_data = p['profile_data']
                created.append(prof_name)
            else:
                profile_data = parse_repo_profile(prof_name, repo_url, p)
    return profile_data

def activate_repo_profiles(url, profiles, complain):
    read_profiles()
    try:
        for p in profiles:
            pname = p[0]
            profile_data = parse_repo_profile(pname, url, p[1])
            attach_profile_data(aa, profile_data)
            write_profile(pname)
            if complain:
                fname = get_profile_filename(pname)
                set_profile_flags(profile_dir + fname, 'complain')
                aaui.UI_Info(_('Setting %s to complain mode.') % pname)
    except Exception as e:
            sys.stderr.write(_("Error activating profiles: %s") % e)

def autodep(bin_name, pname=''):
    bin_full = None
    global repo_cfg
    if not repo_cfg and not cfg['repository'].get('url', False):
        repo_conf = apparmor.config.Config('shell', CONFDIR)
        repo_cfg = repo_conf.read_config('repository.conf')
        if not repo_cfg.get('repository', False) or repo_cfg['repository']['enabled'] == 'later':
            UI_ask_to_enable_repo()
    if bin_name:
        bin_full = find_executable(bin_name)
        #if not bin_full:
        #    bin_full = bin_name
        #if not bin_full.startswith('/'):
            #return None
        # Return if exectuable path not found
        if not bin_full:
            return None
    else:
        bin_full = pname  # for named profiles

    pname = bin_full
    read_inactive_profiles()
    profile_data = get_profile(pname)
    # Create a new profile if no existing profile
    if not profile_data:
        profile_data = create_new_profile(pname)
    file = get_profile_filename(pname)
    attach_profile_data(aa, profile_data)
    attach_profile_data(original_aa, profile_data)
    if os.path.isfile(profile_dir + '/tunables/global'):
        if not filelist.get(file, False):
            filelist[file] = hasher()
        filelist[file]['include']['tunables/global'] = True
        filelist[file]['profiles'][pname] = hasher()
        filelist[file]['profiles'][pname][pname] = True
    write_profile_ui_feedback(pname)

def get_profile_flags(filename, program):
    # To-Do
    # XXX If more than one profile in a file then second one is being ignored XXX
    # Do we return flags for both or
    flags = ''
    with open_file_read(filename) as f_in:
        for line in f_in:
            if RE_PROFILE_START.search(line):
                matches = parse_profile_start_line(line, filename)
                profile = matches['profile']
                flags = matches['flags']
                if profile == program or program is None:
                    return flags

    raise AppArmorException(_('%s contains no profile') % filename)

def change_profile_flags(filename, program, flag, set_flag):
    old_flags = get_profile_flags(filename, program)
    newflags = []
    if old_flags:
        # Flags maybe white-space and/or , separated
        old_flags = old_flags.split(',')

        if not isinstance(old_flags, str):
            for i in old_flags:
                newflags += i.split()
        else:
            newflags = old_flags.split()
        #newflags = [lambda x:x.strip(), oldflags]

    if set_flag:
        if flag not in newflags:
            newflags.append(flag)
    else:
        if flag in newflags:
            newflags.remove(flag)

    newflags = ','.join(newflags)

    set_profile_flags(filename, program, newflags)

def set_profile_flags(prof_filename, program, newflags):
    """Reads the old profile file and updates the flags accordingly"""
    # TODO: count the number of matching lines (separated by profile and hat?) and return it
    #       so that code calling this function can make sure to only report success if there was a match
    # TODO: existing (unrelated) flags of hats and child profiles are overwritten - ideally, we should
    #       keep them and only add or remove a given flag
    # TODO: change child profile flags even if program is specified

    found = False

    if newflags and newflags.strip() == '':
        raise AppArmorBug('New flags for %s contain only whitespace' % prof_filename)

    with open_file_read(prof_filename) as f_in:
        temp_file = tempfile.NamedTemporaryFile('w', prefix=prof_filename, suffix='~', delete=False, dir=profile_dir)
        shutil.copymode(prof_filename, temp_file.name)
        with open_file_write(temp_file.name) as f_out:
            for line in f_in:
                if RE_PROFILE_START.search(line):
                    matches = parse_profile_start_line(line, prof_filename)
                    space = matches['leadingspace'] or ''
                    profile = matches['profile']

                    if profile == program or program is None:
                        found = True
                        header_data = {
                            'attachment': matches['attachment'] or '',
                            'flags': newflags,
                            'profile_keyword': matches['profile_keyword'],
                            'header_comment': matches['comment'] or '',
                        }
                        line = write_header(header_data, len(space)/2, profile, False, True)
                        line = '%s\n' % line[0]
                elif RE_PROFILE_HAT_DEF.search(line):
                    matches = RE_PROFILE_HAT_DEF.search(line)
                    space = matches.group('leadingspace') or ''
                    hat_keyword = matches.group('hat_keyword')
                    hat = matches.group('hat')
                    comment = matches.group('comment') or ''
                    if comment:
                        comment = ' %s' % comment

                    if newflags:
                        line = '%s%s%s flags=(%s) {%s\n' % (space, hat_keyword, hat, newflags, comment)
                    else:
                        line = '%s%s%s {%s\n' % (space, hat_keyword, hat, comment)
                f_out.write(line)
    os.rename(temp_file.name, prof_filename)

    if not found:
        if program is None:
            raise AppArmorBug("%(file)s doesn't contain a valid profile (syntax error?)" % {'file': prof_filename})
        else:
            raise AppArmorBug("%(file)s doesn't contain a valid profile for %(profile)s (syntax error?)" % {'file': prof_filename, 'profile': program})

def profile_exists(program):
    """Returns True if profile exists, False otherwise"""
    # Check cache of profiles

    if existing_profiles.get(program, False):
        return True
    # Check the disk for profile
    prof_path = get_profile_filename(program)
    #print(prof_path)
    if os.path.isfile(prof_path):
        # Add to cache of profile
        existing_profiles[program] = prof_path
        return True
    return False

def sync_profile():
    user, passw = get_repo_user_pass()
    if not user or not passw:
        return None
    repo_profiles = []
    changed_profiles = []
    new_profiles = []
    serialize_opts = hasher()
    status_ok, ret = fetch_profiles_by_user(cfg['repository']['url'],
                                            cfg['repository']['distro'], user)
    if not status_ok:
        if not ret:
            ret = 'UNKNOWN ERROR'
        aaui.UI_Important(_('WARNING: Error synchronizing profiles with the repository:\n%s\n') % ret)
    else:
        users_repo_profiles = ret
        serialize_opts['NO_FLAGS'] = True
        for prof in sorted(aa.keys()):
            if is_repo_profile([aa[prof][prof]]):
                repo_profiles.append(prof)
            if prof in created:
                p_local = serialize_profile(aa[prof], prof, serialize_opts)
                if not users_repo_profiles.get(prof, False):
                    new_profiles.append(prof)
                    new_profiles.append(p_local)
                    new_profiles.append('')
                else:
                    p_repo = users_repo_profiles[prof]['profile']
                    if p_local != p_repo:
                        changed_profiles.append(prof)
                        changed_profiles.append(p_local)
                        changed_profiles.append(p_repo)
        if repo_profiles:
            for prof in repo_profiles:
                p_local = serialize_profile(aa[prof], prof, serialize_opts)
                if not users_repo_profiles.get(prof, False):
                    new_profiles.append(prof)
                    new_profiles.append(p_local)
                    new_profiles.append('')
                else:
                    p_repo = ''
                    if aa[prof][prof]['repo']['user'] == user:
                        p_repo = users_repo_profiles[prof]['profile']
                    else:
                        status_ok, ret = fetch_profile_by_id(cfg['repository']['url'],
                                                             aa[prof][prof]['repo']['id'])
                        if status_ok:
                            p_repo = ret['profile']
                        else:
                            if not ret:
                                ret = 'UNKNOWN ERROR'
                            aaui.UI_Important(_('WARNING: Error synchronizing profiles with the repository\n%s') % ret)
                            continue
                    if p_repo != p_local:
                        changed_profiles.append(prof)
                        changed_profiles.append(p_local)
                        changed_profiles.append(p_repo)
        if changed_profiles:
            submit_changed_profiles(changed_profiles)
        if new_profiles:
            submit_created_profiles(new_profiles)

def fetch_profile_by_id(url, id):
    #To-Do
    return None, None

def fetch_profiles_by_name(url, distro, user):
    #to-Do
    return None, None

def fetch_profiles_by_user(url, distro, user):
    #to-Do
    return None, None

def submit_created_profiles(new_profiles):
    #url = cfg['repository']['url']
    if new_profiles:
        if aaui.UI_mode == 'yast':
            title = 'New Profiles'
            message = 'Please select the newly created profiles that you would like to store in the repository'
            yast_select_and_upload_profiles(title, message, new_profiles)
        else:
            title = 'Submit newly created profiles to the repository'
            message = 'Would you like to upload newly created profiles?'
            console_select_and_upload_profiles(title, message, new_profiles)

def submit_changed_profiles(changed_profiles):
    #url = cfg['repository']['url']
    if changed_profiles:
        if aaui.UI_mode == 'yast':
            title = 'Changed Profiles'
            message = 'Please select which of the changed profiles would you like to upload to the repository'
            yast_select_and_upload_profiles(title, message, changed_profiles)
        else:
            title = 'Submit changed profiles to the repository'
            message = 'The following profiles from the repository were changed.\nWould you like to upload your changes?'
            console_select_and_upload_profiles(title, message, changed_profiles)

def yast_select_and_upload_profiles(title, message, profiles_up):
    url = cfg['repository']['url']
    profile_changes = hasher()
    profs = profiles_up[:]
    for p in profs:
        profile_changes[p[0]] = get_profile_diff(p[2], p[1])
    SendDataToYast({'type': 'dialog-select-profiles',
                    'title': title,
                    'explanation': message,
                    'default_select': 'false',
                    'disable_ask_upload': 'true',
                    'profiles': profile_changes
                    })
    ypath, yarg = GetDataFromYast()
    selected_profiles = []
    changelog = None
    changelogs = None
    single_changelog = False
    if yarg['STATUS'] == 'cancel':
        return
    else:
        selected_profiles = yarg['PROFILES']
        changelogs = yarg['CHANGELOG']
        if changelogs.get('SINGLE_CHANGELOG', False):
            changelog = changelogs['SINGLE_CHANGELOG']
            single_changelog = True
    user, passw = get_repo_user_pass()
    for p in selected_profiles:
        profile_string = serialize_profile(aa[p], p)
        if not single_changelog:
            changelog = changelogs[p]
        status_ok, ret = upload_profile(url, user, passw, cfg['repository']['distro'],
                                        p, profile_string, changelog)
        if status_ok:
            newprofile = ret
            newid = newprofile['id']
            set_repo_info(aa[p][p], url, user, newid)
            write_profile_ui_feedback(p)
        else:
            if not ret:
                ret = 'UNKNOWN ERROR'
            aaui.UI_Important(_('WARNING: An error occurred while uploading the profile %(profile)s\n%(ret)s') % { 'profile': p, 'ret': ret })
    aaui.UI_Info(_('Uploaded changes to repository.'))
    if yarg.get('NEVER_ASK_AGAIN'):
        unselected_profiles = []
        for p in profs:
            if p[0] not in selected_profiles:
                unselected_profiles.append(p[0])
        set_profiles_local_only(unselected_profiles)

def upload_profile(url, user, passw, distro, p, profile_string, changelog):
    # To-Do
    return None, None

def console_select_and_upload_profiles(title, message, profiles_up):
    url = cfg['repository']['url']
    profs = profiles_up[:]
    q = aaui.PromptQuestion()
    q.title = title
    q.headers = ['Repository', url]
    q.explanation = message
    q.functions = ['CMD_UPLOAD_CHANGES', 'CMD_VIEW_CHANGES', 'CMD_ASK_LATER',
                      'CMD_ASK_NEVER', 'CMD_ABORT']
    q.default = 'CMD_VIEW_CHANGES'
    q.options = [i[0] for i in profs]
    q.selected = 0
    ans = ''
    while 'CMD_UPLOAD_CHANGES' not in ans and 'CMD_ASK_NEVER' not in ans and 'CMD_ASK_LATER' not in ans:
        ans, arg = q.promptUser()
        if ans == 'CMD_VIEW_CHANGES':
            display_changes(profs[arg][2], profs[arg][1])
    if ans == 'CMD_NEVER_ASK':
        set_profiles_local_only([i[0] for i in profs])
    elif ans == 'CMD_UPLOAD_CHANGES':
        changelog = aaui.UI_GetString(_('Changelog Entry: '), '')
        user, passw = get_repo_user_pass()
        if user and passw:
            for p_data in profs:
                prof = p_data[0]
                prof_string = p_data[1]
                status_ok, ret = upload_profile(url, user, passw,
                                                cfg['repository']['distro'],
                                                prof, prof_string, changelog)
                if status_ok:
                    newprof = ret
                    newid = newprof['id']
                    set_repo_info(aa[prof][prof], url, user, newid)
                    write_profile_ui_feedback(prof)
                    aaui.UI_Info('Uploaded %s to repository' % prof)
                else:
                    if not ret:
                        ret = 'UNKNOWN ERROR'
                    aaui.UI_Important(_('WARNING: An error occurred while uploading the profile %(profile)s\n%(ret)s') % { 'profile': prof, 'ret': ret })
        else:
            aaui.UI_Important(_('Repository Error\nRegistration or Signin was unsuccessful. User login\ninformation is required to upload profiles to the repository.\nThese changes could not be sent.'))

def set_profiles_local_only(profs):
    for p in profs:
        aa[profs][profs]['repo']['neversubmit'] = True
        write_profile_ui_feedback(profs)


def build_x_functions(default, options, exec_toggle):
    ret_list = []
    fallback_toggle = False
    if exec_toggle:
        if 'i' in options:
            ret_list.append('CMD_ix')
            if 'p' in options:
                ret_list.append('CMD_pix')
                fallback_toggle = True
            if 'c' in options:
                ret_list.append('CMD_cix')
                fallback_toggle = True
            if 'n' in options:
                ret_list.append('CMD_nix')
                fallback_toggle = True
            if fallback_toggle:
                ret_list.append('CMD_EXEC_IX_OFF')
        if 'u' in options:
            ret_list.append('CMD_ux')

    else:
        if 'i' in options:
            ret_list.append('CMD_ix')
        if 'c' in options:
            ret_list.append('CMD_cx')
            fallback_toggle = True
        if 'p' in options:
            ret_list.append('CMD_px')
            fallback_toggle = True
        if 'n' in options:
            ret_list.append('CMD_nx')
            fallback_toggle = True
        if 'u' in options:
            ret_list.append('CMD_ux')

        if fallback_toggle:
            ret_list.append('CMD_EXEC_IX_ON')

    ret_list += ['CMD_DENY', 'CMD_ABORT', 'CMD_FINISHED']
    return ret_list

def handle_children(profile, hat, root):
    entries = root[:]
    pid = None
    p = None
    h = None
    prog = None
    aamode = None
    mode = None
    detail = None
    to_name = None
    uhat = None
    capability = None
    family = None
    sock_type = None
    protocol = None
    global seen_events
    regex_nullcomplain = re.compile('^null(-complain)*-profile$')

    for entry in entries:
        if type(entry[0]) != str:
            handle_children(profile, hat, entry)
        else:
            typ = entry.pop(0)
            if typ == 'fork':
                # If type is fork then we (should) have pid, profile and hat
                pid, p, h = entry[:3]
                if not regex_nullcomplain.search(p) and not regex_nullcomplain.search(h):
                    profile = p
                    hat = h
                if hat:
                    profile_changes[pid] = profile + '//' + hat
                else:
                    profile_changes[pid] = profile
            elif typ == 'unknown_hat':
                # If hat is not known then we (should) have pid, profile, hat, mode and unknown hat in entry
                pid, p, h, aamode, uhat = entry[:5]
                if not regex_nullcomplain.search(p):
                    profile = p
                if aa[profile].get(uhat, False):
                    hat = uhat
                    continue
                new_p = update_repo_profile(aa[profile][profile])
                if new_p and UI_SelectUpdatedRepoProfile(profile, new_p) and aa[profile].get(uhat, False):
                    hat = uhat
                    continue

                default_hat = None
                for hatglob in cfg.options('defaulthat'):
                    if re.search(hatglob, profile):
                        default_hat = cfg['defaulthat'][hatglob]

                context = profile
                context = context + ' -> ^%s' % uhat
                ans = transitions.get(context, 'XXXINVALIDXXX')

                while ans not in ['CMD_ADDHAT', 'CMD_USEDEFAULT', 'CMD_DENY']:
                    q = aaui.PromptQuestion()
                    q.headers += [_('Profile'), profile]

                    if default_hat:
                        q.headers += [_('Default Hat'), default_hat]

                    q.headers += [_('Requested Hat'), uhat]

                    q.functions.append('CMD_ADDHAT')
                    if default_hat:
                        q.functions.append('CMD_USEDEFAULT')
                    q.functions += ['CMD_DENY', 'CMD_ABORT', 'CMD_FINISHED']

                    q.default = 'CMD_DENY'
                    if aamode == 'PERMITTING':
                        q.default = 'CMD_ADDHAT'

                    seen_events += 1

                    ans = q.promptUser()

                    if ans == 'CMD_FINISHED':
                        save_profiles()
                        return

                transitions[context] = ans

                if ans == 'CMD_ADDHAT':
                    hat = uhat
                    aa[profile][hat]['flags'] = aa[profile][profile]['flags']
                elif ans == 'CMD_USEDEFAULT':
                    hat = default_hat
                elif ans == 'CMD_DENY':
                    # As unknown hat is denied no entry for it should be made
                    return None

            elif typ == 'capability':
                # If capability then we (should) have pid, profile, hat, program, mode, capability
                pid, p, h, prog, aamode, capability = entry[:6]
                if not regex_nullcomplain.search(p) and not regex_nullcomplain.search(h):
                    profile = p
                    hat = h
                if not profile or not hat:
                    continue
                prelog[aamode][profile][hat]['capability'][capability] = True

            elif typ == 'ptrace':
                # If ptrace then we (should) have pid, profile, hat, program, mode, access and peer
                pid, p, h, prog, aamode, access, peer = entry
                if not regex_nullcomplain.search(p) and not regex_nullcomplain.search(h):
                    profile = p
                    hat = h
                if not profile or not hat:
                    continue
                prelog[aamode][profile][hat]['ptrace'][peer][access] = True

            elif typ == 'signal':
                # If signal then we (should) have pid, profile, hat, program, mode, access, signal and peer
                pid, p, h, prog, aamode, access, signal, peer = entry
                if not regex_nullcomplain.search(p) and not regex_nullcomplain.search(h):
                    profile = p
                    hat = h
                if not profile or not hat:
                    continue
                prelog[aamode][profile][hat]['signal'][peer][access][signal] = True

            elif typ == 'path' or typ == 'exec':
                # If path or exec then we (should) have pid, profile, hat, program, mode, details and to_name
                pid, p, h, prog, aamode, mode, detail, to_name = entry[:8]
                if not mode:
                    mode = set()
                if not regex_nullcomplain.search(p) and not regex_nullcomplain.search(h):
                    profile = p
                    hat = h
                if not profile or not hat or not detail:
                    continue

                domainchange = 'nochange'
                if typ == 'exec':
                    domainchange = 'change'

                # Escape special characters
                detail = detail.replace('[', '\[')
                detail = detail.replace(']', '\]')
                detail = detail.replace('+', '\+')
                detail = detail.replace('*', '\*')
                detail = detail.replace('{', '\{')
                detail = detail.replace('}', '\}')
                detail = detail.replace('!', '\!')

                # Give Execute dialog if x access requested for something that's not a directory
                # For directories force an 'ix' Path dialog
                do_execute = False
                exec_target = detail

                if mode & str_to_mode('x'):
                    if os.path.isdir(exec_target):
                        raise AppArmorBug('exec permissions requested for directory %s. This should not happen - please open a bugreport!' % exec_target)
                    elif typ != 'exec':
                        raise AppArmorBug('exec permissions requested for %(exec_target)s, but mode is %(mode)s instead of exec. This should not happen - please open a bugreport!' % {'exec_target': exec_target, 'mode':mode})
                    else:
                        do_execute = True

                if mode:
                    path = detail

                    if prelog[aamode][profile][hat]['path'].get(path, False):
                        mode |= prelog[aamode][profile][hat]['path'][path]
                    prelog[aamode][profile][hat]['path'][path] = mode

                if do_execute:
                    if profile_known_exec(aa[profile][hat], 'exec', exec_target):
                        continue

                    p = update_repo_profile(aa[profile][profile])
                    if to_name:
                        if UI_SelectUpdatedRepoProfile(profile, p) and profile_known_exec(aa[profile][hat], 'exec', to_name):
                            continue
                    else:
                        if UI_SelectUpdatedRepoProfile(profile, p) and profile_known_exec(aa[profile][hat], 'exec', exec_target):
                            continue

                    context_new = profile
                    if profile != hat:
                        context_new = context_new + '^%s' % hat
                    context_new = context_new + ' -> %s' % exec_target

                    # ans_new = transitions.get(context_new, '')  # XXX ans meant here?
                    combinedmode = set()
                    combinedaudit = set()
                    ## Check return Value Consistency
                    # Check if path matches any existing regexps in profile
                    cm, am, m = rematchfrag(aa[profile][hat], 'allow', exec_target)
                    if cm:
                        combinedmode |= cm
                    if am:
                        combinedaudit |= am

                    if combinedmode & str_to_mode('x'):
                        nt_name = None
                        for entr in m:
                            if aa[profile][hat]['allow']['path'].get(entr, False):
                                nt_name = entr
                                break
                        if to_name and to_name != nt_name:
                            pass
                        elif nt_name:
                            to_name = nt_name
                    ## Check return value consistency
                    # Check if the includes from profile match
                    cm, am, m = match_prof_incs_to_path(aa[profile][hat], 'allow', exec_target)
                    if cm:
                        combinedmode |= cm
                    if am:
                        combinedaudit |= am
                    if combinedmode & str_to_mode('x'):
                        nt_name = None
                        for entr in m:
                            if aa[profile][hat]['allow']['path'][entry]['to']:
                                nt_name = aa[profile][hat]['allow']['path'][entry]['to']
                                break
                        if to_name and to_name != nt_name:
                            pass
                        elif nt_name:
                            to_name = nt_name

                    # nx is not used in profiles but in log files.
                    # Log parsing methods will convert it to its profile form
                    # nx is internally cx/px/cix/pix + to_name
                    exec_mode = False
                    if contains(combinedmode, 'pix'):
                        if to_name:
                            ans = 'CMD_nix'
                        else:
                            ans = 'CMD_pix'
                        exec_mode = str_to_mode('pixr')
                    elif contains(combinedmode, 'cix'):
                        if to_name:
                            ans = 'CMD_nix'
                        else:
                            ans = 'CMD_cix'
                        exec_mode = str_to_mode('cixr')
                    elif contains(combinedmode, 'Pix'):
                        if to_name:
                            ans = 'CMD_nix_safe'
                        else:
                            ans = 'CMD_pix_safe'
                        exec_mode = str_to_mode('Pixr')
                    elif contains(combinedmode, 'Cix'):
                        if to_name:
                            ans = 'CMD_nix_safe'
                        else:
                            ans = 'CMD_cix_safe'
                        exec_mode = str_to_mode('Cixr')
                    elif contains(combinedmode, 'ix'):
                        ans = 'CMD_ix'
                        exec_mode = str_to_mode('ixr')
                    elif contains(combinedmode, 'px'):
                        if to_name:
                            ans = 'CMD_nx'
                        else:
                            ans = 'CMD_px'
                        exec_mode = str_to_mode('px')
                    elif contains(combinedmode, 'cx'):
                        if to_name:
                            ans = 'CMD_nx'
                        else:
                            ans = 'CMD_cx'
                        exec_mode = str_to_mode('cx')
                    elif contains(combinedmode, 'ux'):
                        ans = 'CMD_ux'
                        exec_mode = str_to_mode('ux')
                    elif contains(combinedmode, 'Px'):
                        if to_name:
                            ans = 'CMD_nx_safe'
                        else:
                            ans = 'CMD_px_safe'
                        exec_mode = str_to_mode('Px')
                    elif contains(combinedmode, 'Cx'):
                        if to_name:
                            ans = 'CMD_nx_safe'
                        else:
                            ans = 'CMD_cx_safe'
                        exec_mode = str_to_mode('Cx')
                    elif contains(combinedmode, 'Ux'):
                        ans = 'CMD_ux_safe'
                        exec_mode = str_to_mode('Ux')
                    else:
                        options = cfg['qualifiers'].get(exec_target, 'ipcnu')
                        if to_name:
                            fatal_error(_('%s has transition name but not transition mode') % entry)

                        ### If profiled program executes itself only 'ix' option
                        ##if exec_target == profile:
                            ##options = 'i'

                        # Don't allow hats to cx?
                        options.replace('c', '')
                        # Add deny to options
                        options += 'd'
                        # Define the default option
                        default = None
                        if 'p' in options and os.path.exists(get_profile_filename(exec_target)):
                            default = 'CMD_px'
                            sys.stdout.write(_('Target profile exists: %s\n') % get_profile_filename(exec_target))
                        elif 'i' in options:
                            default = 'CMD_ix'
                        elif 'c' in options:
                            default = 'CMD_cx'
                        elif 'n' in options:
                            default = 'CMD_nx'
                        else:
                            default = 'DENY'

                        #
                        parent_uses_ld_xxx = check_for_LD_XXX(profile)

                        sev_db.unload_variables()
                        sev_db.load_variables(get_profile_filename(profile))
                        severity = sev_db.rank(exec_target, 'x')

                        # Prompt portion starts
                        q = aaui.PromptQuestion()

                        q.headers += [_('Profile'), combine_name(profile, hat)]
                        if prog and prog != 'HINT':
                            q.headers += [_('Program'), prog]

                        # to_name should not exist here since, transitioning is already handeled
                        q.headers += [_('Execute'), exec_target]
                        q.headers += [_('Severity'), severity]

                        # prompt = '\n%s\n' % context_new  # XXX
                        exec_toggle = False
                        q.functions += build_x_functions(default, options, exec_toggle)

                        # options = '|'.join(options)
                        seen_events += 1
                        regex_options = re.compile('^CMD_(ix|px|cx|nx|pix|cix|nix|px_safe|cx_safe|nx_safe|pix_safe|cix_safe|nix_safe|ux|ux_safe|EXEC_TOGGLE|DENY)$')

                        ans = ''
                        while not regex_options.search(ans):
                            ans = q.promptUser()[0].strip()
                            if ans.startswith('CMD_EXEC_IX_'):
                                exec_toggle = not exec_toggle
                                q.functions = []
                                q.functions += build_x_functions(default, options, exec_toggle)
                                ans = ''
                                continue

                            if ans == 'CMD_FINISHED':
                                save_profiles()
                                return

                            if ans == 'CMD_nx' or ans == 'CMD_nix':
                                arg = exec_target
                                ynans = 'n'
                                if profile == hat:
                                    ynans = aaui.UI_YesNo(_('Are you specifying a transition to a local profile?'), 'n')
                                if ynans == 'y':
                                    if ans == 'CMD_nx':
                                        ans = 'CMD_cx'
                                    else:
                                        ans = 'CMD_cix'
                                else:
                                    if ans == 'CMD_nx':
                                        ans = 'CMD_px'
                                    else:
                                        ans = 'CMD_pix'

                                to_name = aaui.UI_GetString(_('Enter profile name to transition to: '), arg)

                            regex_optmode = re.compile('CMD_(px|cx|nx|pix|cix|nix)')
                            if ans == 'CMD_ix':
                                exec_mode = str_to_mode('ix')
                            elif regex_optmode.search(ans):
                                match = regex_optmode.search(ans).groups()[0]
                                exec_mode = str_to_mode(match)
                                px_default = 'n'
                                px_msg = _("Should AppArmor sanitise the environment when\nswitching profiles?\n\nSanitising environment is more secure,\nbut some applications depend on the presence\nof LD_PRELOAD or LD_LIBRARY_PATH.")
                                if parent_uses_ld_xxx:
                                    px_msg = _("Should AppArmor sanitise the environment when\nswitching profiles?\n\nSanitising environment is more secure,\nbut this application appears to be using LD_PRELOAD\nor LD_LIBRARY_PATH and sanitising the environment\ncould cause functionality problems.")

                                ynans = aaui.UI_YesNo(px_msg, px_default)
                                if ynans == 'y':
                                    # Disable the unsafe mode
                                    exec_mode = exec_mode - (apparmor.aamode.AA_EXEC_UNSAFE | AA_OTHER(apparmor.aamode.AA_EXEC_UNSAFE))
                            elif ans == 'CMD_ux':
                                exec_mode = str_to_mode('ux')
                                ynans = aaui.UI_YesNo(_("Launching processes in an unconfined state is a very\ndangerous operation and can cause serious security holes.\n\nAre you absolutely certain you wish to remove all\nAppArmor protection when executing %s ?") % exec_target, 'n')
                                if ynans == 'y':
                                    ynans = aaui.UI_YesNo(_("Should AppArmor sanitise the environment when\nrunning this program unconfined?\n\nNot sanitising the environment when unconfining\na program opens up significant security holes\nand should be avoided if at all possible."), 'y')
                                    if ynans == 'y':
                                        # Disable the unsafe mode
                                        exec_mode = exec_mode - (apparmor.aamode.AA_EXEC_UNSAFE | AA_OTHER(apparmor.aamode.AA_EXEC_UNSAFE))
                                else:
                                    ans = 'INVALID'
                        transitions[context_new] = ans

                        regex_options = re.compile('CMD_(ix|px|cx|nx|pix|cix|nix)')
                        if regex_options.search(ans):
                            # For inherit we need r
                            if exec_mode & str_to_mode('i'):
                                exec_mode |= str_to_mode('r')
                        else:
                            if ans == 'CMD_DENY':
                                aa[profile][hat]['deny']['path'][exec_target]['mode'] = aa[profile][hat]['deny']['path'][exec_target].get('mode', str_to_mode('x')) | str_to_mode('x')
                                aa[profile][hat]['deny']['path'][exec_target]['audit'] = aa[profile][hat]['deny']['path'][exec_target].get('audit', set())
                                changed[profile] = True
                                # Skip remaining events if they ask to deny exec
                                if domainchange == 'change':
                                    return None

                        if ans != 'CMD_DENY':
                            prelog['PERMITTING'][profile][hat]['path'][exec_target] = prelog['PERMITTING'][profile][hat]['path'].get(exec_target, exec_mode) | exec_mode

                            log_dict['PERMITTING'][profile] = hasher()

                            aa[profile][hat]['allow']['path'][exec_target]['mode'] = aa[profile][hat]['allow']['path'][exec_target].get('mode', exec_mode)

                            aa[profile][hat]['allow']['path'][exec_target]['audit'] = aa[profile][hat]['allow']['path'][exec_target].get('audit', set())

                            if to_name:
                                aa[profile][hat]['allow']['path'][exec_target]['to'] = to_name

                            changed[profile] = True

                            if exec_mode & str_to_mode('i'):
                                interpreter_path, abstraction = get_interpreter_and_abstraction(exec_target)

                                if interpreter_path:
                                    aa[profile][hat]['allow']['path'][interpreter_path]['mode'] = aa[profile][hat]['allow']['path'][interpreter_path].get('mode', str_to_mode('ix')) | str_to_mode('ix')

                                    aa[profile][hat]['allow']['path'][interpreter_path]['audit'] = aa[profile][hat]['allow']['path'][interpreter_path].get('audit', set())

                                    if abstraction:
                                        aa[profile][hat]['include'][abstraction] = True

                    # Update tracking info based on kind of change

                    if ans == 'CMD_ix':
                        if hat:
                            profile_changes[pid] = '%s//%s' % (profile, hat)
                        else:
                            profile_changes[pid] = '%s//' % profile
                    elif re.search('^CMD_(px|nx|pix|nix)', ans):
                        if to_name:
                            exec_target = to_name
                        if aamode == 'PERMITTING':
                            if domainchange == 'change':
                                profile = exec_target
                                hat = exec_target
                                profile_changes[pid] = '%s' % profile

                        # Check profile exists for px
                        if not os.path.exists(get_profile_filename(exec_target)):
                            ynans = 'y'
                            if exec_mode & str_to_mode('i'):
                                ynans = aaui.UI_YesNo(_('A profile for %s does not exist.\nDo you want to create one?') % exec_target, 'n')
                            if ynans == 'y':
                                helpers[exec_target] = 'enforce'
                                if to_name:
                                    autodep('', exec_target)
                                else:
                                    autodep(exec_target, '')
                                reload_base(exec_target)
                    elif ans.startswith('CMD_cx') or ans.startswith('CMD_cix'):
                        if to_name:
                            exec_target = to_name
                        if aamode == 'PERMITTING':
                            if domainchange == 'change':
                                profile_changes[pid] = '%s//%s' % (profile, exec_target)

                        if not aa[profile].get(exec_target, False):
                            ynans = 'y'
                            if exec_mode & str_to_mode('i'):
                                ynans = aaui.UI_YesNo(_('A profile for %s does not exist.\nDo you want to create one?') % exec_target, 'n')
                            if ynans == 'y':
                                hat = exec_target
                                if not aa[profile].get(hat, False):
                                    aa[profile][hat] = profile_storage(profile, hat, 'handle_children()')
                                aa[profile][hat]['profile'] = True

                                if profile != hat:
                                    aa[profile][hat]['flags'] = aa[profile][profile]['flags']

                                stub_profile = create_new_profile(hat, True)

                                aa[profile][hat]['flags'] = 'complain'

                                aa[profile][hat]['allow']['path'] = hasher()
                                if stub_profile[hat][hat]['allow'].get('path', False):
                                    aa[profile][hat]['allow']['path'] = stub_profile[hat][hat]['allow']['path']

                                aa[profile][hat]['include'] = hasher()
                                if stub_profile[hat][hat].get('include', False):
                                    aa[profile][hat]['include'] = stub_profile[hat][hat]['include']

                                file_name = aa[profile][profile]['filename']
                                filelist[file_name]['profiles'][profile][hat] = True

                    elif ans.startswith('CMD_ux'):
                        profile_changes[pid] = 'unconfined'
                        if domainchange == 'change':
                            return None

            elif typ == 'netdomain':
                # If netdomain we (should) have pid, profile, hat, program, mode, network family, socket type and protocol
                pid, p, h, prog, aamode, family, sock_type, protocol = entry[:8]

                if not regex_nullcomplain.search(p) and not regex_nullcomplain.search(h):
                    profile = p
                    hat = h
                if not hat or not profile:
                    continue
                if family and sock_type:
                    prelog[aamode][profile][hat]['netdomain'][family][sock_type] = True

    return None

##### Repo related functions

def UI_SelectUpdatedRepoProfile(profile, p):
    # To-Do
    return False

def UI_repo_signup():
    # To-Do
    return None, None

def UI_ask_to_enable_repo():
    # To-Do
    pass

def UI_ask_to_upload_profiles():
    # To-Do
    pass

def UI_ask_mode_toggles(audit_toggle, owner_toggle, oldmode):
    # To-Do
    return (audit_toggle, owner_toggle)

def parse_repo_profile(fqdbin, repo_url, profile):
    # To-Do
    pass

def set_repo_info(profile_data, repo_url, username, iden):
    # To-Do
    pass

def is_repo_profile(profile_data):
    # To-Do
    pass

def get_repo_user_pass():
    # To-Do
    pass
def get_preferred_user(repo_url):
    # To-Do
    pass
def repo_is_enabled():
    # To-Do
    return False

def update_repo_profile(profile):
    # To-Do
    return None

def order_globs(globs, path):
    """Returns the globs in sorted order, more specific behind"""
    # To-Do
    # ATM its lexicographic, should be done to allow better matches later
    return sorted(globs)

def ask_the_questions():
    found = 0
    global seen_events
    for aamode in sorted(log_dict.keys()):
        # Describe the type of changes
        if aamode == 'PERMITTING':
            aaui.UI_Info(_('Complain-mode changes:'))
        elif aamode == 'REJECTING':
            aaui.UI_Info(_('Enforce-mode changes:'))
        else:
            # This is so wrong!
            fatal_error(_('Invalid mode found: %s') % aamode)

        for profile in sorted(log_dict[aamode].keys()):
            # Update the repo profiles
            p = update_repo_profile(aa[profile][profile])
            if p:
                UI_SelectUpdatedRepoProfile(profile, p)

            found += 1
            # Sorted list of hats with the profile name coming first
            hats = list(filter(lambda key: key != profile, sorted(log_dict[aamode][profile].keys())))
            if log_dict[aamode][profile].get(profile, False):
                hats = [profile] + hats

            for hat in hats:
                for ruletype in ruletypes:
                    for rule_obj in log_dict[aamode][profile][hat][ruletype].rules:
                        # XXX aa-mergeprof also has this code - if you change it, keep aa-mergeprof in sync!

                        if is_known_rule(aa[profile][hat], ruletype, rule_obj):
                            continue

                        default_option = 1
                        options = []
                        newincludes = match_includes(aa[profile][hat], ruletype, rule_obj)
                        q = aaui.PromptQuestion()
                        if newincludes:
                            options += list(map(lambda inc: '#include <%s>' % inc, sorted(set(newincludes))))

                        options.append(rule_obj.get_clean())
                        q.options = options
                        q.selected = default_option - 1

                        seen_events += 1

                        done = False
                        while not done:
                            q.headers = [_('Profile'), combine_name(profile, hat)]
                            q.headers += rule_obj.logprof_header()

                            # Load variables into sev_db? Not needed/used for capabilities and network rules.
                            severity = rule_obj.severity(sev_db)
                            if severity != sev_db.NOT_IMPLEMENTED:
                                q.headers += [_('Severity'), severity]

                            q.functions = available_buttons(rule_obj)

                            # In complain mode: events default to allow
                            # In enforce mode: events default to deny
                            # XXX does this behaviour really make sense, except for "historical reasons"[tm]?
                            q.default = 'CMD_DENY'
                            if rule_obj.log_event == 'PERMITTING':
                                q.default = 'CMD_ALLOW'

                            ans, selected = q.promptUser()
                            if ans == 'CMD_IGNORE_ENTRY':
                                done = True
                                break

                            elif ans == 'CMD_FINISHED':
                                return

                            elif ans.startswith('CMD_AUDIT'):
                                if ans == 'CMD_AUDIT_NEW':
                                    rule_obj.audit = True
                                    rule_obj.raw_rule = None
                                else:
                                    rule_obj.audit = False
                                    rule_obj.raw_rule = None

                                options[len(options) - 1] = rule_obj.get_clean()
                                q.options = options

                            elif ans == 'CMD_ALLOW':
                                done = True
                                changed[profile] = True

                                selection = options[selected]

                                inc = re_match_include(selection)
                                if inc:
                                    deleted = delete_duplicates(aa[profile][hat], inc)

                                    aa[profile][hat]['include'][inc] = True

                                    aaui.UI_Info(_('Adding %s to profile.') % selection)
                                    if deleted:
                                        aaui.UI_Info(_('Deleted %s previous matching profile entries.') % deleted)

                                else:
                                    aa[profile][hat][ruletype].add(rule_obj)

                                    aaui.UI_Info(_('Adding %s to profile.') % rule_obj.get_clean())

                            elif ans == 'CMD_DENY':
                                done = True
                                changed[profile] = True

                                rule_obj.deny = True
                                rule_obj.raw_rule = None  # reset raw rule after manually modifying rule_obj
                                aa[profile][hat][ruletype].add(rule_obj)
                                aaui.UI_Info(_('Adding %s to profile.') % rule_obj.get_clean())

                            else:
                                done = False
                    # END of code (mostly) shared with aa-mergeprof

                # Process all the path entries.
                for path in sorted(log_dict[aamode][profile][hat]['allow']['path'].keys()):
                    mode = log_dict[aamode][profile][hat]['allow']['path'][path]
                    # Lookup modes from profile
                    allow_mode = set()
                    allow_audit = set()
                    deny_mode = set()
                    deny_audit = set()

                    fmode, famode, fm = rematchfrag(aa[profile][hat], 'allow', path)
                    if fmode:
                        allow_mode |= fmode
                    if famode:
                        allow_audit |= famode

                    cm, cam, m = rematchfrag(aa[profile][hat], 'deny', path)
                    if cm:
                        deny_mode |= cm
                    if cam:
                        deny_audit |= cam

                    imode, iamode, im = match_prof_incs_to_path(aa[profile][hat], 'allow', path)
                    if imode:
                        allow_mode |= imode
                    if iamode:
                        allow_audit |= iamode

                    cm, cam, m = match_prof_incs_to_path(aa[profile][hat], 'deny', path)
                    if cm:
                        deny_mode |= cm
                    if cam:
                        deny_audit |= cam

                    if deny_mode & apparmor.aamode.AA_MAY_EXEC:
                        deny_mode |= apparmor.aamode.ALL_AA_EXEC_TYPE

                    # Mask off the denied modes
                    mode = mode - deny_mode

                    # If we get an exec request from some kindof event that generates 'PERMITTING X'
                    # check if its already in allow_mode
                    # if not add ix permission
                    if mode & apparmor.aamode.AA_MAY_EXEC:
                        # Remove all type access permission
                        mode = mode - apparmor.aamode.ALL_AA_EXEC_TYPE
                        if not allow_mode & apparmor.aamode.AA_MAY_EXEC:
                            mode |= str_to_mode('ix')

                    if not mode:
                        continue

                    matches = []

                    if fmode:
                        matches += fm

                    if imode:
                        matches += im

                    if not mode_contains(allow_mode, mode):
                        default_option = 1
                        options = []
                        newincludes = []
                        include_valid = False

                        for incname in include.keys():
                            include_valid = False
                            # If already present skip
                            if aa[profile][hat]['include'].get(incname, False):
                                continue
                            if incname.startswith(profile_dir):
                                incname = incname.replace(profile_dir + '/', '', 1)

                            include_valid = valid_include('', incname)

                            if not include_valid:
                                continue

                            cm, am, m = match_include_to_path(incname, 'allow', path)

                            if cm and mode_contains(cm, mode):
                                dm = match_include_to_path(incname, 'deny', path)[0]
                                # If the mode is denied
                                if not mode & dm:
                                    if not list(filter(lambda s: '/**' == s, m)):
                                        newincludes.append(incname)
                        # Add new includes to the options
                        if newincludes:
                            options += list(map(lambda s: '#include <%s>' % s, sorted(set(newincludes))))
                        # We should have literal the path in options list too
                        options.append(path)
                        # Add any the globs matching path from logprof
                        globs = glob_common(path)
                        if globs:
                            matches += globs
                        # Add any user entered matching globs
                        for user_glob in user_globs:
                            if matchliteral(user_glob, path):
                                matches.append(user_glob)

                        matches = list(set(matches))
                        if path in matches:
                            matches.remove(path)

                        options += order_globs(matches, path)
                        default_option = len(options)

                        sev_db.unload_variables()
                        sev_db.load_variables(get_profile_filename(profile))
                        severity = sev_db.rank(path, mode_to_str(mode))
                        sev_db.unload_variables()

                        audit_toggle = 0
                        owner_toggle = 0
                        if cfg['settings']['default_owner_prompt']:
                            owner_toggle = cfg['settings']['default_owner_prompt']
                        done = False
                        while not done:
                            q = aaui.PromptQuestion()
                            q.headers = [_('Profile'), combine_name(profile, hat),
                                            _('Path'), path]

                            if allow_mode:
                                mode |= allow_mode
                                tail = ''
                                s = ''
                                prompt_mode = None
                                if owner_toggle == 0:
                                    prompt_mode = flatten_mode(mode)
                                    tail = '     ' + _('(owner permissions off)')
                                elif owner_toggle == 1:
                                    prompt_mode = mode
                                elif owner_toggle == 2:
                                    prompt_mode = allow_mode | owner_flatten_mode(mode - allow_mode)
                                    tail = '     ' + _('(force new perms to owner)')
                                else:
                                    prompt_mode = owner_flatten_mode(mode)
                                    tail = '     ' + _('(force all rule perms to owner)')

                                if audit_toggle == 1:
                                    s = mode_to_str_user(allow_mode)
                                    if allow_mode:
                                        s += ', '
                                    s += 'audit ' + mode_to_str_user(prompt_mode - allow_mode) + tail
                                elif audit_toggle == 2:
                                    s = 'audit ' + mode_to_str_user(prompt_mode) + tail
                                else:
                                    s = mode_to_str_user(prompt_mode) + tail

                                q.headers += [_('Old Mode'), mode_to_str_user(allow_mode),
                                                 _('New Mode'), s]

                            else:
                                s = ''
                                tail = ''
                                prompt_mode = None
                                if audit_toggle:
                                    s = 'audit'
                                if owner_toggle == 0:
                                    prompt_mode = flatten_mode(mode)
                                    tail = '     ' + _('(owner permissions off)')
                                elif owner_toggle == 1:
                                    prompt_mode = mode
                                else:
                                    prompt_mode = owner_flatten_mode(mode)
                                    tail = '     ' + _('(force perms to owner)')

                                s = mode_to_str_user(prompt_mode)
                                q.headers += [_('Mode'), s]

                            q.headers += [_('Severity'), severity]
                            q.options = options
                            q.selected = default_option - 1
                            q.functions = ['CMD_ALLOW', 'CMD_DENY', 'CMD_IGNORE_ENTRY', 'CMD_GLOB',
                                              'CMD_GLOBEXT', 'CMD_NEW', 'CMD_ABORT',
                                              'CMD_FINISHED', 'CMD_OTHER']
                            q.default = 'CMD_DENY'
                            if aamode == 'PERMITTING':
                                q.default = 'CMD_ALLOW'

                            seen_events += 1

                            ans, selected = q.promptUser()

                            if ans == 'CMD_FINISHED':
                                save_profiles()
                                return

                            if ans == 'CMD_IGNORE_ENTRY':
                                done = True
                                break

                            if ans == 'CMD_OTHER':
                                audit_toggle, owner_toggle = UI_ask_mode_toggles(audit_toggle, owner_toggle, allow_mode)
                            elif ans == 'CMD_USER_TOGGLE':
                                owner_toggle += 1
                                if not allow_mode and owner_toggle == 2:
                                    owner_toggle += 1
                                if owner_toggle > 3:
                                    owner_toggle = 0
                            elif ans == 'CMD_ALLOW':
                                path = options[selected]
                                done = True
                                match = re_match_include(path)  # .search('^#include\s+<(.+)>$', path)
                                if match:
                                    inc = match  # .groups()[0]
                                    deleted = 0
                                    deleted = delete_duplicates(aa[profile][hat], inc)
                                    aa[profile][hat]['include'][inc] = True
                                    changed[profile] = True
                                    aaui.UI_Info(_('Adding %s to profile.') % path)
                                    if deleted:
                                        aaui.UI_Info(_('Deleted %s previous matching profile entries.') % deleted)

                                else:
                                    if path in aa[profile][hat]['allow']['path']:
                                        if aa[profile][hat]['allow']['path'][path].get('mode', False):
                                            mode |= aa[profile][hat]['allow']['path'][path]['mode']
                                    deleted = []
                                    for entry in aa[profile][hat]['allow']['path'].keys():
                                        if path == entry:
                                            continue

                                        if matchregexp(path, entry):
                                            if mode_contains(mode, aa[profile][hat]['allow']['path'][entry]['mode']):
                                                deleted.append(entry)
                                    for entry in deleted:
                                        aa[profile][hat]['allow']['path'].pop(entry)
                                    deleted = len(deleted)

                                    if owner_toggle == 0:
                                        mode = flatten_mode(mode)
                                    #elif owner_toggle == 1:
                                    #    mode = mode
                                    elif owner_toggle == 2:
                                        mode = allow_mode | owner_flatten_mode(mode - allow_mode)
                                    elif owner_toggle == 3:
                                        mode = owner_flatten_mode(mode)

                                    aa[profile][hat]['allow']['path'][path]['mode'] = aa[profile][hat]['allow']['path'][path].get('mode', set()) | mode

                                    tmpmode = set()
                                    if audit_toggle == 1:
                                        tmpmode = mode - allow_mode
                                    elif audit_toggle == 2:
                                        tmpmode = mode

                                    aa[profile][hat]['allow']['path'][path]['audit'] = aa[profile][hat]['allow']['path'][path].get('audit', set()) | tmpmode

                                    changed[profile] = True

                                    aaui.UI_Info(_('Adding %(path)s %(mode)s to profile') % { 'path': path, 'mode': mode_to_str_user(mode) })
                                    if deleted:
                                        aaui.UI_Info(_('Deleted %s previous matching profile entries.') % deleted)

                            elif ans == 'CMD_DENY':
                                path = options[selected].strip()
                                # Add new entry?
                                aa[profile][hat]['deny']['path'][path]['mode'] = aa[profile][hat]['deny']['path'][path].get('mode', set()) | (mode - allow_mode)

                                aa[profile][hat]['deny']['path'][path]['audit'] = aa[profile][hat]['deny']['path'][path].get('audit', set())

                                changed[profile] = True

                                done = True

                            elif ans == 'CMD_NEW':
                                arg = options[selected]
                                if not re_match_include(arg):
                                    ans = aaui.UI_GetString(_('Enter new path: '), arg)
                                    if ans:
                                        if not matchliteral(ans, path):
                                            ynprompt = _('The specified path does not match this log entry:\n\n  Log Entry: %(path)s\n  Entered Path:  %(ans)s\nDo you really want to use this path?') % { 'path': path, 'ans': ans }
                                            key = aaui.UI_YesNo(ynprompt, 'n')
                                            if key == 'n':
                                                continue

                                        user_globs.append(ans)
                                        options, default_option = add_to_options(options, ans)

                            elif ans == 'CMD_GLOB':
                                newpath = options[selected].strip()
                                if not re_match_include(newpath):
                                    newpath = glob_path(newpath)
                                    options, default_option = add_to_options(options, newpath)

                            elif ans == 'CMD_GLOBEXT':
                                newpath = options[selected].strip()
                                if not re_match_include(newpath):
                                    newpath = glob_path_withext(newpath)
                                    options, default_option = add_to_options(options, newpath)

                            elif re.search('\d', ans):
                                default_option = ans

def available_buttons(rule_obj):
    buttons = []

    if not rule_obj.deny:
        buttons += ['CMD_ALLOW']

    buttons += ['CMD_DENY', 'CMD_IGNORE_ENTRY']

    if rule_obj.audit:
        buttons += ['CMD_AUDIT_OFF']
    else:
        buttons += ['CMD_AUDIT_NEW']

    buttons += ['CMD_ABORT', 'CMD_FINISHED']

    return buttons

def add_to_options(options, newpath):
    if newpath not in options:
        options.append(newpath)

    default_option = options.index(newpath) + 1
    return (options, default_option)

def glob_path(newpath):
    """Glob the given file path"""
    if newpath[-1] == '/':
        if newpath[-4:] == '/**/' or newpath[-3:] == '/*/':
            # /foo/**/ and /foo/*/ => /**/
            newpath = re.sub('/[^/]+/\*{1,2}/$', '/**/', newpath)  # re.sub('/[^/]+/\*{1,2}$/', '/\*\*/', newpath)
        elif re.search('/[^/]+\*\*[^/]*/$', newpath):
            # /foo**/ and /foo**bar/ => /**/
            newpath = re.sub('/[^/]+\*\*[^/]*/$', '/**/', newpath)
        elif re.search('/\*\*[^/]+/$', newpath):
            # /**bar/ => /**/
            newpath = re.sub('/\*\*[^/]+/$', '/**/', newpath)
        else:
            newpath = re.sub('/[^/]+/$', '/*/', newpath)
    else:
            if newpath[-3:] == '/**' or newpath[-2:] == '/*':
                # /foo/** and /foo/* => /**
                newpath = re.sub('/[^/]+/\*{1,2}$', '/**', newpath)
            elif re.search('/[^/]*\*\*[^/]+$', newpath):
                # /**foo and /foor**bar => /**
                newpath = re.sub('/[^/]*\*\*[^/]+$', '/**', newpath)
            elif re.search('/[^/]+\*\*$', newpath):
                # /foo** => /**
                newpath = re.sub('/[^/]+\*\*$', '/**', newpath)
            else:
                newpath = re.sub('/[^/]+$', '/*', newpath)
    return newpath

def glob_path_withext(newpath):
    """Glob given file path with extension"""
    # match /**.ext and /*.ext
    match = re.search('/\*{1,2}(\.[^/]+)$', newpath)
    if match:
        # /foo/**.ext and /foo/*.ext => /**.ext
        newpath = re.sub('/[^/]+/\*{1,2}\.[^/]+$', '/**' + match.groups()[0], newpath)
    elif re.search('/[^/]+\*\*[^/]*\.[^/]+$', newpath):
        # /foo**.ext and /foo**bar.ext => /**.ext
        match = re.search('/[^/]+\*\*[^/]*(\.[^/]+)$', newpath)
        newpath = re.sub('/[^/]+\*\*[^/]*\.[^/]+$', '/**' + match.groups()[0], newpath)
    elif re.search('/\*\*[^/]+\.[^/]+$', newpath):
        # /**foo.ext => /**.ext
        match = re.search('/\*\*[^/]+(\.[^/]+)$', newpath)
        newpath = re.sub('/\*\*[^/]+\.[^/]+$', '/**' + match.groups()[0], newpath)
    else:
        match = re.search('(\.[^/]+)$', newpath)
        if match:
            newpath = re.sub('/[^/]+(\.[^/]+)$', '/*' + match.groups()[0], newpath)
    return newpath

def delete_path_duplicates(profile, incname, allow):
    deleted = []
    for entry in profile[allow]['path'].keys():
        if entry == '#include <%s>' % incname:
            continue
        # XXX Make this code smart enough to know that bare file rules
        #     makes some path rules unnecessary. For example, "/dev/random r,"
        #     would no longer be needed if "file," was present.
        cm, am, m = match_include_to_path(incname, allow, entry)
        if cm and mode_contains(cm, profile[allow]['path'][entry]['mode']) and mode_contains(am, profile[allow]['path'][entry]['audit']):
            deleted.append(entry)

    for entry in deleted:
        profile[allow]['path'].pop(entry)
    return len(deleted)

def delete_duplicates(profile, incname):
    deleted = 0
    # Allow rules covered by denied rules shouldn't be deleted
    # only a subset allow rules may actually be denied

    if include.get(incname, False):
        for rule_type in ruletypes:
            deleted += profile[rule_type].delete_duplicates(include[incname][incname][rule_type])

        deleted += delete_path_duplicates(profile, incname, 'allow')
        deleted += delete_path_duplicates(profile, incname, 'deny')

    elif filelist.get(incname, False):
        for rule_type in ruletypes:
            deleted += profile[rule_type].delete_duplicates(filelist[incname][incname][rule_type])

        deleted += delete_path_duplicates(profile, incname, 'allow')
        deleted += delete_path_duplicates(profile, incname, 'deny')

    return deleted

def match_includes(profile, rule_type, rule_obj):
    newincludes = []
    for incname in include.keys():
        # XXX type check should go away once we init all profiles correctly
        if valid_include(profile, incname) and include[incname][incname].get(rule_type, False) and include[incname][incname][rule_type].is_covered(rule_obj):
            newincludes.append(incname)

    return newincludes

def valid_include(profile, incname):
    if profile and profile['include'].get(incname, False):
        return False

    if cfg['settings']['custom_includes']:
        for incm in cfg['settings']['custom_includes'].split():
            if incm == incname:
                return True

    if incname.startswith('abstractions/') and os.path.isfile(profile_dir + '/' + incname):
        return True

    return False

def set_logfile(filename):
    ''' set logfile to a) the specified filename or b) if not given, the first existing logfile from logprof.conf'''

    global logfile

    if filename:
        logfile = filename
    else:
        logfile = conf.find_first_file(cfg['settings']['logfiles']) or '/var/log/syslog'

    if not os.path.exists(logfile):
        if filename:
            raise AppArmorException(_('The logfile %s does not exist. Please check the path') % logfile)
        else:
            raise AppArmorException('Can\'t find system log "%s".' % (logfile))
    elif os.path.isdir(logfile):
        raise AppArmorException(_('%s is a directory. Please specify a file as logfile') % logfile)

def do_logprof_pass(logmark='', passno=0, pid=pid):
    # set up variables for this pass
#    t = hasher()
#    transitions = hasher()
#    seen = hasher()  # XXX global?
    global log
    log = []
    global existing_profiles
    global sev_db
#    aa = hasher()
#    profile_changes = hasher()
#     prelog = hasher()
#     log_dict = hasher()
#     changed = dict()
#    skip = hasher()  # XXX global?
#    filelist = hasher()

    aaui.UI_Info(_('Reading log entries from %s.') % logfile)

    if not passno:
        aaui.UI_Info(_('Updating AppArmor profiles in %s.') % profile_dir)
        read_profiles()

    if not sev_db:
        sev_db = apparmor.severity.Severity(CONFDIR + '/severity.db', _('unknown'))
    #print(pid)
    #print(existing_profiles)
    ##if not repo_cf and cfg['repostory']['url']:
    ##    repo_cfg = read_config('repository.conf')
    ##    if not repo_cfg['repository'].get('enabled', False) or repo_cfg['repository]['enabled'] not in ['yes', 'no']:
    ##    UI_ask_to_enable_repo()

    log_reader = apparmor.logparser.ReadLog(pid, logfile, existing_profiles, profile_dir, log)
    log = log_reader.read_log(logmark)
    #read_log(logmark)

    for root in log:
        handle_children('', '', root)
    #for root in range(len(log)):
        #log[root] = handle_children('', '', log[root])
    #print(log)
    for pid in sorted(profile_changes.keys()):
        set_process(pid, profile_changes[pid])

    collapse_log()

    ask_the_questions()

    if aaui.UI_mode == 'yast':
        # To-Do
        pass

    finishing = False
    # Check for finished
    save_profiles()

    ##if not repo_cfg['repository'].get('upload', False) or repo['repository']['upload'] == 'later':
    ##    UI_ask_to_upload_profiles()
    ##if repo_enabled():
    ##    if repo_cgf['repository']['upload'] == 'yes':
    ##        sync_profiles()
    ##    created = []

    # If user selects 'Finish' then we want to exit logprof
    if finishing:
        return 'FINISHED'
    else:
        return 'NORMAL'


def save_profiles():
    # Ensure the changed profiles are actual active profiles
    for prof_name in changed.keys():
        if not is_active_profile(prof_name):
            print("*** save_profiles(): removing %s" % prof_name)
            print('*** This should not happen. Please open a bugreport!')
            changed.pop(prof_name)

    changed_list = sorted(changed.keys())

    if changed_list:

        if aaui.UI_mode == 'yast':
            # To-Do
            # selected_profiles = []  # XXX selected_profiles_ref?
            profile_changes = dict()
            for prof in changed_list:
                oldprofile = serialize_profile(original_aa[prof], prof)
                newprofile = serialize_profile(aa[prof], prof)
                profile_changes[prof] = get_profile_diff(oldprofile, newprofile)
            explanation = _('Select which profile changes you would like to save to the\nlocal profile set.')
            title = _('Local profile changes')
            SendDataToYast({'type': 'dialog-select-profiles',
                            'title': title,
                            'explanation': explanation,
                            'dialog_select': 'true',
                            'get_changelog': 'false',
                            'profiles': profile_changes
                            })
            ypath, yarg = GetDataFromYast()
            if yarg['STATUS'] == 'cancel':
                return None
            else:
                selected_profiles_ref = yarg['PROFILES']
                for profile_name in selected_profiles_ref:
                    write_profile_ui_feedback(profile_name)
                    reload_base(profile_name)

        else:
            q = aaui.PromptQuestion()
            q.title = 'Changed Local Profiles'
            q.explanation = _('The following local profiles were changed. Would you like to save them?')
            q.functions = ['CMD_SAVE_CHANGES', 'CMD_SAVE_SELECTED', 'CMD_VIEW_CHANGES', 'CMD_VIEW_CHANGES_CLEAN', 'CMD_ABORT']
            q.default = 'CMD_VIEW_CHANGES'
            q.options = changed
            q.selected = 0
            ans = ''
            arg = None
            while ans != 'CMD_SAVE_CHANGES':
                if not changed:
                    return
                ans, arg = q.promptUser()
                if ans == 'CMD_SAVE_SELECTED':
                    profile_name = list(changed.keys())[arg]
                    write_profile_ui_feedback(profile_name)
                    reload_base(profile_name)

                elif ans == 'CMD_VIEW_CHANGES':
                    which = list(changed.keys())[arg]
                    oldprofile = None
                    if aa[which][which].get('filename', False):
                        oldprofile = aa[which][which]['filename']
                    else:
                        oldprofile = get_profile_filename(which)

                    try:
                        newprofile = serialize_profile_from_old_profile(aa[which], which, '')
                    except AttributeError:
                        # see https://bugs.launchpad.net/ubuntu/+source/apparmor/+bug/1528139
                        newprofile = "###\n###\n### Internal error while generating diff, please use '%s' instead\n###\n###\n" % _('View Changes b/w (C)lean profiles')

                    display_changes_with_comments(oldprofile, newprofile)

                elif ans == 'CMD_VIEW_CHANGES_CLEAN':
                    which = list(changed.keys())[arg]
                    oldprofile = serialize_profile(original_aa[which], which, '')
                    newprofile = serialize_profile(aa[which], which, '')

                    display_changes(oldprofile, newprofile)

            for profile_name in sorted(changed.keys()):
                write_profile_ui_feedback(profile_name)
                reload_base(profile_name)

def get_pager():
    pass

def generate_diff(oldprofile, newprofile):
    oldtemp = tempfile.NamedTemporaryFile('w')

    oldtemp.write(oldprofile)
    oldtemp.flush()

    newtemp = tempfile.NamedTemporaryFile('w')
    newtemp.write(newprofile)
    newtemp.flush()

    difftemp = tempfile.NamedTemporaryFile('w', delete=False)

    subprocess.call('diff -u -p %s %s > %s' % (oldtemp.name, newtemp.name, difftemp.name), shell=True)

    oldtemp.close()
    newtemp.close()
    return difftemp

def get_profile_diff(oldprofile, newprofile):
    difftemp = generate_diff(oldprofile, newprofile)
    diff = []
    with open_file_read(difftemp.name) as f_in:
        for line in f_in:
            if not (line.startswith('---') and line .startswith('+++') and line.startswith('@@')):
                    diff.append(line)

    difftemp.delete = True
    difftemp.close()
    return ''.join(diff)

def display_changes(oldprofile, newprofile):
    if aaui.UI_mode == 'yast':
        aaui.UI_LongMessage(_('Profile Changes'), get_profile_diff(oldprofile, newprofile))
    else:
        difftemp = generate_diff(oldprofile, newprofile)
        subprocess.call('less %s' % difftemp.name, shell=True)
        difftemp.delete = True
        difftemp.close()

def display_changes_with_comments(oldprofile, newprofile):
    """Compare the new profile with the existing profile inclusive of all the comments"""
    if not os.path.exists(oldprofile):
        raise AppArmorException(_("Can't find existing profile %s to compare changes.") % oldprofile)
    if aaui.UI_mode == 'yast':
        #To-Do
        pass
    else:
        newtemp = tempfile.NamedTemporaryFile('w')
        newtemp.write(newprofile)
        newtemp.flush()

        difftemp = tempfile.NamedTemporaryFile('w')

        subprocess.call('diff -u -p %s %s > %s' % (oldprofile, newtemp.name, difftemp.name), shell=True)

        newtemp.close()
        subprocess.call('less %s' % difftemp.name, shell=True)
        difftemp.close()

def set_process(pid, profile):
    # If process not running don't do anything
    if not os.path.exists('/proc/%s/attr/current' % pid):
        return None

    process = None
    try:
        process = open_file_read('/proc/%s/attr/current' % pid)
    except IOError:
        return None
    current = process.readline().strip()
    process.close()

    if not re.search('^null(-complain)*-profile$', current):
        return None

    stats = None
    try:
        stats = open_file_read('/proc/%s/stat' % pid)
    except IOError:
        return None
    stat = stats.readline().strip()
    stats.close()

    match = re.search('^\d+ \((\S+)\) ', stat)
    if not match:
        return None

    try:
        process = open_file_write('/proc/%s/attr/current' % pid)
    except IOError:
        return None
    process.write('setprofile %s' % profile)
    process.close()

def collapse_log():
    for aamode in prelog.keys():
        for profile in prelog[aamode].keys():
            for hat in prelog[aamode][profile].keys():

                log_dict[aamode][profile][hat] = profile_storage(profile, hat, 'collapse_log()')

                for path in prelog[aamode][profile][hat]['path'].keys():
                    mode = prelog[aamode][profile][hat]['path'][path]

                    combinedmode = set()
                    # Is path in original profile?
                    if aa[profile][hat]['allow']['path'].get(path, False):
                        combinedmode |= aa[profile][hat]['allow']['path'][path]['mode']

                    # Match path to regexps in profile
                    combinedmode |= rematchfrag(aa[profile][hat], 'allow', path)[0]

                    # Match path from includes

                    combinedmode |= match_prof_incs_to_path(aa[profile][hat], 'allow', path)[0]

                    if not combinedmode or not mode_contains(combinedmode, mode):
                        if log_dict[aamode][profile][hat]['allow']['path'].get(path, False):
                            mode |= log_dict[aamode][profile][hat]['allow']['path'][path]

                        log_dict[aamode][profile][hat]['allow']['path'][path] = mode

                for cap in prelog[aamode][profile][hat]['capability'].keys():
                    cap_event = CapabilityRule(cap, log_event=True)
                    if not is_known_rule(aa[profile][hat], 'capability', cap_event):
                        log_dict[aamode][profile][hat]['capability'].add(cap_event)

                nd = prelog[aamode][profile][hat]['netdomain']
                for family in nd.keys():
                    for sock_type in nd[family].keys():
                        net_event = NetworkRule(family, sock_type, log_event=True)
                        if not is_known_rule(aa[profile][hat], 'network', net_event):
                            log_dict[aamode][profile][hat]['network'].add(net_event)

                ptrace = prelog[aamode][profile][hat]['ptrace']
                for peer in ptrace.keys():
                    for access in ptrace[peer].keys():
                        ptrace_event = PtraceRule(access, peer, log_event=True)
                        if not is_known_rule(aa[profile][hat], 'ptrace', ptrace_event):
                            log_dict[aamode][profile][hat]['ptrace'].add(ptrace_event)

                sig = prelog[aamode][profile][hat]['signal']
                for peer in sig.keys():
                    for access in sig[peer].keys():
                        for signal in sig[peer][access].keys():
                            signal_event = SignalRule(access, signal, peer, log_event=True)
                            if not is_known_rule(aa[profile][hat], 'signal', signal_event):
                                log_dict[aamode][profile][hat]['signal'].add(signal_event)


PROFILE_MODE_RE      = re.compile('^(r|w|l|m|k|a|ix|ux|px|pux|cx|pix|cix|cux|Ux|Px|PUx|Cx|Pix|Cix|CUx)+$')
PROFILE_MODE_DENY_RE = re.compile('^(r|w|l|m|k|a|x)+$')

def validate_profile_mode(mode, allow, nt_name=None):
    if allow == 'deny':
        if PROFILE_MODE_DENY_RE.search(mode):
            return True
        else:
            return False

    else:
        if PROFILE_MODE_RE.search(mode):
            return True
        else:
            return False


def is_skippable_file(path):
    """Returns True if filename matches something to be skipped (rpm or dpkg backup files, hidden files etc.)
        The list of skippable files needs to be synced with apparmor initscript and libapparmor _aa_is_blacklisted()
        path: filename (with or without directory)"""

    basename = os.path.basename(path)

    if not basename or basename[0] == '.' or basename == 'README':
        return True

    skippable_suffix = ('.dpkg-new', '.dpkg-old', '.dpkg-dist', '.dpkg-bak', '.rpmnew', '.rpmsave', '.orig', '.rej', '~')
    if basename.endswith(skippable_suffix):
        return True

    return False

def is_skippable_dir(path):
    if re.search('^(.*/)?(disable|cache|force-complain|lxc)/?$', path):
        return True
    return False

def read_profiles():
    # we'll read all profiles from disk, so reset the storage first (autodep() might have created/stored
    # a profile already, which would cause a 'Conflicting profile' error in attach_profile_data())
    global aa, original_aa
    aa = hasher()
    original_aa = hasher()

    try:
        os.listdir(profile_dir)
    except:
        fatal_error(_("Can't read AppArmor profiles in %s") % profile_dir)

    for file in os.listdir(profile_dir):
        if os.path.isfile(profile_dir + '/' + file):
            if is_skippable_file(file):
                continue
            else:
                read_profile(profile_dir + '/' + file, True)

def read_inactive_profiles():
    if not os.path.exists(extra_profile_dir):
        return None
    try:
        os.listdir(profile_dir)
    except:
        fatal_error(_("Can't read AppArmor profiles in %s") % extra_profile_dir)

    for file in os.listdir(profile_dir):
        if os.path.isfile(extra_profile_dir + '/' + file):
            if is_skippable_file(file):
                continue
            else:
                read_profile(extra_profile_dir + '/' + file, False)

def read_profile(file, active_profile):
    data = None
    try:
        with open_file_read(file) as f_in:
            data = f_in.readlines()
    except IOError:
        debug_logger.debug("read_profile: can't read %s - skipping" % file)
        return None

    profile_data = parse_profile_data(data, file, 0)

    if profile_data and active_profile:
        attach_profile_data(aa, profile_data)
        attach_profile_data(original_aa, profile_data)
    elif profile_data:
        attach_profile_data(extras, profile_data)


def attach_profile_data(profiles, profile_data):
    # Make deep copy of data to avoid changes to
    # arising due to mutables
    for p in profile_data.keys():
        if profiles.get(p, False):
            for hat in profile_data[p].keys():
                if profiles[p].get(hat, False):
                    raise AppArmorException(_("Conflicting profiles for %s defined in two files:\n- %s\n- %s") %
                            (combine_name(p, hat), profiles[p][hat]['filename'], profile_data[p][hat]['filename']))

        profiles[p] = deepcopy(profile_data[p])


def parse_profile_start(line, file, lineno, profile, hat):
    matches = parse_profile_start_line(line, file)

    if profile:  # we are inside a profile, so we expect a child profile
        if not matches['profile_keyword']:
            raise AppArmorException(_('%(profile)s profile in %(file)s contains syntax errors in line %(line)s: missing "profile" keyword.') % {
                    'profile': profile, 'file': file, 'line': lineno + 1 })
        if profile != hat:
            # nesting limit reached - a child profile can't contain another child profile
            raise AppArmorException(_('%(profile)s profile in %(file)s contains syntax errors in line %(line)s: a child profile inside another child profile is not allowed.') % {
                    'profile': profile, 'file': file, 'line': lineno + 1 })

        hat = matches['profile']
        in_contained_hat = True
        pps_set_profile = True
        pps_set_hat_external = False

    else:  # stand-alone profile
        profile = matches['profile']
        if len(profile.split('//')) >= 2:
            profile, hat = profile.split('//')[:2]
            pps_set_hat_external = True
        else:
            hat = profile
            pps_set_hat_external = False

        in_contained_hat = False
        pps_set_profile = False

    attachment = matches['attachment']
    flags = matches['flags']

    return (profile, hat, attachment, flags, in_contained_hat, pps_set_profile, pps_set_hat_external)

def parse_profile_data(data, file, do_include):
    profile_data = hasher()
    profile = None
    hat = None
    in_contained_hat = None
    repo_data = None
    parsed_profiles = []
    initial_comment = ''
    lastline = None

    if do_include:
        profile = file
        hat = file
        profile_data[profile][hat] = profile_storage(profile, hat, 'parse_profile_data() do_include')
        profile_data[profile][hat]['filename'] = file

    for lineno, line in enumerate(data):
        line = line.strip()
        if not line:
            continue
        # we're dealing with a multiline statement
        if lastline:
            line = '%s %s' % (lastline, line)
            lastline = None
        # Starting line of a profile
        if RE_PROFILE_START.search(line):
            (profile, hat, attachment, flags, in_contained_hat, pps_set_profile, pps_set_hat_external) = parse_profile_start(line, file, lineno, profile, hat)

            if profile_data[profile].get(hat, False):
                raise AppArmorException('Profile %(profile)s defined twice in %(file)s, last found in line %(line)s' %
                    { 'file': file, 'line': lineno + 1, 'profile': combine_name(profile, hat) })

            profile_data[profile][hat] = profile_storage(profile, hat, 'parse_profile_data() profile_start')

            if attachment:
                profile_data[profile][hat]['attachment'] = attachment
            if pps_set_profile:
                profile_data[profile][hat]['profile'] = True
            if pps_set_hat_external:
                profile_data[profile][hat]['external'] = True

            # Profile stored
            existing_profiles[profile] = file

            # save profile name and filename
            profile_data[profile][hat]['name'] = profile
            profile_data[profile][hat]['filename'] = file
            filelist[file]['profiles'][profile][hat] = True

            profile_data[profile][hat]['flags'] = flags

            # Save the initial comment
            if initial_comment:
                profile_data[profile][hat]['initial_comment'] = initial_comment

            initial_comment = ''

            if repo_data:
                profile_data[profile][profile]['repo']['url'] = repo_data['url']
                profile_data[profile][profile]['repo']['user'] = repo_data['user']

        elif RE_PROFILE_END.search(line):
            # If profile ends and we're not in one
            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected End of Profile reached in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            if in_contained_hat:
                hat = profile
                in_contained_hat = False
            else:
                parsed_profiles.append(profile)
                profile = None

            initial_comment = ''

        elif CapabilityRule.match(line):
            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected capability entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            profile_data[profile][hat]['capability'].add(CapabilityRule.parse(line))

        elif RE_PROFILE_LINK.search(line):
            matches = RE_PROFILE_LINK.search(line).groups()

            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected link entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            audit = False
            if matches[0]:
                audit = True

            allow = 'allow'
            if matches[1] and matches[1].strip() == 'deny':
                allow = 'deny'

            subset = matches[3]
            link = strip_quotes(matches[6])
            value = strip_quotes(matches[7])
            profile_data[profile][hat][allow]['link'][link]['to'] = value
            profile_data[profile][hat][allow]['link'][link]['mode'] = profile_data[profile][hat][allow]['link'][link].get('mode', set()) | apparmor.aamode.AA_MAY_LINK

            if subset:
                profile_data[profile][hat][allow]['link'][link]['mode'] |= apparmor.aamode.AA_LINK_SUBSET

            if audit:
                profile_data[profile][hat][allow]['link'][link]['audit'] = profile_data[profile][hat][allow]['link'][link].get('audit', set()) | apparmor.aamode.AA_LINK_SUBSET
            else:
                profile_data[profile][hat][allow]['link'][link]['audit'] = set()

        elif ChangeProfileRule.match(line):
            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected change profile entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            profile_data[profile][hat]['change_profile'].add(ChangeProfileRule.parse(line))

        elif RE_PROFILE_ALIAS.search(line):
            matches = RE_PROFILE_ALIAS.search(line).groups()

            from_name = strip_quotes(matches[0])
            to_name = strip_quotes(matches[1])

            if profile:
                profile_data[profile][hat]['alias'][from_name] = to_name
            else:
                if not filelist.get(file, False):
                    filelist[file] = hasher()
                filelist[file]['alias'][from_name] = to_name

        elif RlimitRule.match(line):
            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected rlimit entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            profile_data[profile][hat]['rlimit'].add(RlimitRule.parse(line))

        elif RE_PROFILE_BOOLEAN.search(line):
            matches = RE_PROFILE_BOOLEAN.search(line).groups()

            if profile and not do_include:
                raise AppArmorException(_('Syntax Error: Unexpected boolean definition found inside profile in file: %(file)s line: %(line)s') % {
                        'file': file, 'line': lineno + 1 })

            bool_var = matches[0]
            value = matches[1]

            profile_data[profile][hat]['lvar'][bool_var] = value

        elif RE_PROFILE_VARIABLE.search(line):
            # variable additions += and =
            matches = RE_PROFILE_VARIABLE.search(line).groups()

            list_var = strip_quotes(matches[0])
            var_operation = matches[1]
            value = matches[2]

            if profile:
                if not profile_data[profile][hat].get('lvar', False):
                    profile_data[profile][hat]['lvar'][list_var] = []
                store_list_var(profile_data[profile]['lvar'], list_var, value, var_operation, file)
            else:
                if not filelist[file].get('lvar', False):
                    filelist[file]['lvar'][list_var] = []
                store_list_var(filelist[file]['lvar'], list_var, value, var_operation, file)

        elif RE_PROFILE_CONDITIONAL.search(line):
            # Conditional Boolean
            pass

        elif RE_PROFILE_CONDITIONAL_VARIABLE.search(line):
            # Conditional Variable defines
            pass

        elif RE_PROFILE_CONDITIONAL_BOOLEAN.search(line):
            # Conditional Boolean defined
            pass

        elif RE_PROFILE_BARE_FILE_ENTRY.search(line):
            matches = RE_PROFILE_BARE_FILE_ENTRY.search(line)

            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected bare file rule found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            audit, deny, allow_keyword, comment = parse_modifiers(matches)
            # TODO: honor allow_keyword and comment
            if deny:
                allow = 'deny'
            else:
                allow = 'allow'

            mode = apparmor.aamode.AA_BARE_FILE_MODE
            if not matches.group('owner'):
                mode |= AA_OTHER(apparmor.aamode.AA_BARE_FILE_MODE)

            path_rule = profile_data[profile][hat][allow]['path'][ALL]
            path_rule['mode'] = mode
            path_rule['audit'] = set()
            if audit:
                path_rule['audit'] = mode
            path_rule['file_prefix'] = True

        elif RE_PROFILE_PATH_ENTRY.search(line):
            matches = RE_PROFILE_PATH_ENTRY.search(line).groups()

            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected path entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            audit = False
            if matches[0]:
                audit = True

            allow = 'allow'
            if matches[1] and matches[1].strip() == 'deny':
                allow = 'deny'

            user = False
            if matches[2]:
                user = True

            file_prefix = False
            if matches[3]:
                file_prefix = True

            path = strip_quotes(matches[4].strip())
            mode = matches[5]
            nt_name = matches[7]
            if nt_name:
                nt_name = nt_name.strip()

            p_re = convert_regexp(path)
            try:
                re.compile(p_re)
            except:
                raise AppArmorException(_('Syntax Error: Invalid Regex %(path)s in file: %(file)s line: %(line)s') % { 'path': path, 'file': file, 'line': lineno + 1 })

            if not validate_profile_mode(mode, allow, nt_name):
                raise AppArmorException(_('Invalid mode %(mode)s in file: %(file)s line: %(line)s') % {'mode': mode, 'file': file, 'line': lineno + 1 })

            tmpmode = set()
            if user:
                tmpmode = str_to_mode('%s::' % mode)
            else:
                tmpmode = str_to_mode(mode)

            profile_data[profile][hat][allow]['path'][path]['mode'] = profile_data[profile][hat][allow]['path'][path].get('mode', set()) | tmpmode

            if file_prefix:
                profile_data[profile][hat][allow]['path'][path]['file_prefix'] = True

            if nt_name:
                profile_data[profile][hat][allow]['path'][path]['to'] = nt_name

            if audit:
                profile_data[profile][hat][allow]['path'][path]['audit'] = profile_data[profile][hat][allow]['path'][path].get('audit', set()) | tmpmode
            else:
                profile_data[profile][hat][allow]['path'][path]['audit'] = set()

        elif re_match_include(line):
            # Include files
            include_name = re_match_include(line)
            if include_name.startswith('local/'):
                profile_data[profile][hat]['localinclude'][include_name] = True

            if profile:
                profile_data[profile][hat]['include'][include_name] = True
            else:
                if not filelist.get(file):
                    filelist[file] = hasher()
                filelist[file]['include'][include_name] = True
            # If include is a directory
            if os.path.isdir(profile_dir + '/' + include_name):
                for file_name in include_dir_filelist(profile_dir, include_name):
                    if not include.get(file_name, False):
                        load_include(file_name)
            else:
                if not include.get(include_name, False):
                    load_include(include_name)

        elif NetworkRule.match(line):
            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected network entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            # init rule class (if not done yet)
            if not profile_data[profile][hat].get('network', False):
                profile_data[profile][hat]['network'] = NetworkRuleset()

            profile_data[profile][hat]['network'].add(NetworkRule.parse(line))

        elif RE_PROFILE_DBUS.search(line):
            matches = RE_PROFILE_DBUS.search(line).groups()

            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected dbus entry found in file: %(file)s line: %(line)s') % {'file': file, 'line': lineno + 1 })

            audit = False
            if matches[0]:
                audit = True
            allow = 'allow'
            if matches[1] and matches[1].strip() == 'deny':
                allow = 'deny'
            dbus = matches[2]

            #parse_dbus_rule(profile_data[profile], dbus, audit, allow)
            dbus_rule = parse_dbus_rule(dbus)
            dbus_rule.audit = audit
            dbus_rule.deny = (allow == 'deny')

            dbus_rules = profile_data[profile][hat][allow].get('dbus', list())
            dbus_rules.append(dbus_rule)
            profile_data[profile][hat][allow]['dbus'] = dbus_rules

        elif RE_PROFILE_MOUNT.search(line):
            matches = RE_PROFILE_MOUNT.search(line).groups()

            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected mount entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            audit = False
            if matches[0]:
                audit = True
            allow = 'allow'
            if matches[1] and matches[1].strip() == 'deny':
                allow = 'deny'
            mount = matches[2]

            mount_rule = parse_mount_rule(mount)
            mount_rule.audit = audit
            mount_rule.deny = (allow == 'deny')

            mount_rules = profile_data[profile][hat][allow].get('mount', list())
            mount_rules.append(mount_rule)
            profile_data[profile][hat][allow]['mount'] = mount_rules

        elif SignalRule.match(line):
            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected signal entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            profile_data[profile][hat]['signal'].add(SignalRule.parse(line))

        elif PtraceRule.match(line):
            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected ptrace entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            profile_data[profile][hat]['ptrace'].add(PtraceRule.parse(line))

        elif RE_PROFILE_PIVOT_ROOT.search(line):
            matches = RE_PROFILE_PIVOT_ROOT.search(line).groups()

            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected pivot_root entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            audit = False
            if matches[0]:
                audit = True
            allow = 'allow'
            if matches[1] and matches[1].strip() == 'deny':
                allow = 'deny'
            pivot_root = matches[2].strip()

            pivot_root_rule = parse_pivot_root_rule(pivot_root)
            pivot_root_rule.audit = audit
            pivot_root_rule.deny = (allow == 'deny')

            pivot_root_rules = profile_data[profile][hat][allow].get('pivot_root', list())
            pivot_root_rules.append(pivot_root_rule)
            profile_data[profile][hat][allow]['pivot_root'] = pivot_root_rules

        elif RE_PROFILE_UNIX.search(line):
            matches = RE_PROFILE_UNIX.search(line).groups()

            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected unix entry found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            audit = False
            if matches[0]:
                audit = True
            allow = 'allow'
            if matches[1] and matches[1].strip() == 'deny':
                allow = 'deny'
            unix = matches[2].strip()

            unix_rule = parse_unix_rule(unix)
            unix_rule.audit = audit
            unix_rule.deny = (allow == 'deny')

            unix_rules = profile_data[profile][hat][allow].get('unix', list())
            unix_rules.append(unix_rule)
            profile_data[profile][hat][allow]['unix'] = unix_rules

        elif RE_PROFILE_CHANGE_HAT.search(line):
            matches = RE_PROFILE_CHANGE_HAT.search(line).groups()

            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected change hat declaration found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            aaui.UI_Important(_('Ignoring no longer supported change hat declaration "^%(hat)s," found in file: %(file)s line: %(line)s') % {
                    'hat': matches[0], 'file': file, 'line': lineno + 1 })

        elif RE_PROFILE_HAT_DEF.search(line):
            # An embedded hat syntax definition starts
            matches = RE_PROFILE_HAT_DEF.search(line)
            if not profile:
                raise AppArmorException(_('Syntax Error: Unexpected hat definition found in file: %(file)s line: %(line)s') % { 'file': file, 'line': lineno + 1 })

            in_contained_hat = True
            hat = matches.group('hat')
            hat = strip_quotes(hat)

            # if hat is already known, the filelist check some lines below will error out.
            # nevertheless, just to be sure, don't overwrite existing profile_data.
            if not profile_data[profile].get(hat, False):
                profile_data[profile][hat] = profile_storage(profile, hat, 'parse_profile_data() hat_def')
                profile_data[profile][hat]['filename'] = file

            flags = matches.group('flags')

            profile_data[profile][hat]['flags'] = flags

            if initial_comment:
                profile_data[profile][hat]['initial_comment'] = initial_comment
            initial_comment = ''
            if filelist[file]['profiles'][profile].get(hat, False) and not do_include:
                raise AppArmorException(_('Error: Multiple definitions for hat %(hat)s in profile %(profile)s.') % { 'hat': hat, 'profile': profile })
            filelist[file]['profiles'][profile][hat] = True

        elif line[0] == '#':
            # Handle initial comments
            if not profile:
                if line.startswith('# Last Modified:'):
                    continue
                elif line.startswith('# REPOSITORY:'): # TODO: allow any number of spaces/tabs
                    parts = line.split()
                    if len(parts) == 3 and parts[2] == 'NEVERSUBMIT':
                        repo_data = {'neversubmit': True}
                    elif len(parts) == 5:
                        repo_data = {'url': parts[2],
                                     'user': parts[3],
                                     'id': parts[4]}
                    else:
                        aaui.UI_Important(_('Warning: invalid "REPOSITORY:" line in %s, ignoring.') % file)
                        initial_comment = initial_comment + line + '\n'
                else:
                    initial_comment = initial_comment + line + '\n'

        elif not RE_RULE_HAS_COMMA.search(line):
            # Bah, line continues on to the next line
            if RE_HAS_COMMENT_SPLIT.search(line):
                # filter trailing comments
                lastline = RE_HAS_COMMENT_SPLIT.search(line).group('not_comment')
            else:
                lastline = line
        else:
            raise AppArmorException(_('Syntax Error: Unknown line found in file %(file)s line %(lineno)s:\n    %(line)s') % { 'file': file, 'lineno': lineno + 1, 'line': line })

    # Below is not required I'd say
    if not do_include:
        for hatglob in cfg['required_hats'].keys():
            for parsed_prof in sorted(parsed_profiles):
                if re.search(hatglob, parsed_prof):
                    for hat in cfg['required_hats'][hatglob].split():
                        if not profile_data[parsed_prof].get(hat, False):
                            profile_data[parsed_prof][hat] = profile_storage(parsed_prof, hat, 'parse_profile_data() required_hats')

    # End of file reached but we're stuck in a profile
    if profile and not do_include:
        raise AppArmorException(_("Syntax Error: Missing '}' or ','. Reached end of file %(file)s while inside profile %(profile)s") % { 'file': file, 'profile': profile })

    return profile_data

# RE_DBUS_ENTRY = re.compile('^dbus\s*()?,\s*$')
#   use stuff like '(?P<action>(send|write|w|receive|read|r|rw))'

def parse_dbus_rule(line):
    # XXX Do real parsing here
    return aarules.Raw_DBUS_Rule(line)

    #matches = RE_DBUS_ENTRY.search(line).groups()
    #if len(matches) == 1:
        # XXX warn?
        # matched nothing
    #    print('no matches')
    #    return aarules.DBUS_Rule()
    #print(line)

def parse_mount_rule(line):
    # XXX Do real parsing here
    return aarules.Raw_Mount_Rule(line)

def parse_pivot_root_rule(line):
    # XXX Do real parsing here
    return aarules.Raw_Pivot_Root_Rule(line)

def parse_unix_rule(line):
    # XXX Do real parsing here
    return aarules.Raw_Unix_Rule(line)

def separate_vars(vs):
    """Returns a list of all the values for a variable"""
    data = set()
    vs = vs.strip()

    RE_VARS = re.compile('^(("[^"]*")|([^"\s]+))\s*(.*)$')
    while RE_VARS.search(vs):
        matches = RE_VARS.search(vs).groups()
        data.add(strip_quotes(matches[0]))
        vs = matches[3].strip()

    if vs:
        raise AppArmorException('Variable assignments contains invalid parts (unbalanced quotes?): %s' % vs)

    return data

def is_active_profile(pname):
    if aa.get(pname, False):
        return True
    else:
        return False

def store_list_var(var, list_var, value, var_operation, filename):
    """Store(add new variable or add values to variable) the variables encountered in the given list_var
       - the 'var' parameter will be modified
       - 'list_var' is the variable name, for example '@{foo}'
        """
    vlist = separate_vars(value)
    if var_operation == '=':
        if not var.get(list_var, False):
            var[list_var] = set(vlist)
        else:
            raise AppArmorException(_('Redefining existing variable %(variable)s: %(value)s in %(file)s') % { 'variable': list_var, 'value': value, 'file': filename })
    elif var_operation == '+=':
        if var.get(list_var, False):
            var[list_var] |= vlist
        else:
            raise AppArmorException(_('Values added to a non-existing variable %(variable)s: %(value)s in %(file)s') % { 'variable': list_var, 'value': value, 'file': filename })
    else:
        raise AppArmorException(_('Unknown variable operation %(operation)s for variable %(variable)s in %(file)s') % { 'operation': var_operation, 'variable': list_var, 'file': filename })


def escape(escape):
    escape = strip_quotes(escape)
    escape = re.sub('((?<!\\))"', r'\1\\', escape)
    if re.search('(\s|^$|")', escape):
        return '"%s"' % escape
    return escape

def write_header(prof_data, depth, name, embedded_hat, write_flags):
    pre = ' ' * int(depth * 2)
    data = []
    unquoted_name = name
    name = quote_if_needed(name)

    attachment = ''
    if prof_data['attachment']:
        attachment = ' %s' % quote_if_needed(prof_data['attachment'])

    comment = ''
    if prof_data['header_comment']:
        comment = ' %s' % prof_data['header_comment']

    if (not embedded_hat and re.search('^[^/]', unquoted_name)) or (embedded_hat and re.search('^[^^]', unquoted_name)) or prof_data['attachment'] or prof_data['profile_keyword']:
        name = 'profile %s%s' % (name, attachment)

    flags = ''
    if write_flags and prof_data['flags']:
        flags = ' flags=(%s)' % prof_data['flags']

    data.append('%s%s%s {%s' % (pre, name, flags, comment))

    return data

def write_single(prof_data, depth, allow, name, prefix, tail):
    pre = '  ' * depth
    data = []
    ref, allow = set_ref_allow(prof_data, allow)

    if ref.get(name, False):
        for key in sorted(ref[name].keys()):
            qkey = quote_if_needed(key)
            data.append('%s%s%s%s%s' % (pre, allow, prefix, qkey, tail))
        if ref[name].keys():
            data.append('')

    return data

def set_allow_str(allow):
    if allow == 'deny':
        return 'deny '
    elif allow == 'allow':
        return ''
    elif allow == '':
        return ''
    else:
        raise AppArmorException(_("Invalid allow string: %(allow)s"))

def set_ref_allow(prof_data, allow):
    if allow:
        return prof_data[allow], set_allow_str(allow)
    else:
        return prof_data, ''


def write_pair(prof_data, depth, allow, name, prefix, sep, tail, fn):
    pre = '  ' * depth
    data = []
    ref, allow = set_ref_allow(prof_data, allow)

    if ref.get(name, False):
        for key in sorted(ref[name].keys()):
            value = fn(ref[name][key])  # eval('%s(%s)' % (fn, ref[name][key]))
            data.append('%s%s%s%s%s%s' % (pre, allow, prefix, key, sep, value))
        if ref[name].keys():
            data.append('')

    return data

def write_includes(prof_data, depth):
    return write_single(prof_data, depth, '', 'include', '#include <', '>')

def write_change_profile(prof_data, depth):
    data = []
    if prof_data.get('change_profile', False):
        data = prof_data['change_profile'].get_clean(depth)
    return data

def write_alias(prof_data, depth):
    return write_pair(prof_data, depth, '', 'alias', 'alias ', ' -> ', ',', quote_if_needed)

def write_rlimits(prof_data, depth):
    data = []
    if prof_data.get('rlimit', False):
        data = prof_data['rlimit'].get_clean(depth)
    return data

def var_transform(ref):
    data = []
    for value in ref:
        if not value:
            value = '""'
        data.append(quote_if_needed(value))
    return ' '.join(data)

def write_list_vars(prof_data, depth):
    return write_pair(prof_data, depth, '', 'lvar', '', ' = ', '', var_transform)

def write_capabilities(prof_data, depth):
    data = []
    if prof_data.get('capability', False):
        data = prof_data['capability'].get_clean(depth)
    return data

def write_netdomain(prof_data, depth):
    data = []
    if prof_data.get('network', False):
        data = prof_data['network'].get_clean(depth)
    return data

def write_dbus_rules(prof_data, depth, allow):
    pre = '  ' * depth
    data = []

    # no dbus rules, so return
    if not prof_data[allow].get('dbus', False):
        return data

    for dbus_rule in prof_data[allow]['dbus']:
        data.append('%s%s' % (pre, dbus_rule.serialize()))
    data.append('')
    return data

def write_dbus(prof_data, depth):
    data = write_dbus_rules(prof_data, depth, 'deny')
    data += write_dbus_rules(prof_data, depth, 'allow')
    return data

def write_mount_rules(prof_data, depth, allow):
    pre = '  ' * depth
    data = []

    # no mount rules, so return
    if not prof_data[allow].get('mount', False):
        return data

    for mount_rule in prof_data[allow]['mount']:
        data.append('%s%s' % (pre, mount_rule.serialize()))
    data.append('')
    return data

def write_mount(prof_data, depth):
    data = write_mount_rules(prof_data, depth, 'deny')
    data += write_mount_rules(prof_data, depth, 'allow')
    return data

def write_signal(prof_data, depth):
    data = []
    if prof_data.get('signal', False):
        data = prof_data['signal'].get_clean(depth)
    return data

def write_ptrace(prof_data, depth):
    data = []
    if prof_data.get('ptrace', False):
        data = prof_data['ptrace'].get_clean(depth)
    return data

def write_pivot_root_rules(prof_data, depth, allow):
    pre = '  ' * depth
    data = []

    # no pivot_root rules, so return
    if not prof_data[allow].get('pivot_root', False):
        return data

    for pivot_root_rule in prof_data[allow]['pivot_root']:
        data.append('%s%s' % (pre, pivot_root_rule.serialize()))
    data.append('')
    return data

def write_pivot_root(prof_data, depth):
    data = write_pivot_root_rules(prof_data, depth, 'deny')
    data += write_pivot_root_rules(prof_data, depth, 'allow')
    return data

def write_unix_rules(prof_data, depth, allow):
    pre = '  ' * depth
    data = []

    # no unix rules, so return
    if not prof_data[allow].get('unix', False):
        return data

    for unix_rule in prof_data[allow]['unix']:
        data.append('%s%s' % (pre, unix_rule.serialize()))
    data.append('')
    return data

def write_unix(prof_data, depth):
    data = write_unix_rules(prof_data, depth, 'deny')
    data += write_unix_rules(prof_data, depth, 'allow')
    return data

def write_link_rules(prof_data, depth, allow):
    pre = '  ' * depth
    data = []
    allowstr = set_allow_str(allow)

    if prof_data[allow].get('link', False):
        for path in sorted(prof_data[allow]['link'].keys()):
            to_name = prof_data[allow]['link'][path]['to']
            subset = ''
            if prof_data[allow]['link'][path]['mode'] & apparmor.aamode.AA_LINK_SUBSET:
                subset = 'subset'
            audit = ''
            if prof_data[allow]['link'][path].get('audit', False):
                audit = 'audit '
            path = quote_if_needed(path)
            to_name = quote_if_needed(to_name)
            data.append('%s%s%slink %s%s -> %s,' % (pre, audit, allowstr, subset, path, to_name))
        data.append('')

    return data

def write_links(prof_data, depth):
    data = write_link_rules(prof_data, depth, 'deny')
    data += write_link_rules(prof_data, depth, 'allow')

    return data

def write_path_rules(prof_data, depth, allow):
    pre = '  ' * depth
    data = []
    allowstr = set_allow_str(allow)

    if prof_data[allow].get('path', False):
        for path in sorted(prof_data[allow]['path'].keys()):
            filestr = ''
            if prof_data[allow]['path'][path].get('file_prefix', False):
                filestr = 'file'
            mode = prof_data[allow]['path'][path]['mode']
            audit = prof_data[allow]['path'][path]['audit']
            tail = ''
            if prof_data[allow]['path'][path].get('to', False):
                tail = ' -> %s' % prof_data[allow]['path'][path]['to']
            user, other = split_mode(mode)
            user_audit, other_audit = split_mode(audit)

            while user or other:
                ownerstr = ''
                tmpmode = 0
                tmpaudit = False
                if user - other:
                    # if no other mode set
                    ownerstr = 'owner '
                    tmpmode = user - other
                    tmpaudit = user_audit
                    user = user - tmpmode
                else:
                    if user_audit - other_audit & user:
                        ownerstr = 'owner '
                        tmpaudit = user_audit - other_audit & user
                        tmpmode = user & tmpaudit
                        user = user - tmpmode
                    else:
                        ownerstr = ''
                        tmpmode = user | other
                        tmpaudit = user_audit | other_audit
                        user = user - tmpmode
                        other = other - tmpmode

                if path == ALL:
                    path = ''

                if tmpmode & tmpaudit:
                    modestr = mode_to_str(tmpmode & tmpaudit)
                    if modestr:
                        modestr = ' ' + modestr
                    path = quote_if_needed(path)
                    if filestr and path:
                        filestr += ' '
                    data.append('%saudit %s%s%s%s%s%s,' % (pre, allowstr, ownerstr, filestr, path, modestr, tail))
                    tmpmode = tmpmode - tmpaudit

                if tmpmode:
                    modestr = mode_to_str(tmpmode)
                    if modestr:
                        modestr = ' ' + modestr
                    path = quote_if_needed(path)
                    if filestr and path:
                        filestr += ' '
                    data.append('%s%s%s%s%s%s%s,' % (pre, allowstr, ownerstr, filestr, path, modestr, tail))

        data.append('')
    return data

def write_paths(prof_data, depth):
    data = write_path_rules(prof_data, depth, 'deny')
    data += write_path_rules(prof_data, depth, 'allow')

    return data

def write_rules(prof_data, depth):
    data = write_alias(prof_data, depth)
    data += write_list_vars(prof_data, depth)
    data += write_includes(prof_data, depth)
    data += write_rlimits(prof_data, depth)
    data += write_capabilities(prof_data, depth)
    data += write_netdomain(prof_data, depth)
    data += write_dbus(prof_data, depth)
    data += write_mount(prof_data, depth)
    data += write_signal(prof_data, depth)
    data += write_ptrace(prof_data, depth)
    data += write_pivot_root(prof_data, depth)
    data += write_unix(prof_data, depth)
    data += write_links(prof_data, depth)
    data += write_paths(prof_data, depth)
    data += write_change_profile(prof_data, depth)

    return data

def write_piece(profile_data, depth, name, nhat, write_flags):
    pre = '  ' * depth
    data = []
    wname = None
    inhat = False
    if name == nhat:
        wname = name
    else:
        wname = name + '//' + nhat
        name = nhat
        inhat = True
    data += write_header(profile_data[name], depth, wname, False, write_flags)
    data += write_rules(profile_data[name], depth + 1)

    pre2 = '  ' * (depth + 1)

    if not inhat:
        # Embedded hats
        for hat in list(filter(lambda x: x != name, sorted(profile_data.keys()))):
            if not profile_data[hat]['external']:
                data.append('')
                if profile_data[hat]['profile']:
                    data += list(map(str, write_header(profile_data[hat], depth + 1, hat, True, write_flags)))
                else:
                    data += list(map(str, write_header(profile_data[hat], depth + 1, '^' + hat, True, write_flags)))

                data += list(map(str, write_rules(profile_data[hat], depth + 2)))

                data.append('%s}' % pre2)

        data.append('%s}' % pre)

        # External hats
        for hat in list(filter(lambda x: x != name, sorted(profile_data.keys()))):
            if name == nhat and profile_data[hat].get('external', False):
                data.append('')
                data += list(map(lambda x: '  %s' % x, write_piece(profile_data, depth - 1, name, nhat, write_flags)))
                data.append('  }')

    return data

def serialize_profile(profile_data, name, options):
    string = ''
    include_metadata = False
    include_flags = True
    data = []

    if options:  # and type(options) == dict:
        if options.get('METADATA', False):
            include_metadata = True
        if options.get('NO_FLAGS', False):
            include_flags = False

    if include_metadata:
        string = '# Last Modified: %s\n' % time.asctime()

        if (profile_data[name].get('repo', False) and
                profile_data[name]['repo']['url'] and
                profile_data[name]['repo']['user'] and
                profile_data[name]['repo']['id']):
            repo = profile_data[name]['repo']
            string += '# REPOSITORY: %s %s %s\n' % (repo['url'], repo['user'], repo['id'])
        elif profile_data[name]['repo']['neversubmit']:
            string += '# REPOSITORY: NEVERSUBMIT\n'

#     if profile_data[name].get('initial_comment', False):
#         comment = profile_data[name]['initial_comment']
#         comment.replace('\\n', '\n')
#         string += comment + '\n'

    prof_filename = get_profile_filename(name)
    if filelist.get(prof_filename, False):
        data += write_alias(filelist[prof_filename], 0)
        data += write_list_vars(filelist[prof_filename], 0)
        data += write_includes(filelist[prof_filename], 0)

    #Here should be all the profiles from the files added write after global/common stuff
    for prof in sorted(filelist[prof_filename]['profiles'].keys()):
        if prof != name:
            if original_aa[prof][prof].get('initial_comment', False):
                comment = original_aa[prof][prof]['initial_comment']
                comment.replace('\\n', '\n')
                data += [comment + '\n']
            data += write_piece(original_aa[prof], 0, prof, prof, include_flags)
        else:
            if profile_data[name].get('initial_comment', False):
                comment = profile_data[name]['initial_comment']
                comment.replace('\\n', '\n')
                data += [comment + '\n']
            data += write_piece(profile_data, 0, name, name, include_flags)

    string += '\n'.join(data)

    return string + '\n'

def serialize_parse_profile_start(line, file, lineno, profile, hat, prof_data_profile, prof_data_external, correct):
    (profile, hat, attachment, flags, in_contained_hat, pps_set_profile, pps_set_hat_external) = parse_profile_start(line, file, lineno, profile, hat)

    if hat and profile != hat and '%s//%s'%(profile, hat) in line and not prof_data_external:
        correct = False

    return (profile, hat, attachment, flags, in_contained_hat, correct)

def serialize_profile_from_old_profile(profile_data, name, options):
    data = []
    string = ''
    include_metadata = False
    include_flags = True
    prof_filename = get_profile_filename(name)

    write_filelist = deepcopy(filelist[prof_filename])
    write_prof_data = deepcopy(profile_data)

    # XXX profile_data / write_prof_data contain only one profile with its hats
    # XXX this will explode if a file contains multiple profiles, see https://bugs.launchpad.net/ubuntu/+source/apparmor/+bug/1528139
    # XXX fixing this needs lots of write_prof_data[hat] -> write_prof_data[profile][hat] changes (and of course also a change in the calling code)
    # XXX (the better option is a full rewrite of serialize_profile_from_old_profile())

    if options:  # and type(options) == dict:
        if options.get('METADATA', False):
            include_metadata = True
        if options.get('NO_FLAGS', False):
            include_flags = False

    if include_metadata:
        string = '# Last Modified: %s\n' % time.asctime()

        if (profile_data[name].get('repo', False) and
                profile_data[name]['repo']['url'] and
                profile_data[name]['repo']['user'] and
                profile_data[name]['repo']['id']):
            repo = profile_data[name]['repo']
            string += '# REPOSITORY: %s %s %s\n' % (repo['url'], repo['user'], repo['id'])
        elif profile_data[name]['repo']['neversubmit']:
            string += '# REPOSITORY: NEVERSUBMIT\n'

    if not os.path.isfile(prof_filename):
        raise AppArmorException(_("Can't find existing profile to modify"))

    # profiles_list = filelist[prof_filename].keys()  # XXX

    with open_file_read(prof_filename) as f_in:
        profile = None
        hat = None
        write_methods = {'alias': write_alias,
                         'lvar': write_list_vars,
                         'include': write_includes,
                         'rlimit': write_rlimits,
                         'capability': write_capabilities,
                         'network': write_netdomain,
                         'dbus': write_dbus,
                         'mount': write_mount,
                         'signal': write_signal,
                         'ptrace': write_ptrace,
                         'pivot_root': write_pivot_root,
                         'unix': write_unix,
                         'link': write_links,
                         'path': write_paths,
                         'change_profile': write_change_profile,
                         }
        default_write_order = [ 'alias',
                                'lvar',
                                'include',
                                'rlimit',
                                'capability',
                                'network',
                                'dbus',
                                'mount',
                                'signal',
                                'ptrace',
                                'pivot_root',
                                'unix',
                                'link',
                                'path',
                                'change_profile',
                              ]
        # prof_correct = True  # XXX correct?
        segments = {'alias': False,
                    'lvar': False,
                    'include': False,
                    'rlimit': False,
                    'capability': False,
                    'network': False,
                    'dbus': False,
                    'mount': True, # not handled otherwise yet
                    'signal': True, # not handled otherwise yet
                    'ptrace': True, # not handled otherwise yet
                    'pivot_root': True, # not handled otherwise yet
                    'unix': True, # not handled otherwise yet
                    'link': False,
                    'path': False,
                    'change_profile': False,
                    'include_local_started': False, # unused
                    }

        def write_prior_segments(prof_data, segments, line):
            data = []
            for segs in list(filter(lambda x: segments[x], segments.keys())):
                depth = len(line) - len(line.lstrip())
                data += write_methods[segs](prof_data, int(depth / 2))
                segments[segs] = False
                # delete rules from prof_data to avoid duplication (they are in data now)
                if prof_data['allow'].get(segs, False):
                    prof_data['allow'].pop(segs)
                if prof_data['deny'].get(segs, False):
                    prof_data['deny'].pop(segs)
                if prof_data.get(segs, False):
                    t = type(prof_data[segs])
                    prof_data[segs] = t()
            return data

        #data.append('reading prof')
        for line in f_in:
            correct = True
            line = line.rstrip('\n')
            #data.append(' ')#data.append('read: '+line)
            if RE_PROFILE_START.search(line):

                (profile, hat, attachment, flags, in_contained_hat, correct) = serialize_parse_profile_start(
                        line, prof_filename, None, profile, hat, write_prof_data[hat]['profile'], write_prof_data[hat]['external'], correct)

                if not write_prof_data[hat]['name'] == profile:
                    correct = False

                if not write_filelist['profiles'][profile][hat] is True:
                    correct = False

                if not write_prof_data[hat]['flags'] == flags:
                    correct = False

                #Write the profile start
                if correct:
                    if write_filelist:
                        data += write_alias(write_filelist, 0)
                        data += write_list_vars(write_filelist, 0)
                        data += write_includes(write_filelist, 0)
                    data.append(line)
                else:
                    if write_prof_data[hat]['name'] == profile:
                        depth = len(line) - len(line.lstrip())
                        data += write_header(write_prof_data[name], int(depth / 2), name, False, include_flags)

            elif RE_PROFILE_END.search(line):
                # DUMP REMAINDER OF PROFILE
                if profile:
                    depth = int(len(line) - len(line.lstrip()) / 2) + 1

                    # first write sections that were modified
                    #for segs in write_methods.keys():
                    for segs in default_write_order:
                        if segments[segs]:
                            data += write_methods[segs](write_prof_data[name], depth)
                            segments[segs] = False
                            # delete rules from write_prof_data to avoid duplication (they are in data now)
                            if write_prof_data[name]['allow'].get(segs, False):
                                write_prof_data[name]['allow'].pop(segs)
                            if write_prof_data[name]['deny'].get(segs, False):
                                write_prof_data[name]['deny'].pop(segs)
                            if write_prof_data[name].get(segs, False):
                                t = type(write_prof_data[name][segs])
                                write_prof_data[name][segs] = t()

                    # then write everything else
                    for segs in default_write_order:
                        data += write_methods[segs](write_prof_data[name], depth)

                    write_prof_data.pop(name)

                    #Append local includes
                    data.append(line)

                if not in_contained_hat:
                    # Embedded hats
                    depth = int((len(line) - len(line.lstrip())) / 2)
                    pre2 = '  ' * (depth + 1)
                    for hat in list(filter(lambda x: x != name, sorted(profile_data.keys()))):
                        if not profile_data[hat]['external']:
                            data.append('')
                            if profile_data[hat]['profile']:
                                data += list(map(str, write_header(profile_data[hat], depth + 1, hat, True, include_flags)))
                            else:
                                data += list(map(str, write_header(profile_data[hat], depth + 1, '^' + hat, True, include_flags)))

                            data += list(map(str, write_rules(profile_data[hat], depth + 2)))

                            data.append('%s}' % pre2)

                    # External hats
                    for hat in list(filter(lambda x: x != name, sorted(profile_data.keys()))):
                        if profile_data[hat].get('external', False):
                            data.append('')
                            data += list(map(lambda x: '  %s' % x, write_piece(profile_data, depth - 1, name, name, include_flags)))
                            data.append('  }')

                if in_contained_hat:
                    #Hat processed, remove it
                    hat = profile
                    in_contained_hat = False
                else:
                    profile = None

            elif CapabilityRule.match(line):
                cap = CapabilityRule.parse(line)
                if write_prof_data[hat]['capability'].is_covered(cap, True, True):
                    if not segments['capability'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['capability'] = True
                    write_prof_data[hat]['capability'].delete(cap)
                    data.append(line)
                else:
                    # To-Do
                    pass
            elif RE_PROFILE_LINK.search(line):
                matches = RE_PROFILE_LINK.search(line).groups()
                audit = False
                if matches[0]:
                    audit = True
                allow = 'allow'
                if matches[1] and matches[1].strip() == 'deny':
                    allow = 'deny'

                subset = matches[3]
                link = strip_quotes(matches[6])
                value = strip_quotes(matches[7])
                if not write_prof_data[hat][allow]['link'][link]['to'] == value:
                    correct = False
                if not write_prof_data[hat][allow]['link'][link]['mode'] & apparmor.aamode.AA_MAY_LINK:
                    correct = False
                if subset and not write_prof_data[hat][allow]['link'][link]['mode'] & apparmor.aamode.AA_LINK_SUBSET:
                    correct = False
                if audit and not write_prof_data[hat][allow]['link'][link]['audit'] & apparmor.aamode.AA_LINK_SUBSET:
                    correct = False

                if correct:
                    if not segments['link'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['link'] = True
                    write_prof_data[hat][allow]['link'].pop(link)
                    data.append(line)
                else:
                    # To-Do
                    pass

            elif ChangeProfileRule.match(line):
                change_profile_obj = ChangeProfileRule.parse(line)
                if write_prof_data[hat]['change_profile'].is_covered(change_profile_obj, True, True):
                    if not segments['change_profile'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['change_profile'] = True
                    write_prof_data[hat]['change_profile'].delete(change_profile_obj)
                    data.append(line)

            elif RE_PROFILE_ALIAS.search(line):
                matches = RE_PROFILE_ALIAS.search(line).groups()

                from_name = strip_quotes(matches[0])
                to_name = strip_quotes(matches[1])

                if profile:
                    if not write_prof_data[hat]['alias'][from_name] == to_name:
                        correct = False
                else:
                    if not write_filelist['alias'][from_name] == to_name:
                        correct = False

                if correct:
                    if not segments['alias'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['alias'] = True
                    if profile:
                        write_prof_data[hat]['alias'].pop(from_name)
                    else:
                        write_filelist['alias'].pop(from_name)
                    data.append(line)
                else:
                    #To-Do
                    pass

            elif RlimitRule.match(line):
                rlimit_obj = RlimitRule.parse(line)

                if write_prof_data[hat]['rlimit'].is_covered(rlimit_obj, True, True):
                    if not segments['rlimit'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['rlimit'] = True
                    write_prof_data[hat]['rlimit'].delete(rlimit_obj)
                    data.append(line)
                else:
                    #To-Do
                    pass

            elif RE_PROFILE_BOOLEAN.search(line):
                matches = RE_PROFILE_BOOLEAN.search(line).groups()
                bool_var = matches[0]
                value = matches[1]

                if not write_prof_data[hat]['lvar'][bool_var] == value:
                    correct = False

                if correct:
                    if not segments['lvar'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['lvar'] = True
                    write_prof_data[hat]['lvar'].pop(bool_var)
                    data.append(line)
                else:
                    #To-Do
                    pass
            elif RE_PROFILE_VARIABLE.search(line):
                matches = RE_PROFILE_VARIABLE.search(line).groups()
                list_var = strip_quotes(matches[0])
                var_operation = matches[1]
                value = strip_quotes(matches[2])
                var_set = hasher()
                if var_operation == '+=':
                    correct = False  # adding proper support for "add to variable" needs big changes
                    # (like storing a variable's "history" - where it was initially defined and what got added where)
                    # so just skip any comparison and assume a non-match
                elif profile:
                    store_list_var(var_set, list_var, value, var_operation, prof_filename)
                    if not var_set[list_var] == write_prof_data['lvar'].get(list_var, False):
                        correct = False
                else:
                    store_list_var(var_set, list_var, value, var_operation, prof_filename)
                    if not var_set[list_var] == write_filelist['lvar'].get(list_var, False):
                        correct = False

                if correct:
                    if not segments['lvar'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['lvar'] = True
                    if profile:
                        write_prof_data[hat]['lvar'].pop(list_var)
                    else:
                        write_filelist['lvar'].pop(list_var)
                    data.append(line)
                else:
                    #To-Do
                    pass

            elif RE_PROFILE_BARE_FILE_ENTRY.search(line):
                matches = RE_PROFILE_BARE_FILE_ENTRY.search(line).groups()

                allow = 'allow'
                if matches[1] and matches[1].strip() == 'deny':
                    allow = 'deny'

                mode = apparmor.aamode.AA_BARE_FILE_MODE
                if not matches[2]:
                    mode |= AA_OTHER(apparmor.aamode.AA_BARE_FILE_MODE)

                audit = set()
                if matches[0]:
                    audit = mode

                path_rule = write_prof_data[hat][allow]['path'][ALL]
                if path_rule.get('mode', set()) & mode and \
                   (not audit or path_rule.get('audit', set()) & audit) and \
                   path_rule.get('file_prefix', set()):
                    if not segments['path'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['path'] = True
                    write_prof_data[hat][allow]['path'].pop(ALL)
                    data.append(line)

            elif RE_PROFILE_PATH_ENTRY.search(line):
                matches = RE_PROFILE_PATH_ENTRY.search(line).groups()
                audit = False
                if matches[0]:
                    audit = True
                allow = 'allow'
                if matches[1] and matches[1].split() == 'deny':
                    allow = 'deny'

                user = False
                if matches[2]:
                    user = True

                path = strip_quotes(matches[4].strip())
                mode = matches[5]
                nt_name = matches[7]
                if nt_name:
                    nt_name = nt_name.strip()

                tmpmode = set()
                if user:
                    tmpmode = str_to_mode('%s::' % mode)
                else:
                    tmpmode = str_to_mode(mode)

                if not write_prof_data[hat][allow]['path'].get(path):
                    correct = False
                else:
                    if not write_prof_data[hat][allow]['path'][path].get('mode', set()) & tmpmode:
                        correct = False

                    if nt_name and not write_prof_data[hat][allow]['path'][path].get('to', False) == nt_name:
                        correct = False

                    if audit and not write_prof_data[hat][allow]['path'][path].get('audit', set()) & tmpmode:
                        correct = False

                if correct:
                    if not segments['path'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['path'] = True
                    write_prof_data[hat][allow]['path'].pop(path)
                    data.append(line)
                else:
                    #To-Do
                    pass

            elif re_match_include(line):
                include_name = re_match_include(line)
                if profile:
                    if write_prof_data[hat]['include'].get(include_name, False):
                        if not segments['include'] and True in segments.values():
                            data += write_prior_segments(write_prof_data[name], segments, line)
                        segments['include'] = True
                        write_prof_data[hat]['include'].pop(include_name)
                        data.append(line)
                else:
                    if write_filelist['include'].get(include_name, False):
                        write_filelist['include'].pop(include_name)
                        data.append(line)

            elif NetworkRule.match(line):
                network_obj = NetworkRule.parse(line)
                if write_prof_data[hat]['network'].is_covered(network_obj, True, True):
                    if not segments['network'] and True in segments.values():
                        data += write_prior_segments(write_prof_data[name], segments, line)
                    segments['network'] = True
                    write_prof_data[hat]['network'].delete(network_obj)
                    data.append(line)

            elif RE_PROFILE_CHANGE_HAT.search(line):
                # "^hat," declarations are no longer supported, ignore them and don't write out the line
                # (parse_profile_data() already prints a warning about that)
                pass
            elif RE_PROFILE_HAT_DEF.search(line):
                matches = RE_PROFILE_HAT_DEF.search(line)
                in_contained_hat = True
                hat = matches.group('hat')
                hat = strip_quotes(hat)
                flags = matches.group('flags')

                if not write_prof_data[hat]['flags'] == flags:
                    correct = False
                if not write_filelist['profile'][profile][hat]:
                    correct = False
                if correct:
                    data.append(line)
                else:
                    #To-Do
                    pass
            else:
                if correct:
                    data.append(line)
                else:
                    #To-Do
                    pass
#     data.append('prof done')
#     if write_filelist:
#         data += write_alias(write_filelist, 0)
#         data += write_list_vars(write_filelist, 0)
#         data += write_includes(write_filelist, 0)
#     data.append('from filelist over')
#     data += write_piece(write_prof_data, 0, name, name, include_flags)

    string += '\n'.join(data)

    return string + '\n'

def write_profile_ui_feedback(profile):
    aaui.UI_Info(_('Writing updated profile for %s.') % profile)
    write_profile(profile)

def write_profile(profile):
    prof_filename = None
    if aa[profile][profile].get('filename', False):
        prof_filename = aa[profile][profile]['filename']
    else:
        prof_filename = get_profile_filename(profile)

    newprof = tempfile.NamedTemporaryFile('w', suffix='~', delete=False, dir=profile_dir)
    if os.path.exists(prof_filename):
        shutil.copymode(prof_filename, newprof.name)
    else:
        #permission_600 = stat.S_IRUSR | stat.S_IWUSR    # Owner read and write
        #os.chmod(newprof.name, permission_600)
        pass

    serialize_options = {}
    serialize_options['METADATA'] = True

    profile_string = serialize_profile(aa[profile], profile, serialize_options)
    newprof.write(profile_string)
    newprof.close()

    os.rename(newprof.name, prof_filename)

    if profile in changed:
        changed.pop(profile)
    else:
        debug_logger.info("Unchanged profile written: %s (not listed in 'changed' list)" % profile)

    original_aa[profile] = deepcopy(aa[profile])

def matchliteral(aa_regexp, literal):
    p_regexp = '^' + convert_regexp(aa_regexp) + '$'
    match = False
    try:
        match = re.search(p_regexp, literal)
    except:
        return None
    return match

def profile_known_exec(profile, typ, exec_target):
    if typ == 'exec':
        cm = None
        am = None
        m = []

        cm, am, m = rematchfrag(profile, 'deny', exec_target)
        if cm & apparmor.aamode.AA_MAY_EXEC:
            return -1

        cm, am, m = match_prof_incs_to_path(profile, 'deny', exec_target)
        if cm & apparmor.aamode.AA_MAY_EXEC:
            return -1

        cm, am, m = rematchfrag(profile, 'allow', exec_target)
        if cm & apparmor.aamode.AA_MAY_EXEC:
            return 1

        cm, am, m = match_prof_incs_to_path(profile, 'allow', exec_target)
        if cm & apparmor.aamode.AA_MAY_EXEC:
            return 1

    return 0

def is_known_rule(profile, rule_type, rule_obj):
    # XXX get rid of get() checks after we have a proper function to initialize a profile
    if profile.get(rule_type, False):
        if profile[rule_type].is_covered(rule_obj, False):
            return True

    includelist = list(profile['include'].keys())
    checked = []

    while includelist:
        incname = includelist.pop(0)
        checked.append(incname)

        if os.path.isdir(profile_dir + '/' + incname):
            includelist += include_dir_filelist(profile_dir, incname)
        else:
            if include[incname][incname].get(rule_type, False):
                if include[incname][incname][rule_type].is_covered(rule_obj, False):
                    return True

            for childinc in include[incname][incname]['include'].keys():
                if childinc not in checked:
                    includelist += [childinc]

    return False

def reload_base(bin_path):
    if not check_for_apparmor():
        return None

    prof_filename = get_profile_filename(bin_path)

    # XXX use reload_profile() from tools.py instead (and don't hide output in /dev/null)
    subprocess.call("cat '%s' | %s -I%s -r >/dev/null 2>&1" % (prof_filename, parser, profile_dir), shell=True)

def reload(bin_path):
    bin_path = find_executable(bin_path)
    if not bin_path:
        return None

    return reload_base(bin_path)

def get_include_data(filename):
    data = []
    filename = profile_dir + '/' + filename
    if os.path.exists(filename):
        with open_file_read(filename) as f_in:
            data = f_in.readlines()
    else:
        raise AppArmorException(_('File Not Found: %s') % filename)
    return data

def include_dir_filelist(profile_dir, include_name):
    '''returns a list of files in the given profile_dir/include_name directory, except skippable files'''
    files = []
    for path in os.listdir(profile_dir + '/' + include_name):
        path = path.strip()
        if is_skippable_file(path):
            continue
        if os.path.isfile(profile_dir + '/' + include_name + '/' + path):
            file_name = include_name + '/' + path
            file_name = file_name.replace(profile_dir + '/', '')
            files.append(file_name)

    return files

def load_include(incname):
    load_includeslist = [incname]
    while load_includeslist:
        incfile = load_includeslist.pop(0)
        if include.get(incfile, {}).get(incfile, False):
            pass  # already read, do nothing
        elif os.path.isfile(profile_dir + '/' + incfile):
            data = get_include_data(incfile)
            incdata = parse_profile_data(data, incfile, True)
            attach_profile_data(include, incdata)
        #If the include is a directory means include all subfiles
        elif os.path.isdir(profile_dir + '/' + incfile):
            load_includeslist += include_dir_filelist(profile_dir, incfile)
        else:
            raise AppArmorException("Include file %s not found" % (profile_dir + '/' + incfile) )

    return 0

def rematchfrag(frag, allow, path):
    combinedmode = set()
    combinedaudit = set()
    matches = []
    if not frag:
        return combinedmode, combinedaudit, matches
    for entry in frag[allow]['path'].keys():
        match = matchliteral(entry, path)
        if match:
            #print(frag[allow]['path'][entry]['mode'])
            combinedmode |= frag[allow]['path'][entry].get('mode', set())
            combinedaudit |= frag[allow]['path'][entry].get('audit', set())
            matches.append(entry)

    return combinedmode, combinedaudit, matches

def match_include_to_path(incname, allow, path):
    combinedmode = set()
    combinedaudit = set()
    matches = []
    includelist = [incname]
    while includelist:
        incfile = str(includelist.pop(0))
        # ret = load_include(incfile)
        load_include(incfile)
        if not include.get(incfile, {}):
            continue
        cm, am, m = rematchfrag(include[incfile].get(incfile, {}), allow, path)
        #print(incfile, cm, am, m)
        if cm:
            combinedmode |= cm
            combinedaudit |= am
            matches += m

        if path in include[incfile][incfile][allow]['path']:
            combinedmode |= include[incfile][incfile][allow]['path'][path]['mode']
            combinedaudit |= include[incfile][incfile][allow]['path'][path]['audit']

        if include[incfile][incfile]['include'].keys():
            includelist += include[incfile][incfile]['include'].keys()

    return combinedmode, combinedaudit, matches

def match_prof_incs_to_path(frag, allow, path):
    combinedmode = set()
    combinedaudit = set()
    matches = []

    includelist = list(frag['include'].keys())
    while includelist:
        incname = includelist.pop(0)
        cm, am, m = match_include_to_path(incname, allow, path)
        if cm:
            combinedmode |= cm
            combinedaudit |= am
            matches += m

    return combinedmode, combinedaudit, matches

def check_qualifiers(program):
    if cfg['qualifiers'].get(program, False):
        if cfg['qualifiers'][program] != 'p':
            fatal_error(_("%s is currently marked as a program that should not have its own\nprofile.  Usually, programs are marked this way if creating a profile for \nthem is likely to break the rest of the system.  If you know what you\'re\ndoing and are certain you want to create a profile for this program, edit\nthe corresponding entry in the [qualifiers] section in /etc/apparmor/logprof.conf.") % program)
    return False

def get_subdirectories(current_dir):
    """Returns a list of all directories directly inside given directory"""
    if sys.version_info < (3, 0):
        return os.walk(current_dir).next()[1]
    else:
        return os.walk(current_dir).__next__()[1]

def loadincludes():
    incdirs = get_subdirectories(profile_dir)
    for idir in incdirs:
        if is_skippable_dir(idir):
            continue
        for dirpath, dirname, files in os.walk(profile_dir + '/' + idir):
            if is_skippable_dir(dirpath):
                continue
            for fi in files:
                if is_skippable_file(fi):
                    continue
                else:
                    fi = dirpath + '/' + fi
                    fi = fi.replace(profile_dir + '/', '', 1)
                    load_include(fi)

def glob_common(path):
    globs = []

    if re.search('[\d\.]+\.so$', path) or re.search('\.so\.[\d\.]+$', path):
        libpath = path
        libpath = re.sub('[\d\.]+\.so$', '*.so', libpath)
        libpath = re.sub('\.so\.[\d\.]+$', '.so.*', libpath)
        if libpath != path:
            globs.append(libpath)

    for glob in cfg['globs']:
        if re.search(glob, path):
            globbedpath = path
            globbedpath = re.sub(glob, cfg['globs'][glob], path)
            if globbedpath != path:
                globs.append(globbedpath)

    return sorted(set(globs))

def combine_name(name1, name2):
    if name1 == name2:
        return name1
    else:
        return '%s^%s' % (name1, name2)

def commonprefix(new, old):
    match = re.search(r'^([^\0]*)[^\0]*(\0\1[^\0]*)*$', '\0'.join([new, old]))
    if match:
        return match.groups()[0]
    return match

def commonsuffix(new, old):
    match = commonprefix(new[-1::-1], old[-1::-1])
    if match:
        return match[-1::-1]

def matchregexp(new, old):
    if re.search('\{.*(\,.*)*\}', old):
        return None

#     if re.search('\[.+\]', old) or re.search('\*', old) or re.search('\?', old):
#
#         new_reg = convert_regexp(new)
#         old_reg = convert_regexp(old)
#
#         pref = commonprefix(new, old)
#         if pref:
#             if convert_regexp('(*,**)$') in pref:
#                 pref = pref.replace(convert_regexp('(*,**)$'), '')
#             new = new.replace(pref, '', 1)
#             old = old.replace(pref, '', 1)
#
#         suff = commonsuffix(new, old)
#         if suffix:
#             pass
    new_reg = convert_regexp(new)
    if re.search(new_reg, old):
        return True

    return None

def logger_path():
    logger = conf.find_first_file(cfg['settings']['logger']) or '/bin/logger'
    if not os.path.isfile(logger) or not os.access(logger, os.EX_OK):
        raise AppArmorException("Can't find logger!\nPlease make sure %s exists, or update the 'logger' path in logprof.conf." % logger)
    return logger

######Initialisations######

conf = apparmor.config.Config('ini', CONFDIR)
cfg = conf.read_config('logprof.conf')

# prevent various failures if logprof.conf doesn't exist
if not cfg.sections():
    cfg.add_section('settings')
    cfg.add_section('required_hats')

if cfg['settings'].get('default_owner_prompt', False):
    cfg['settings']['default_owner_prompt'] = ''

profile_dir = conf.find_first_dir(cfg['settings'].get('profiledir')) or '/etc/apparmor.d'
if not os.path.isdir(profile_dir):
    raise AppArmorException('Can\'t find AppArmor profiles')

extra_profile_dir = conf.find_first_dir(cfg['settings'].get('inactive_profiledir')) or '/usr/share/apparmor/extra-profiles/'

parser = conf.find_first_file(cfg['settings'].get('parser')) or '/sbin/apparmor_parser'
if not os.path.isfile(parser) or not os.access(parser, os.EX_OK):
    raise AppArmorException('Can\'t find apparmor_parser')

