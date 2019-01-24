#ifndef _RENDERINGH_
#define _RENDERINGH_

extern void drawCalibrationCallback(int16_t x, int16_t y);
extern void drawCalibrationScreen();

extern void drawWeather();
extern void drawAbout();

extern void drawClearScreen();
extern void drawScreen(int screen);
extern void drawWifiQuality();
extern void drawTime(bool time_style_12h);
extern void drawTemperatur(float temp);
extern void drawTouch(int x, int y);
extern void drawCommit();

#endif
