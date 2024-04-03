/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/*
 * $Id: cdb.c,v 1.4.2.2 1998/05/29 16:33:12 cvs Exp $
 *
 * Exim - CDB database lookup module
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyright (c) 1998 Nigel Metheringham, Planet Online Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 *
 * This code implements Dan Bernstein's Constant DataBase (cdb) spec.
 * Information, the spec and sample code for cdb can be obtained from
 *      http://www.pobox.com/~djb/cdb.html
 *
 * This implementation borrows some code from Dan Bernstein's
 * implementation (which has no license restrictions applied to it).
 * This (read-only) implementation is completely contained within
 * cdb.[ch] it does *not* link against an external cdb library.
 *
 *
 * There are 2 varients included within this code.  One uses MMAP and
 * should give better performance especially for multiple lookups on a
 * modern machine.  The other is the default implementation which is
 * used in the case where the MMAP fails or if MMAP was not compiled
 * in.  this implementation is the same as the original reference cdb
 * implementation.  The MMAP version is compiled in if the HAVE_MMAP
 * preprocessor define is defined - this should be set in the system
 * specific os.h file.
 *
 */


#include "../exim.h"
#include "cdb.h"

#ifdef HAVE_MMAP
#  include <sys/mman.h>
/* Not all implementations declare MAP_FAILED */
#  ifndef MAP_FAILED
#    define MAP_FAILED ((void *) -1)
#  endif /* MAP_FAILED */
#endif /* HAVE_MMAP */


#define CDB_HASH_SPLIT 256     /* num pieces the hash table is split into */
#define CDB_HASH_MASK  255     /* mask to and off split value */
#define CDB_HASH_ENTRY 8       /* how big each offset it */
#define CDB_HASH_TABLE (CDB_HASH_SPLIT * CDB_HASH_ENTRY)

/* State information for cdb databases that are open NB while the db
 * is open its contents will not change (cdb dbs are normally updated
 * atomically by renaming).  However the lifetime of one of these
 * state structures should be limited - ie a long running daemon
 * that opens one may hit problems....
 */

struct cdb_state {
  int  fileno;
  off_t filelen;
  char *cdb_map;
  char *cdb_offsets;
};

/* 32 bit unsigned type - this is an int on all modern machines */
typedef unsigned int uint32;



/*
 * cdb_hash()
 * Internal function to make hash value */

static uint32
cdb_hash(uschar *buf, unsigned int len)
{
  uint32 h;

  h = 5381;
  while (len) {
    --len;
    h += (h << 5);
    h ^= (uint32) *buf++;
  }
  return h;
}



/*
 * cdb_bread()
 * Internal function to read len bytes from disk, coping with oddities */

static int
cdb_bread(int fd,
	 char *buf,
	 int len)
{
  int r;
  while (len > 0) {
    do
      r = read(fd,buf,len);
    while ((r == -1) && (errno == EINTR));
    if (r == -1) return -1;
    if (r == 0) { errno = EIO; return -1; }
    buf += r;
    len -= r;
  }
  return 0;
}



/*
 * cdb_bread()
 * Internal function to parse 4 byte number (endian independant) */

/* Argument changed from unsigned char * to char * by PH, in order to stop
complaints from Sun's compiler since all calls to this function pass over
char * arguments. Do the casting to unsigned inside the function. */

static uint32
cdb_unpack(char *buf)
{
  uint32 num;
  uschar *ubuf = (uschar *)buf;
  num = ubuf[3]; num <<= 8;
  num += ubuf[2]; num <<= 8;
  num += ubuf[1]; num <<= 8;
  num += ubuf[0];
  return num;
}



