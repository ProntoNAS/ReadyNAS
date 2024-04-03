# Make file for building a library containing all the available lookups and
# calling it lookups.a. This is called from the main make file, after cd'ing
# to the lookups subdirectory. When the relevant LOOKUP_ macros are not
# defined, dummy modules get compiled.

OBJ = cdb.o dbmdb.o dnsdb.o ldap.o lsearch.o mysql.o nis.o nisplus.o pgsql.o \
      testdb.o

lookups.a:       $(OBJ)
		 /bin/rm -f lookups.a
		 $(AR) lookups.a $(OBJ)
		 $(RANLIB) $@
		 /bin/rm -rf ../drtables.o

.SUFFIXES:       .o .c
.c.o:;           $(CC) -c $(CFLAGS) $(INCLUDE) $*.c

cdb.o:           $(HDRS) cdb.c       cdb.h
dbmdb.o:         $(HDRS) dbmdb.c     dbmdb.h
dnsdb.o:         $(HDRS) dnsdb.c     dnsdb.h
ldap.o:          $(HDRS) ldap.c      ldap.h
lsearch.o:       $(HDRS) lsearch.c   lsearch.h
mysql.o:         $(HDRS) mysql.c     mysql.h
nis.o:           $(HDRS) nis.c       nis.h
nisplus.o:       $(HDRS) nisplus.c   nisplus.h
pgsql.o:         $(HDRS) pgsql.c     pgsql.h
testdb.o:        $(HDRS) testdb.c    testdb.h

# End
