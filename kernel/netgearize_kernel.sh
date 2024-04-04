#!/bin/bash
#
# netgearize_kernel.sh - kernel patching script
#
#
# Accepted syntax in series file
#
#	alias=targetA|targetB|targetC...
#
#	if target
#	...
#	endif
#
#	if target
#	...
#	else
#	...
#	endif
#
#	# comments
#
# No nesting of if/else allowed.
#

# Default stuff.
echo "SYSTYPE is ${SYSTYPE:=x86_64}..."
echo

IFELSEENDIF=false
PATCHESDIR=../patches
SERIES="$PATCHESDIR/series"

if grep -qE '^[[:space:]]*if[[:space:]]' < $SERIES; then
	IFELSEENDIF=true
elif [ -n "$SYSTYPE" -a -s "${SERIES}.${SYSTYPE}" ]; then
  SERIES="${SERIES}.${SYSTYPE}"
fi

error_out() {
  echo "Command failed.  You need to fix it before proceeding."
  exit 1
}

if [ -n "$1" ]; then
  if [ "$1" = "-r" ]; then
    echo "Reversing patch"
    OPT="-R"
  elif [ "$1" = "-f" ]; then
    FORCE=1
  elif [ "$1" = "-g" ]; then
    GIT_AM=1
  fi
fi

IFS='
'
if [ "$OPT" = "-R" ]; then
  for line in $(tac $SERIES); do
    if ! [[ "$line" =~ ^# ]]; then
      echo "Applying $line..."
      patch -F0 -p1 $OPT < "$PATCHESDIR/$line" || error_out
      #patch -p1 $OPT < "$PATCHESDIR/$line" || error_out
    fi
  done
  exit
fi

INIF=false
INELSE=false
GO=true
FIRSTLINE=true

ALIASES=

systypeset() {
	local line=`echo "$1" | sed 's|[[:space:]]||g'`
	ALIASES="$line:$ALIASES"
}

systypematch() {
	[ "$1" == "$SYSTYPE" ] && return

	local line
	local al
	local systype
	local ifs="$IFS"
	IFS=':'
	for line in `echo "$ALIASES"`
	do
		al=`echo $line | awk -F= '{ print $1 }'`
		[ "$al" = "$1" ] || continue
		al=`echo $line | awk -F= '{ print $2 }'`
		for systype in `echo $al | sed 's/|/:/g'`
		do
			[ "$systype" = "$SYSTYPE" ] && return
		done
	done
	IFS="$ifs"
	false
}

for line in $(cat $SERIES); do
  if $FIRSTLINE && [[ "$line" =~ ^"# if-else-endif" ]]; then
	FIRSTLINE=false
  elif [ -z "$line" ]; then
	continue
  elif [[ "$line" == "break" ]]; then
	if ! $IFELSEENDIF || $GO; then
    		exit
	fi
  elif [[ "$line" == "pause" ]]; then
	if ! $IFELSEENDIF || $GO; then
    		echo -n "Hit return key to continue: " >&2
		read ans
	fi
  elif [[ "$line" =~ ^# ]]; then
	if ! $IFELSEENDIF || $GO; then
		if [[ "$line" =~ ^"# " ]]; then
			echo
			echo "$line"
		fi
	fi
  elif [[ "$line" =~ ^[[:space:]]*.*=.* ]]; then
	systypeset "$line"
  elif [[ "$line" =~ ^[[:space:]]*if ]]; then
	if ! $IFELSEENDIF; then
		continue
	fi
	if $INIF || $INELSE; then
		echo "ERROR: Nested 'if'"
		echo $line
		exit 1
	fi
	TOKEN=`echo "$line" | awk '{print $2$3}'`
	NEGATE=false
	while [[ "$TOKEN" =~ ^! ]]; do
		if $NEGATE; then
			NEGATE=false
		else
			NEGATE=true
		fi
		TOKEN=`echo "$TOKEN" | cut -c2-`
	done

	if [ -z "$TOKEN" ];then
		echo "ERROR: Null if condition"
		echo $line
		exit 1
	elif systypematch "$TOKEN" ; then
		GO=false
	else
		GO=true
	fi

	if [ "$NEGATE" == "$GO" ]; then
		GO=true
	else
		GO=false
	fi
	INIF=true
  elif [[ "$line" =~ ^[[:space:]]*else ]]; then
	if ! $IFELSEENDIF; then
		continue
	fi
	if ! $INIF || $INELSE; then
		echo "ERROR: Spurious 'else'"
		echo $line
		exit 1
	fi

	if $GO; then
		GO=false
	else
		GO=true
	fi
  elif [[ "$line" =~ ^[[:space:]]*endif ]]; then
	if ! $IFELSEENDIF; then
		continue
	fi
	if ! $INIF && ! $INELSE; then
		echo "ERROR: Spurious 'endif'"
		echo $line
		exit 1
	fi

	GO=true
	INIF=false
	INELSE=false
  else
	if ! $IFELSEENDIF || $GO; then
		if [ -n "$GIT_AM" ]; then
			git am "$PATCHESDIR/$line" || error_out
		else
			echo "Applying $line..."
			patch -F0 -p1 $OPT < "$PATCHESDIR/$line" || error_out
			#patch -p1 $OPT < "$PATCHESDIR/$line" || error_out
		fi
	fi
  fi
done

# If this is a dry run, exit here
if [ -n "$1" -a "$1" = "-t" ]; then
  exit
fi

# Run make oldconfig for both kernels, to pull in any new settings
echo -e "\n# Checking for new config options..."

if [ -n "$SYSTYPE" -a -s ../defconfig.$SYSTYPE ]; then
  DEFCONFIG="../defconfig.$SYSTYPE"
elif [ -n "$ARCH" -a -s ../defconfig.$ARCH ]; then
  DEFCONFIG="../defconfig.$ARCH"
else
  DEFCONFIG="../defconfig.x86_64"
fi
cp $DEFCONFIG .config
arch=$(awk -F'[/ ]' '/# Linux/ { print $3 }' .config)
make ARCH=$arch oldconfig
if ! diff -q .config $DEFCONFIG >/dev/null; then
    cp .config $DEFCONFIG
    if [ -z "$FORCE" ]; then
      echo -e "\nPlease check in the new $DEFCONFIG!\nPress Enter to continue\n"
      read -n1
    fi
fi

if [[ "$arch" =~ "86" ]]; then
    cp .config arch/x86/configs/defconfig.$arch
elif [ -n "$SYSTYPE" ]; then
    cp .config arch/$arch/configs/defconfig.$SYSTYPE
else
    cp .config arch/$arch/configs/defconfig
fi
