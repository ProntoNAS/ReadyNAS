#!/bin/bash
HEADURL='$HeadURL: svn+ssh://svn@svn-lf.netgear.com/svn/RAIDiator-Platform/branches/6.3-dev/boot/initramfs/buildroot/custom/customize-x86_64.sh $'
HOST=x86_64-buildroot-linux-uclibc
ARCH=x86_64
if [ -z "$MODULE_BASE" ]; then
  MODULE_DIR=/data/readynasos/bin/${ARCH}/modules
else
  MODULE_DIR=${MODULE_BASE}/${ARCH}/modules
fi

if [ -z "$BRANCH" ]; then
  BRANCH=$(echo $HEADURL | sed -e 's#.*RAIDiator-Platform/##' -e 's#/boot/.*##')
fi
if [ -z "$LRNBRANCH" ]; then
  if [ "$BRANCH" = "trunk" ]; then
    LRNBRANCH="branches/R6";
  else
    LRNBRANCH="$BRANCH";
  fi
fi

check_out() {
  target=$3
  [ -z "$target" ] && target=$(basename $2)
  if [ "$1" = "svn" ]; then
    (cd custom && svn co svn+ssh://svn@svn-lf.netgear.com/svn/${2} ${target})
  elif [ "$1" = "cvs" ]; then
    (cd custom && cvs -d :pserver:jmaggard@calzone:/cvs co ${2})
  fi
}

if [ -z "$1" ]; then
  echo "No target directory specified!"
  exit 1
fi

rm -f  $1/bin/{findmnt,lsblk,mountpoint,wdctl}
rm -rf $1/etc/{init.d/*,lvm,smartd.conf}
rm -rf $1/home/*
rm -f  $1/lib/libmount.so.1*
rm -f  $1/sbin/{blkdiscard,chcpu,ctrlaltdel,fdisk,findfs,fsck.minix,fsfreeze,fstrim,hwclock,ldconfig,losetup,mkfs,mkfs.bfs,mkfs.minix,sfdisk,swaplabel}
rm -f  $1/usr/bin/{attr,cal,chacl,chkdupexe,col*,cytune,envsubst,flock,getconf,getfattr,getopt,gettext*,i386,iconv,ipcmk,ipcrm,ipcs,isosize,linux32,linux64,llconf,logger,look,lscpu,lslocks,mcookie,namei,ngettext,nsenter,prlimit,renice,rev,script*,setarch,setfattr,setsid,sg*,sha3sum,sqlite3,tailf,uuidgen,whereis,x86_64}
rm -f  $1/usr/lib/{libstdc++.so.6.0.18-gdb.py,*.so,libattr.so.1*}
rm -f  $1/usr/sbin/{biosdecode,blkdeactivate,dmeventd,e4defrag,fdformat,fsadm,kdump,ldattach,lvmconf,lvmdump,ownership,readprofile,rtcwake,smartd,tunelp,update-smart-drivedb,vgimportclone,vmcore-dmesg,vpddecode}
rm -rf $1/usr/share/{getopt,smartmontools,locale,bash-completion}
rm -rf $1/var/{pcmcia,lib/pcmcia,www}

ln -sf ../bin/rnutil $1/usr/sbin/chroot
mv $1/etc/mke2fs.conf.e2fsprogs-old $1/etc/mke2fs.conf
cp custom/skeleton/etc/mdev.conf output/target/etc/mdev.conf

[ -d "custom/memtest86+-4.00" ] || (check_out cvs memtest86+-4.00 || exit 1)
[ -d "custom/libreadynas" ] || (check_out svn libreadynas/$LRNBRANCH libreadynas || exit 1)
[ -d "custom/init" ] || (check_out svn RAIDiator-Platform/$BRANCH/boot/init || exit 1)
[ -d "custom/rnutil" ] || (check_out svn RAIDiator-Platform/$BRANCH/utils/rnutil || exit 1)
[ -d "custom/RAIDar" ] || (check_out svn RAIDiator-Platform/$BRANCH/packages/RAIDard-G2 RAIDar || exit 1)

export PATH="$(dirname $1)/host/usr/bin:$PATH"
export CC=${HOST}-gcc
export AR=${HOST}-ar
export LIBTOOL="${PWD}/output/build/popt-1.16/libtool"
export CFLAGS="-I${PWD}/custom -I${PWD}/custom/libreadynas/src -fno-stack-protector"
export LDFLAGS="-L${PWD}/custom/libreadynas/libreadynas_lgpl/.libs -L${PWD}/custom/libreadynas/src/.libs"
export LIBREADYNAS_LOC="${PWD}/custom/libreadynas/src"
export LIBREADYNAS_LGPL_LOC="${PWD}/custom/libreadynas/libreadynas_lgpl"
(cd custom/memtest86+-4.00 && make clean && cvs update && make G2=1 && cp -av memtest_shared $1/etc/memtest_image) || exit 1
(cd custom/libreadynas/libreadynas_lgpl && make clean && svn update && make LIBTOOL=$LIBTOOL CC=$CC all && cp -a .libs/libreadynas_lgpl.so.* $1/usr/lib) || exit 1
(cd custom/libreadynas/src && make clean && svn update && make LIBTOOL=$LIBTOOL CC=$CC all && cp -a .libs/libreadynas.so.* $1/usr/lib && ln -sfT . libreadynas) || exit 1
(cd custom/init && make clean && svn update && make && cp -av init $1) || exit 1
(cd custom/rnutil && make clean && svn update && make && cp -av rnutil $1/usr/bin) || exit 1
(cd custom/RAIDar && make distclean; svn update && ./configure --host=${HOST} && make && ${HOST}-strip raidard && cp -av raidard $1/usr/sbin) || exit 1
(cd custom/nbnsd && make clean; make && cp -av nbnsd $1/usr/sbin) || exit 1
(cd custom/mdnsd && make clean; make && cp -av mdnsd $1/usr/sbin) || exit 1

# Copy in out-of-tree modules
rm -rf $1/lib/modules/*
[ -d "$MODULE_DIR" ] && (cd "$MODULE_DIR" && tar c */extra 2>/dev/null | tar x -C $1/lib/modules/)

exit 0
