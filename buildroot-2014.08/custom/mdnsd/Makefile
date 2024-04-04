#
# Makefile for tinysvcmdns
#

CFLAGS += -Wall -pedantic -std=gnu99
#CFLAGS += -g
CFLAGS += -s -Os -DNDEBUG
LDLIBS = -lpthread

ifneq ($(CROSS_COMPILE),)
  CC = gcc
  CC := $(CROSS_COMPILE)$(CC)
  AR := $(CROSS_COMPILE)$(AR)
endif

BIN=mdnsd

LIBTINYSVCMDNS_OBJS = mdns.o mdnsd.o

.PHONY: all clean

all: $(BIN) libtinysvcmdns.a

clean:
	-$(RM) *.o
	-$(RM) *.bin
	-$(RM) mdns
	-$(RM) $(BIN)
	-$(RM) libtinysvcmdns.a

mdns: mdns.o

testmdnsd: testmdnsd.o libtinysvcmdns.a

mdnsd: rnmdnsd.o libtinysvcmdns.a

libtinysvcmdns.a: $(patsubst %, libtinysvcmdns.a(%), $(LIBTINYSVCMDNS_OBJS))

