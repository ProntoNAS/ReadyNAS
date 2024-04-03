/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Exim gets and frees all its store through these functions. In the original
implementation there was a lot of mallocing and freeing of small bits of
store. The philosophy has now changed to a scheme which includes two "stacking
pools" of store. For the short-lived processes, there isn't any real need to do
any garbage collection, but the stack concept allows quick resetting, which
means some activities such as filtering can easily throw away the store they
used - this reduces the total, which might get large if there were many
adddresses.

Obviously the very long-running processes (the daemon, the queue runner, and
eximon) must take care not to eat store.

Three different kinds of store are recognized:

. Long-lived: This is implemented by retaining the original malloc/free
  functions, and it used for permanent working buffers and for the caching of
  lookup data.

. Medium-term: Most of the dynamic store falls into this category. It is
  implemented as a stacking pool (POOL_MAIN) which can be thrown away after
  accepting a message, for example.

. Short-term: This is another stacking pool (POOL_TEMP) which is used for
  processing .forward (including filter) and alias files. The resulting new
  addresses are then created in the medium pool so that the store used in this
  processing can then be thrown away.

The conversion to the new scheme happened for versions of Exim after 1.92. */


#include "exim.h"


/* Define this to turn on the availability of debugging output via the
-dm option. It could be turned off to get a little bit more performance. */

#define DEBUG_STORE

/* We need to know how to align blocks of data for general use. I'm not sure
how to get an alignment factor in general. In the current world, a value of 8
is probably right, and this is sizeof(double) on some systems and sizeof(void
*) on others, so take the larger of those. Since everything in this expression
is a constant, the compiler should optimize it to a simple constant wherever it
appears (I checked that gcc does do this). */

#define alignment \
  ((sizeof(void *) > sizeof(double))? sizeof(void *) : sizeof(double))

/* Size of block to get from malloc to carve up into smaller ones. This
must be a multiple of the alignment. We assume that 8192 is going to be
suitably aligned. */

#define STORE_BLOCK_SIZE (8192 - alignment)

/* Structure describing the beginning of each big block. */

typedef struct storeblock {
  struct storeblock *next;
  size_t length;
} storeblock;

/* Static variables holding data for the local pools of store. The current pool
number is held in the global store_pool. Setting the initial length values to
-1 forces a malloc for the first call, even if the length is zero (which is
used for getting a point to reset to). */

static storeblock *chainbase[2] = { NULL, NULL };
static storeblock *current_block[2] = { NULL, NULL };
static void *next_yield[2] = { NULL, NULL };
static int yield_length[2] = { -1, -1 };

/* pool_malloc holds the amount of memory used by the store pools; this goes up
and down as store is reset or released. nonpool_malloc is the total got by
malloc from other calls; this doesn't go down because it is just freed by
pointer. */

static int pool_malloc = 0;
static int nonpool_malloc = 0;



/*************************************************
*                   Get a block                  *
*************************************************/

/* Running out of store is a total disaster. This function is called via the
macro store_get(). It passes back a block of store within the current big
block, getting a new one if necessary.

Arguments:
  size        amount wanted
  filename    source file from which called
  linenumber  line number in source file.

Returns:      pointer to store (panic on malloc failure)
*/

