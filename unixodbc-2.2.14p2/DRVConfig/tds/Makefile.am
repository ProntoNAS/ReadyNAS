lib_LTLIBRARIES = libtdsS.la

INCLUDES = -I@top_srcdir@/include $(LTDLINCL)

libtdsS_la_LDFLAGS = -no-undefined  -version-info 1:0:0 -module

DEFS = -DUNIXODBC @DEFS@

libtdsS_la_SOURCES = tdsS.c
	
