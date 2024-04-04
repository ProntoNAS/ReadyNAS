#!/usr/bin/python
#
# Copyright (C) 2011, Nokia Corporation <ivan.frade@nokia.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#

"""
Tests trying to simulate the behaviour of applications working with tracker
"""

import os
import random

import unittest2 as ut
from common.utils.applicationstest import CommonTrackerApplicationTest as CommonTrackerApplicationTest
from common.utils.helpers import log


class TrackerCameraTestSuite (CommonTrackerApplicationTest):
    """
    Common functionality for camera tests.
    """

    def insert_photo_resource_info (self, urn, file_url):
        """
        Insert new photo resource in the store, including nie:mimeType and nie:url
        """
        insert = """
        INSERT { <%(urn)s>
            a nie:InformationElement,
            nie:DataObject,
            nfo:Image,
            nfo:Media,
            nfo:Visual,
            nmm:Photo
        }

        DELETE { <%(urn)s> nie:mimeType ?_1 }
        WHERE { <%(urn)s> nie:mimeType ?_1 }

        INSERT { <%(urn)s>
            a rdfs:Resource ;
            nie:mimeType \"image/jpeg\"
        }

        DELETE { <%(urn)s> nie:url ?_2 }
        WHERE { <%(urn)s> nie:url ?_2 }

        INSERT { <%(urn)s>
            a rdfs:Resource ;
            nie:url \"%(file_url)s\" ;
            nie:isStoredAs <%(urn)s>
        }
        """ % locals()
        self.tracker.update (insert)
        self.assertEquals (self.get_urn_count_by_url (file_url), 1)

    def insert_video_resource_info (self, urn, file_url):
        """
        Insert new video resource in the store, including nie:mimeType and nie:url
        """
        insert = """
        INSERT { <%(urn)s>
            a nie:InformationElement,
            nie:DataObject,
            nfo:Video,
            nfo:Media,
            nfo:Visual,
            nmm:Video
        }

        DELETE { <%(urn)s> nie:mimeType ?_1 }
        WHERE { <%(urn)s> nie:mimeType ?_1 }

        INSERT { <%(urn)s>
            a rdfs:Resource ;
            nie:mimeType \"video/mp4\"
        }

        DELETE { <%(urn)s> nie:url ?_2 }
        WHERE { <%(urn)s> nie:url ?_2 }

        INSERT { <%(urn)s>
            a rdfs:Resource ;
            nie:url \"%(file_url)s\" ;
            nie:isStoredAs <%(urn)s>
        }
        """ % locals()
        self.tracker.update (insert)
        self.assertEquals (self.get_urn_count_by_url (file_url), 1)

    def insert_dummy_location_info (self, fileurn, geolocationurn, postaladdressurn):
        """
        Insert placeholder location info for a file
        """
        location_insert = """
        INSERT { <%s> a             nco:PostalAddress ;
                      nco:country  \"SPAIN\" ;
                      nco:locality \"Tres Cantos\"
        }

        INSERT { <%s> a                 slo:GeoLocation ;
                      slo:postalAddress <%s>
        }

        INSERT { <%s> a            rdfs:Resource ;
                      slo:location <%s>
        }
        """ % (postaladdressurn, geolocationurn, postaladdressurn, fileurn, geolocationurn)
        self.tracker.update (location_insert)


