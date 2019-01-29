#include "settings.h"

#include <ILI9341_SPI.h>
#include <MiniGrafx.h>
#include <XPT2046_Touchscreen.h>

#define MINI_BLACK 15
#define MINI_WHITE 0

#include "xpm/palette.h"

#include "xpm/fan_on.h"
#include "xpm/fan_off.h"
#include "xpm/fire_on.h"
#include "xpm/fire_off.h"
#include "xpm/font_0.h"
#include "xpm/font_1.h"
#include "xpm/font_2.h"
#include "xpm/font_3.h"
#include "xpm/font_4.h"
#include "xpm/font_5.h"
#include "xpm/font_6.h"
#include "xpm/font_7.h"
#include "xpm/font_8.h"
#include "xpm/font_9.h"

#include "xpm/min_label.h"
#include "xpm/c_label.h"

#include "xpm/clock_icon.h"
#include "xpm/sensor_icon.h"
#include "xpm/target_icon.h"

static const char* FONT_DATA[10] = {FONT_0_DATA, FONT_1_DATA, FONT_2_DATA, FONT_3_DATA, FONT_4_DATA, FONT_5_DATA, FONT_6_DATA, FONT_7_DATA, FONT_8_DATA, FONT_9_DATA};
static const uint16_t FONT_WIDTH[10] = {FONT_0_WIDTH, FONT_1_WIDTH, FONT_2_WIDTH, FONT_3_WIDTH, FONT_4_WIDTH, FONT_5_WIDTH, FONT_6_WIDTH, FONT_7_WIDTH, FONT_8_WIDTH, FONT_9_WIDTH};

ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);

char str_tmp[128];
int center_width;
int center_height;

void drawXPM(int16_t pos_x, int16_t pos_y, int16_t width, int16_t height, const char *xpm) {
  if (xpm) {
    char * buf = new char[width*height];
    if (buf) {
      // copy data from progmem
      memcpy_P(buf, xpm, width*height);
      int x, y, id = 0;
      for (y=0; y<height; y++) {
        for (x=0; x<width; x++) {
          gfx.setColor(buf[id]);
          gfx.setPixel(pos_x+x, pos_y+y);
          id++;
        }
      }
      delete [] buf;
    }
  }
}

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

  // icons
  drawXPM(49, 17, CLOCK_ICON_WIDTH, CLOCK_ICON_HEIGHT, CLOCK_ICON_DATA);
  drawXPM(49, 138, TARGET_ICON_WIDTH, TARGET_ICON_HEIGHT, TARGET_ICON_DATA);
  drawXPM(164, 138, SENSOR_ICON_WIDTH, SENSOR_ICON_HEIGHT, SENSOR_ICON_DATA);

  // labels
  drawXPM(272, 84, MIN_LABEL_WIDTH, MIN_LABEL_HEIGHT, MIN_LABEL_DATA);
  drawXPM(162, 205, C_LABEL_WIDTH, C_LABEL_HEIGHT, C_LABEL_DATA);
  drawXPM(272, 205, C_LABEL_WIDTH, C_LABEL_HEIGHT, C_LABEL_DATA);
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
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setFont(ArialMT_Plain_10);
  gfx.drawString(120, 160, "Please calibrate\ntouch screen by\ntouch point");
}

void drawNumber(int n, int posX, int posY, bool bForceAllDigits = false) {
  // '4' needs additional cleanup
  gfx.setColor(MINI_BLACK);
  gfx.fillRect(posX-3, posY, 3, 85);
  gfx.fillRect(posX+40-3, posY, 3, 85);

  // draw digits
  int id1 = 0;
  if (n>9 || bForceAllDigits) {
    id1 = (int)(n/10.f);
    drawXPM(posX-((id1==4)?3:0), posY, FONT_WIDTH[id1], 85, FONT_DATA[id1]);
  }
  int id2 = n - id1*10;
  drawXPM(posX+40-((id2==4)?3:0), posY, FONT_WIDTH[id2], 85, FONT_DATA[id2]);
}

int g_curTemp = -1;
int g_curTarget = -1;
void drawTemperatur(float curTemp, float curTarget, bool bAnimation) {
  if (g_curTemp != (int)curTemp) {
    g_curTemp = (int)(curTemp+.5f);
    g_curTemp = (g_curTemp>99)?99:(g_curTemp<0)?0:g_curTemp;
    drawNumber(g_curTemp, 192, 138);
  }

  if (g_curTarget != (int)curTarget) {
    g_curTarget = (int)(curTarget+.5f);
    g_curTarget = (g_curTarget>99)?99:(g_curTarget<0)?0:g_curTarget;
    drawNumber(g_curTarget, 82, 138);
  }

  if (!bAnimation) {
    drawXPM(49, 199, FAN_ON_WIDTH, FAN_ON_HEIGHT, FAN_ON_DATA);
    drawXPM(51, 169, FIRE_OFF_WIDTH, FIRE_OFF_HEIGHT, FIRE_OFF_DATA);
  } else {
    drawXPM(49, 199, FAN_OFF_WIDTH, FAN_OFF_HEIGHT, FAN_OFF_DATA);
    drawXPM(50, 169, FIRE_ON_WIDTH, FIRE_ON_HEIGHT, FIRE_ON_DATA);
  }
}

int g_curMin = -1;
int g_curSec = -1;
void drawTimer(int min, int sec, bool bAnimation) {
  if (g_curMin != min) {
    g_curMin = min;
    drawNumber(g_curMin, 82, 17);
  }
  if (g_curSec != sec) {
    g_curSec = sec;
    drawNumber(g_curSec, 192, 17, true);
  }

  gfx.setColor(bAnimation ? MINI_BLACK : MINI_WHITE);
  gfx.fillRect(170, 66, 10, 10);
  gfx.fillRect(170, 92, 10, 10);
}

void drawTouch(int x, int y) {
  // gfx.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  // gfx.setColor(MINI_YELLOW);
  // gfx.setFont(ArialMT_Plain_24);
  //
  // char tmp_str[180];
  // sprintf(tmp_str, "Touch: %d/%d", x, y);
  //
  // gfx.drawString(center_width, center_height+24, tmp_str);
}
