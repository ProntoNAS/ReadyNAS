/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* A set of functions to search databases in various formats. An open
database is represented by a void * value which is returned from a lookup-
specific "open" function. These are now all held in individual modules in the
lookups subdirectory and the functions here form a generic interface.

Caching is used to improve performance. Open files are cached until a tidyup
function is called, and for each file the result of the last lookup is cached.
However, if too many files are opened, some of those that are not in use have
to be closed. Those open items that use real files are kept on a LRU chain to
help with this.

All the data is held in malloc'd store so as to be independent of the stacking
store pools. */

#include "exim.h"


/* Tree in which to cache open files until tidyup called. */

static tree_node *search_tree = NULL;

/* Two-way chain of open databases that use real files. This is maintained in
recently-used order for the purposes of closing the least recently used when
too many files are open. */

static tree_node *open_top = NULL;
static tree_node *open_bot = NULL;

/* Count of open databases that use real files */

static int open_filecount = 0;



/*************************************************
*        Validate a lookup type name             *
*************************************************/

/* Only those names that are recognized and whose code is included in the
binary give an OK response.

Arguments:
  name       lookup type name
  errmsg     where to put an error message

Returns:     +ve => valid lookup name; value is offset in lookup_list
             -ve => invalid name; message in errmsg.
*/

int
search_findtype(char *name, char **errmsg)
{
int n = 0;
lookup_info *li;

for (li = lookup_list; li->name[0] != 0; n++,li++)
  {
  if (strcmp(name, li->name) == 0)
    {
    if (li->find != NULL) return n;
    *errmsg = string_sprintf("lookup type \"%s\" is not available "
    "(not in the binary - check buildtime LOOKUP configuration)", name);
    return -1;
    }
  }

*errmsg = string_sprintf("unknown lookup type \"%s\"", name);
return -1;
}



/*************************************************
*         Check a file's credentials             *
*************************************************/

/* fstat can normally be expected to work on an open file, but there are some
NFS states where it may not.

Arguments:
  fd         an open file descriptor or -1
  filename   a file name if fd is -1
  modemask   a mask specifying mode bits that must *not* be set
  owners     NULL or a list of of allowable uids, count in the first item
  owngroups  NULL or a list of allowable gids, count in the first item
  type       name of lookup type for putting in error message
  errmsg     where to point an error message

Returns:     -1 stat() or fstat() failed
              0 OK
             +1 something didn't match

Side effect: sets errno to ERRNO_BADUGID, ERRNO_NOTREGULAR or ERRNO_BADMODE for
             bad uid/gid, not a regular file, or bad mode; otherwise leaves it
             to what fstat set it to.
*/

int
search_check_file(int fd, char *filename, int modemask, uid_t *owners,
  gid_t *owngroups, char *type, char **errmsg)
{
int i;
struct stat statbuf;

if ((fd >= 0 && fstat(fd, &statbuf) != 0) ||
    (fd  < 0 && stat(filename, &statbuf) != 0))
  {
  int save_errno = errno;
  *errmsg = string_sprintf("%s: stat failed", filename);
  errno = save_errno;
  return -1;
  }

if ((statbuf.st_mode & S_IFMT) != S_IFREG)
  {
  *errmsg = string_sprintf("%s is not a regular file (%s lookup)",
    filename, type);
  errno = ERRNO_NOTREGULAR;
  return +1;
  }

if ((statbuf.st_mode & modemask) != 0)
  {
  *errmsg = string_sprintf("%s (%s lookup): file mode %.4o should not contain "
    "%.4o", filename, type,  statbuf.st_mode & 07777,
    statbuf.st_mode & modemask);
  errno = ERRNO_BADMODE;
  return +1;
  }

if (owners != NULL)
  {
  BOOL uid_ok = FALSE;
  for (i = 1; i <= (int)owners[0]; i++)
    if (owners[i] == statbuf.st_uid) { uid_ok = TRUE; break; }
  if (!uid_ok)
    {
    *errmsg = string_sprintf("%s (%s lookup): file has wrong owner", filename,
      type);
    errno = ERRNO_BADUGID;
    return +1;
    }
  }

if (owngroups != NULL)
  {
  BOOL gid_ok = FALSE;
  for (i = 1; i <= (int)owngroups[0]; i++)
    if (owngroups[i] == statbuf.st_gid) { gid_ok = TRUE; break; }
  if (!gid_ok)
    {
    *errmsg = string_sprintf("%s (%s lookup): file has wrong group", filename,
      type);
    errno = ERRNO_BADUGID;
    return +1;
    }
  }

return 0;
}