void *
store_get_3(int size, char *filename, int linenumber)
{
void *yield;

/* Round up the size to a multiple of the alignment. Although this looks a
messy statement, because "alignment" is a constant expression, the compiler can
do a reasonable job of optimizing, especially if the value of "alignment" is a
power of two. I checked this with -O2, and gcc did very well, compiling it to 4
instructions on a Sparc (alignment = 8). */

if (size % alignment != 0) size += alignment - (size % alignment);

/* If there isn't room in the current block, get a new one. The minimum
size is STORE_BLOCK_SIZE, and we would expect this to be the norm, since
these functions are mostly called for small amounts of store. */

if (size > yield_length[store_pool])
  {
  int length = (size <= STORE_BLOCK_SIZE)? STORE_BLOCK_SIZE : size;
  int mlength = length + sizeof(storeblock);
  storeblock *newblock;

  #ifdef DEBUG_STORE
  if (debug_trace_memory > 0)
    {
    pool_malloc += mlength;           /* Used in pools */
    nonpool_malloc -= mlength;        /* Exclude from overall total */
    }
  #endif

  newblock = store_malloc(mlength);
  newblock->next = NULL;
  newblock->length = length;

  if (chainbase[store_pool] == NULL) chainbase[store_pool] = newblock;
    else current_block[store_pool]->next = newblock;

  current_block[store_pool] = newblock;
  yield_length[store_pool] = length;
  next_yield[store_pool] =
    (void *)((char *)current_block[store_pool] + sizeof(storeblock));
  }

/* There's (now) enough room in the current block; the yield is the next
pointer. */

yield = next_yield[store_pool];

#ifdef DEBUG_STORE
if (debug_trace_memory > 0)
  {
  if (debug_trace_memory == 1)
    debug_printf("---%d Get %5d\n", store_pool, size);
  else
    debug_printf("---%d Get %6d %5d %-14s %4d\n", store_pool, yield, size,
      filename, linenumber);
  }
#endif

/* Update next pointer and number of bytes left in the current block. */

next_yield[store_pool] = (void *)((char *)next_yield[store_pool] + size);
yield_length[store_pool] -= size;

return yield;
}



/*************************************************
*      Extend a block if it is at the top        *
*************************************************/

/* While reading strings of unknown length, it is often the case that the
string is being read into the block at the top of the stack. If it needs to be
extended, it is more efficient just to extend the top block rather than
allocate a new block and then have to copy the data. This function is provided
for the use of string_cat(), but of course can be used elsewhere too.

Arguments:
  ptr        pointer to store block
  oldsize    current size of the block, as requested by user
  newsize    new size required
  filename   source file from which called
  linenumber line number in source file

Returns:     TRUE if the block is at the top of the stack and has been
             extended; FALSE if it isn't at the top of the stack, or cannot
             be extended
*/

BOOL
store_extend_3(void *ptr, int oldsize, int newsize, char *filename,
  int linenumber)
{
int inc = newsize - oldsize;
int rounded_oldsize = oldsize;

if (rounded_oldsize % alignment != 0)
  rounded_oldsize += alignment - (rounded_oldsize % alignment);

if ((char *)ptr + rounded_oldsize != (char *)(next_yield[store_pool]) ||
    inc > yield_length[store_pool] + rounded_oldsize - oldsize)
  return FALSE;

#ifdef DEBUG_STORE
if (debug_trace_memory > 0)
  {
  if (debug_trace_memory == 1)
    debug_printf("---%d Ext %5d\n", store_pool, newsize);
  else
    debug_printf("---%d Ext %6d %5d %-14s %4d\n", store_pool, ptr, newsize,
      filename, linenumber);
  }
#endif

if (newsize % alignment != 0) newsize += alignment - (newsize % alignment);
next_yield[store_pool] = (char *)ptr + newsize;
yield_length[store_pool] -= newsize - rounded_oldsize;
return TRUE;
}




/*************************************************
*    Back up to a previous point on the stack    *
*************************************************/

/* This function resets the next pointer, freeing any subsequent whole blocks
that are now unused. Normally it is given a pointer that was the yield of a
call to store_get, and is therefore aligned, but it may be given an offset
after such a pointer in order to release the end of a block and anything that
follows.

Arguments:
  ptr         place to back up to
  filename    source file from which called
  linenumber  line number in source file

Returns:      nothing
*/

void
store_reset_3(void *ptr, char *filename, int linenumber)
{
storeblock *bb;
storeblock *b = current_block[store_pool];
char *bc = (char *)b + sizeof(storeblock);
int newlength;

/* See if the place is in the current block - as it often will be. Otherwise,
search for the block in which it lies. */

if ((char *)ptr < bc || (char *)ptr > bc + b->length)
  {
  for (b = chainbase[store_pool]; b != NULL; b = b->next)
    {
    bc = (char *)b + sizeof(storeblock);
    if ((char *)ptr >= bc && (char *)ptr <= bc + b->length) break;
    }
  if (b == NULL)
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "internal error: store_reset(%d) "
      "failed: pool=%d %-14s %4d", ptr, store_pool, filename, linenumber);
  }