void *
cdb_open(char *filename,
	char **errmsg)
{
  int fileno;
  struct cdb_state *cdbp;
  struct stat statbuf;
  void * mapbuf;

  fileno = open(filename, O_RDONLY);
  if (fileno == -1) {
    int save_errno = errno;
    *errmsg = string_open_failed(errno, "%s for cdb lookup", filename);
    errno = save_errno;
    return NULL;
  }

  if (fstat(fileno, &statbuf) == 0) {
    /* If this is a valid file, then it *must* be at least
     * CDB_HASH_TABLE bytes long */
    if (statbuf.st_size < CDB_HASH_TABLE) {
      int save_errno = errno;
      *errmsg = string_open_failed(errno,
				  "%s to short for cdb lookup",
				  filename);
      errno = save_errno;
      return NULL;
    }
  } else {
    int save_errno = errno;
    *errmsg = string_open_failed(errno,
				"fstat(%s) failed - cannot do cdb lookup",
				filename);
    errno = save_errno;
    return NULL;
  }

  /* Having got a file open we need the structure to put things in */
  cdbp = store_malloc(sizeof(struct cdb_state));
  /* store_malloc() does not return if memory was not available... */
  /* preload the structure.... */
  cdbp->fileno = fileno;
  cdbp->filelen = statbuf.st_size;
  cdbp->cdb_map = NULL;
  cdbp->cdb_offsets = NULL;

  /* if we are allowed to we use mmap here.... */
#ifdef HAVE_MMAP
  mapbuf = mmap(NULL,
	       statbuf.st_size,
	       PROT_READ,
	       MAP_SHARED,
	       fileno,
	       0);
  if (mapbuf != MAP_FAILED) {
    /* We have an mmap-ed section.  Now we can just use it */
    cdbp->cdb_map = mapbuf;
    /* The offsets can be set to the same value since they should
     * effectively be cached as well
     */
    cdbp->cdb_offsets = mapbuf;

    /* Now return the state struct */
    return(cdbp);
  } else {
    /* If we got here the map failed.  Basically we can ignore
     * this since we fall back to slower methods....
     * However lets debug log it...
     */
    DEBUG(1) debug_printf("cdb mmap failed - %d\n", errno);
  }
#endif /* HAVE_MMAP */

  /* In this case we have either not got MMAP allowed, or it failed */

  /* get a buffer to stash the basic offsets in - this should speed
   * things up a lot - especially on multiple lookups */
  cdbp->cdb_offsets = store_malloc(CDB_HASH_TABLE);

  /* now fill the buffer up... */
  if (cdb_bread(fileno, cdbp->cdb_offsets, CDB_HASH_TABLE) == -1) {
    /* read of hash table failed, oh dear, oh.....
     * time to give up I think....
     * call the close routine (deallocs the memory), and return NULL */
    *errmsg = string_open_failed(errno,
				"cannot read header from %s for cdb lookup",
				filename);
    cdb_close(cdbp);
    return NULL;
  }

  /* Everything else done - return the cache structure */
  return cdbp;
}



/*************************************************
*             Check entry point                  *
*************************************************/

BOOL
cdb_check(void *handle,
	 char *filename,
	 int modemask,
	 uid_t *owners,
	 gid_t *owngroups,
	 char **errmsg)
{
  struct cdb_state * cdbp = handle;
  return search_check_file(cdbp->fileno,
			  filename,
			  modemask,
			  owners,
			  owngroups,
			  "cdb",
			  errmsg) == 0;
}



/*************************************************
*              Find entry point                  *
*************************************************/

