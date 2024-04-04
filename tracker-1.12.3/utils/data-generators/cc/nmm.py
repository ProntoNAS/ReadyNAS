# -*- coding: utf-8 -*-

import tools
import gen_data as gen

####################################################################################
nfo_Equipment = '''
<%(equipment_uri)s> a nfo:Equipment;
    nfo:manufacturer "%(equipment_manufacturer)s";
    nfo:model "%(equipment_model)s".
'''
def generateEquipment(index):
  me = 'nfo#Equipment'
  equipment_manufacturer = gen.create_equipment_manufacturer()
  equipment_model = gen.create_equipment_model(equipment_manufacturer)
  equipment_uri = 'urn:equipment:%s:%s:' % (equipment_manufacturer,equipment_model)

  tools.addItem( me, equipment_uri, nfo_Equipment % locals() )

def generateOwnEquipment():
  me = 'nfo#Equipment'
  equipment_manufacturer = 'NOKIA'
  equipment_model = ''
  equipment_uri = 'urn:equipment:%s:%s:' % (equipment_manufacturer,equipment_model)

  tools.addItem( me, equipment_uri, nfo_Equipment % locals() )  


####################################################################################
nmm_Photo = '''
<%(photo_uri)s> a nie:DataObject, nfo:FileDataObject, nfo:Media, nfo:Visual, nfo:Image, nmm:Photo ;
    tracker:available    true ;
    nie:url              "%(photo_url)s";
    nie:byteSize         "%(photo_size)s";
    nie:contentCreated   "%(photo_date)s";
    nie:mimeType         "image/jpeg";
    nfo:width            "100";
    nfo:height           "100";
    nfo:fileLastModified "%(photo_date)s";
    nfo:fileLastAccessed "%(photo_date)s";
    nfo:fileSize         "%(photo_size)s";
    nfo:fileName         "%(photo_filename)s" ;
    nfo:equipment        <%(photo_equipment)s> ;
    nmm:exposureTime     "%(photo_exposure)s";
    nmm:fnumber          "%(photo_fnumber)s";
    nmm:focalLength      "%(photo_focal_length)s";
    nmm:flash            <http://www.tracker-project.org/temp/nmm#flash-off> ;
    nmm:meteringMode     <http://www.tracker-project.org/temp/nmm#meteringMode-pattern> ;
    nmm:whiteBalance     <http://www.tracker-project.org/temp/nmm#whiteBalance-auto> ;
    nao:hasTag           [a nao:Tag ; nao:prefLabel "%(photo_tag)s"] .
'''
def generatePhoto(index):
  me = 'nmm#Photo'
  photo_uri          = 'urn:photo:%d' % index
  photo_filename     = 'photo%d.jpg' % index
  photo_url          = 'file:///path/' + photo_filename
  photo_size         = str(1000000 + index)
  photo_equipment    = tools.getRandomUri( 'nfo#Equipment' )
  photo_exposure     = '0.%d' % index
  photo_fnumber      = '%d.0' % (1 + (index % 20))
  photo_focal_length = '%d.0' % (1 + (index % 500))
  photo_date         = '%d-%02d-%02dT%02d:01:01Z' % (1100 + (index % 997), (index % 12) + 1, (index % 31) + 1, (index %24))
  photo_tag          = ('TEST', 'nomatch') [index %2]

  tools.addItem( me, photo_uri, nmm_Photo % locals() )

####################################################################################
nmm_Video = '''
<%(video_uri)s> a nie:DataObject, nfo:FileDataObject, nfo:Media, nfo:Visual, nfo:Video, nmm:Video ;
    nie:url              "%(video_url)s";
    nie:byteSize         "%(video_size)s";
    nie:contentCreated   "%(video_date)s";
    nie:mimeType         "video/x-msvideo";
    nfo:width            "100";
    nfo:height           "100";
    nfo:fileLastModified "%(video_date)s";
    nfo:fileLastAccessed "%(video_date)s";
    nfo:fileSize         "%(video_size)s";
    nfo:fileName         "%(video_filename)s" ;
    nfo:equipment        <%(video_equipment)s> ;
    nfo:sampleRate       "%(video_samplerate)s" ;
    nfo:duration         "%(video_duration)s" ;
    nao:hasTag           [a nao:Tag ; nao:prefLabel "%(video_tag)s"] .
'''
def generateVideo(index):
  me = 'nmm#Video'
  video_uri          = 'urn:video:%d' % index
  video_filename     = 'video%d.jpg' % index
  video_url          = 'file:///path/' + video_filename
  video_size         = str(1000000 + index)
  video_date         = '%d-%02d-%02dT01:01:01Z' % (2000 + (index % 10), (index % 12) + 1, (index % 25) + 1)
  video_equipment    = tools.getRandomUri( 'nfo#Equipment' )
  video_samplerate   = str(index)
  video_duration     = str(index)
  video_tag          = ('TEST', 'nomatch') [index %2]

  tools.addItem( me, video_uri, nmm_Video % locals() )

