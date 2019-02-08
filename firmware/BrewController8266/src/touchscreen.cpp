#include "touchscreen.h"
#include "rendering.h"

const char *CALIBRATION_FILENAME = "/calibration.txt";

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);
TouchControllerWS touchController(&ts);

void setupTouchScreen(uint8_t rotation, bool forceCalibration) {
  ts.begin();
  ts.setRotation(rotation);

  boolean isCalibrationAvailable = touchController.loadCalibration();
  if (!isCalibrationAvailable || forceCalibration) {
    Serial.println("Calibration not available");
    touchController.startCalibration();
    while (!touchController.isCalibrationFinished()) {
      drawCalibrationScreen();
      touchController.continueCalibration();
      drawCommit();
      yield();
    }
    touchController.saveCalibration();
  }
}

bool g_bTouched = false;
TS_Point g_pLastPointTouched;
void updateTouchScreen(long timestamp) {
  if (touchController.isTouched()) {
    g_bTouched = true;
    g_pLastPointTouched = touchController.getPoint();
    for (int i=0; i<CONTROL_COUNT; i++) {
      controls[i]->verifyPressed(g_pLastPointTouched.x, g_pLastPointTouched.y, timestamp);
    }
  } else {
    if (g_bTouched) {
      for (int i=0; i<CONTROL_COUNT; i++) {
        controls[i]->verifyReleased(g_pLastPointTouched.x, g_pLastPointTouched.y, timestamp);
      }
    }
    g_bTouched = false;
  }
}

// bool isTouched(int16_t debounceMillis) {
//   return touchController.isTouched(debounceMillis);
// }

TS_Point getTouchPoint() {
  return touchController.getPoint();
}

TouchControllerWS::TouchControllerWS(XPT2046_Touchscreen *touchScreen) {
  this->touchScreen = touchScreen;
}

bool TouchControllerWS::loadCalibration() {
  if (SPIFFS.begin()) {
    File f = SPIFFS.open(CALIBRATION_FILENAME, "r");
    if (f) {
        String dxStr = f.readStringUntil('\n');
        String dyStr = f.readStringUntil('\n');
        String axStr = f.readStringUntil('\n');
        String ayStr = f.readStringUntil('\n');
        dx = dxStr.toFloat();
        dy = dyStr.toFloat();
        ax = axStr.toInt();
        ay = ayStr.toInt();
        f.close();
    }
    SPIFFS.end();

    // configuration sanaty check
    if (ax == 8191 && ay == 8191) {
        return false;
    }
    return true;
  }
  return false;
}

bool TouchControllerWS::saveCalibration() {
  if(SPIFFS.begin()) {
    File f = SPIFFS.open(CALIBRATION_FILENAME, "w");
    if(f) {
      f.println(dx);
      f.println(dy);
      f.println(ax);
      f.println(ay);
      f.close();
    }
    SPIFFS.end();
    return true;
  }
  return false;
}

void TouchControllerWS::startCalibration() {
  state = 0;
}

void TouchControllerWS::continueCalibration() {
  TS_Point p = touchScreen->getPoint();

  if (state == 0) {
    drawCalibrationCallback(10, 10);
    if (touchScreen->touched()) {
      p1 = p;
      state++;
      m_lastStateChange = millis();
    }
  } else if (state == 1) {
    drawCalibrationCallback(TFT_WIDTH-10, TFT_HEIGHT-10);
    if (touchScreen->touched() && (millis() - m_lastStateChange > 1000)) {
      p2 = p;
      state++;
      m_lastStateChange = millis();
      dx = float(TFT_WIDTH) / abs(p1.y - p2.y);
      dy = float(TFT_HEIGHT) / abs(p1.x - p2.x);
      ax = p1.y < p2.y ? p1.y : p2.y;
      ay = p1.x < p2.x ? p1.x : p2.x;
    }
  }
}

bool TouchControllerWS::isCalibrationFinished() {
  return state == 2;
}

bool TouchControllerWS::isTouched() {
  return touchScreen->touched();
}

// bool TouchControllerWS::isTouched(uint16_t debounceMillis) {
//   if (touchScreen->touched() && millis() - m_lastTouched > debounceMillis) {
//     m_lastTouched = millis();
//     return true;
//   }
//   return false;
// }

TS_Point TouchControllerWS::getPoint() {
    TS_Point p = touchScreen->getPoint();
    int x = (p.x - ax) * dx;
    int y = (p.y - ay) * dy;
    p.x = TFT_WIDTH - x;
    p.y = TFT_HEIGHT - y;
    return p;
}
