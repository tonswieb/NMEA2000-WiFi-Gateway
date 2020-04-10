#ifndef _N2KPREFERENCES_H_
#define _N2KPREFERENCES_H_

#include <Preferences.h>
#include <nvs.h>
#include <nvs_flash.h>

class N2KPreferences {

private:
  Preferences prefs;
  volatile bool blEnabled;
  volatile bool demoEnabled;

public:
  N2KPreferences();
  ~N2KPreferences();

  void setBlEnabled(bool value);
  bool isBlEnabled();
  void setDemoEnabled(bool value);
  bool isDemoEnabled();
};

#endif

