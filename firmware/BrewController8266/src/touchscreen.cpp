#include "touchscreen.h"
#include "rendering.h"

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);
TouchControllerWS touchController(&ts);

void setupTouchScreen() {
  ts.begin();
  boolean isCalibrationAvailable = touchController.loadCalibration();
  if (!isCalibrationAvailable) {
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

bool isTouched(int16_t debounceMillis) {
  return touchController.isTouched(debounceMillis);
}

TS_Point getTouchPoint() {
  return touchController.getPoint();
}

TouchControllerWS::TouchControllerWS(XPT2046_Touchscreen *touchScreen) {
  this->touchScreen = touchScreen;
}

bool TouchControllerWS::loadCalibration() {
  // always use this to "mount" the filesystem
  bool result = SPIFFS.begin();
  Serial.println("SPIFFS opened: " + result);

  // this opens the file "f.txt" in read-mode
  File f = SPIFFS.open("/calibration.txt", "r");

  if (!f) {
    return false;
  } else {
      //Lets read line by line from the file
      String dxStr = f.readStringUntil('\n');
      String dyStr = f.readStringUntil('\n');
      String axStr = f.readStringUntil('\n');
      String ayStr = f.readStringUntil('\n');
      dx = dxStr.toFloat();
      dy = dyStr.toFloat();
      ax = axStr.toInt();
      ay = ayStr.toInt();
  }
  f.close();

  // Serial.printf("%f %f %d %d\n", dx, dy, ax, ay);
  if (ax == 8191 && ay == 8191) {
      return false;
  }
  return true;
}

bool TouchControllerWS::saveCalibration() {
  bool result = SPIFFS.begin();

  // open the file in write mode
  File f = SPIFFS.open("/calibration.txt", "w");
  // if (!f) {
  //   Serial.println("file creation failed");
  // }
  // now write two lines in key/value style with  end-of-line characters
  f.println(dx);
  f.println(dy);
  f.println(ax);
  f.println(ay);

  f.close();
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
      lastStateChange = millis();
    }
  } else if (state == 1) {
    drawCalibrationCallback(TFT_WIDTH-10, TFT_HEIGHT-10);
    if (touchScreen->touched() && (millis() - lastStateChange > 1000)) {
      p2 = p;
      state++;
      lastStateChange = millis();
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
  touchScreen->touched();
}

bool TouchControllerWS::isTouched(int16_t debounceMillis) {
  if (touchScreen->touched() && millis() - lastTouched > debounceMillis) {
    lastTouched = millis();
    return true;
  }
  return false;
}

TS_Point TouchControllerWS::getPoint() {
    TS_Point p = touchScreen->getPoint();
    int x = (p.y - ax) * dx;
    int y = TFT_WIDTH - (p.x - ay) * dy;
    p.x = x;
    p.y = y;
    return p;
}
