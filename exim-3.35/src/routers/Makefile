# Make file for building a library containing all the available routers and
# calling it routers.a. This is called from the main make file, after cd'ing
# to the directors subdirectory.

OBJ = domainlist.o ipliteral.o iplookup.o lookuphost.o queryprogram.o

routers.a:       $(OBJ)
		 /bin/rm -f routers.a
		 $(AR) routers.a $(OBJ)
		 $(RANLIB) $@
		 /bin/rm -rf ../drtables.o

.SUFFIXES:       .o .c
.c.o:;           $(CC) -c $(CFLAGS) $(INCLUDE) $*.c

domainlist.o:    $(HDRS) domainlist.c domainlist.h
ipliteral.o:     $(HDRS) ipliteral.c ipliteral.h
iplookup.o:      $(HDRS) iplookup.c iplookup.h
lookuphost.o:    $(HDRS) lookuphost.c lookuphost.h
queryprogram.o:  $(HDRS) queryprogram.c queryprogram.h

# End
