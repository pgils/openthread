#!/usr/bin/env bash
#
# A toolchain can be specified by setting USE_TOOLCHAIN

MAKEOPT="-f examples/Makefile-nrf52840 USB=1 BOOTLOADER=1"
FTDFILE="output/nrf52840/bin/ot-shirt-ftd"

function usage() {
    echo
    echo "  usage: $0 [-cts]"
    echo "    c: clean before build."
    echo "    t: search for toolchain in parent directory."
    echo "    s: use ccache."
}

function find_toolchain() {
    # check if an override has been provided
    if [[ -n "$USE_TOOLCHAIN" ]]; then
        return 0
    fi

    TOOLCHAIN_PATH="$(find ../ -maxdepth 1 -type d -name "*gcc*arm*"|sort|head -1)/bin"
    if [[ -x "$TOOLCHAIN_PATH"/arm-none-eabi-gcc ]]; then
        GCC_VER=$("$TOOLCHAIN_PATH"/arm-none-eabi-gcc -dumpversion)
        GCC_MAJOR=$(echo "$GCC_VER" | sed -E "s/^([[:digit:]]+)\..*/\1/")

        # check for GCC 7 or 8
        if [[ "$GCC_MAJOR" -lt 9 && "$GCC_MAJOR" -ge 7 ]]; then
            export PATH="$(realpath $TOOLCHAIN_PATH):$PATH"
        else
            echo "Not a valid GCC version for $TOOLCHAIN_PATH: $GCC_MAJOR"
            return 1
        fi
    else
        echo "No toolchain found."
        return 1
    fi

    return 0
}

while getopts ":tcs" opt; do
    case $opt in
        t)
        find_toolchain
        if [[ 0 != $? ]]; then
            # no toolchain found
            exit 1
        fi
        ;;
        c)
        MAKE_CLEAN="true"
        ;;
        s)
        MAKEOPT="$MAKEOPT CCPREFIX=ccache"
        ;;
        \?)
        usage
        exit 1
        ;;
    esac
done

if [[ -n "$USE_TOOLCHAIN" ]]; then
    export PATH="$(realpath $USE_TOOLCHAIN/bin):$PATH"
fi

if [[ -n "$MAKE_CLEAN" ]]; then
    make $MAKEOPT clean
    if [[ 0 -ne $? ]]; then
        # clean failed.
        exit
    fi
fi

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
