FILE=$1

if [ "$FILE" = "" -o ! -f "$FILE" ]; then
        echo "Please specify a .nw file"
        exit 1
fi

noroots $FILE | sed 's/^<''<\(.*\)>>$/\1/' |
        while read chunk; do
                case $chunk in
                        *.pl|*.sh)
                                echo -e "$chunk : $FILE"
                                echo -e "\tnotangle -R\$@ $< >\$@"
                                echo -e "\tchmod 755 $chunk"
                                ;;
                        *.c)
                                echo -e "$chunk : $FILE"
                                echo -e "\tnotangle -L -R\$@ $< | cpif \$@"
                                echo -e "include ${chunk%.c}.d"
                                ;;
                        *.h)
                                echo -e "$chunk : $FILE"
                                echo -e "\tnotangle -L -R\$@ $< | cpif \$@"
                                ;;
                        *)
                                echo -e "$chunk : $FILE"
                                echo -e "\tnotangle -t8 -R\$@ $< >\$@"
                                ;;
                esac
        done