/* Back up, rounding to the alignment if necessary */

newlength = bc + b->length - (char *)ptr;
yield_length[store_pool] = newlength - (newlength % alignment);
next_yield[store_pool] = (char *)ptr + (newlength % alignment);
current_block[store_pool] = b;

/* Free any subsequent blocks, if any */

bb = b->next;
b->next = NULL;

while (bb != NULL)
  {
  b = bb;
  bb = bb->next;

  #ifdef DEBUG_STORE
  if (debug_trace_memory > 0) pool_malloc -= b->length;
  #endif

  store_free_3(b, filename, linenumber);
  }

#ifdef DEBUG_STORE
if (debug_trace_memory > 0)
  {
  if (debug_trace_memory == 1)
    debug_printf("---%d Rst    ** %d\n", store_pool, pool_malloc);
  else
    debug_printf("---%d Rst %6d    ** %-14s %4d %d\n", store_pool, ptr,
      filename, linenumber, pool_malloc);
  }
#endif
}





/************************************************
*             Release store                     *
************************************************/

/* This function is specifically provided for use when reading very
long strings, e.g. header lines. When the string gets longer than a
complete block, it gets copied to a new block. It is helpful to free
the old block iff the previous copy of the string is at its start,
and therefore the only thing in it. Otherwise, for very long strings,
dead store can pile up somewhat disastrously. This function checks that
the pointer it is given is the first thing in a block, and if so,
releases that block.

Arguments:
  block       block of store to consider
  filename    source file from which called
  linenumber  line number in source file

Returns:      nothing
*/

void
store_release_3(void *block, char *filename, int linenumber)
{
storeblock *b;

/* It will never be the first block, so no need to check that. */

for (b = chainbase[store_pool]; b != NULL; b = b->next)
  {
  storeblock *bb = b->next;
  if (bb != NULL && (char *)block == (char *)bb + sizeof(storeblock))
    {
    b->next = bb->next;
    #ifdef DEBUG_STORE
    if (debug_trace_memory > 0)
      {
      pool_malloc -= bb->length;
      if (debug_trace_memory == 1)
        debug_printf("-Release       %d\n", pool_malloc);
      else
        debug_printf("-Release %6d %-20s %4d %d\n", bb, filename,
          linenumber, pool_malloc);
      }
    #endif
    free(bb);
    return;
    }
  }
}




/*************************************************
*                Malloc store                    *
*************************************************/

/* Running out of store is a total disaster for exim. Some malloc functions
do not run happily on very small sizes, nor do they document this fact. This
function is called via the macro store_malloc().

Arguments:
  size        amount of store wanted
  filename    source file from which called
  linenumber  line number in source file

Returns:      pointer to gotten store (panic on failure)
*/

void *
store_malloc_3(int size, char *filename, int linenumber)
{
void *yield;

if (size < 16) size = 16;
yield = malloc((size_t)size);

if (yield == NULL)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "failed to malloc %d bytes of memory: "
    "called from line %d of %s", size, linenumber, filename);

#ifdef DEBUG_STORE
if (debug_trace_memory > 0)
  {
  nonpool_malloc += size;
  if (debug_trace_memory == 1)
    debug_printf("--Malloc %5d %d %d\n", size, pool_malloc,
      nonpool_malloc);
  else
    debug_printf("--Malloc %6d %5d %-14s %4d %d %d\n", yield, size,
      filename, linenumber, pool_malloc, nonpool_malloc);
  }
#endif

return yield;
}


/************************************************
*             Free store                        *
************************************************/

/* This function is called by the macro store_free().

Arguments:
  block       block of store to free
  filename    source file from which called
  linenumber  line number in source file

Returns:      nothing
*/

void
store_free_3(void *block, char *filename, int linenumber)
{
#ifdef DEBUG_STORE
if (debug_trace_memory > 0)
  {
  if (debug_trace_memory == 1)
    debug_printf("----Free\n");
  else
    debug_printf("----Free %6d %-20s %4d\n", block, filename, linenumber);
  }
#endif
free(block);
}

/* End of store.c */
