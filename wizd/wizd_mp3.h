/*
    mp3tech.h - Headers for mp3tech.c

    Copyright (C) 2000-2001  Cedric Tefft <cedric@earthling.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  ***************************************************************************

  This file is based in part on:

	* MP3Info 0.5 by Ricardo Cerqueira <rmc@rccn.net>
	* MP3Stat 0.9 by Ed Sweetman <safemode@voicenet.com> and 
			 Johannes Overmann <overmann@iname.com>

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <curses.h>
#include <signal.h>
#ifdef __CYGWIN32__
#include <getopt.h>
#endif

/* MIN_CONSEC_GOOD_FRAMES defines how many consecutive valid MP3 frames
   we need to see before we decide we are looking at a real MP3 file */
#define MIN_CONSEC_GOOD_FRAMES 4
#define FRAME_HEADER_SIZE 4
#define MIN_FRAME_SIZE 21
#define NUM_SAMPLES 4

enum VBR_REPORT { VBR_VARIABLE, VBR_AVERAGE, VBR_MEDIAN };
enum SCANTYPE { SCAN_NONE, SCAN_QUICK, SCAN_FULL };

typedef struct {
	unsigned long	sync;
	unsigned int	version;
	unsigned int	layer;
	unsigned int	crc;
	unsigned int	bitrate;
	unsigned int	freq;
	unsigned int	padding;
	unsigned int	extension;
	unsigned int	mode;
	unsigned int	mode_extension;
	unsigned int	copyright;
	unsigned int	original;
	unsigned int	emphasis;
} mp3header;

typedef struct {
	char title[31];
	char artist[31];
	char album[31];
	char year[5];
	char comment[31];
	unsigned char track[1];
	unsigned char genre[1];
} id3tag;

typedef struct {
	char *filename;
	FILE *file;
	off_t datasize;
	int header_isvalid;
	mp3header header;
	int id3_isvalid;
	id3tag id3;
	int vbr;
	float vbr_average;
	int seconds;
	int frames;
	int badframes;
} mp3info;


int get_header(FILE *file,mp3header *header);
int frame_length(mp3header *header);
int header_layer(mp3header *h);
int header_bitrate(mp3header *h);
int sameConstant(mp3header *h1, mp3header *h2);
int get_mp3_info(mp3info *mp3,int scantype, int fullscan_vbr);
int get_id3(mp3info *mp3);
char *pad(char *string, int length);
char *unpad(char *string);
int write_tag(mp3info *mp3);
int header_frequency(mp3header *h);
char *header_emphasis(mp3header *h);
char *header_mode(mp3header *h);
int get_first_header(mp3info *mp3,long startpos);
int get_next_header(mp3info *mp3);

#define CTRL_C		'\003'
#define TEXT_FIELD_LEN	30
#define INT_FIELD_LEN	4

void CenterText(int line, char *text);
char *checkinput(int c, char *string, int length);
char *curs_addparam(WINDOW *win, int line, int length, char *buf);
void buildtag(char *buf, id3tag *tag);
void display_help();
void display_genres(int alphagenreindex[],char *typegenre[]);
unsigned int get_genre (char *genre);
void print_header(mp3header *header,off_t filesize);
void translate_escapes (char *string);
void determine_tasks (char *format_string,int *want_id3,int *scantype, int *fullscan_vbr,int vbr_report);
void format_output (char *format_string,mp3info *mp3, int vbr_report);

#define VERSION "MP3Info 0.8.4"
#define COPYRIGHT "Copyright (C) 2001 Cedric Tefft and Ricardo Cerqueira"
#define MAXGENRE   147
#define GENREROWS  50


#ifdef __MAIN
char *typegenre [MAXGENRE+2] = {
   "Blues","Classic Rock","Country","Dance","Disco","Funk","Grunge",
   "Hip-Hop","Jazz","Metal","New Age","Oldies","Other","Pop","R&B",
   "Rap","Reggae","Rock","Techno","Industrial","Alternative","Ska",
   "Death Metal","Pranks","Soundtrack","Euro-Techno","Ambient",
   "Trip-Hop","Vocal","Jazz+Funk","Fusion","Trance","Classical",
   "Instrumental","Acid","House","Game","Sound Clip","Gospel","Noise",
   "Alt. Rock","Bass","Soul","Punk","Space","Meditative",
   "Instrumental Pop","Instrumental Rock","Ethnic","Gothic",
   "Darkwave","Techno-Industrial","Electronic","Pop-Folk","Eurodance",
   "Dream","Southern Rock","Comedy","Cult","Gangsta Rap","Top 40",
   "Christian Rap","Pop/Funk","Jungle","Native American","Cabaret",
   "New Wave","Psychedelic","Rave","Showtunes","Trailer","Lo-Fi",
   "Tribal","Acid Punk","Acid Jazz","Polka","Retro","Musical",
   "Rock & Roll","Hard Rock","Folk","Folk/Rock","National Folk",
   "Swing","Fast-Fusion","Bebob","Latin","Revival","Celtic",
   "Bluegrass","Avantgarde","Gothic Rock","Progressive Rock",
   "Psychedelic Rock","Symphonic Rock","Slow Rock","Big Band",
   "Chorus","Easy Listening","Acoustic","Humour","Speech","Chanson",
   "Opera","Chamber Music","Sonata","Symphony","Booty Bass","Primus",
   "Porn Groove","Satire","Slow Jam","Club","Tango","Samba",
   "Folklore","Ballad","Power Ballad","Rhythmic Soul","Freestyle",
   "Duet","Punk Rock","Drum Solo","A Cappella","Euro-House",
   "Dance Hall","Goa","Drum & Bass","Club-House","Hardcore","Terror",
   "Indie","BritPop","Negerpunk","Polsk Punk","Beat",
   "Christian Gangsta Rap","Heavy Metal","Black Metal","Crossover",
   "Contemporary Christian","Christian Rock","Merengue","Salsa",
   "Thrash Metal","Anime","JPop","Synthpop",""
};

int galphagenreindex[MAXGENRE+2] = {
   148,123,74,73,34,99,40,20,26,145,90,
   116,41,135,85,96,138,89,0,107,132,65,88,
   104,102,97,136,61,141,1,32,128,112,57,140,
   2,139,58,125,3,50,22,4,55,127,122,120,
   98,52,48,124,25,54,84,81,115,80,119,5,
   30,36,59,126,38,91,49,6,79,129,137,7,
   35,100,131,19,46,47,33,146,29,8,63,86,
   71,45,142,9,77,82,64,133,10,66,39,11,
   103,12,75,134,53,62,13,109,117,23,108,92,
   93,67,121,43,14,15,68,16,76,87,118,78,
   17,143,114,110,69,21,111,95,105,42,37,24,
   56,44,101,83,94,106,147,113,51,18,130,144,
   60,70,31,72,27,28
};


int *alphagenreindex=&(galphagenreindex[1]);          

#else
	extern char *typegenre [MAXGENRE+2];
	extern int alphagenreindex [MAXGENRE+1];
	extern int galphagenreindex [MAXGENRE+2];
#endif

void tagedit_curs(char *filename, int filenum, int fileoutof, id3tag *tag);


