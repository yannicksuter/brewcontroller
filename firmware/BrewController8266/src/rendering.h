#ifndef RENDERING_H
#define RENDERING_H

#include <Arduino.h>
#include "button.h"

extern void setupRendering(int rotation);

extern void drawCalibrationCallback(int16_t x, int16_t y);
extern void drawCalibrationScreen();

extern void drawClearScreen();
extern void drawControls(bool forceDraw = false);
extern void drawTimer(int min, int sec, bool bAnimation);
extern void drawTemperatur(float curTemp, float curTarget, bool bAnimation);
extern void drawIcons(bool bHeater, bool bAgitator, bool forceDraw = false);
extern void drawCommit();

extern void drawXPM(int16_t pos_x, int16_t pos_y, int16_t width, int16_t height, const char *xpm);

enum CONTROLS_ID {
  CNTL_TIME_PLUS=0,
  CNTL_TIME_MINUS,
  CNTL_TIMER,
  CNTL_TEMP_PLUS,
  CNTL_TEMP_MINUS,
  CNTL_HEATER,
};

enum TOGGLE_STATE {
  TOGGLE_STATE_PLAY = 0,
  TOGGLE_STATE_PAUSE
};

static const int CONTROL_COUNT = 6;
extern Button controls[CONTROL_COUNT];

#endif
