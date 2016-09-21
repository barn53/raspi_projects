#include "bmp085.h"

int main()
{
    printf("%.1f °C\n", temperature());
    printf("%.2f hPa\n", pressureSeaLevel(285));

    return 0;
}
