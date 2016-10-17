#!/bin/bash

g++ -Wall -shared -o ./bin/libsend433.so  ./src/send433.c -lm -lwiringPi
g++ -Wall -o ./bin/test433 ./src/test433.c -L./bin -lsend433 -lwiringPi

export LD_LIBRARY_PATH=./bin:$LD_LIBRARY_PATH

