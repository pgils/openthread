#!/usr/bin/env bash

MAKEOPT="-f examples/Makefile-nrf52840 USB=1 BOOTLOADER=1"
FTDFILE="output/nrf52840/bin/ot-shirt-ftd"

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

time make $MAKEOPT

if [[ 0 -ne $? ]]; then
    # build failed.
    exit
fi

echo
echo -n "Generating HEX file..."

if [[ -r "$FTDFILE" ]]; then
    arm-none-eabi-objcopy -O ihex "$FTDFILE" "$FTDFILE.hex"
    if [[ 0 -eq $? ]]; then
        echo "Done"
        echo "hex: $FTDFILE.hex"
    fi

    # Create a flashable zip if nrfutil is available
    type nrfutil >/dev/null 2>&1
    if [[ 0 -eq $? ]]; then
        echo -n "Creating flashable zip..."
        nrfutil pkg generate --hw-version 52 --debug-mode --sd-req 0x00 \
        --application $FTDFILE.hex $FTDFILE.zip
        if [[ 0 -eq $? ]]; then
            echo "Done"
        fi
    fi
fi
