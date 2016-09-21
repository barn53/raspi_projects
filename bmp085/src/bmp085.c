#include "bmp085.h"
#include <math.h>

short swapBytes(int value)
{
    short left = (value & 0xff00) >> 8;
    return ((value & 0xff) << 8) | left;
}

void printHandle(BMP085Handle* handle)
{
    printf("calibration coefficients\n");
    printf("  AC1: %hd - 0x%hx\n", handle->AC1, handle->AC1);
    printf("  AC2: %hd - 0x%hx\n", handle->AC2, handle->AC2);
    printf("  AC3: %hd - 0x%hx\n", handle->AC3, handle->AC3);
    printf("  AC4: %hu - 0x%hx\n", handle->AC4, handle->AC4);
    printf("  AC5: %hu - 0x%hx\n", handle->AC5, handle->AC5);
    printf("  AC6: %hu - 0x%hx\n", handle->AC6, handle->AC6);
    printf("  B1:  %hd - 0x%hx\n", handle->B1, handle->B1);
    printf("  B2:  %hd - 0x%hx\n", handle->B2, handle->B2);
    printf("  MB:  %hd - 0x%hx\n", handle->MB, handle->MB);
    printf("  MC:  %hd - 0x%hx\n", handle->MC, handle->MC);
    printf("  MD:  %hd - 0x%hx\n", handle->MD, handle->MD);
    printf("  B5:  %hd - 0x%hx\n", handle->B5, handle->B5);
    printf("accuracyMode: %hd\n", handle->accuracyMode);
}

int bmp085Init(BMP085Handle* handle, int altitude)
{
    handle->error[0] = 0;
    handle->fd = wiringPiI2CSetup(BMP085_I2C_DEVICE_ADDRESS);

    // device id must be 0x55
    short deviceId = wiringPiI2CReadReg8(handle->fd, BMP085_I2C_REGISTER_DEVICE_ID);
    if (deviceId != 0x55)
    {
        snprintf(handle->error, ERROR_LENGTH, "Invalid device ID 0x%x (must be 0x55)", deviceId);
        return -1;
    }

    handle->AC1 = swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_AC1));
    handle->AC2 = swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_AC2));
    handle->AC3 = swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_AC3));
    handle->AC4 = (unsigned short) swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_AC4));
    handle->AC5 = (unsigned short) swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_AC5));
    handle->AC6 = (unsigned short) swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_AC6));
    handle->B1 = swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_B1));
    handle->B2 = swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_B2));
    handle->MB = swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_MB));
    handle->MC = swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_MC));
    handle->MD = swapBytes(wiringPiI2CReadReg16(handle->fd, BMP085_I2C_REGISTER_MD));

    handle->B5 = 0;

    handle->altitude = altitude;

    handle->accuracyMode = Standard;

    return 0;
}

void setPressureAccuracyMode(BMP085Handle* handle, PressureAccuracyMode mode)
{
    handle->accuracyMode = mode;
}

int readUncompensatedTemperature(BMP085Handle* handle)
{
    wiringPiI2CWriteReg8(handle->fd, BMP085_I2C_REGISTER_MEASURE_CONTROL, BMP085_MEASURE_CONTROL_TEMPERATURE);
    delay(delayTemperature);
    int msb = wiringPiI2CReadReg8(handle->fd, BMP085_I2C_REGISTER_MSB);
    int lsb = wiringPiI2CReadReg8(handle->fd, BMP085_I2C_REGISTER_LSB);
    return ((msb << 8) | lsb);
}

int readUncompensatedPressure(BMP085Handle* handle)
{
    short control = (handle->accuracyMode << 6) | BMP085_MEASURE_CONTROL_PRESSURE;
    wiringPiI2CWriteReg8(handle->fd, BMP085_I2C_REGISTER_MEASURE_CONTROL, control);
    delay(delayPressure[handle->accuracyMode]);
    int msb = wiringPiI2CReadReg8(handle->fd, BMP085_I2C_REGISTER_MSB);
    int lsb = wiringPiI2CReadReg8(handle->fd, BMP085_I2C_REGISTER_LSB);
    int xlsb = wiringPiI2CReadReg8(handle->fd, BMP085_I2C_REGISTER_XLSB);
    return ((((msb << 16) | (lsb << 8)) | xlsb) >> (8 - handle->accuracyMode));
}

float calculateTrueTemperature(BMP085Handle* handle, int uncompensated)
{
    int x1 = (uncompensated - handle->AC6) * handle->AC5 >> 15;
    int x2 = (handle->MC << 11) / (x1 + handle->MD);
    int b5 = x1 + x2;
    handle->B5 = b5;
    return (float) ((b5 + 8) >> 4) / 10;
}

float calculateTruePressure(BMP085Handle* handle, int uncompensated)
{
    int b6 = handle->B5 - 4000;
    int x1 = (handle->B2 * (b6 * b6 >> 12)) >> 11;
    int x2 = handle->AC2 * b6 >> 11;
    int x3 = x1 + x2;
    int b3 = (((handle->AC1 * 4 + x3) << handle->accuracyMode) + 2) / 4;
    x1 = handle->AC3 * b6 >> 13;
    x2 = (handle->B1 * (b6 * b6 >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    unsigned int b4 = handle->AC4 * (unsigned int) (x3 + (1 << 15)) >> 15;
    unsigned int b7 = ((unsigned int) uncompensated - b3) * (50000 >> handle->accuracyMode);
    int p = (b7 / b4) * 2;
    if (b7 < 0x80000000)
    {
        p = (b7 * 2) / b4;
    }
    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p = p + ((x1 + x2 + 3791) >> 4);
    return (float) p / 100;
}

float absoluteToSeaLevelPressure(float pressure, int altitude)
{
    return ((double) pressure / pow(1 - (altitude / (double) 44330), 5.255));
}

float readTrueTemperature(BMP085Handle* handle)
{
    return calculateTrueTemperature(handle, readUncompensatedTemperature(handle));
}

float readTruePressure(BMP085Handle* handle)
{
    if (handle->B5 == 0)
    {// we need the temperature to compensate the measure
        readTrueTemperature(handle);
    }
    return calculateTruePressure(handle, readUncompensatedPressure(handle));
}

float readPressureSeaLevel(BMP085Handle* handle)
{
    return absoluteToSeaLevelPressure(readTruePressure(handle), handle->altitude);
}

float temperature()
{
    BMP085Handle handle;

    if (0 != bmp085Init(&handle, 0))
    {
        printf("Error:\n  %s\n", handle.error);
        return -1.;
    }
    return readTrueTemperature(&handle);
}

float pressure()
{
    BMP085Handle handle;

    if (0 != bmp085Init(&handle, 0))
    {
        printf("Error:\n  %s\n", handle.error);
        return -1.;
    }
    return readTruePressure(&handle);
}

float pressureSeaLevel(int altitude)
{
    BMP085Handle handle;

    if (0 != bmp085Init(&handle, altitude))
    {
        printf("Error:\n  %s\n", handle.error);
        return -1.;
    }
    return absoluteToSeaLevelPressure(readTruePressure(&handle), altitude);
}
