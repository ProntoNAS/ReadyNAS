#! /bin/bash

rename_pkg()
{
  src=$1
  dest=$2
  for ext in preinst postinst prerm postrm doc-base; do
    if [ -f $src.$ext ]; then
      if [ -f $dest.ext ]; then
	echo already exists: $dest.$ext
      else
	echo "$src.$ext --> $dest.$ext"
	mv $src.$ext $dest.$ext
      fi
    fi
  done
}

v_new=3.3
v_old=3.2

for p in chill cpp gcc g++ g77 gpc gij gcj gobjc protoize; do
  rename_pkg $p-$v_old $p-$v_new
done

for p in cpp gcc g77 gnat; do
  rename_pkg $p-$v_old-doc $p-$v_new-doc
done

rename_pkg gcc-$v_old-base gcc-$v_new-base
rename_pkg gcc-$v_old-sparc64 gcc-$v_new-sparc64