int
cdb_find(void *handle,
	char *filename,
	char *keystring,
	int  key_len,
	char **result,
	char **errmsg)
{
  struct cdb_state * cdbp = handle;
  uint32 item_key_len,
    item_dat_len,
    key_hash,
    item_hash,
    item_posn,
    cur_offset,
    end_offset,
    hash_offset_entry,
    hash_offset,
    hash_offlen,
    hash_slotnm;
  int loop;

  key_hash = cdb_hash((uschar *)keystring, key_len);

  hash_offset_entry = CDB_HASH_ENTRY * (key_hash & CDB_HASH_MASK);
  hash_offset = cdb_unpack(cdbp->cdb_offsets + hash_offset_entry);
  hash_offlen = cdb_unpack(cdbp->cdb_offsets + hash_offset_entry + 4);

  /* If the offset length is zero this key cannot be in the file */
  if (hash_offlen == 0) {
    return FAIL;
  }
  hash_slotnm = (key_hash >> 8) % hash_offlen;

  /* check to ensure that the file is not corrupt
   * if the hash_offset + (hash_offlen * CDB_HASH_ENTRY) is longer
   * than the file, then we have problems.... */
  if ((hash_offset + (hash_offlen * CDB_HASH_ENTRY)) > cdbp->filelen) {
    *errmsg = string_sprintf("cdb: corrupt cdb file %s (too short)",
			    filename);
    DEBUG(3) debug_printf("%s\n", *errmsg);
    return DEFER;
  }

  cur_offset = hash_offset + (hash_slotnm * CDB_HASH_ENTRY);
  end_offset = hash_offset + (hash_offlen * CDB_HASH_ENTRY);
  /* if we are allowed to we use mmap here.... */
#ifdef HAVE_MMAP
  /* make sure the mmap was OK */
  if (cdbp->cdb_map != NULL) {
    char * cur_pos = cur_offset + cdbp->cdb_map;
    char * end_pos = end_offset + cdbp->cdb_map;
    for (loop = 0; (loop < hash_offlen); ++loop) {
      item_hash = cdb_unpack(cur_pos);
      cur_pos += 4;
      item_posn = cdb_unpack(cur_pos);
      cur_pos += 4;
      /* if the position is zero then we have a definite miss */
      if (item_posn == 0)
       return FAIL;

      if (item_hash == key_hash) {
       /* matching hash value */
       char * item_ptr = cdbp->cdb_map + item_posn;
       item_key_len = cdb_unpack(item_ptr);
       item_ptr += 4;
       item_dat_len = cdb_unpack(item_ptr);
       item_ptr += 4;
       /* check key length matches */
       if (item_key_len == key_len) {
	 /* finally check if key matches */
	 if (strncmp(keystring, item_ptr, key_len) == 0) {
	   /* we have a match....
	    * make item_ptr point to data */
	   item_ptr += item_key_len;
	   /* ... and the returned result */
	   *result = store_malloc(item_dat_len + 1);
	   memcpy(*result, item_ptr, item_dat_len);
	   (*result)[item_dat_len] = 0;
	   return OK;
	 }
       }
      }
      /* handle warp round of table */
      if (cur_pos == end_pos)
       cur_pos = cdbp->cdb_map + hash_offset;
    }
    /* looks like we failed... */
    return FAIL;
  }
#endif /* HAVE_MMAP */
  for (loop = 0; (loop < hash_offlen); ++loop) {
    char packbuf[8];
    if (lseek(cdbp->fileno, (off_t) cur_offset,SEEK_SET) == -1) return DEFER;
    if (cdb_bread(cdbp->fileno, packbuf,8) == -1) return DEFER;
    item_hash = cdb_unpack(packbuf);
    item_posn = cdb_unpack(packbuf + 4);
    /* if the position is zero then we have a definite miss */
    if (item_posn == 0)
      return FAIL;

    if (item_hash == key_hash) {
      /* matching hash value */
      if (lseek(cdbp->fileno, (off_t) item_posn, SEEK_SET) == -1) return DEFER;
      if (cdb_bread(cdbp->fileno, packbuf, 8) == -1) return DEFER;
      item_key_len = cdb_unpack(packbuf);
      /* check key length matches */
      if (item_key_len == key_len) {
       /* finally check if key matches */
       char * item_key = store_malloc(key_len);
       if (cdb_bread(cdbp->fileno, item_key, key_len) == -1) return DEFER;
       if (strncmp(keystring, item_key, key_len) == 0) {
	 /* matches - get data length */
	 item_dat_len = cdb_unpack(packbuf + 4);
	 /* then we build a new result string */
	 *result = store_malloc(item_dat_len + 1);
	 if (cdb_bread(cdbp->fileno, *result, item_dat_len) == -1)
	   return DEFER;
	 (*result)[item_dat_len] = 0;
	 return OK;
       }
      }
    }
    cur_offset += 8;

    /* handle warp round of table */
    if (cur_offset == end_offset)
      cur_offset = hash_offset;
  }
  return FAIL;
}



/*************************************************
*              Close entry point                 *
*************************************************/

/* See local README for interface description */

void
cdb_close(void *handle)
{
struct cdb_state * cdbp = handle;

#ifdef HAVE_MMAP
 if (cdbp->cdb_map) {
   munmap(cdbp->cdb_map, cdbp->filelen);
   if (cdbp->cdb_map == cdbp->cdb_offsets)
     cdbp->cdb_offsets = NULL;
 }
#endif /* HAVE_MMAP */
 if (cdbp->cdb_offsets)
   store_free(cdbp->cdb_offsets);

 close(cdbp->fileno);
 store_free(cdbp);
}

/* End of lookups/cdb.c */
