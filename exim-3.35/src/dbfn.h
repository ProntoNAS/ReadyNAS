/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* This file had its name changed from db.h to dbfn.h because BSDI (for one)
has a file in /usr/include called db.h which gets called from inside
/usr/include/ndbm.h. In fact, db.h is the standard header for Berkeley DB,
which in its latest incarnation also has functions that start db_. To avoid
problems with this, all Exim's db_ names have now been changed to start with
dbfn_ or dbdata_ instead. */


/* Additional code (courtesy of Nigel Metheringham) added so that either ndbm
or Berkeley db 1.x in native mode can be used for the db files. This is
selected on the USE_DB define. All the db functions that are used by Exim are
redefined as macros. Subsequently further macros were added to permit the use
of Berkeley DB 2.x, and 3.x, which can be distinguished automatically from 1.x.
Later still, additional code (courtesy of James Antill) has been added to
support the use of tdb as yet another alternative DBM library. */


# ifdef USE_TDB

/* ************************* tdb interface ************************ */

#include <tdb.h>

/* Basic DB type */
#define EXIM_DB TDB_CONTEXT

/* Cursor type: tdb uses the previous "key" in _nextkey() (really it wants
tdb_traverse to be called) */
#define EXIM_CURSOR TDB_DATA

/* The datum type used for queries */
#define EXIM_DATUM TDB_DATA

/* Some text for messages */
#define EXIM_DBTYPE "tdb"

/* Access functions */

/* EXIM_DBOPEN - sets *dbpp to point to an EXIM_DB, NULL if failed */
#define EXIM_DBOPEN(name, flags, mode, dbpp) \
       *(dbpp) = tdb_open(name, 0, TDB_DEFAULT, flags, mode)

/* EXIM_DBGET - returns TRUE if successful, FALSE otherwise */
#define EXIM_DBGET(db, key, data)      \
       (data = tdb_fetch(db, key), data.dptr != NULL)

/* EXIM_DBPUT - returns nothing useful, assumes replace mode */
#define EXIM_DBPUT(db, key, data)      \
       tdb_store(db, key, data, TDB_REPLACE)

/* EXIM_DBPUTB - non-overwriting for use by dbmbuild */
#define EXIM_DBPUTB(db, key, data)      \
       tdb_store(db, key, data, TDB_INSERT)

/* Returns from EXIM_DBPUTB */

#define EXIM_DBPUTB_OK  0
#define EXIM_DBPUTB_DUP (-1)

/* EXIM_DBDEL */
#define EXIM_DBDEL(db, key) tdb_delete(db, key)

/* EXIM_DBCREATE_CURSOR - initialize for scanning operation */
#define EXIM_DBCREATE_CURSOR(db, cursor) { \
   *(cursor) = malloc(sizeof(TDB_DATA)); (*(cursor))->dptr = NULL; }

/* EXIM_DBSCAN - This is complicated because we have to free the last datum
free() must not die when passed NULL */
#define EXIM_DBSCAN(db, key, data, first, cursor)      \
       (key = (first ? tdb_firstkey(db) : tdb_nextkey(db, *(cursor))), \
        free((cursor)->dptr), *(cursor) = key, \
        key.dptr != NULL)

/* EXIM_DBDELETE_CURSOR - terminate scanning operation. */
#define EXIM_DBDELETE_CURSOR(cursor) free(cursor)

/* EXIM_DBCLOSE */
#define EXIM_DBCLOSE(db)        tdb_close(db)

/* Datum access types - these are intended to be assignable */

#define EXIM_DATUM_SIZE(datum)  (datum).dsize
#define EXIM_DATUM_DATA(datum)  (datum).dptr

/* Free the stuff inside the datum. */

#define EXIM_DATUM_FREE(datum)  (free((datum).dptr), (datum).dptr = NULL)

/* No initialization is needed. */

#define EXIM_DATUM_INIT(datum)



/********************* Berkeley db native definitions **********************/

#elif defined USE_DB

#include <db.h>


/* We can distinguish between versions 1.x and 2.x/3.x by looking for a
definition of DB_VERSION_STRING, which is present in versions 2.x onwards. */

#ifdef DB_VERSION_STRING

/* The API changed (again!) between the 2.x and 3.x versions */

#if DB_VERSION_MAJOR >= 3

/******************* Berkeley db 3.x native definitions ********************/

/* Basic DB type */
#define EXIM_DB       DB

/* Cursor type, for scanning */
#define EXIM_CURSOR   DBC

/* The datum type used for queries */
#define EXIM_DATUM    DBT

/* Some text for messages */
#define EXIM_DBTYPE   "db (v3)"