####################################################################################
nmm_Artist = '''
<%(artist_uri)s> a nmm:Artist ;
    nmm:artistName "%(artist_name)s" .
'''
def generateArtist(index):
  me = 'nmm#Artist'
  artist_uri  = 'urn:artist:%d' % index
  artist_name = 'Artist %d' % (index % 1000)

  tools.addItem( me, artist_uri, nmm_Artist % locals() )

####################################################################################
nmm_MusicAlbumDisc = '''
<%(album_disc_uri)s> a nmm:MusicAlbumDisc .
'''
def generateMusicAlbumDisc(index):
  me = 'nmm#MusicAlbumDisc'
  album_disc_uri = 'urn:album-disk:%d' % index
  album_disc_name = 'Album Disc %d' % (index % 1000)

  tools.addItem( me, album_disc_uri, nmm_MusicAlbumDisc % locals() )

  return album_disc_uri

####################################################################################
nmm_MusicAlbum = '''
<%(album_uri)s> a nmm:MusicAlbum ;
    nie:title        "%(album_name)s" .
'''
def generateAlbum(index):
  me = 'nmm#MusicAlbum'
  album_uri  = 'urn:album:%d' % index
  album_name = 'Album %d' % (index % 1000)

  tools.addItem( me, album_uri, nmm_MusicAlbum % locals() )


####################################################################################
nmm_MusicPiece = '''
<%(music_piece_uri)s> a nmm:MusicPiece, nfo:FileDataObject, nfo:Audio;
    tracker:available          true ;
    nie:byteSize               "%(music_piece_size)s" ;
    nie:url                    "%(music_piece_url)s" ;
    nfo:belongsToContainer     <%(music_piece_container)s> ;
    nie:title                  "%(music_piece_title)s" ;
    nie:mimeType               "%(music_piece_mime)s" ;
    nie:contentCreated         "%(music_piece_date)s" ;
    nie:isLogicalPartOf        <%(music_piece_album)s> ;
    nco:contributor            <%(music_piece_artist)s> ;
    nfo:fileLastAccessed       "%(music_piece_last_accessed)s" ;
    nfo:fileSize               "%(music_piece_size)s" ;
    nfo:fileName               "%(music_piece_filename)s" ;
    nfo:fileLastModified       "%(music_piece_last_modified)s" ;
    nfo:codec                  "%(music_piece_codec)s" ;
    nfo:averageBitrate         "%(music_piece_bitrate)s" ;
    nfo:genre                  "%(music_piece_genre)s" ;
    nfo:channels               "%(music_piece_channels)s" ;
    nfo:sampleRate             "%(music_piece_sample_rate)s" ;
    nmm:musicAlbum             <%(music_piece_album)s> ;
    nmm:musicAlbumDisc         <%(music_piece_album_disk)s> ;
    nmm:performer              <%(music_piece_artist)s> ;
    nfo:duration               "%(music_piece_length)s" ;
    nmm:trackNumber            "%(music_piece_track)s" .
'''
def generateMusicPiece(index, discUri):
  me = 'nmm#MusicPiece'
  music_piece_uri           = 'urn:music:%d' % index
  music_piece_title         = 'Song %d' % index
  music_piece_size          = str(index)
  music_piece_container     = 'file://music/'
  music_piece_filename      = 'Song_%d.mp3' % index
  music_piece_url           = music_piece_container + music_piece_filename
  music_piece_mime          = 'audio/mpeg'
  music_piece_date          = '%d-%02d-%02dT01:01:01Z' % (2000 + (index % 10), (index % 12) + 1, (index % 25) + 1)
  music_piece_last_accessed = music_piece_date
  music_piece_last_modified = music_piece_date
  music_piece_codec         = 'MPEG'
  music_piece_bitrate       = '%d' % (16 + (index % 300))
  music_piece_genre         = 'Genre %d' % (index % 1000)
  music_piece_channels      = '2'
  music_piece_sample_rate   = '44100.0'
  music_piece_album         = tools.getLastUri( 'nmm#MusicAlbum' )
  music_piece_artist        = tools.getLastUri( 'nmm#Artist' )
  music_piece_album_disk    = discUri
  music_piece_length        = '%d' % (1 + (index % 1000))
  music_piece_track         = '%d' % (1 + (index % 100))

  tools.addItem( me, music_piece_uri, nmm_MusicPiece % locals() )
