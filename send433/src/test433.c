#include "wiringPi.h"
#include "send433.h"

#include <stdio.h>
#include <iostream>
#include <iomanip>

using namespace std;

int main()
{
    setup(0);

    while (true) {

        sendInterTechno(13928962, 2, false);
        sendElro(31, 1, true);
        delayMicroseconds(100000);

        sendInterTechno(13928962, 2, true);
        sendElro(31, 1, false);
        delayMicroseconds(100000);
    }

    return 0;
}

