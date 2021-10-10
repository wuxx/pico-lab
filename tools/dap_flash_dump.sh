#!/bin/bash

#OPENOCD_ROOT=/home/pi/oss/pico/openocd

if [ ${#OPENOCD_ROOT} -ne 0 ]; then
    OPENOCD_BIN=${OPENOCD_ROOT}/src/openocd
else
    OPENOCD_BIN=openocd
fi


FLASH_ADDR=0x10000000

FLASH_SIZE=10240

sudo ${OPENOCD_BIN} -s ${OPENOCD_ROOT}/tcl -f ${OPENOCD_ROOT}/tcl/interface/cmsis-dap.cfg -f ${OPENOCD_ROOT}/tcl/target/rp2040.cfg -c \
"
    init;
    reset halt;
    dump_image flash_image_dump.bin ${FLASH_ADDR} ${FLASH_SIZE}
    shutdown;
"
