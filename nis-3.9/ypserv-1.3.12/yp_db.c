/*
** yp_db.c - database functions for the maps
**
** This file is part of the NYS YP Server.
**
** The NYS YP Server is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of the
** License, or (at your option) any later version.
**
** The NYS YP Server is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public
** License along with the NYS YP Server; see the file COPYING.  If
** not, write to the Free Software Foundation, Inc., 675 Mass Ave,
** Cambridge, MA 02139, USA.
**
** Author: Thorsten Kukuk <kukuk@suse.de>
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "ypserv.h"
#include "yp_msg.h"
#include "yp_db.h"
#include "yp.h"

#if defined(HAVE_LIBGDBM)
#include <gdbm.h>
#elif defined(HAVE_NDBM)
#include <ndbm.h>
#endif

#if defined(HAVE_LIBGDBM)

/* Open a GDBM database */
static GDBM_FILE
_db_open (const char *domain, const char *map)
{
  GDBM_FILE dbp;
  char buf[MAXPATHLEN + 2];

  if (debug_flag)
    yp_msg ("\tdb_open(\"%s\", \"%s\")\n", domain, map);

  if (map[0] == '.' || strchr (map, '/'))
    {
      if (debug_flag)
	yp_msg ("\t\t->Returning 0\n");
      return 0;
    }

  if (strlen (domain) + strlen (map) < MAXPATHLEN)
    {
      sprintf (buf, "%s/%s", domain, map);

      dbp = gdbm_open (buf, 0, GDBM_READER, 0, NULL);

      if (debug_flag && dbp == NULL)
	yp_msg ("gdbm_open: GDBM Error Code #%d\n", gdbm_errno);
      else if (debug_flag)
	yp_msg ("\t\t->Returning OK!\n");
    }
  else
    {
      dbp = NULL;
      yp_msg ("Path to long: %s/%s\n", domain, map);
    }

  return dbp;
}

static inline int
_db_close (GDBM_FILE file)
{
  gdbm_close (file);
  return 0;
}

#elif defined(HAVE_NDBM)

/*****************************************************
  The following stuff is for NDBM suport !
******************************************************/

/* Open a NDBM database */
static DB_FILE
_db_open (const char *domain, const char *map)
{
  DB_FILE dbp;
  char buf[MAXPATHLEN + 2];

  if (debug_flag)
    yp_msg ("\tdb_open(\"%s\", \"%s\")\n", domain, map);

  if (map[0] == '.' || strchr (map, '/'))
    {
      if (debug_flag)
	yp_msg ("\t\t->Returning 0\n");
      return 0;
    }

  if (strlen (domain) + strlen (map) < MAXPATHLEN)
    {
      sprintf (buf, "%s/%s", domain, map);

      dbp = dbm_open (buf, O_RDONLY, 0600);

      if (debug_flag && dbp == NULL)
	yp_msg ("dbm_open: NDBM Error Code #%d\n", errno);
      else if (debug_flag)
	yp_msg ("\t\t->Returning OK!\n");
    }
  else
    {
      dbp = NULL;
      yp_msg ("Path to long: %s/%s\n", domain, map);
    }

  return dbp;
}

static inline int
_db_close (DB_FILE file)
{
  dbm_close (file);
  return 0;
}

int
ypdb_exists (DB_FILE dbp, datum key)
{
  datum tmp = dbm_fetch (dbp, key);

  if (tmp.dptr != NULL)
    return 1;
  else
    return 0;
}

datum
ypdb_nextkey (DB_FILE file, datum key)
{
  datum tkey;

  tkey = dbm_firstkey (file);
  while ((key.dsize != tkey.dsize) ||
	   (strncmp (key.dptr, tkey.dptr, tkey.dsize) != 0))
    {
      tkey = dbm_nextkey (file);
      if (tkey.dptr == NULL)
	return tkey;
    }
  tkey = dbm_nextkey (file);

  return tkey;
}

#else

#error "No database found or selected!"

#endif

typedef struct _fopen
{
  char *domain;
  char *map;
  DB_FILE dbp;
  int flag;
}
Fopen, *FopenP;

#define F_OPEN_FLAG 1
#define F_MUST_CLOSE 2

/* MAX_FOPEN:
   big -> slow list searching, we go 3 times through the list.
   little -> have to close/open very often.
   We now uses 30, because searching 3 times in the list is faster
   then reopening the database.
*/
#define MAX_FOPEN 30

static int fast_open_init = -1;
static Fopen fast_open_files[MAX_FOPEN];

int
ypdb_close_all (void)
{
  int i;

  if (debug_flag)
    yp_msg ("ypdb_close_all() called\n");

  if (fast_open_init == -1)
    return 0;

  for (i = 0; i < MAX_FOPEN; i++)
    {
      if (fast_open_files[i].dbp != NULL)
	{
	  if (fast_open_files[i].flag & F_OPEN_FLAG)
	    fast_open_files[i].flag |= F_MUST_CLOSE;
	  else
	    {
	      free (fast_open_files[i].domain);
	      free (fast_open_files[i].map);
	      _db_close (fast_open_files[i].dbp);
	      fast_open_files[i].dbp = NULL;
	      fast_open_files[i].flag = 0;
	    }
	}
    }
  return 0;
}

