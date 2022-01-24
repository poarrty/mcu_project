#!/bin/bash

JLinkExe -device STM32F103RD  -si SWD -speed 4000 -CommanderScript ./flash.jlink
