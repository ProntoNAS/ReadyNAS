#!/bin/bash
HOST=x86_64-buildroot-linux-uclibc
ARCH=x86_64

if [ -z "$1" ]; then
  echo "No target directory specified!"
  exit 1
fi

rm -f  $1/bin/{findmnt,lsblk,mountpoint,wdctl}
rm -rf $1/etc/{init.d/*,smartd.conf}
rm -rf $1/home/*
rm -f  $1/lib/libmount.so.1*
rm -f  $1/sbin/{chcpu,ctrlaltdel,fdisk,findfs,fsck.minix,fsfreeze,fstrim,hwclock,ldconfig,losetup,mkfs,mkfs.bfs,mkfs.minix,sfdisk,swaplabel}
rm -f  $1/usr/bin/{attr,btrfs-convert,btrfs-debug-tree,btrfs-find-root,btrfs-image,btrfs-map-logical,btrfs-s*,btrfstune,getfattr,setfattr,tailf,renice,ipcmk,ipcrm,ipcs,isosize,cal,col*,cytune,logger,look,mcookie,namei,rev,script*,setarch,setsid,i386,linux32,linux64,x86_64,chkdupexe,getconf,getopt,flock,lscpu,whereis,sg*,sqlite3,llconf,iconv,lslocks,prlimit,uuidgen,sha3sum}
rm -f  $1/usr/lib/{libstdc++.so.6.0.18-gdb.py,*.so,libattr.so.1*}
rm -f  $1/usr/sbin/{readprofile,vgimportclone,update-smart-drivedb,smartd,tunelp,e4defrag,fdformat,rtcwake,ldattach,fsadm,lvmconf,lvmdump,kdump,biosdecode,vpddecode,ownership}
rm -rf $1/usr/share/{getopt,smartmontools,locale,bash-completion}
rm -rf $1/var/{pcmcia,lib/pcmcia,www}

ln -sf ../bin/rnutil $1/usr/sbin/chroot
mv $1/etc/mke2fs.conf.e2fsprogs-old $1/etc/mke2fs.conf
cp custom/skeleton/etc/mdev.conf output/target/etc/mdev.conf
cp custom/skeleton/etc/network/interfaces output/target/etc/network/interfaces

MEMTEST_DIR="../../../boot/memtest86+"
LIBREADYNAS_DIR="../../../libs/libreadynas"
INIT_DIR="../../../boot/init"
RNUTIL_DIR="../../../utils/rnutil"
RAIDARD_DIR="../../../packages/raidard"
[ -d "$LIBREADYNAS_DIR" ] || exit 1
[ -d "$INIT_DIR" ] || exit 1
[ -d "$RNUTIL_DIR" ] || exit 1
[ -d "$RAIDARD_DIR" ] || exit 1

export PATH="$(dirname $1)/host/usr/bin:$PATH"
export CC=${HOST}-gcc
export AR=${HOST}-ar
export LIBTOOL="${PWD}/output/build/popt-1.16/libtool"
export CFLAGS="-I${PWD}/custom -I${PWD}/$LIBREADYNAS_DIR/src -fno-stack-protector"
export LDFLAGS="-L${PWD}/$LIBREADYNAS_DIR/src/.libs"
export LIBREADYNAS_LOC="${PWD}/$LIBREADYNAS_DIR/src"
(cd $MEMTEST_DIR && make clean && make G2=1 && cp -av memtest_shared $1/etc/memtest_image) || exit 1
(cd $LIBREADYNAS_DIR/src && make clean && make LIBTOOL=$LIBTOOL CC=$CC all && cp -a .libs/libreadynas.so.* $1/usr/lib && ln -sfT . libreadynas) || exit 1
(cd $INIT_DIR && make clean && make && cp -av init $1) || exit 1
(cd $RNUTIL_DIR && make clean && make && cp -av rnutil $1/usr/bin) || exit 1
(cd $RAIDARD_DIR && make distclean; ./configure --host=${HOST} && make && ${HOST}-strip raidard && cp -av raidard $1/usr/sbin) || exit 1
(cd custom/nbnsd && make clean; make && cp -av nbnsd $1/usr/sbin) || exit 1
(cd custom/mdnsd && make clean; make && cp -av mdnsd $1/usr/sbin) || exit 1

exit 0
