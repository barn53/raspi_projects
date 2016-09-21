#ifndef BMP085_H
#define BMP085_H

#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define BMP085_I2C_DEVICE_ADDRESS 0x77
#define BMP085_I2C_REGISTER_DEVICE_ID 0xd0
#define BMP085_I2C_REGISTER_AC1  0xaa
#define BMP085_I2C_REGISTER_AC2  0xac
#define BMP085_I2C_REGISTER_AC3  0xae
#define BMP085_I2C_REGISTER_AC4  0xb0
#define BMP085_I2C_REGISTER_AC5  0xb2
#define BMP085_I2C_REGISTER_AC6  0xb4
#define BMP085_I2C_REGISTER_B1   0xb6
#define BMP085_I2C_REGISTER_B2   0xb8
#define BMP085_I2C_REGISTER_MB   0xba
#define BMP085_I2C_REGISTER_MC   0xbc
#define BMP085_I2C_REGISTER_MD   0xbe

#define BMP085_I2C_REGISTER_MEASURE_CONTROL   0xF4

#define BMP085_I2C_REGISTER_MSB   0xF6
#define BMP085_I2C_REGISTER_LSB   0xF7
#define BMP085_I2C_REGISTER_XLSB  0xF8

#define BMP085_I2C_REGISTER_SOFT_RESET   0xE0

#define BMP085_MEASURE_CONTROL_TEMPERATURE 0x2E
#define BMP085_MEASURE_CONTROL_PRESSURE 0x34

#define ERROR_LENGTH 255

typedef enum
{
    UltraLowPower = 0,
    Standard = 1,
    HighResolution = 2,
    UltraHighResolution = 3
} PressureAccuracyMode;

typedef struct
{
    // calibration coefficients
    short AC1;
    short AC2;
    short AC3;
    unsigned short AC4;
    unsigned short AC5;
    unsigned short AC6;
    short B1;
    short B2;
    short MB;
    short MC;
    short MD;

    int B5; // this is from temperature measurement

    int altitude;

    PressureAccuracyMode accuracyMode;

    // file descriptor for interface
    int fd;

    // error message
    char error[ERROR_LENGTH + 1];
} BMP085Handle;

// delay in ms depending on pressure accuracy mode
static const short delayPressure[] = {5, 8, 14, 26};
static const short delayTemperature = 5;

int bmp085Init(BMP085Handle* handle, int altitude);
void setPressureAccuracyMode(BMP085Handle* handle, PressureAccuracyMode mode);

float readTrueTemperature(BMP085Handle* handle);
float readTruePressure(BMP085Handle* handle);
float readPressureSeaLevel(BMP085Handle* handle);

// lazy methods
float temperature();
float pressure();
float pressureSeaLevel(int altitude);

#endif // BMP085_H
