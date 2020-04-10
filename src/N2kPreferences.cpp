#include "N2KPreferences.h"

N2KPreferences::N2KPreferences() {
    // prefs.begin("N2k-bridge");
    // blEnabled = prefs.getBool("blEnabled",true);
    // demoEnabled = prefs.getBool("demoEnabled", false);
}

N2KPreferences::~N2KPreferences() {
    prefs.end();
}

void N2KPreferences::setBlEnabled(bool value) {
    // prefs.putBool("blEnabled",value);
    this->blEnabled = value;
}

void N2KPreferences::setDemoEnabled(bool value) {
    // prefs.putBool("demoEnabled",value);
    this->demoEnabled = value;
 }

bool N2KPreferences::isBlEnabled() {
    return blEnabled;
}

bool N2KPreferences::isDemoEnabled() {
    return demoEnabled;
}