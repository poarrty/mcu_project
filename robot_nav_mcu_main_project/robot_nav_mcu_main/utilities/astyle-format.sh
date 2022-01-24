#!/bin/sh

cd `dirname $0`/../

astyle --style=ansi --add-braces -s4 --convert-tabs --indent-preprocessor --pad-header --break-blocks -nTpoOwSK --recursive --exclude=./hal/core ./*.c,*.C,*.H,*.h
