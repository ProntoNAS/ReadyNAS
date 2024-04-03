# Make file for building a library containing all the available directors and
# calling it directors.a. This is called from the main make file, after cd'ing
# to the directors subdirectory. The library also contains some functions that
# are used by more than one director.

OBJ = aliasfile.o forwardfile.o localuser.o smartuser.o \
      df_extracted.o df_generated.o df_lookup.o

directors.a:     $(OBJ)
		 /bin/rm -f directors.a
		 $(AR) directors.a $(OBJ)
		 $(RANLIB) $@
		 /bin/rm -rf ../drtables.o

.SUFFIXES:       .o .c
.c.o:;           $(CC) -c $(CFLAGS) $(INCLUDE) $*.c

aliasfile.o:     $(HDRS) aliasfile.c    dfunctions.h aliasfile.h
df_extracted.o:  $(HDRS) df_extracted.c dfunctions.h
df_generated.o:  $(HDRS) df_generated.c dfunctions.h
df_lookup.o:     $(HDRS) df_lookup.c    dfunctions.h
forwardfile.o:   $(HDRS) forwardfile.c  dfunctions.h forwardfile.h
localuser.o:     $(HDRS) localuser.c    dfunctions.h localuser.h
smartuser.o:     $(HDRS) smartuser.c    dfunctions.h smartuser.h

# End
