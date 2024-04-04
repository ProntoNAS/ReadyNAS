#! /usr/bin/env python
# ------------------------------------------------------------------
#
#    Copyright (C) 2015 Christian Boltz <apparmor@cboltz.de>
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of version 2 of the GNU General Public
#    License published by the Free Software Foundation.
#
# ------------------------------------------------------------------

import unittest
from common_test import AATest, setup_all_loops
import apparmor.aa as apparmor

import os
from apparmor.common import open_file_read, AppArmorException

# This testcase will parse all parser/tst/simple_tests with parse_profile_data(),
# except the files listed in one of the arrays below.
#
# Files listed in skip_startswith will be completely skipped.
# Files listed in the other arrays will be checked, but with the opposite of the expected result.


# XXX tests listed here will be *** SKIPPED *** XXX
skip_startswith = (
    # lots of invalid dbus rules (the tools currently just store them without any parsing)
    'generated_dbus/bad-perms-',
    'generated_dbus/bad-formatting-',
    'generated_dbus/duplicated-conditionals-',
    'generated_dbus/eavesdrop-incompat-',
    'generated_dbus/message-incompat-',
    'generated_dbus/pairing-unsupported-',
    'generated_dbus/service-incompat-',

    # the tools don't check for conflicting x permissions (yet?)
    'generated_x/conflict-',
    'generated_x/ambiguous-',
    'generated_x/dominate-',

    # permissions before path
    'generated_perms_leading/',

    # 'safe' and 'unsafe' keywords
    'generated_perms_safe/',

    # Pux and Cux (which actually mean PUx and CUx) get rejected by the tools
    'generated_x/exact-',
)

