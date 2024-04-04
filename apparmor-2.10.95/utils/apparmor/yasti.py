# ----------------------------------------------------------------------
#    Copyright (C) 2013 Kshitij Gupta <kgupta8592@gmail.com>
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
import sys
try:
    import ycp
except ImportError:
    # ycp isn't found everywhere.
    ycp = None

from apparmor.common import error, DebugLogger

# Set up UI logger for separate messages from YaST module
debug_logger = DebugLogger('YaST')


def setup_yast():
    # To-Do
    pass

def shutdown_yast():
    # To-Do
    pass

def yastLog(text):
    ycp.y2milestone(text)

def SendDataToYast(data):
    debug_logger.info('SendDataToYast: Waiting for YCP command')
    for line in sys.stdin:
        ycommand, ypath, yargument = ParseCommand(line)
        if ycommand and ycommand == 'Read':
            debug_logger.info('SendDataToYast: Sending--%s' % data)
            ycp.Return(data)
            return True
        else:
            debug_logger.info('SendDataToYast: Expected \'Read\' but got-- %s' % line)
    error('SendDataToYast: didn\'t receive YCP command before connection died')

def GetDataFromYast():
    debug_logger.inf('GetDataFromYast: Waiting for YCP command')
    for line in sys.stdin:
        debug_logger.info('GetDataFromYast: YCP: %s' % line)
        ycommand, ypath, yarg = ParseCommand(line)
        debug_logger.info('GetDataFromYast: Recieved--\n%s' % yarg)
        if ycommand and ycommand == 'Write':
            ycp.Return('true')
            return ypath, yarg
        else:
            debug_logger.info('GetDataFromYast: Expected Write but got-- %s' % line)
    error('GetDataFromYast: didn\'t receive YCP command before connection died')

def ParseCommand(commands):
    term = ParseTerm(commands)
    if term:
        command = term[0]
        term = term[1:]
    else:
        command = ''
    path = ''
    pathref = None
    if term:
        pathref = term[0]
        term = term[1:]
    if pathref:
        if pathref.strip():
            path = pathref.strip()
        elif command != 'result':
            ycp.y2error('The first arguement is not a path. (%s)' % pathref)
    argument = None
    if term:
        argument = term[0]
    if len(term) > 1:
        ycp.y2warning('Superfluous command arguments ignored')
    return (command, path, argument)

def ParseTerm(inp):
    regex_term = re.compile('^\s*`?(\w*)\s*')
    term = regex_term.search(inp)
    ret = []
    symbol = None
    if term:
        symbol = term.groups()[0]
    else:
        ycp.y2error('No term symbol')
    ret.append(symbol)
    inp = regex_term.sub('', inp)
    if not inp.startswith('('):
        ycp.y2error('No term parantheses')
    argref, err, rest = ycp.ParseYcpTermBody(inp)
    if err:
        ycp.y2error('%s (%s)' % (err, rest))
    else:
        ret += argref
    return ret
