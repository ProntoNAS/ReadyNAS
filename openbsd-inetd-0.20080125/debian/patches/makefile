--- /dev/null
+++ b/Makefile.debian
@@ -0,0 +1,16 @@
+DEFS := -DLIBWRAP
+LIBS := -lwrap
+
+inetd_OBJECTS := inetd.o setproctitle.o strlcpy.o
+
+all: inetd
+
+.c.o:
+	$(CC) $(DEFS) $(CFLAGS) -c $<
+
+inetd: $(inetd_OBJECTS)
+	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
+
+clean:
+	rm -f inetd inetd.o setproctitle.o strlcpy.o
+
