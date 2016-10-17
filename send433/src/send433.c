#include "wiringPi.h"
#include "send433.h"

#define ELRO_SHORT 275
#define ELRO_LONG  1025
#define ELRO_SYNC  3000

#define INTERTECHNO_SHORT 275
#define INTERTECHNO_LONG 1200
#define INTERTECHNO_SYNC 2650

static unsigned int GPIO_PIN = 0;

void bit0Elro()
{
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(ELRO_SHORT);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(ELRO_LONG);
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(ELRO_SHORT);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(ELRO_LONG);
}

void bitFElro()
{
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(ELRO_SHORT);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(ELRO_LONG);
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(ELRO_LONG);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(ELRO_SHORT);
}

void syncElro()
{
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(ELRO_SHORT);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(ELRO_SYNC);
}

void bit0interTechno()
{
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(INTERTECHNO_SHORT);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(INTERTECHNO_SHORT);
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(INTERTECHNO_SHORT);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(INTERTECHNO_LONG);
}

void bit1interTechno()
{
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(INTERTECHNO_SHORT);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(INTERTECHNO_LONG);
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(INTERTECHNO_SHORT);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(INTERTECHNO_SHORT);
}

void syncinterTechno()
{
        digitalWrite(GPIO_PIN, 1) ;
        delayMicroseconds(INTERTECHNO_SHORT);
        digitalWrite(GPIO_PIN, 0) ;
        delayMicroseconds(INTERTECHNO_SYNC);
}

bool isBitSet(unsigned int number, unsigned short bit)
{
    unsigned int mask(1);
    mask = mask << bit;
    return number & mask;
}

void sendElro(unsigned int id, unsigned int unit, bool on, unsigned int repeat)
{
    for (unsigned int rr = 0; rr < repeat; ++rr) {

        for (unsigned int ii = 0; ii < 5; ++ii) {
            if (isBitSet(id, ii)) {
                bit0Elro();
            }
            else {
                bitFElro();
            }
        }

        for (unsigned int ii = 0; ii < 5; ++ii) {
            if (isBitSet(unit, ii)) {
                bit0Elro();
            }
            else {
                bitFElro();
            }
        }

        if (on) {
            bitFElro();
            bitFElro();
        }
        else {
            bitFElro();
            bit0Elro();
        }

        syncElro();
    }
}

void sendInterTechno(unsigned int id, unsigned int unit, bool on, unsigned int repeat)
{
    for (unsigned int rr = 0; rr < repeat; ++rr) {

        syncinterTechno();

        for (unsigned int ii = 26; ii > 0; --ii) {
            if (isBitSet(id, ii - 1)) {
                bit1interTechno();
            }
            else {
                bit0interTechno();
            }
        }

        bit0interTechno(); // Alle 1 / einzeln 0

        if (on) {
            bit1interTechno();
        }
        else {
            bit0interTechno();
        }

        for (unsigned int ii = 4; ii > 0; --ii) {
            if (isBitSet(unit, ii - 1)) {
                bit1interTechno();
            }
            else {
                bit0interTechno();
            }
        }

        syncinterTechno();
        delayMicroseconds(7000);
    }
}

void setup(unsigned int gpioPin)
{
    wiringPiSetup();
    GPIO_PIN = gpioPin;
    pinMode (GPIO_PIN, OUTPUT) ;
}

