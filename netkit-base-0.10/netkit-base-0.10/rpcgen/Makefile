all: rpcgen

include ../MCONFIG
include ../MRULES

OBJECTS = rpc_clntout.o rpc_cout.o rpc_hout.o rpc_main.o rpc_parse.o \
  	  rpc_scan.o rpc_svcout.o rpc_tblout.o rpc_util.o rpc_sample.o

rpcgen: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(OBJECTS): proto.h rpc_parse.h rpc_scan.h rpc_util.h

install: rpcgen
	install -s -m$(BINMODE) rpcgen $(INSTALLROOT)$(BINDIR)
	install -m$(MANMODE) rpcgen.1 $(INSTALLROOT)$(MANDIR)/man1

clean:
	rm -f *.o rpcgen