# testcases that should raise an exception, but don't
exception_not_raised = [
    'capability/bad_1.sd',
    'capability/bad_2.sd',
    'capability/bad_3.sd',
    'capability/bad_4.sd',
    'change_hat/bad_parsing.sd',
    'dbus/bad_bind_1.sd',
    'dbus/bad_bind_2.sd',
    'dbus/bad_eavesdrop_1.sd',
    'dbus/bad_modifier_1.sd',
    'dbus/bad_modifier_2.sd',
    'dbus/bad_modifier_3.sd',
    'dbus/bad_modifier_4.sd',
    'dbus/bad_peer_1.sd',
    'dbus/bad_regex_01.sd',
    'dbus/bad_regex_02.sd',
    'dbus/bad_regex_03.sd',
    'dbus/bad_regex_04.sd',
    'dbus/bad_regex_05.sd',
    'dbus/bad_regex_06.sd',
    'file/bad_append_1.sd',
    'file/bad_append_2.sd',
    'file/bad_embedded_spaces_1.sd',
    'file/bad_re_brace_2.sd',
    'file/bad_re_brace_3.sd',
    'file/file/bad_append_1.sd',
    'file/file/bad_embedded_spaces_1.sd',
    'file/file/owner/bad_3.sd',
    'file/file/owner/bad_5.sd',
    'file/owner/bad_3.sd',
    'file/owner/bad_5.sd',
    'mount/bad_opt_10.sd',
    'mount/bad_opt_11.sd',
    'mount/bad_opt_12.sd',
    'mount/bad_opt_13.sd',
    'mount/bad_opt_14.sd',
    'mount/bad_opt_15.sd',
    'mount/bad_opt_16.sd',
    'mount/bad_opt_17.sd',
    'mount/bad_opt_18.sd',
    'mount/bad_opt_19.sd',
    'mount/bad_opt_1.sd',
    'mount/bad_opt_20.sd',
    'mount/bad_opt_21.sd',
    'mount/bad_opt_22.sd',
    'mount/bad_opt_23.sd',
    'mount/bad_opt_24.sd',
    'mount/bad_opt_2.sd',
    'mount/bad_opt_3.sd',
    'mount/bad_opt_4.sd',
    'mount/bad_opt_5.sd',
    'mount/bad_opt_6.sd',
    'mount/bad_opt_7.sd',
    'mount/bad_opt_8.sd',
    'mount/bad_opt_9.sd',
    'profile/flags/flags_bad10.sd',
    'profile/flags/flags_bad11.sd',
    'profile/flags/flags_bad12.sd',
    'profile/flags/flags_bad13.sd',
    'profile/flags/flags_bad15.sd',
    'profile/flags/flags_bad18.sd',
    'profile/flags/flags_bad19.sd',
    'profile/flags/flags_bad20.sd',
    'profile/flags/flags_bad2.sd',
    'profile/flags/flags_bad3.sd',
    'profile/flags/flags_bad4.sd',
    'profile/flags/flags_bad5.sd',
    'profile/flags/flags_bad6.sd',
    'profile/flags/flags_bad7.sd',
    'profile/flags/flags_bad8.sd',
    'profile/flags/flags_bad_debug_1.sd',
    'profile/flags/flags_bad_debug_2.sd',
    'profile/flags/flags_bad_debug_3.sd',
    'profile/flags/flags_bad_debug_4.sd',
    'profile/simple_bad_no_close_brace4.sd',
    'profile/profile_ns_bad8.sd',  # 'profile :ns/t' without terminating ':'
    'ptrace/bad_05.sd',  # actually contains a capability rule with invalid (ptrace-related) keyword
    'ptrace/bad_06.sd',  # actually contains a capability rule with invalid (ptrace-related) keyword
    'ptrace/bad_10.sd',  # peer with invalid regex
    'signal/bad_21.sd',  # invalid regex
    'unix/bad_attr_1.sd',
    'unix/bad_attr_2.sd',
    'unix/bad_attr_3.sd',
    'unix/bad_attr_4.sd',
    'unix/bad_bind_1.sd',
    'unix/bad_bind_2.sd',
    'unix/bad_create_1.sd',
    'unix/bad_create_2.sd',
    'unix/bad_listen_1.sd',
    'unix/bad_listen_2.sd',
    'unix/bad_modifier_1.sd',
    'unix/bad_modifier_2.sd',
    'unix/bad_modifier_3.sd',
    'unix/bad_modifier_4.sd',
    'unix/bad_opt_1.sd',
    'unix/bad_opt_2.sd',
    'unix/bad_opt_3.sd',
    'unix/bad_opt_4.sd',
    'unix/bad_peer_1.sd',
    'unix/bad_regex_01.sd',
    'unix/bad_regex_02.sd',
    'unix/bad_regex_03.sd',
    'unix/bad_regex_04.sd',
    'unix/bad_shutdown_1.sd',
    'unix/bad_shutdown_2.sd',
    'vars/boolean/boolean_bad_1.sd',
    'vars/boolean/boolean_bad_2.sd',
    'vars/boolean/boolean_bad_3.sd',
    'vars/boolean/boolean_bad_4.sd',
    'vars/boolean/boolean_bad_6.sd',
    'vars/boolean/boolean_bad_7.sd',
    'vars/boolean/boolean_bad_8.sd',
    'vars/vars_bad_3.sd',
    'vars/vars_bad_4.sd',
    'vars/vars_bad_5.sd',
    'vars/vars_bad_7.sd',
    'vars/vars_bad_8.sd',
    'vars/vars_bad_trailing_comma_1.sd',
    'vars/vars_bad_trailing_comma_2.sd',
    'vars/vars_bad_trailing_comma_3.sd',
    'vars/vars_bad_trailing_comma_4.sd',
    'vars/vars_dbus_bad_01.sd',
    'vars/vars_dbus_bad_02.sd',
    'vars/vars_dbus_bad_03.sd',
    'vars/vars_dbus_bad_04.sd',
    'vars/vars_dbus_bad_05.sd',
    'vars/vars_dbus_bad_06.sd',
    'vars/vars_dbus_bad_07.sd',
    'vars/vars_file_evaluation_7.sd',
    'vars/vars_file_evaluation_8.sd',

    # profile name in var doesn't start with /
    'vars/vars_profile_name_bad_1.sd',

    # profile name is undefined variable
    'vars/vars_profile_name_bad_2.sd',
    'vars/vars_profile_name_23.sd',

    # attachment in var doesn't start with /
    'vars/vars_profile_name_07.sd',
    'vars/vars_profile_name_08.sd',
    'vars/vars_profile_name_12.sd',
    'vars/vars_profile_name_13.sd',
    'vars/vars_profile_name_15.sd',
    'vars/vars_profile_name_22.sd',

    'vars/vars_recursion_1.sd',
    'vars/vars_recursion_2.sd',
    'vars/vars_recursion_3.sd',
    'vars/vars_recursion_4.sd',
    'vars/vars_simple_assignment_10.sd',
    'vars/vars_simple_assignment_3.sd',
    'vars/vars_simple_assignment_8.sd',
    'vars/vars_simple_assignment_9.sd',
    'xtrans/simple_bad_conflicting_x_10.sd',
    'xtrans/simple_bad_conflicting_x_11.sd',
    'xtrans/simple_bad_conflicting_x_12.sd',
    'xtrans/simple_bad_conflicting_x_13.sd',
    'xtrans/simple_bad_conflicting_x_14.sd',
    'xtrans/simple_bad_conflicting_x_15.sd',
    'xtrans/simple_bad_conflicting_x_1.sd',
    'xtrans/simple_bad_conflicting_x_2.sd',
    'xtrans/simple_bad_conflicting_x_3.sd',
    'xtrans/simple_bad_conflicting_x_4.sd',
    'xtrans/simple_bad_conflicting_x_5.sd',
    'xtrans/simple_bad_conflicting_x_6.sd',
    'xtrans/simple_bad_conflicting_x_7.sd',
    'xtrans/simple_bad_conflicting_x_8.sd',
    'xtrans/simple_bad_conflicting_x_9.sd',
    'xtrans/x-conflict.sd',
]

