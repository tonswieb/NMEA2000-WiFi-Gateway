#ifndef _DEMO_H_
#define _DEMO_H_

#define UpdatePeriod 5000

#include <Arduino.h>
#include <N2kMessages.h>
#include "nmea/N2kToN183.h"

void SendN2KMessages(N2kToN183 *pN2kToN183);

#endif