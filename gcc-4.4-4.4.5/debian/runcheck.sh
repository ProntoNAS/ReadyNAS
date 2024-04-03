#! /bin/sh

mkdir -p build

cat >build/runcheck.c <<EOF
#include <stdio.h>
int main()
{
	return printf("yes\n") != 4;
}
EOF

if m=$(${CC:-gcc} -o build/runcheck build/runcheck.c 2>&1); then
    m=$(build/runcheck 2>&1)
    echo ${m#* } > build/runcheck.out
    echo ${m#* }
else
    echo ${m##*:} > build/runcheck.out
    echo ${m##*:}
fi
