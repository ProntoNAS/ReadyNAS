#!/usr/bin/python

## Code taken and modified from unittest2 framework (case.py)

## Copyright (c) 1999-2003 Steve Purcell
## Copyright (c) 2003-2010 Python Software Foundation
## Copyright (c) 2010, Nokia (ivan.frade@nokia.com)

## This module is free software, and you may redistribute it and/or modify
## it under the same terms as Python itself, so long as this copyright message
## and disclaimer are retained in their original form.

## IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
## SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF
## THIS CODE, EVEN IF THE AUTHOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
## DAMAGE.

## THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
## PARTICULAR PURPOSE.  THE CODE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS,
## AND THERE IS NO OBLIGATION WHATSOEVER TO PROVIDE MAINTENANCE,
## SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

"""
Write values in tracker and check the actual values are written
on the files. Note that these tests are highly platform dependant.
"""
import sys
import unittest2 as ut
from unittest2.compatibility import wraps
import configuration as cfg

def expectedFailureBug(bugnumber):
    """
    Decorator to mark bugs with ExpectedFailure. In case that a expected failure PASS
    it will raise an exception pointing to the Bug number.

    Keep your bugs and tests in sync!
    """
    def decorator (func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            try:
                func(*args, **kwargs)
            except Exception:
                raise ut.case._ExpectedFailure(sys.exc_info())
            raise Exception ("Unexpected success. This should fail because of bug " +str(bugnumber))
        return wrapper
    return decorator

def expectedFailureJournal():
    """
    Decorator to handle tests that are expected to fail when journal is disabled.
    """
    def decorator (func):
        # no wrapping if journal is enabled, test is expected to pass
        if not cfg.disableJournal:
            return func

        @wraps(func)
        def wrapper(*args, **kwargs):
            try:
                func(*args, **kwargs)
            except Exception:
                raise ut.case._ExpectedFailure(sys.exc_info())
            raise Exception ("Unexpected success. This should fail because journal is disabled")
        return wrapper
    return decorator
