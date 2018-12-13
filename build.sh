#!/usr/bin/env bash

MAKEOPT="-f examples/Makefile-nrf52840 USB=1 BOOTLOADER=1"
FTDFILE="output/nrf52840/bin/ot-cli-ftd"

function usage() {
    echo
    echo "  usage: $0 [-c]"
    echo "    c: clean before build."
}

while getopts ":c" opt; do
    case $opt in
        c)
        make $MAKEOPT clean
        if [[ 0 -ne $? ]]; then
            # clean failed.
            exit
        fi
        ;;
        \?)
        usage
        exit 1
        ;;
    esac
done

make $MAKEOPT

if [[ 0 -ne $? ]]; then
    # build failed.
    exit
fi

echo
echo -n "Generating HEX file..."

if [[ -r "$FTDFILE" ]]; then
    arm-none-eabi-objcopy -O ihex "$FTDFILE" "$FTDFILE.hex"
fi

echo "Done"
echo "hex: $FTDFILE.hex"
