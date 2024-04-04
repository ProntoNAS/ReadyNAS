######################################################################
# Makefile.am                                              August 2001
# * Copyright (C) 2005, Infrant Technologies, Inc.        
# * All rights reserved 
######################################################################

bin_PROGRAMS = raidar
sbin_PROGRAMS = raidard

raidard_SOURCES =\
        checksum.c              checksum.h \
        int.h \
        raidard.h \
        raidard_serve.c \
        interface.c             interface.h \
        interfaces.c            interfaces.h \
        packet.c                packet.h \
        util.c                  util.h

raidar_SOURCES =\
        checksum.c              checksum.h \
        connection.c            connection.h \
        int.h \
        raidard.h \
        raidar_client.c \
        interface.c             interface.h \
        interfaces.c            interfaces.h \
        packet.c                packet.h \
        util.c                  util.h

LIBS =\
	-lz

EXTRA_DIST=autogen.sh
