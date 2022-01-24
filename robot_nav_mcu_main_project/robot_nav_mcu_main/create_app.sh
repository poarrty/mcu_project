#!/bin/bash

make iap

../bootloader/SECoreBin/Makefile_ide/postbuild.sh "." "./build/robot_nav_mcu_main.elf" "./build/robot_nav_mcu_main.bin" "1" "1"

