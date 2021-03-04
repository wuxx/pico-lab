#!/bin/bash

OPENOCD_ROOT=/home/pi/oss/pico/openocd

OPENOCD_BIN=${OPENOCD_ROOT}/src/openocd


sudo ${OPENOCD_BIN} -s ${OPENOCD_ROOT}/tcl \
         -f ${OPENOCD_ROOT}/tcl/interface/raspberrypi-swd.cfg \
         -f ${OPENOCD_ROOT}/tcl/target/rp2040.cfg -c \
"
    init;
    reset;
    shutdown;
"