# testcases with lines that don't match any regex and end up as "unknown line"
unknown_line = [
    # 'other' keyword
    'file/allow/ok_other_1.sd',
    'file/allow/ok_other_2.sd',
    'file/ok_other_1.sd',
    'file/ok_other_2.sd',
    'file/ok_other_3.sd',

    # permissions before path
    'file/file/front_perms_ok_1.sd',
    'file/front_perms_ok_1.sd',
    'profile/local/local_named_profile_ok1.sd',
    'profile/local/local_profile_ok1.sd',
    'xtrans/simple_ok_cx_1.sd',

    # permissions before path and owner / audit {...} blocks
    'file/file/owner/ok_1.sd',
    'file/owner/ok_1.sd',
    'profile/entry_mods_audit_ok1.sd',

    # namespace
    'profile/profile_ns_named_ok1.sd',  # profile keyword?
    'profile/profile_ns_named_ok2.sd',  # profile keyword?
    'profile/profile_ns_named_ok3.sd',  # profile keyword?
    'profile/profile_ns_ok1.sd',
    'profile/profile_ns_ok2.sd',
    'profile/profile_ns_ok3.sd',  # profile keyword?
    'profile/re_named_ok4.sd',  # profile keyword
    'profile/re_ok4.sd',
]

# testcases with various unexpected failures
syntax_failure = [
    # profile keyword?
    'profile/re_named_ok2.sd',

    # Syntax Error: Unexpected hat definition found (external hat)
    'change_hat/new_style4.sd',

    # Syntax Errors caused by boolean conditions (parse_profile_data() gets confused by the closing '}')
    'conditional/defined_1.sd',
    'conditional/defined_2.sd',
    'conditional/else_1.sd',
    'conditional/else_2.sd',
    'conditional/else_3.sd',
    'conditional/else_if_1.sd',
    'conditional/else_if_2.sd',
    'conditional/else_if_3.sd',
    'conditional/else_if_5.sd',
    'conditional/ok_1.sd',
    'conditional/ok_2.sd',
    'conditional/ok_3.sd',
    'conditional/ok_4.sd',
    'conditional/ok_5.sd',
    'conditional/ok_6.sd',
    'conditional/ok_7.sd',
    'conditional/ok_8.sd',
    'conditional/ok_9.sd',
    'conditional/stress_1.sd',

    # unexpected uppercase vs. lowercase in *x rules
    'file/ok_5.sd',  # Invalid mode UX
    'file/ok_2.sd',  # Invalid mode RWM
    'file/ok_4.sd',  # Invalid mode iX
    'xtrans/simple_ok_pix_1.sd',  # Invalid mode pIx
    'xtrans/simple_ok_pux_1.sd',  # Invalid mode rPux

    # misc
    'vars/vars_simple_assignment_12.sd',  # Redefining existing variable @{BAR} ('\' not handled)
    'rewrite/alias_good_5.sd',  # Values added to a non-existing variable @{FOO} (defined in include, lp:1331856)
    'bare_include_tests/ok_2.sd',  # two #include<...> in one line
]

