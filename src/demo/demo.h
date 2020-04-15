#define UpdatePeriod 5000

#ifndef _DEMO_H_
#define _DEMO_H_

#include <Arduino.h>
#include <N2kMessages.h>
#include "N2kDataToNMEA0183.h"

void SendN2KMessages(tN2kDataToNMEA0183 *nk2To0183);

#endif