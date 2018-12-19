#!/usr/bin/env bash

SERIAL_PORT=/dev/ttyACM0

function usage() {
    echo
    echo "  usage: $0 <zip-file> [<serial port>]"
    echo
    echo "  zip-file            : DFU zip package"
    echo "  serial port(opt)    : default: /dev/ttyACM0"
    exit
}

# check for argument
if [[ -z $1 ]]; then
    echo
    echo "Missing argument."
    usage
fi

if [[ -n $2 ]]; then
    SERIAL_PORT=$2
fi

# check if the zipfile exists and is readable
if [[ ! -r $1 ]]; then
    echo "File: $1 does not exist or is not readable."
    exit 1
fi

# check for nrfutil
type nrfutil >/dev/null 2>&1
if [[ 0 -ne $? ]]; then
    echo "nrfutil not found. exiting.."
    exit 1
fi

# check if the serial port 'exists' and is readable
if [[ ! -w $SERIAL_PORT ]]; then
    echo "Serial port: $SERIAL_PORT not found or not writable."
    exit 1
fi

nrfutil dfu usb-serial -pkg $1 -p $SERIAL_PORT -b 115200
