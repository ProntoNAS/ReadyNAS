CFLAGS += -DHAVE_CONFIG_H
OBJECTS := src/rrd_tool-netgear.c
LIBS :=	-Wl,--as-needed -Wl,-Bstatic src/.libs/librrd.a -lpangocairo-1.0 -lpangoft2-1.0 -lpango-1.0 \
	-lcairo -lpixman-1 -lfontconfig -lfreetype -lthai -ldatrie -Wl,-Bdynamic -lgobject-2.0 \
	-lgmodule-2.0 -lexpat -lpng12 -lz -lm -lglib-2.0 -lpthread
ifneq (,$(findstring harfbuzz,$(shell pkg-config --static --libs pangoft2)))
LIBS += -lharfbuzz
endif

all: rrdtool

rrdtool: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(LIBS) -o rrdtool

install: rrdtool
	install -D rrdtool $(DESTDIR)/usr/bin/rrd-tool

clean:
	rm -f *.o */*.o rrdtool *~