/*************************************************
*               Release cached resources         *
*************************************************/

/* When search_open is called it caches the "file" that it opens in
search_tree. The name of the tree node is a concatenation of the search type
with the file name. For query-style lookups, the file name is empty. Real files
are normally closed only when this tidyup routine is called, typically at the
end of sections of code where a number of lookups might occur. However, if too
many files are open simultaneously, some get closed beforehand. They can't be
removed from the tree. There is also a general tidyup function which is called
for the lookup driver, if it exists.

First, there is an internal, recursive subroutine.

Argument:   a pointer to a search_openfile tree node
Returns:    nothing
*/

static void
tidyup_subtree(tree_node *t)
{
int search_type = t->name[0] - '0';
search_cache *c = (search_cache *)(t->data.ptr);

if (t->left != NULL) tidyup_subtree(t->left);
if (t->right != NULL) tidyup_subtree(t->right);

if (c->handle != NULL && lookup_list[search_type].close != NULL)
  lookup_list[search_type].close(c->handle);

if (c->key != NULL) store_free(c->key);
if (c->data != NULL) store_free(c->data);
store_free(c);
store_free(t);
}


/* The external entry point

Argument: none
Returns:  nothing
*/

void
search_tidyup(void)
{
lookup_info *li;

DEBUG(9) debug_printf("search_tidyup called\n");

/* Close individually each cached open file. */

if (search_tree != NULL)
  {
  tidyup_subtree(search_tree);
  search_tree = NULL;
  }
open_top = open_bot = NULL;
open_filecount = 0;

/* Call the general tidyup entry for any drivers that have one. */

for (li = lookup_list; li->name[0] != 0; li++)
  if (li->tidy != NULL) (li->tidy)();
}




/*************************************************
*             Open search database               *
*************************************************/

/* A mode, and lists of owners and groups, are passed over for checking in
the cases where the database is one or more files. Return NULL, with a message
pointed to by message, in cases of error.

For search types that use a file or files, check up on the mode after
opening. It is tempting to do a stat before opening the file, and use it as
an existence check. However, doing that opens a small security loophole in
that the status could be changed before the file is opened. Can't quite see
what problems this might lead to, but you can't be too careful where security
is concerned. Fstat() on an open file can normally be expected to succeed,
but there are some NFS states where it does not.

There are two styles of query: (1) in the "single-key+file" style, a single
key string and a file name are given, for example, for linear searches, DBM
files, or for NIS. (2) In the "query" style, no "filename" is given; instead
just a single query string is passed. This applies to multiple-key lookup
types such as NIS+.

Before opening, scan the tree of cached files to see if this file is already
open for the correct search type. If so, return the saved handle. If not, put
the handle in the tree for possible subsequent use. See search_tidyup above for
closing all the cached files.

A count of open databases which use real files is maintained, and if this
gets too large, we have to close a cached file. Its entry remains in the tree,
but is marked closed.

Arguments:
  filename       the name of the file for single-key+file style lookups,
                 NULL for query-style lookups
  search_type    the type of search required
  modemask       if a real single file is used, this specifies mode bits that
                 must not be set; otherwise it is ignored
  owners         if a real single file is used, this specifies the possible
                 owners of the file; otherwise it is ignored
  owngroups      if a real single file is used, this specifies the possible
                 group owners of the file; otherwise it is ignored
  message        points to a char * which can be set to point to an error
                 message when something goes wrong

Returns:         an identifying handle for the open database;
                 this is the pointer to the tree block in the
                 cache of open files; return NULL on open failure
*/