class TrackerCameraPicturesApplicationTests (TrackerCameraTestSuite):

    def test_01_camera_picture (self):
        """
        Camera simulation:

        1. Create resource in the store for the new file
        2. Write the file
        3. Wait for miner-fs to index it
        4. Ensure no duplicates are found
        """

        fileurn = "tracker://test_camera_picture_01/" + str(random.randint (0,100))
        origin_filepath = os.path.join (self.get_data_dir (), self.get_test_image ())
        dest_filepath = os.path.join (self.get_dest_dir (), self.get_test_image ())
        dest_fileuri = "file://" + dest_filepath

        self.insert_photo_resource_info (fileurn, dest_fileuri)

        # Copy the image to the dest path
        self.slowcopy_file (origin_filepath, dest_filepath)
        assert os.path.exists (dest_filepath)
        dest_id, dest_urn = self.system.store.await_resource_inserted ('nmm:Photo', dest_fileuri)
        self.assertEquals (self.get_urn_count_by_url (dest_fileuri), 1)

        # Clean the new file so the test directory is as before
        log ("Remove and wait")
        os.remove (dest_filepath)
        self.system.store.await_resource_deleted (dest_id)
        self.assertEquals (self.get_urn_count_by_url (dest_fileuri), 0)

    def test_02_camera_picture_geolocation (self):
        """
        Camera simulation:

        1. Create resource in the store for the new file
        2. Set nlo:location
        2. Write the file
        3. Wait for miner-fs to index it
        4. Ensure no duplicates are found
        """

        fileurn = "tracker://test_camera_picture_02/" + str(random.randint (0,100))
        dest_filepath = os.path.join (self.get_dest_dir (), self.get_test_image ())
        dest_fileuri = "file://" + dest_filepath

        geolocationurn = "tracker://test_camera_picture_02_geolocation/" + str(random.randint (0,100))
        postaladdressurn = "tracker://test_camera_picture_02_postaladdress/" + str(random.randint (0,100))

        self.insert_photo_resource_info (fileurn, dest_fileuri)

        # FIRST, open the file for writing, and just write some garbage, to simulate that
        # we already started recording the video...
        fdest = open (dest_filepath, 'wb')
        fdest.write ("some garbage written here")
        fdest.write ("to simulate we're recording something...")
        fdest.seek (0)

        # SECOND, set slo:location
        self.insert_dummy_location_info (fileurn, geolocationurn, postaladdressurn)

        #THIRD, start copying the image to the dest path
        original_file = os.path.join (self.get_data_dir (),self.get_test_image ())
        self.slowcopy_file_fd (original_file, fdest)
        fdest.close ()
        assert os.path.exists (dest_filepath)

        # FOURTH, ensure we have only 1 resource
        dest_id, dest_urn = self.system.store.await_resource_inserted ('nmm:Photo', dest_fileuri)
        self.assertEquals (self.get_urn_count_by_url (dest_fileuri), 1)

        # Clean the new file so the test directory is as before
        log ("Remove and wait")
        os.remove (dest_filepath)
        self.system.store.await_resource_deleted (dest_id)
        self.assertEquals (self.get_urn_count_by_url (dest_fileuri), 0)


class TrackerCameraVideosApplicationTests (TrackerCameraTestSuite):

    def test_01_camera_video (self):
        """
        Camera video recording simulation:

        1. Create resource in the store for the new file
        2. Write the file
        3. Wait for miner-fs to index it
        4. Ensure no duplicates are found
        """

        fileurn = "tracker://test_camera_video_01/" + str(random.randint (0,100))
        origin_filepath = os.path.join (self.get_data_dir (), self.get_test_video ())
        dest_filepath = os.path.join (self.get_dest_dir (), self.get_test_video ())
        dest_fileuri = "file://" + dest_filepath

        self.insert_video_resource_info(fileurn, dest_fileuri)

        # Copy the image to the dest path
        self.slowcopy_file (origin_filepath, dest_filepath)
        assert os.path.exists (dest_filepath)
        dest_id, dest_urn = self.system.store.await_resource_inserted ('nmm:Video', dest_fileuri)
        self.assertEquals (self.get_urn_count_by_url (dest_fileuri), 1)

        # Clean the new file so the test directory is as before
        log ("Remove and wait")
        os.remove (dest_filepath)
        self.system.store.await_resource_deleted (dest_id)
        self.assertEquals (self.get_urn_count_by_url (dest_fileuri), 0)


    def test_02_camera_video_geolocation (self):
        """
        Camera simulation:

        1. Create resource in the store for the new file
        2. Set nlo:location
        2. Write the file
        3. Wait for miner-fs to index it
        4. Ensure no duplicates are found
        """

        fileurn = "tracker://test_camera_video_02/" + str(random.randint (0,100))
        origin_filepath = os.path.join (self.get_data_dir (), self.get_test_video ())
        dest_filepath = os.path.join (self.get_dest_dir (), self.get_test_video ())
        dest_fileuri = "file://" + dest_filepath

        geolocationurn = "tracker://test_camera_video_02_geolocation/" + str(random.randint (0,100))
        postaladdressurn = "tracker://test_camera_video_02_postaladdress/" + str(random.randint (0,100))

        self.insert_video_resource_info (fileurn, dest_fileuri)

        # FIRST, open the file for writing, and just write some garbage, to simulate that
        # we already started recording the video...
        fdest = open (dest_filepath, 'wb')
        fdest.write ("some garbage written here")
        fdest.write ("to simulate we're recording something...")
        fdest.seek (0)

        # SECOND, set slo:location
        self.insert_dummy_location_info (fileurn, geolocationurn, postaladdressurn)

        #THIRD, start copying the image to the dest path
        self.slowcopy_file_fd (origin_filepath, fdest)
        fdest.close ()
        assert os.path.exists (dest_filepath)

        # FOURTH, ensure we have only 1 resource
        dest_id, dest_urn = self.system.store.await_resource_inserted ('nmm:Video', dest_fileuri)
        self.assertEquals (self.get_urn_count_by_url (dest_fileuri), 1)

        # Clean the new file so the test directory is as before
        log ("Remove and wait")
        os.remove (dest_filepath)
        self.system.store.await_resource_deleted (dest_id)
        self.assertEquals (self.get_urn_count_by_url (dest_fileuri), 0)

if __name__ == "__main__":
	ut.main()


