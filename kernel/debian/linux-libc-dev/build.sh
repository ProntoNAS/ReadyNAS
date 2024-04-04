#!/bin/bash
error() {
  echo "ERROR: $1"
  exit 1
}

# Clean up
rm -f *.deb &>/dev/null
[ -e data ] && rm -rf data
[ -f data.tar.xz ] && rm -f data.tar.xz
[ -f control.tar.gz ] && rm -f control.tar.gz

[ -n "$1" ] || error "No architecture specified!"
[ -n "$2" ] || error "No version specified!"

# Set up skeleton
echo 2.0 > debian-binary
mkdir control data

if [ "$1" == "amd64" ]; then
  vers=${2/x86_64.}
  kvers=$(echo $2 | tr 'A-Z' 'a-z' | tr _ -)
elif [ "$1" == "armel" ]; then
  if [[ "$2" == *"armada"* ]]; then
    vers=${2/armada.}
    kvers="$2 or ${2/armada/alpine}"
  elif [[ "$2" == *"alpine"* ]]; then
    vers=${2/alpine.}
    kvers="$2 or ${2/alpine/armada}"
  else
    error "Wrong version"
  fi
else
  error "Wrong arch/version"
fi
sed -e "s/%ARCH%/$1/" -e "s/%VERSION%/$vers/" -e "s/%KVERSION%/$kvers/"  control.in > control/control || error "Creating control failed"

(cd control && tar cf ../control.tar .) || error "Creating control failed"
gzip -9 control.tar || error "Compressing control failed"

if [ -d files ]; then
  if [ "$1" == "amd64" ]; then
    (mkdir files/usr/include/x86_64-linux-gnu && mv files/usr/include/asm files/usr/include/x86_64-linux-gnu) || error "Moving asm headers failed"
  fi
  if [ "$1" == "armel" ]; then
    (mkdir files/usr/include/arm-linux-gnueabi && mv files/usr/include/asm files/usr/include/arm-linux-gnueabi) || error "Moving asm headers failed"
  fi
  rm files/usr/include/linux/..install.cmd || error "Removing ..install.cmd failed"
  (cd files; find | grep -v '\.svn' | cpio -pdum ../data 2>/dev/null) || error "Creating data failed"
else
  cat list | cpio -pdum data 2>/dev/null || error "Creating data failed"
fi
(cd data && tar cf ../data.tar .) || error "Creating data failed"
xz -9 data.tar || error "Compressing data failed"

arch=$1
name=$(awk '/^Package:/ { print $2 }' control/control)
vers=$(awk '/^Version:/ { print $2 }' control/control)
ar cr ${name}_${vers}_${arch}.deb debian-binary control.tar.gz data.tar.xz

rm -rf debian-binary data.tar.xz data control.tar.gz control

echo "Successfully built package $name in '${name}_${vers}_${arch}.deb'"
