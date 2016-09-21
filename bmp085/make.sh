#!/bin/bash

echo create shared library libbmp085.so
gcc -Wall -shared -o bin/libbmp085.so  src/bmp085.c -lm -lwiringPi

echo create test programs
gcc -Wall  -o bin/temperature  src/temperature.c -lwiringPi -lbmp085 -L./bin
gcc -Wall  -o bin/pressure  src/pressure.c -lwiringPi -lbmp085 -L./bin
gcc -Wall  -o bin/lazy  src/lazy.c -lwiringPi -lbmp085 -L./bin
gcc -Wall  -o bin/main  src/main.c -lwiringPi -lbmp085 -L./bin

export LD_LIBRARY_PATH=.:./bin:$LD_LIBRARY_PATH

echo execute test programs
echo temperature in degree celcius
./bin/temperature
echo
echo pressure at sea level in hecto Pascal resp. milli bar
./bin/pressure
echo
echo and the lazy output
./bin/lazy
echo
