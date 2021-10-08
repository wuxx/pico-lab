#!/bin/bash

#CURRENT_DIR=$(cd $(dirname $0); pwd)

CURRENT_DIR=$(pwd)

export OPENOCD_ROOT=/home/pi/oss/pico/openocd

export PATH=${PATH}:${CURRENT_DIR}
