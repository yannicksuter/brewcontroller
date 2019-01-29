#ifndef _RENDERINGH_
#define _RENDERINGH_

extern void drawCalibrationCallback(int16_t x, int16_t y);
extern void drawCalibrationScreen();

extern void drawClearScreen();
extern void drawScreen(int screen);
extern void drawTouch(int x, int y);
extern void drawCommit();

extern void drawTimer(int min, int sec, bool bAnimation);
extern void drawTemperatur(float curTemp, float curTarget, bool bAnimation);

#endif
