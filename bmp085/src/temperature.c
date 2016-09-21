#include "bmp085.h"


int main()
{
    BMP085Handle handle;

    if (0 != bmp085Init(&handle, 0))
    {
        printf("Error:\n  %s\n", handle.error);
        return -1;
    }

    printf("%.1f", readTrueTemperature(&handle));

    return 0;
}