class TestParseParserTests(AATest):
    tests = []  # filled by parse_test_profiles()

    def _run_test(self, params, expected):
        with open_file_read(params['file']) as f_in:
            data = f_in.readlines()

        if params['disabled']:
            # skip disabled testcases
            return

        if params['tools_wrong']:
            # if the tools are marked as being wrong about a profile, expect the opposite result
            # this makes sure we notice any behaviour change, especially not being wrong anymore
            expected = not expected

        if expected:
            apparmor.parse_profile_data(data, params['file'], 0)
        else:
            with self.assertRaises(AppArmorException):
                apparmor.parse_profile_data(data, params['file'], 0)

def parse_test_profiles(file_with_path):
    '''parse the test-related headers of a profile (for example EXRESULT) and add the profile to the set of tests'''
    exresult = None
    exresult_found = False
    description = None
    todo = False
    disabled = False

    with open_file_read(file_with_path) as f_in:
        data = f_in.readlines()

    relfile = os.path.relpath(file_with_path, apparmor.profile_dir)

    for line in data:
        if line.startswith('#=EXRESULT '):
            exresult = line.split()[1]
            if exresult == 'PASS':
                exresult == True
                exresult_found = True
            elif exresult == 'FAIL':
                exresult = False
                exresult_found = True
            else:
                raise Exception('%s contains unknown EXRESULT %s' % (file_with_path, exresult))

        elif line.upper().startswith('#=DESCRIPTION '):
            description = line.split()[1]

        elif line.rstrip() == '#=TODO':
            todo = True

        elif line.rstrip() == '#=DISABLED':
            disabled = True

    if not exresult_found:
        raise Exception('%s does not contain EXRESULT' % file_with_path)

    if not description:
        raise Exception('%s does not contain description' % file_with_path)

    tools_wrong = False
    if relfile in exception_not_raised:
        if exresult:
            raise Exception("%s listed in exception_not_raised, but has EXRESULT PASS" % file_with_path)
        tools_wrong = 'EXCEPTION_NOT_RAISED'
    elif relfile.startswith(skip_startswith):
        return 1  # XXX *** SKIP *** those tests
    elif relfile in unknown_line:
        if not exresult:
            raise Exception("%s listed in unknown_line, but has EXRESULT FAIL" % file_with_path)
        tools_wrong = 'UNKNOWN_LINE'
    elif relfile in syntax_failure:
        if not exresult:
            raise Exception("%s listed in syntax_failure, but has EXRESULT FAIL" % file_with_path)
        tools_wrong = 'SYNTAX_FAILURE'

    params = {
        'file': file_with_path,
        'relfile': relfile,
        'todo': todo,
        'disabled': disabled,
        'tools_wrong': tools_wrong,
        'exresult': exresult,
    }

    TestParseParserTests.tests.append((params, exresult))
    return 0


def find_and_setup_test_profiles(profile_dir):
    '''find all profiles in the given profile_dir, excluding
    - skippable files
    - include directories
    - files in the main directory (readme, todo etc.)
    '''
    skipped = 0

    profile_dir = os.path.abspath(profile_dir)

    apparmor.profile_dir = profile_dir

    print('Searching for parser simpe_tests... (this will take a while)')

    for root, dirs, files in os.walk(profile_dir):
        relpath = os.path.relpath(root, profile_dir)

        if relpath == '.':
            # include files are checked as part of the profiles that include them (also, they don't contain EXRESULT)
            dirs.remove('includes')
            dirs.remove('include_tests')
            dirs.remove('includes-preamble')

        for file in files:
            file_with_path = os.path.join(root, file)
            if not apparmor.is_skippable_file(file) and relpath != '.':
                skipped += parse_test_profiles(file_with_path)

    if skipped:
        print('Skipping %s test profiles listed in skip_startswith.' % skipped)

    print('Running %s parser simple_tests...' % len(TestParseParserTests.tests))


find_and_setup_test_profiles('../../parser/tst/simple_tests/')

setup_all_loops(__name__)
if __name__ == '__main__':
    unittest.main(verbosity=1)  # reduced verbosity due to the big number of tests
