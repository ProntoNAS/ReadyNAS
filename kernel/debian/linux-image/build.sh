#!/bin/bash
error() {
  echo "ERROR: $1"
  exit 1
}

# Clean up
rm -f *.deb &>/dev/null
[ -e data ] && rm -rf data
[ -f data.tar.gz ] && rm -f data.tar.gz
[ -f control.tar.gz ] && rm -f control.tar.gz

[ -n "$1" ] || error "No architecture specified!"
[ -n "$2" ] || error "No version specified!"

# Set up skeleton
echo 2.0 > debian-binary
mkdir control data

sed -e "s/%ARCH%/$1/" -e "s/%VERSION%/$(echo $2 | tr 'A-Z' 'a-z' | tr _ -)/" control.in > control/control || error "Creating control failed"

(cd control && tar cf ../control.tar .) || error "Creating control failed"
gzip -9 control.tar || error "Compressing control failed"

if [ -d files ]; then
  (cd files; find | grep -v '\.svn' | cpio -pdum ../data 2>/dev/null) || error "Creating data failed"
elif [ -f list ]; then
  (cat list | cpio -pdum data 2>/dev/null) || error "Creating data failed"
fi
(cd data && tar cf ../data.tar .) || error "Creating data failed"
gzip -9 data.tar || error "Compressing data failed"

arch=$1
name=$(awk '/^Package:/ { print $2 }' control/control)
vers=$(awk '/^Version:/ { print $2 }' control/control)
ar cr ${name}_${vers}_${arch}.deb debian-binary control.tar.gz data.tar.gz

rm -rf debian-binary data.tar.gz data control.tar.gz control

echo "Successfully built package $name in '${name}_${vers}_${arch}.deb'"
