#!/usr/bin/python

# Copyright (C) 2010, Nokia (ivan.frade@nokia.com)
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
#
"""
Write values in tracker and check the actual values are written
on the files. Note that these tests are highly platform dependant.
"""
import os
import time

from common.utils.extractor import get_tracker_extract_output
from common.utils.writebacktest import CommonTrackerWritebackTest as CommonTrackerWritebackTest
import unittest2 as ut
from common.utils.expectedFailure import expectedFailureBug

REASONABLE_TIMEOUT = 5 # Seconds we wait for tracker-writeback to do the work

class WritebackBasicDataTest (CommonTrackerWritebackTest):
    """
    Write in tracker store the properties witih writeback support and check
    that the new values are actually in the file
    """
    def setUp (self):
        self.tracker = self.system.store
        self.extractor = self.system.extractor

    def __clean_property (self, property_name, fileuri, expectFailure=True):
        """
        Remove the property for the fileuri (file://...)
        """
        CLEAN = """
           DELETE { ?u %s ?whatever }
           WHERE  {
               ?u nie:url '%s' ;
                  %s ?whatever .
           
           }
        """
        try:
            self.tracker.update (CLEAN % (property_name, fileuri, property_name))
        except Exception, e:
            print e
            assert expectFailure
                                

    def __writeback_test (self, filename, mimetype, prop, expectedKey=None):
        """
        Set a value in @prop for the @filename. Then ask tracker-extractor
        for metadata and check in the results dictionary if the property is there.

        Note: given the special translation of some property-names in the dictionary
        with extracted metadata, there is an optional parameter @expectedKey
        to specify what property to check in the dictionary. If None, then
        the @prop is used.
        """

        # FIXME: filename is actually a URI! :(
        filename_real = filename[len('file://'):]
        initial_mtime = os.stat(filename_real).st_mtime

        TEST_VALUE = prop.replace (":","") + "test"
        SPARQL_TMPL = """
           INSERT { ?u %s '%s' }
           WHERE  { ?u nie:url '%s' }
        """ 
        self.__clean_property (prop, filename)
        self.tracker.update (SPARQL_TMPL % (prop, TEST_VALUE, filename))

        self.wait_for_file_change(filename_real, initial_mtime)

        results = get_tracker_extract_output (filename, mimetype)
        keyDict = expectedKey or prop
        self.assertIn (TEST_VALUE, results[keyDict])
        self.__clean_property (prop, filename, False)


    def __writeback_hasTag_test (self, filename, mimetype):

        SPARQL_TMPL = """
            INSERT {
              <test://writeback-hasTag-test/1> a nao:Tag ;
                        nao:prefLabel "testTag" .

              ?u nao:hasTag <test://writeback-hasTag-test/1> .
            } WHERE {
              ?u nie:url '%s' .
            }
        """

        CLEAN_VALUE = """
           DELETE {
              <test://writeback-hasTag-test/1> a rdfs:Resource.
              ?u nao:hasTag <test://writeback-hasTag-test/1> .
           } WHERE {
              ?u nao:hasTag <test://writeback-hasTag-test/1> .
           }
        """

        self.tracker.update (SPARQL_TMPL % (filename))

        time.sleep (REASONABLE_TIMEOUT)

        results = get_tracker_extract_output (filename, mimetype)
        self.assertIn ("testTag", results ["nao:hasTag"])


    # JPEG test
    def test_001_jpeg_title (self):
        #FILENAME = "test-writeback-monitored/writeback-test-1.jpeg"
        self.__writeback_test (self.get_test_filename_jpeg (), "image/jpeg", "nie:title")

    def test_002_jpeg_description (self):
        #FILENAME = "test-writeback-monitored/writeback-test-1.jpeg"
        self.__writeback_test (self.get_test_filename_jpeg (), "image/jpeg", "nie:description")

    def test_003_jpeg_keyword (self):
        #FILENAME = "test-writeback-monitored/writeback-test-1.jpeg"
        self.__writeback_test (self.get_test_filename_jpeg (), "image/jpeg",
                               "nie:keyword", "nao:hasTag")

    def test_004_jpeg_hasTag (self):
        #FILENAME = "test-writeback-monitored/writeback-test-1.jpeg"
        self.__writeback_hasTag_test (self.get_test_filename_jpeg (), "image/jpeg")

        
    # TIFF tests
    def test_011_tiff_title (self):
        #FILANAME = "test-writeback-monitored/writeback-test-2.tif"
        self.__writeback_test (self.get_test_filename_tiff (), "image/tiff", "nie:title")

    def test_012_tiff_description (self):
        FILENAME = "test-writeback-monitored/writeback-test-2.tif"
        self.__writeback_test (self.get_test_filename_tiff (), "image/tiff", "nie:description")
        
    def test_013_tiff_keyword (self):
        FILENAME = "test-writeback-monitored/writeback-test-2.tif"
        self.__writeback_test (self.get_test_filename_tiff (), "image/tiff",
                               "nie:keyword", "nao:hasTag")

    def test_014_tiff_hasTag (self):
        FILENAME = "test-writeback-monitored/writeback-test-2.tif"
        self.__writeback_hasTag_test (self.get_test_filename_tiff (), "image/tiff")
      
        

    # PNG tests
    @expectedFailureBug ("NB#185070")
    def test_021_png_title (self):
        FILENAME = "test-writeback-monitored/writeback-test-4.png"
        self.__writeback_test (self.get_test_filaname_png (), "image/png", "nie:title")

    @expectedFailureBug ("NB#185070")
    def test_022_png_description (self):
        FILENAME = "test-writeback-monitored/writeback-test-4.png"
        self.__writeback_test (self.get_test_filaname_png (), "image/png", "nie:description")
        
    @expectedFailureBug ("NB#185070")
    def test_023_png_keyword (self):
        FILENAME = "test-writeback-monitored/writeback-test-4.png"
        self.__writeback_test (self.get_test_filaname_png (), "image/png", "nie:keyword", "nao:hasTag:prefLabel")

    @expectedFailureBug("NB#185070")
    def test_024_png_hasTag (self):
        FILENAME = "test-writeback-monitored/writeback-test-4.png"
        self.__writeback_hasTag_test (self.get_test_filaname_png (), "image/png")

if __name__ == "__main__":
    ut.main ()
