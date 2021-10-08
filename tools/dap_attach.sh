#!/bin/bash

#OPENOCD_ROOT=/home/pi/oss/pico/openocd

#OPENOCD_BIN=openocd
if [ ${#OPENOCD_ROOT} -ne 0 ]; then
    OPENOCD_BIN=${OPENOCD_ROOT}/src/openocd
else
    OPENOCD_BIN=openocd
fi


#-d4 
sudo ${OPENOCD_BIN} -s ${OPENOCD_ROOT}/tcl -f ${OPENOCD_ROOT}/tcl/interface/cmsis-dap.cfg -f ${OPENOCD_ROOT}/tcl/target/rp2040.cfg