int
ypdb_close (DB_FILE file)
{
  int i;

  if (debug_flag)
    yp_msg ("ypdb_close() called\n");

  if (fast_open_init != -1)
    {
      for (i = 0; i < MAX_FOPEN; i++)
	{
	  if (fast_open_files[i].dbp == file)
	    {
	      if (fast_open_files[i].flag & F_MUST_CLOSE)
		{
		  if (debug_flag)
		    yp_msg ("ypdb_MUST_close (%s/%s|%d)\n",
			    fast_open_files[i].domain,
			    fast_open_files[i].map, i);
		  free (fast_open_files[i].domain);
		  free (fast_open_files[i].map);
		  _db_close (fast_open_files[i].dbp);
		  fast_open_files[i].dbp = NULL;
		  fast_open_files[i].flag = 0;
		}
	      else
		{
		  fast_open_files[i].flag &= ~F_OPEN_FLAG;
                }
	      return 0;
	    }
	}
    }
  yp_msg ("ERROR: Could not close file!\n");
  return 1;
}

DB_FILE
ypdb_open (const char *domain, const char *map)
{
  int x;

  /* First call, initialize the fast_open_init struct */
  if (fast_open_init == -1)
    {
      fast_open_init = 0;
      for (x = 0; x < MAX_FOPEN; x++)
	{
	  fast_open_files[x].domain =
	    fast_open_files[x].map = NULL;
	  fast_open_files[x].dbp = (DB_FILE) NULL;
	  fast_open_files[x].flag = 0;
	}
    }

  /* Search if we have already open the domain/map file */
  for (x = 0; x < MAX_FOPEN; x++)
    {
      if (fast_open_files[x].dbp != NULL)
	{
	  if ((strcmp (domain, fast_open_files[x].domain) == 0) &&
	      (strcmp (map, fast_open_files[x].map) == 0))
	    {
	      /* The file is open and we know the file handle */
	      if (debug_flag)
		yp_msg ("Found: %s/%s (%d)\n", fast_open_files[x].domain,
			fast_open_files[x].map, x);

	      if (fast_open_files[x].flag & F_OPEN_FLAG)
                {
		  /* The file is already in use, don't open it twice.
		     I think this could never happen. */
		  yp_msg ("\t%s/%s already open.\n", domain, map);
		  return NULL;
                }
	      else
		{
		  /* Mark the file as open */
		  fast_open_files[x].flag |= F_OPEN_FLAG;
		  return fast_open_files[x].dbp;
		}
	    }
	}
    }

  /* Search for free entry. I we do not found one, close the LRU */
  for (x = 0; x < MAX_FOPEN; x++)
    {
#if 0
      /* Bad Idea. If one of them is NULL, we will get a seg.fault
	 I think it will only work with Linux libc 5.x */
      yp_msg ("Opening: %s/%s (%d) %x\n",
	      fast_open_files[x].domain,
	      fast_open_files[x].map,
	      x, fast_open_files[x].dbp);
#endif
      if (fast_open_files[x].dbp == NULL)
	{
	  /* Good, we have a free entry and don't need to close a map */
	  int j;
	  Fopen tmp;

	  if ((fast_open_files[x].dbp = _db_open (domain, map)) == NULL)
	    return NULL;
	  fast_open_files[x].domain = strdup (domain);
	  fast_open_files[x].map = strdup (map);
	  fast_open_files[x].flag |= F_OPEN_FLAG;

	  if (debug_flag)
	    yp_msg ("Opening: %s/%s (%d) %x\n", domain, map, x,
		    fast_open_files[x].dbp);

	  /* LRU: put this entry at the first position, move all the other
	     one back */
	  tmp = fast_open_files[x];
	  for (j = x; j >= 1; --j)
	    fast_open_files[j] = fast_open_files[j-1];

	  fast_open_files[0] = tmp;
	  return fast_open_files[0].dbp;
	}
    }

  /* The badest thing, which could happen: no free cache entrys.
     Search the last entry, which isn't in use. */
  for (x = (MAX_FOPEN - 1); x > 0; --x)
    if ((fast_open_files[x].flag & F_OPEN_FLAG) != F_OPEN_FLAG)
      {
        int j;
	Fopen tmp;

	if (debug_flag)
	  {
	    yp_msg ("Closing %s/%s (%d)\n",
		    fast_open_files[x].domain,
		    fast_open_files[x].map, x);
	    yp_msg ("Opening: %s/%s (%d)\n", domain, map, x);
	  }
	free (fast_open_files[x].domain);
	free (fast_open_files[x].map);
	_db_close (fast_open_files[x].dbp);

	fast_open_files[x].domain = strdup (domain);
	fast_open_files[x].map = strdup (map);
	fast_open_files[x].flag = F_OPEN_FLAG;
	fast_open_files[x].dbp = _db_open (domain, map);

	/* LRU: Move the new entry to the first positon */
	tmp = fast_open_files[x];
	for (j = x; j >= 1; --j)
	  fast_open_files[j] = fast_open_files[j-1];

	fast_open_files[j] = tmp;
	return fast_open_files[j].dbp;
      }

  yp_msg ("ERROR: Couldn't find a free cache entry!\n");

  for (x = 0; x < MAX_FOPEN; x++)
    {
      yp_msg ("Open files: %s/%s (%d) %x (%x)\n",
	      fast_open_files[x].domain,
	      fast_open_files[x].map,
	      x, fast_open_files[x].dbp,
              fast_open_files[x].flag);
    }
  return NULL;
}
