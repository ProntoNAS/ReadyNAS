#! /usr/bin/env python
# ------------------------------------------------------------------
#
#    Copyright (C) 2014 Canonical Ltd.
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of version 2 of the GNU General Public
#    License published by the Free Software Foundation.
#
# ------------------------------------------------------------------

import apparmor.aa as aa
import unittest
from common_test import AAParseTest, setup_regex_tests

class AAParseDBUSTest(AAParseTest):

    def setUp(self):
        self.parse_function = aa.parse_dbus_rule

    tests = [
        ('dbus,', 'dbus base keyword'),
        ('dbus send,', 'dbus simple send rule'),
    ]

if __name__ == '__main__':
    setup_regex_tests(AAParseDBUSTest)
    unittest.main(verbosity=2)