/* Access functions */

/* EXIM_DBOPEN - sets *dbpp to point to an EXIM_DB, NULL if failed */
#define EXIM_DBOPEN(name, flags, mode, dbpp)         \
       if (db_create(dbpp, NULL, 0) != 0 || \
         (*dbpp)->open(*dbpp, name, NULL, DB_HASH, \
         ((flags) == O_RDONLY)? DB_RDONLY : DB_CREATE, \
         mode) != 0) *(dbpp) = NULL

/* EXIM_DBGET - returns TRUE if successful, FALSE otherwise */
#define EXIM_DBGET(db, key, data)      \
       ((db)->get(db, NULL, &key, &data, 0) == 0)

/* EXIM_DBPUT - returns nothing useful, assumes replace mode */
#define EXIM_DBPUT(db, key, data)      \
       (db)->put(db, NULL, &key, &data, 0)

/* EXIM_DBPUTB - non-overwriting for use by dbmbuild */
#define EXIM_DBPUTB(db, key, data)      \
       (db)->put(db, NULL, &key, &data, DB_NOOVERWRITE)

/* Return values from EXIM_DBPUTB */

#define EXIM_DBPUTB_OK  0
#define EXIM_DBPUTB_DUP DB_KEYEXIST

/* EXIM_DBDEL */
#define EXIM_DBDEL(db, key)     (db)->del(db, NULL, &key, 0)

/* EXIM_DBCREATE_CURSOR - initialize for scanning operation */

#define EXIM_DBCREATE_CURSOR(db, cursor) \
       (db)->cursor(db, NULL, cursor, 0)

/* EXIM_DBSCAN - returns TRUE if data is returned, FALSE at end */
#define EXIM_DBSCAN(db, key, data, first, cursor)      \
       ((cursor)->c_get(cursor, &key, &data,         \
         (first? DB_FIRST : DB_NEXT)) == 0)

/* EXIM_DBDELETE_CURSOR - terminate scanning operation */
#define EXIM_DBDELETE_CURSOR(cursor) \
       (cursor)->c_close(cursor)

/* EXIM_DBCLOSE */
#define EXIM_DBCLOSE(db)        (db)->close(db, 0)

/* Datum access types - these are intended to be assignable. */

#define EXIM_DATUM_SIZE(datum)  (datum).size
#define EXIM_DATUM_DATA(datum)  (datum).data

/* The whole datum structure contains other fields that must be cleared
before use, but we don't have to free anything after reading data. */

#define EXIM_DATUM_INIT(datum)   memset(&datum, 0, sizeof(datum))
#define EXIM_DATUM_FREE(datum)


#else /* DB_VERSION_MAJOR >= 3 */

/******************* Berkeley db 2.x native definitions ********************/

/* Basic DB type */
#define EXIM_DB       DB

/* Cursor type, for scanning */
#define EXIM_CURSOR   DBC

/* The datum type used for queries */
#define EXIM_DATUM    DBT

/* Some text for messages */
#define EXIM_DBTYPE   "db (v2)"

/* Access functions */

/* EXIM_DBOPEN - sets *dbpp to point to an EXIM_DB, NULL if failed */
#define EXIM_DBOPEN(name, flags, mode, dbpp)         \
       if ((errno = db_open(name, DB_HASH,           \
         ((flags) == O_RDONLY)? DB_RDONLY : DB_CREATE, \
         mode, NULL, NULL, dbpp)) != 0) *(dbpp) = NULL

/* EXIM_DBGET - returns TRUE if successful, FALSE otherwise */
#define EXIM_DBGET(db, key, data)      \
       ((db)->get(db, NULL, &key, &data, 0) == 0)

/* EXIM_DBPUT - returns nothing useful, assumes replace mode */
#define EXIM_DBPUT(db, key, data)      \
       (db)->put(db, NULL, &key, &data, 0)

/* EXIM_DBPUTB - non-overwriting for use by dbmbuild */
#define EXIM_DBPUTB(db, key, data)      \
       (db)->put(db, NULL, &key, &data, DB_NOOVERWRITE)

/* Return values from EXIM_DBPUTB */

#define EXIM_DBPUTB_OK  0
#define EXIM_DBPUTB_DUP DB_KEYEXIST

/* EXIM_DBDEL */
#define EXIM_DBDEL(db, key)     (db)->del(db, NULL, &key, 0)

/* EXIM_DBCREATE_CURSOR - initialize for scanning operation */

/* The API of this function was changed between releases 2.4.14 and 2.7.3. I do
not know exactly where the change happened, but the Change Log for 2.5.9 lists
the new option that is available, so I guess that it happened at 2.5.x. */

