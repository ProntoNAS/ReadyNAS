
# strip/clean the code from potentially dangerous patented code
for codec in 'h26.*' mpeg2video mpeg4 'msmpeg4.*'; do
    F=libavcodec/allcodecs.c
    sed -i "/REGISTER_ENCODER.*\\<$codec\\>/d" $F
    sed -i "s/REGISTER_ENCDEC\\(.*\\<$codec\\>\\)/REGISTER_DECODER\\1/" $F
    F=libavcodec/*.c
    sed -i "/AVCodec *${codec}_encoder *=/,/^[[:space:]]*}/d" $F
done
