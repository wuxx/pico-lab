#!/bin/bash

if [ ${#1} -eq 0 ]; then
    echo "usage: $0 image-file"
    exit 0
fi

CURRENT_DIR=$(cd $(dirname $0); pwd)

#xxx.hex or xxx.bin
IMAGE_FILE=$1

if [ ${#OPENOCD_ROOT} -ne 0 ]; then
    OPENOCD_BIN=${OPENOCD_ROOT}/src/openocd
else
    OPENOCD_BIN=openocd
fi

FLASH_ADDR=0x10000000
FLASH_SIZE=0x00200000


EXT="${IMAGE_FILE##*.}"
echo EXT: $EXT

#flash write_image erase xxx.hex;
#flash write_image erase xxx.bin 0x08000000;

if [ "${EXT}" == "hex" ]; then
    TARGET="$IMAGE_FILE"
elif [ "${EXT}" == "bin" ]; then
    TARGET="$IMAGE_FILE $FLASH_ADDR"
else
    echo "illegal suffix [$EXT]"
    exit 1
fi

sudo ${OPENOCD_BIN} -s ${OPENOCD_ROOT}/tcl -f ${OPENOCD_ROOT}/tcl/interface/cmsis-dap.cfg -f ${OPENOCD_ROOT}/tcl/target/rp2040.cfg -c \
"   init;
    reset halt;
    targets rp2040.core0;
    flash write_image erase ${TARGET};
    reset;
    shutdown;
"

