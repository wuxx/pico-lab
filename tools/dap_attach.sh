#!/bin/bash

OPENOCD_ROOT=/home/pi/oss/pico/openocd

#OPENOCD_BIN=openocd
OPENOCD_BIN=${OPENOCD_ROOT}/src/openocd

#-d4 
sudo ${OPENOCD_BIN} -s ${OPENOCD_ROOT}/tcl -f ${OPENOCD_ROOT}/tcl/interface/cmsis-dap.cfg -f ${OPENOCD_ROOT}/tcl/target/rp2040.cfg