#if DB_VERSION_MINOR >= 5
#define EXIM_DBCREATE_CURSOR(db, cursor) \
       (db)->cursor(db, NULL, cursor, 0)
#else
#define EXIM_DBCREATE_CURSOR(db, cursor) \
       (db)->cursor(db, NULL, cursor)
#endif

/* EXIM_DBSCAN - returns TRUE if data is returned, FALSE at end */
#define EXIM_DBSCAN(db, key, data, first, cursor)      \
       ((cursor)->c_get(cursor, &key, &data,         \
         (first? DB_FIRST : DB_NEXT)) == 0)

/* EXIM_DBDELETE_CURSOR - terminate scanning operation */
#define EXIM_DBDELETE_CURSOR(cursor) \
       (cursor)->c_close(cursor)

/* EXIM_DBCLOSE */
#define EXIM_DBCLOSE(db)        (db)->close(db, 0)

/* Datum access types - these are intended to be assignable. */

#define EXIM_DATUM_SIZE(datum)  (datum).size
#define EXIM_DATUM_DATA(datum)  (datum).data

/* The whole datum structure contains other fields that must be cleared
before use, but we don't have to free anything after reading data. */

#define EXIM_DATUM_INIT(datum)   memset(&datum, 0, sizeof(datum))
#define EXIM_DATUM_FREE(datum)

#endif /* DB_VERSION_MAJOR >= 3 */



/* If DB_VERSION_TYPE is not defined, we have version 1.x */

#else  /* DB_VERSION_TYPE */

/******************* Berkeley db 1.x native definitions ********************/

/* Basic DB type */
#define EXIM_DB       DB

/* Cursor type, not used with DB 1.x: just set up a dummy */
#define EXIM_CURSOR   int

/* The datum type used for queries */
#define EXIM_DATUM    DBT

/* Some text for messages */
#define EXIM_DBTYPE   "db (v1)"

/* Access functions */

/* EXIM_DBOPEN - sets *dbpp to point to an EXIM_DB, NULL if failed */
#define EXIM_DBOPEN(name, flags, mode, dbpp) \
       *(dbpp) = dbopen(name, flags, mode, DB_HASH, NULL)

/* EXIM_DBGET - returns TRUE if successful, FALSE otherwise */
#define EXIM_DBGET(db, key, data)      \
       ((db)->get(db, &key, &data, 0) == 0)

/* EXIM_DBPUT - returns nothing useful, assumes replace mode */
#define EXIM_DBPUT(db, key, data)      \
       (db)->put(db, &key, &data, 0)

/* EXIM_DBPUTB - non-overwriting for use by dbmbuild */
#define EXIM_DBPUTB(db, key, data)      \
       (db)->put(db, &key, &data, R_NOOVERWRITE)

/* Returns from EXIM_DBPUTB */

#define EXIM_DBPUTB_OK  0
#define EXIM_DBPUTB_DUP 1

/* EXIM_DBDEL */
#define EXIM_DBDEL(db, key)     (db)->del(db, &key, 0)

/* EXIM_DBCREATE_CURSOR - initialize for scanning operation (null) */
#define EXIM_DBCREATE_CURSOR(db, cursor) {}

/* EXIM_DBSCAN - returns TRUE if data is returned, FALSE at end */
#define EXIM_DBSCAN(db, key, data, first, cursor)      \
       ((db)->seq(db, &key, &data, (first? R_FIRST : 0)) == 0)

/* EXIM_DBDELETE_CURSOR - terminate scanning operation (null). Make it
refer to cursor, to keep picky compilers happy. */
#define EXIM_DBDELETE_CURSOR(cursor) { cursor = cursor; }

/* EXIM_DBCLOSE */
#define EXIM_DBCLOSE(db)        (db)->close(db)

/* Datum access types - these are intended to be assignable */

#define EXIM_DATUM_SIZE(datum)  (datum).size
#define EXIM_DATUM_DATA(datum)  (datum).data

/* There's no clearing required before use, and we don't have to free anything
after reading data. */

#define EXIM_DATUM_INIT(datum)
#define EXIM_DATUM_FREE(datum)

#endif /* DB_VERSION_STRING */
#else  /* USE_DB */

/* If neither USE_DB nor USE_TDB is set, the default is the NDBM interface */


/********************* ndbm interface definitions **********************/

#include <ndbm.h>

/* Basic DB type */
#define EXIM_DB DBM

/* Cursor type, not used with ndbm: just set up a dummy */
#define EXIM_CURSOR int

/* The datum type used for queries */
#define EXIM_DATUM datum

/* Some text for messages */

#define EXIM_DBTYPE "ndbm"

