#include "settings.h"

#include "gfx/ILI9341_SPI.h"
#include "gfx/MiniGrafx.h"
#include <XPT2046_Touchscreen.h>

#define MINI_BLACK 0
#define MINI_WHITE 1
#define MINI_YELLOW 2
#define MINI_BLUE 3
uint16_t palette[] = {ILI9341_BLACK, ILI9341_WHITE, ILI9341_YELLOW, 0x7E3C};

// uint16_t palette[] = {ILI9341_BLACK, // 0
//                       ILI9341_WHITE, // 1
//                       ILI9341_NAVY, // 2
//                       ILI9341_DARKCYAN, // 3
//                       ILI9341_DARKGREEN, // 4
//                       ILI9341_MAROON, // 5
//                       ILI9341_PURPLE, // 6
//                       ILI9341_OLIVE, // 7
//                       ILI9341_LIGHTGREY, // 8
//                       ILI9341_DARKGREY, // 9
//                       ILI9341_BLUE, // 10
//                       ILI9341_GREEN, // 11
//                       ILI9341_CYAN, // 12
//                       ILI9341_RED, // 13
//                       ILI9341_MAGENTA, // 14
//                       ILI9341_YELLOW}; // 15

ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);

char str_tmp[128];

int center_width;
int center_height;

void setupRendering(int rotation) {
  gfx.init();
  gfx.setRotation(rotation);
  gfx.fillBuffer(MINI_BLACK);
  gfx.commit();

  center_width = gfx.getWidth() >> 1;
  center_height = gfx.getHeight() >> 1;
}

void drawClearScreen() {
  gfx.fillBuffer(MINI_BLACK);
}

void drawCommit() {
  gfx.commit();
}

void drawCalibrationCallback(int16_t x, int16_t y) {
  gfx.setColor(1);
  gfx.fillCircle(x, y, 10);
}

void drawCalibrationScreen() {
  gfx.fillBuffer(MINI_BLACK);
  gfx.setColor(MINI_YELLOW);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setFont(ArialMT_Plain_10);
  gfx.drawString(120, 160, "Please calibrate\ntouch screen by\ntouch point");
}

void drawProgress(uint8_t percentage, String text) {
  gfx.fillBuffer(MINI_BLACK);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);

  gfx.setColor(MINI_YELLOW);
  gfx.drawString(120, 146, text);
  gfx.setColor(MINI_WHITE);
  gfx.drawRect(10, 168, 240 - 20, 15);
  gfx.setColor(MINI_BLUE);
  gfx.fillRect(12, 170, 216 * percentage / 100, 11);

  gfx.commit();
}

void drawScreen(int screen) {
  gfx.setColor(MINI_YELLOW);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setFont(ArialMT_Plain_10);
  gfx.drawString(120, 160, String("Screen: ") + String(screen));
}

#include <time.h>
extern void getTimeDate(struct tm** timeinfo, String& date);

void drawTime(bool time_style_12h) {
  struct tm * timeinfo;
  String date;
  getTimeDate(&timeinfo, date);

  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setFont(ArialMT_Plain_10);
  // gfx.setFont(ArialRoundedMTBold_14);
  gfx.setColor(MINI_WHITE);
  date = date.substring(0,11) + String(1900 + timeinfo->tm_year);
  gfx.drawString(center_width, 6, date);
  char time_str[11];
  if (time_style_12h) {
    int hour = (timeinfo->tm_hour+11)%12+1;
    sprintf(time_str, "%2d:%02d:%02d %s\n",hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_hour>=12?"PM":"AM");
    gfx.drawString(center_width, 20, time_str);
  } else {
    sprintf(time_str, "%02d:%02d:%02d\n",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    gfx.drawString(center_width, 20, time_str);
  }
}

void drawTemperatur(float temp) {
  gfx.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  gfx.setColor(MINI_YELLOW);
  gfx.setFont(ArialMT_Plain_24);
  gfx.drawString(center_width, center_height, String("Temp: ") + String(temp));

  gfx.drawRaw(0,0,0,0,NULL);

}

void drawTouch(int x, int y) {
  gfx.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  gfx.setColor(MINI_YELLOW);
  gfx.setFont(ArialMT_Plain_24);

  char tmp_str[180];
  sprintf(tmp_str, "Touch: %d/%d", x, y);

  gfx.drawString(center_width, center_height+24, tmp_str);
}

void drawLabelValue(uint8_t line, String label, String value) {
  const uint8_t labelX = 35;
  const uint8_t valueX = 150;
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setColor(MINI_YELLOW);
  gfx.drawString(labelX, 30 + line * 15, label);
  gfx.setColor(MINI_WHITE);
  gfx.drawString(valueX, 30 + line * 15, value);
}
