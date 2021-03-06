#include "settings.h"
#include <FS.h>
#include <XPT2046_Touchscreen.h>

#ifndef _TOUCH_CONTROLLERWSH_
#define _TOUCH_CONTROLLERWSH_

#define SLEEP_INTERVAL_SECS 0

class TouchControllerWS {
  public:
    TouchControllerWS(XPT2046_Touchscreen *touchScreen);
    bool loadCalibration();
    bool saveCalibration();
    void startCalibration();
    void continueCalibration();
    bool isCalibrationFinished();
    bool isTouched();
    // bool isTouched(uint16_t debounceMillis);
    TS_Point getPoint();

  private:
    XPT2046_Touchscreen *touchScreen;
    float dx = 0.0;
    float dy = 0.0;
    int ax = 0;
    int ay = 0;
    int state = 0;
    uint16_t m_lastStateChange = 0;
    uint16_t m_lastTouched = 0;
    TS_Point p1, p2;
};

#endif

extern void setupTouchScreen(uint8_t rotation, bool forceCalibration=false);
extern void updateTouchScreen(long timestamp);

// extern bool isTouched(int16_t debounceMillis);
extern TS_Point getTouchPoint();
