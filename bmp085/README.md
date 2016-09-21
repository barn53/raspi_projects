bmp085
======

####Library to utilize the BOSCH temperature and barometric pressure sensors BMP085 / BMP180 with Raspberry Pi.
This library depends on wiringPi: http://wiringpi.com/

The sensor communicates via I²C.

The sensor has been discontinued by the manufacturer and replaced with the new model BMP180, which has the same interface and specifications.

The sensor is available on breakout boards from e.g. adafruit: 
http://www.adafruit.com/products/391 resp.
http://www.adafruit.com/products/1603

To build the library execute:
```
./make.sh
```

As a result there will be the library `libbmp085.so` and further executables in the `bin/` directory.
The files `src/lazy.c`, `src/temperature.c` and `src/pressure.c` demonstrate the usage of the library.

Running `./make.sh` with a connected BMP085 outputs the following:
```
create shared library libbmp085.so
create test programs
execute test programs
temperature in degree celcius
21.0
pressure at sea level in hecto Pascal resp. milli bar
1012.55
and the lazy output
21.0 °C
1012.62 hPa
```

When running `./make.sh` and you do not have a BMP085 connected to your Pi, you will see a consle output like this:
```
create shared library libbmp085.so
create test programs
execute test programs
temperature in degree celcius
Error:
  Invalid device ID 0xffffffff (must be 0x55)

pressure at sea level in hecto Pascal resp. milli bar
Error:
  Invalid device ID 0xffffffff (must be 0x55)

and the lazy output
Error:
  Invalid device ID 0xffffffff (must be 0x55)
-1.0 °C
Error:
  Invalid device ID 0xffffffff (must be 0x55)
-1.00 hPa
```

To use the library in your own project, just `#include bmp085.h` and link with the `libbmp085.so` library

```
gcc -Wall  -o myownprogram  myownprogram.c -lwiringPi -lbmp085 -L<path to libbmp085.so>
```

You have to make sure, your executable is able to load the library `libbmp085.so`.
The path to the library should be included in the `LD_LIBRARY_PATH` environment variable.

```
export LD_LIBRARY_PATH=<path to libbmp085.so>:$LD_LIBRARY_PATH
```

Have fun!
