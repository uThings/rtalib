#!/bin/sh
gcc -o$1 $1.c rtalib.c wav.c libportaudio.so -lm


