# Comment/uncomment the following line to enable/disable debugging

EXTRA_CFLAGS            += -DRT2500_DBG

rt2500-objs := rtmp_main.o mlme.o connect.o sync.o assoc.o \
		auth.o auth_rsp.o rtmp_data.o rtmp_init.o sanity.o \
		rtmp_wep.o rtmp_info.o eeprom.o rtmp_tkip.o wpa.o md5.o

obj-$(CONFIG_RT2500) += rt2500.o
