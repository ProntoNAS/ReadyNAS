#
# debian/strip.sh forcefully removes certain encoders which are expected
# to be available in some parts of the code. They aren't used if they
# are disabled properly, but certain macros do need to be defined.
#
# This script fixes up the build by adding adding required #defines to
# config.h

EXPECTED_CODECS="H261 H263 H263P MSMPEG4V1 MSMPEG4V2 MSMPEG4V3 MPEG4 MPEG2VIDEO"
echo "#ifndef FIXUP_CONFIG_"
echo "#define FIXUP_CONFIG_"
for codec in $EXPECTED_CODECS; do
    echo "#define ENABLE_${codec}_ENCODER 0"
done
echo "#endif"
