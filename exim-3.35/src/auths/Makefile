# Make file for building a library containing all the available authorization
# methods, and calling it auths.a. In addition, there are functions that are
# of general use in several methods; these are in separate modules so they are
# linked in only when needed. This Makefile is called from the main make file,
# after cd'ing to the auths subdirectory. When the relevant AUTH_ macros are
# defined, the equivalent modules herein is not included in the final binary.

OBJ = b64encode.o b64decode.o call_pam.o xtextencode.o xtextdecode.o get_data.o \
      md5.o cram_md5.o plaintext.o

auths.a:         $(OBJ)
		 /bin/rm -f auths.a
		 $(AR) auths.a $(OBJ)
		 $(RANLIB) $@
		 /bin/rm -rf ../drtables.o

.SUFFIXES:       .o .c
.c.o:;           $(CC) -c $(CFLAGS) $(INCLUDE) $*.c

b64encode.o:     $(HDRS) b64encode.c
b64decode.o:     $(HDRS) b64decode.c
call_pam.o:      $(HDRS) call_pam.c
get_data.o:      $(HDRS) get_data.c
md5.o:           $(HDRS) md5.c
xtextencode.o:   $(HDRS) xtextencode.c
xtextdecode.o:   $(HDRS) xtextdecode.c

cram_md5.o:      $(HDRS) cram_md5.c cram_md5.h
plaintext.o:     $(HDRS) plaintext.c plaintext.h

# End