void *
search_open(char *filename, int search_type, int modemask, uid_t *owners,
  gid_t *owngroups, char **message)
{
void *handle;
tree_node *t;
search_cache *c;
lookup_info *lk = lookup_list + search_type;
char keybuffer[256];

DEBUG(9) debug_printf("search_open: %s \"%s\"\n", lk->name,
  (filename == NULL)? "NULL" : filename);

/* See if we already have this open for this type of search, and if so,
pass back the tree block as the handle. The key for the tree node is the search
type plus '0' concatenated with the file name. There may be entries in the tree
with closed files if a lot of files have been opened. */

sprintf(keybuffer, "%c%.254s", search_type + '0',
  (filename == NULL)? "" : filename);

if ((t = tree_search(search_tree, keybuffer)) != NULL)
  {
  c = (search_cache *)(t->data.ptr);
  if (c->handle != NULL)
    {
    DEBUG(9) debug_printf("  cached open\n");
    return t;
    }
  DEBUG(9) debug_printf("  cached closed\n");
  }

/* Otherwise, we need to open the file or database - each search type has its
own code, which is now split off into separately compiled modules. Before doing
this, if the search type is one that uses real files, check on the number that
we are holding open in the cache. If the limit is reached, close the least
recently used one. */

if (lk->type == lookup_absfile && open_filecount >= lookup_open_max)
  {
  if (open_bot == NULL)
    log_write(0, LOG_MAIN|LOG_PANIC, "too many lookups open, but can't find "
      "one to close");
  else
    {
    search_cache *c = (search_cache *)(open_bot->data.ptr);
    DEBUG(9) debug_printf("Too many lookup files open\n  closing %s\n",
      open_bot->name);
    open_bot = c->up;
    if (open_bot != NULL)
      ((search_cache *)(open_bot->data.ptr))->down = NULL;
    else
      open_top = NULL;
    ((lookup_list + c->search_type)->close)(c->handle);
    c->handle = NULL;
    open_filecount--;
    }
  }

/* If opening is successful, call the file-checking function if there is one,
and if all is still well, enter the open database into the tree. */

handle = lk->open(filename, message);
if (handle == NULL) return handle;

if (lk->check != NULL &&
   !lk->check(handle, filename, modemask, owners, owngroups, message))
  {
  lk->close(handle);
  return NULL;
  }

/* If this is a search type that uses real files, keep count. */

if (lk->type == lookup_absfile) open_filecount++;

/* If we found a previously opened entry in the tree, re-use it; otherwise
insert a new entry. On re-use, leave any cached lookup data and the lookup
count alone. */

if (t == NULL)
  {
  t = store_malloc(sizeof(tree_node) + (int)strlen(keybuffer));
  t->data.ptr = c = store_malloc(sizeof(search_cache));
  c->key = NULL;
  c->data = NULL;
  strcpy(t->name, keybuffer);
  tree_insertnode(&search_tree, t);
  }
else c = t->data.ptr;

c->handle = handle;
c->search_type = search_type;
c->up = c->down = NULL;

return t;
}





/*************************************************
*  Internal function: Find one item in database  *
*************************************************/

/* The answer is always put into dynamic store. If the key contains a colon,
then it is treated as a double key: the first part is the key for the record in
the file, and the remainder is a subkey that is used to extract a subfield from
the main data. Subfields are specified as subkey=value in the records. The last
lookup for each handle is cached.

Arguments:
  handle       the handle from search_open; points to tree node
  filename     the filename that was handed to search_open, or
               NULL for query-style searches
  keystring    the keystring for single-key+file lookups, or
               the querystring for query-style lookups
  errmsg       somewhere to point to an error message

Returns:       a pointer to a dynamic string containing the answer,
               or NULL if the query failed or was deferred; in the
               latter case, search_find_defer is set TRUE
*/

