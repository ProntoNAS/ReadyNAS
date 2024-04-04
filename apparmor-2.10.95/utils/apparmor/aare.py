# ----------------------------------------------------------------------
#    Copyright (C) 2015 Christian Boltz <apparmor@cboltz.de>
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

import re

from apparmor.common import convert_regexp, type_is_str, AppArmorBug, AppArmorException

class AARE(object):
    '''AARE (AppArmor Regular Expression) wrapper class'''

    def __init__(self, regex, is_path, log_event=None):
        '''create an AARE instance for the given AppArmor regex
        If is_path is true, the regex is expected to be a path and therefore must start with / or a variable.'''
        # using the specified variables when matching.

        if is_path:
            if regex.startswith('/'):
                pass
            elif regex.startswith('@{'):
                pass  # XXX ideally check variable content - each part must start with / - or another variable, which must start with /
            else:
                raise AppArmorException("Path doesn't start with / or variable: %s" % regex)

        if log_event:
            self.orig_regex = regex
            self.regex = convert_expression_to_aare(regex)
        else:
            self.orig_regex = None
            self.regex = regex

        self._regex_compiled = None  # done on first use in match() - that saves us some re.compile() calls
        # self.variables = variables  # XXX

    def __repr__(self):
        '''returns a "printable" representation of AARE'''
        return "AARE('%s')" % self.regex

    def __deepcopy__(self, memo):
        # thanks to http://bugs.python.org/issue10076, we need to implement this ourself
        if self.orig_regex:
            return AARE(self.orig_regex, is_path=False, log_event=True)
        else:
            return AARE(self.regex, is_path=False)

    def match(self, expression):
        '''check if the given expression (string or AARE) matches the regex'''

        if type(expression) == AARE:
            if expression.orig_regex:
                expression = expression.orig_regex
            else:
                return self.is_equal(expression)  # better safe than sorry
        elif not type_is_str(expression):
            raise AppArmorBug('AARE.match() called with unknown object: %s' % str(expression))

        if self._regex_compiled is None:
            self._regex_compiled = re.compile(convert_regexp(self.regex))

        return bool(self._regex_compiled.match(expression))

    def is_equal(self, expression):
        '''check if the given expression is equal'''

        if type(expression) == AARE:
            return self.regex == expression.regex
        elif type_is_str(expression):
            return self.regex == expression
        else:
            raise AppArmorBug('AARE.is_equal() called with unknown object: %s' % str(expression))


def convert_expression_to_aare(expression):
    '''convert an expression (taken from audit.log) to an AARE string'''

    aare_escape_chars = ['\\', '?', '*', '[', ']', '{', '}', '"', '!']
    for char in aare_escape_chars:
        expression = expression.replace(char, '\\' + char)

    return expression
