lib_LTLIBRARIES = librpcsecgss.la
librpcsecgss_la_LDFLAGS = -version-info 3:0:0
librpcsecgss_la_SOURCES = \
	auth_gss.c authgss_prot.c svc.c svc_run.c svc_auth.c \
	svc_auth_none.c svc_auth_unix.c svc_auth_gss.c \
	svc_raw.c svc_simple.c svc_tcp.c svc_udp.c \
	rpc_commondata.c \
	clnt_generic.c clnt_perror.c clnt_raw.c clnt_simple.c \
	clnt_tcp.c clnt_udp.c

nobase_include_HEADERS = \
	../include/rpcsecgss/rpc/auth.h \
	../include/rpcsecgss/rpc/auth_gss.h \
	../include/rpcsecgss/rpc/rpc.h \
	../include/rpcsecgss/rpc/rpcsecgss_rename.h \
	../include/rpcsecgss/rpc/svc.h \
	../include/rpcsecgss/rpc/svc_auth.h


EXTRA_DIST = \
	../include/rpcsecgss/rpc/auth.h \
	../include/rpcsecgss/rpc/auth_gss.h \
	../include/rpcsecgss/rpc/rpc.h \
	../include/rpcsecgss/rpc/rpcsecgss_rename.h \
	../include/rpcsecgss/rpc/svc.h \
	../include/rpcsecgss/rpc/svc_auth.h

INCLUDES = -I$(srcdir)/../include/rpcsecgss $(GSSAPI_CFLAGS)

OLD_HDRS = \
	../include/rpcsecgss/rpc/auth_unix.h \
	../include/rpcsecgss/rpc/clnt.h \
	../include/rpcsecgss/rpc/pmap_clnt.h \
	../include/rpcsecgss/rpc/pmap_prot.h \
	../include/rpcsecgss/rpc/pmap_rmt.h \
	../include/rpcsecgss/rpc/rpc_des.h \
	../include/rpcsecgss/rpc/rpc_msg.h \
	../include/rpcsecgss/rpc/types.h \
	../include/rpcsecgss/rpc/xdr.h

install-data-hook:
	for h in $(OLD_HDRS); do \
                if [ -e $(DESTDIR)$(includedir)/$$h ]; then \
                        echo "Removing $(DESTDIR)$(includedir)/$$h installed from previous version"; \
			rm $(DESTDIR)$(includedir)/$$h; \
                fi \
        done