static char *
internal_search_find(void *handle, char *filename, char *keystring,
  char **errmsg)
{
tree_node *t = (tree_node *)handle;
search_cache *c = (search_cache *)(t->data.ptr);
int search_type = t->name[0] - '0';

char *subkey = NULL;
char *colon = NULL;

*errmsg = "";              /* indicates no error, or ordinary failed to find */
search_find_defer = FALSE;

/* If there's a colon in a key for a single-key+file lookup, temporarily
terminate the main key, and set up the subkey. */

if (filename != NULL)
  {
  colon = strchr(keystring, ':');
  if (colon != NULL)
    {
    subkey = colon + 1;
    *colon = 0;
    }
  }

DEBUG(9) debug_printf("internal_search_find: file=\"%s\"\n  "
  "type=%s key=\"%s\"\n", (filename == NULL)? "NULL" : filename,
  lookup_list[search_type].name, keystring);

/* Insurance. If the keystring is empty, just fail. */

if (keystring[0] == 0) return NULL;

/* Look up the data for the key, unless it is already in the cache for this
file. */

if (c->key == NULL || strcmp(c->key, keystring) != 0)
  {
  char *data = NULL;
  int keylength = (int)strlen(keystring);

  DEBUG(9)
    {
    if (filename != NULL)
      debug_printf("file lookup required for %s%s%s\n  in %s\n",
        keystring,
        (subkey == NULL)? "" : ":",
        (subkey == NULL)? "" : subkey,
        filename);
    else
      debug_printf("database lookup required for %s\n", keystring);
    }

  /* Free any previous cache */

  if (c->key != NULL) store_free(c->key);
  if (c->data != NULL) store_free(c->data);
  c->key = c->data = NULL;

  /* Call the code for the different kinds of search. DEFER is handled
  like FAIL, except that search_find_defer is set so the caller can
  distinguish if necessary. */

  if (lookup_list[search_type].find(c->handle, filename, keystring, keylength,
      &data, errmsg) == DEFER)
    {
    search_find_defer = TRUE;
    }

  /* A record that has been found is now in data, which is either NULL
  or points to a bit of dynamic store. Cache the result of the lookup. */

  else
    {
    int len = keylength + 1;
    c->key = store_malloc(len);
    memcpy(c->key, keystring, len);
    if (data != NULL)
      {
      len = (int)strlen(data) + 1;
      c->data = store_malloc(len);
      memcpy(c->data, data, len);
      }
    }
  }

else DEBUG(9) debug_printf("cached data used for lookup of %s%s%s%s%s\n",
  keystring,
  (subkey == NULL)? "" : ":",
  (subkey == NULL)? "" : subkey,
  (filename == NULL)? "" : "\n  in ",
  (filename == NULL)? "" : filename);

/* Debug: output the main record */

DEBUG(9)
  {
  if (c->data == NULL)
    {
    if (search_find_defer) debug_printf("lookup deferred\n");
      else debug_printf("lookup failed\n");
    }
  else debug_printf("lookup yielded: %s\n", c->data);
  }

/* Put back the colon if it was overwritten */

if (colon != NULL) *colon = ':';

/* If we have found data, pick out the subfield if required. Otherwise
return the whole cached string. */

return (c->data == NULL)? NULL :
  (subkey != NULL)?
    expand_getkeyed(subkey, c->data) : string_copy(c->data);
}




/*************************************************
* Find one item in database, possibly wildcarded *
*************************************************/