/* Access functions */

/* EXIM_DBOPEN - returns a EXIM_DB *, NULL if failed */
#define EXIM_DBOPEN(name, flags, mode, dbpp) \
       *(dbpp) = dbm_open(name, flags, mode)

/* EXIM_DBGET - returns TRUE if successful, FALSE otherwise */
#define EXIM_DBGET(db, key, data)      \
       (data = dbm_fetch(db, key), data.dptr != NULL)

/* EXIM_DBPUT - returns nothing useful, assumes replace mode */
#define EXIM_DBPUT(db, key, data)      \
       dbm_store(db, key, data, DBM_REPLACE)

/* EXIM_DBPUTB - non-overwriting for use by dbmbuild */
#define EXIM_DBPUTB(db, key, data)      \
       dbm_store(db, key, data, DBM_INSERT)

/* Returns from EXIM_DBPUTB */

#define EXIM_DBPUTB_OK  0
#define EXIM_DBPUTB_DUP 1

/* EXIM_DBDEL */
#define EXIM_DBDEL(db, key) dbm_delete(db, key)

/* EXIM_DBCREATE_CURSOR - initialize for scanning operation (null) */
#define EXIM_DBCREATE_CURSOR(db, cursor) {}

/* EXIM_DBSCAN */
#define EXIM_DBSCAN(db, key, data, first, cursor)      \
       (key = (first? dbm_firstkey(db) : dbm_nextkey(db)), key.dptr != NULL)

/* EXIM_DBDELETE_CURSOR - terminate scanning operation (null). Make it
refer to cursor, to keep picky compilers happy. */
#define EXIM_DBDELETE_CURSOR(cursor) { cursor = cursor; }

/* EXIM_DBCLOSE */
#define EXIM_DBCLOSE(db) dbm_close(db)

/* Datum access types - these are intended to be assignable */

#define EXIM_DATUM_SIZE(datum) (datum).dsize
#define EXIM_DATUM_DATA(datum) (datum).dptr

/* There's no clearing required before use, and we don't have to free anything
after reading data. */

#define EXIM_DATUM_INIT(datum)
#define EXIM_DATUM_FREE(datum)

#endif /* USE_DB */

/********************* End of dbm library definitions **********************/


/* Structure for carrying around an open DBM file, and an open locking file
that relates to it. */

typedef struct {
  EXIM_DB *dbptr;
  int lockfd;
} open_db;


/* Structures for records stored in exim database dbm files. They all
start with the same fields, described in the generic type. */


typedef struct {
  time_t time_stamp;      /* Timestamp of writing */
} dbdata_generic;


/* This structure keeps track of retry information for a host or a local
address. */

typedef struct {
  time_t time_stamp;
  /*************/
  time_t first_failed;    /* Time of first failure */
  time_t last_try;        /* Time of last try */
  time_t next_try;        /* Time of next try */
  BOOL   expired;         /* Retry time has expired */
  int    basic_errno;     /* Errno of last failure */
  int    more_errno;      /* Additional information */
  char   text[1];         /* Text message for last failure */
} dbdata_retry;

/* This structure keeps track of messages that are waiting for a particular
host for a particular transport. */

typedef struct {
  time_t time_stamp;
  /*************/
  int    count;           /* Count of message ids */
  int    sequence;        /* Sequence for continued records */
  char   text[1];         /* One long character string */
} dbdata_wait;

/* This structure records recent message rejections. The key is the bad
address + host identification. */

typedef struct {
  time_t time_stamp;
  /*************/
  BOOL   rejected_mail_from;  /* Already tried rejecting MAIL FROM */
  int    temp_count;          /* Count of temporary rejections */
  float  temp_rate;           /* Rate of temporary rejections*/
} dbdata_reject;

/* This structure records a connection to a particular host, for the
purpose of serializing access to certain hosts. For possible future extension,
a field is defined for holding the count of connections, but it is not
at present in use. */

typedef struct {
  time_t time_stamp;
  /*************/
  int    count;           /* Reserved for possible connection count */
} dbdata_serialize;


/* Functions for reading/writing exim database files */

void  dbfn_close(open_db *);
int   dbfn_delete(open_db *, char *);
open_db *dbfn_open(char *, int, open_db *, BOOL);
void *dbfn_read_with_length(open_db *, char *, int *);
char *dbfn_scan(open_db *, BOOL, EXIM_CURSOR **);
int   dbfn_write(open_db *, char *, void *, int);

/* Macro for the common call to read without wanting to know the length. */

#define dbfn_read(a, b) dbfn_read_with_length(a, b, NULL)

/* End of dbfn.h */
