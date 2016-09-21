#include "bmp085.h"


int main()
{
    BMP085Handle handle;

    if (0 != bmp085Init(&handle, 285))
    {
        printf("Error:\n  %s\n", handle.error);
        return -1;
    }

    printf("%.1f\n", readTrueTemperature(&handle));
    printf("%.2f\n", readPressureSeaLevel(&handle));

    return 0;
}
