# You can do "make SUB=blah" to make only a few, or edit here, or both
# You can also run make directly in the subdirs you want.

include MCONFIG

SUB = inetd ping rpcinfo

# ifneq ($(USE_GLIBC),1)
SUB += rpcgen
# endif

%.build:
	$(MAKE) -C $(patsubst %.build, %, $@)

%.install:
	$(MAKE) -C $(patsubst %.install, %, $@) install

%.clean:
	$(MAKE) -C $(patsubst %.clean, %, $@) clean

all:     $(patsubst %, %.build, $(SUB))
install: $(patsubst %, %.install, $(SUB))
clean:   $(patsubst %, %.clean, $(SUB))