/* This function calls the internal function above; once only if there
is no partial matching, but repeatedly when partial matching is requested.

Arguments:
  handle       the handle from search_open
  filename     the filename that was handed to search_open, or
               NULL for query-style searches
  keystring    the keystring for single-key+file lookups, or
               the querystring for query-style lookups
  partial      if the value is greater than 2000, subtract 2048 and
               set the "try * to the left of @" flag; then
               if the value is greater than 1000, subtract 1024 to
               get the real value, and set the "try '*' if all else
               fails" flag; after this,
               -1 means no partial matching;
               otherwise it's the minimum number of components;
  expand_setup pointer to offset for setting up expansion strings;
               don't do any if < 0
  errmsg       somewhere to point to an error message

Returns:       a pointer to a dynamic string containing the answer,
               or NULL if the query failed or was deferred; in the
               latter case, search_find_defer is set TRUE
*/

char *
search_find(void *handle, char *filename, char *keystring, int partial,
  int *expand_setup, char **errmsg)
{
tree_node *t = (tree_node *)handle;
BOOL set_null_wild = FALSE;
BOOL do_star = FALSE;
BOOL do_at = FALSE;
char *yield;

DEBUG(9) debug_printf("search_find: file=\"%s\"\n  key=\"%s\" partial=%d\n",
  (filename == NULL)? "NULL" : filename, keystring, partial);

/* Arrange to put this database at the top of the LRU chain if it is a type
that opens real files. */

if (open_top != (tree_node *)handle &&
    lookup_list[t->name[0]-'0'].type == lookup_absfile)
  {
  search_cache *c = (search_cache *)(t->data.ptr);
  tree_node *up = c->up;
  tree_node *down = c->down;

  /* Cut it out of the list. A newly opened file will a NULL up pointer.
  Otherwise there will be a non-NULL up pointer, since we checked above that
  this block isn't already at the top of the list. */

  if (up != NULL)
    {
    ((search_cache *)(up->data.ptr))->down = down;
    if (down != NULL)
      ((search_cache *)(down->data.ptr))->up = up;
    else open_bot = up;
    }

  /* Now put it at the head of the list. */

  c->up = NULL;
  c->down = open_top;
  if (open_top == NULL) open_bot = t; else
    ((search_cache *)(open_top->data.ptr))->up = t;
  open_top = t;
  }

DEBUG(9)
  {
  tree_node *t = open_top;
  debug_printf("LRU list:\n");
  while (t != NULL)
    {
    search_cache *c = (search_cache *)(t->data.ptr);
    debug_printf("  %s\n", t->name);
    if (t == open_bot) debug_printf("  End\n");
    t = c->down;
    }
  }


/* Determine whether independent "*" or "*@" matching is wanted and adjust the
partial count. If there is no filename, partial matching can never happen. */

if (filename != NULL)
  {
  if (partial > 2000)
    {
    do_at = TRUE;
    partial -= 2048;
    }
  if (partial > 1000)
    {
    do_star = TRUE;
    partial -= 1024;
    }
  }
else partial = -1;

/* First of all, try to match the key string verbatim. If matched a complete
entry but could have been partial, flag to set up variables. */

yield = internal_search_find(handle, filename, keystring, errmsg);
if (search_find_defer) return NULL;
if (yield != NULL) { if (partial >= 0) set_null_wild = TRUE; }

/* Not matched a complete entry; handle partial lookups, but only if the full
search didn't defer. Don't use string_sprintf() to construct the initial key,
just in case the original key is too long for the string_sprintf() buffer (it
*has* happened!). */

else if (partial >= 0)
  {
  int len = (int)strlen(keystring);
  char *keystring2 = store_get(len + 3);

  strcpy(keystring2 + 2, keystring);
  keystring2[0] = '*';
  keystring2[1] = '.';

  DEBUG(9) debug_printf("trying partial match %s\n", keystring2);
  yield = internal_search_find(handle, filename, keystring2, errmsg);
  if (search_find_defer) return NULL;

  /* The key in its entirety did not match a wild entry; try chopping off
  leading components. */

  if (yield == NULL)
    {
    int dotcount = 0;
    char *keystring3 = keystring2 + 2;
    char *s = keystring3;
    while (*s != 0) if (*s++ == '.') dotcount++;

    while (dotcount-- >= partial)
      {
      while (keystring3[1] != '.' && keystring3[1] != 0) keystring3++;
      *keystring3 = '*';
      DEBUG(9) debug_printf("trying partial match %s\n", keystring3);
      yield = internal_search_find(handle, filename, keystring3, errmsg);
      if (search_find_defer) return NULL;
      if (yield != NULL)
        {
        /* First variable is the wild part; second is the fixed part. Take care
        to get it right when keystring3 is just "*". */

        if (expand_setup != NULL && *expand_setup >= 0)
          {
          int fixedlength = (int)strlen(keystring3) - 2;
          int wildlength = (int)strlen(keystring) - fixedlength - 1;
          *expand_setup += 1;
          expand_nstring[*expand_setup] = keystring;
          expand_nlength[*expand_setup] = wildlength;
          *expand_setup += 1;
          expand_nstring[*expand_setup] = keystring + wildlength + 1;
          expand_nlength[*expand_setup] = (fixedlength < 0)? 0 : fixedlength;
          }
        break;
        }
      keystring3++;    /* Must work for null components */
      }
    }

  else set_null_wild = TRUE; /* Matched a wild entry without any wild part */
  }

/* If nothing has been matched, but the option to look for "*@" is set, try
replacing everthing to the left of @ by *. After a match, the wild part
is set to the string to the left of the @. */

if (yield == NULL && do_at)
  {
  char *atat = strrchr(keystring, '@');
  if (atat != NULL && atat > keystring)
    {
    int savechar;
    savechar = *(--atat);
    *atat = '*';

    DEBUG(9) debug_printf("trying default match %s\n", atat);
    yield = internal_search_find(handle, filename, atat, errmsg);
    *atat = savechar;
    if (search_find_defer) return NULL;

    if (yield != NULL && expand_setup != NULL && *expand_setup >= 0)
      {
      *expand_setup += 1;
      expand_nstring[*expand_setup] = keystring;
      expand_nlength[*expand_setup] = atat - keystring + 1;
      *expand_setup += 1;
      expand_nstring[*expand_setup] = keystring;
      expand_nlength[*expand_setup] = 0;
      }
    }
  }

/* If we still haven't matched anything, and the option to look for "*" is
set, try that, but not if partial == 0, because in that case it will already
have been searched for. If we do match, the first variable (the wild part)
is the whole key, and the second is empty. */

if (yield == NULL && do_star && partial != 0)
  {
  DEBUG(9) debug_printf("trying to match *\n");
  yield = internal_search_find(handle, filename, "*", errmsg);
  if (yield != NULL && expand_setup != NULL && *expand_setup >= 0)
    {
    *expand_setup += 1;
    expand_nstring[*expand_setup] = keystring;
    expand_nlength[*expand_setup] = (int)strlen(keystring);
    *expand_setup += 1;
    expand_nstring[*expand_setup] = keystring;
    expand_nlength[*expand_setup] = 0;
    }
  }

/* If this was a potentially partial lookup, and we matched either a
complete non-wild domain entry, or we matched a wild-carded entry without
chopping off any of the domain components, set up the expansion variables
(if required) so that the first one is empty, and the second one is the
fixed part of the domain. The set_null_wild flag is set only when yield is not
NULL. */

if (set_null_wild && expand_setup != NULL && *expand_setup >= 0)
  {
  *expand_setup += 1;
  expand_nstring[*expand_setup] = keystring;
  expand_nlength[*expand_setup] = 0;
  *expand_setup += 1;
  expand_nstring[*expand_setup] = keystring;
  expand_nlength[*expand_setup] = (int)strlen(keystring);
  }

return yield;
}

/* End of search.c */
