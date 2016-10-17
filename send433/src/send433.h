#ifndef WIRINGTEST_H
#define WIRINGTEST_H

// see: https://www.sweetpi.de/blog/329/ein-ueberblick-ueber-433mhz-funksteckdosen-und-deren-protokolle
//      https://www.pilight.org/

#ifdef __cplusplus
extern "C"
{
#endif

void sendElro(unsigned int id, unsigned int unit, bool on, unsigned int repeat = 10);
void sendInterTechno(unsigned int id, unsigned int unit, bool on, unsigned int repeat = 10);
void setup(unsigned int gpioPin);

#ifdef __cplusplus
}
#endif

#endif // WIRINGTEST_H

