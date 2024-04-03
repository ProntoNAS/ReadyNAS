# Make file for building a library containing all the available transports and
# calling it transports.a. This is called from the main make file, after cd'ing
# to the transports subdirectory.

OBJ = appendfile.o autoreply.o lmtp.o pipe.o smtp.o

transports.a:    $(OBJ)
		 /bin/rm -f transports.a
		 $(AR) transports.a $(OBJ)
		 $(RANLIB) $@
		 /bin/rm -rf ../drtables.o

.SUFFIXES:       .o .c
.c.o:;           $(CC) -c $(CFLAGS) $(INCLUDE) $*.c

appendfile.o:    $(HDRS) appendfile.c appendfile.h
autoreply.o:     $(HDRS) autoreply.c autoreply.h
lmtp.o:          $(HDRS) lmtp.c lmtp.h
pipe.o:          $(HDRS) pipe.c pipe.h
smtp.o:          $(HDRS) smtp.c smtp.h

# End
