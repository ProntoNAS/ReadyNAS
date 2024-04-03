#!/bin/sh
# Used to create the dfsg .orig.tar from the upstream source
rm -r ElectricFence
rm -r sntp
rm util/ansi2knr.c
rm util/ansi2knr.1
rm html/hints/solaris-dosynctodr.html
rm libntp/adjtime.c
rm include/adjtime.h
rm include/timepps-SCO.h
rm include/timepps-Solaris.h
rm include/timepps-SunOS.h
rm kernel/tty_chu_STREAMS.c
rm kernel/tty_clk_STREAMS.c
rm ports/winnt/libntp/messages.mc
rm ports/winnt/include/hopf_PCI_io.h
rm scripts/monitoring/lr.pl
rm scripts/monitoring/ntp.pl
rm scripts/monitoring/ntploopstat
rm scripts/monitoring/ntploopwatch
rm scripts/monitoring/ntptrap
rm scripts/ntpver.in
rm libparse/clk_wharton.c
rm ntpd/refclock_neoclock4x.c
