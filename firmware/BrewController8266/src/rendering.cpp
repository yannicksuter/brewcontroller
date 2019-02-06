#include "settings.h"
#include "rendering.h"

#include <ILI9341_SPI.h>
#include <MiniGrafx.h>
#include <XPT2046_Touchscreen.h>
#include "button.h"

#define COLOR_BLACK 15
#define COLOR_GREY 10
#define COLOR_WHITE 0

#include "xpm/palette.h"
#include "images.h"

static const char* FONT_DATA[10] = {FONT_0_DATA, FONT_1_DATA, FONT_2_DATA, FONT_3_DATA, FONT_4_DATA, FONT_5_DATA, FONT_6_DATA, FONT_7_DATA, FONT_8_DATA, FONT_9_DATA};
static const uint16_t FONT_WIDTH[10] = {FONT_0_WIDTH, FONT_1_WIDTH, FONT_2_WIDTH, FONT_3_WIDTH, FONT_4_WIDTH, FONT_5_WIDTH, FONT_6_WIDTH, FONT_7_WIDTH, FONT_8_WIDTH, FONT_9_WIDTH};

Button controls[CONTROL_COUNT] = {
  Button(CNTL_TIME_PLUS, 1, 17, PLUS_UP_WIDTH, PLUS_UP_HEIGHT, PLUS_UP_DATA, PLUS_DOWN_DATA),
  Button(CNTL_TIME_MINUS, 1, 61, MINUS_UP_WIDTH, MINUS_UP_HEIGHT, MINUS_UP_DATA, MINUS_DOWN_DATA),
  Button(CNTL_TIMER, 278, 17, PLAY_UP_WIDTH, PLAY_UP_HEIGHT, PLAY_UP_DATA, PLAY_DOWN_DATA),
  Button(CNTL_TEMP_PLUS, 1, 138, PLUS_UP_WIDTH, PLUS_UP_HEIGHT, PLUS_UP_DATA, PLUS_DOWN_DATA),
  Button(CNTL_TEMP_MINUS, 1, 182, MINUS_UP_WIDTH, MINUS_UP_HEIGHT, MINUS_UP_DATA, MINUS_DOWN_DATA),
  Button(CNTL_HEATER, 278, 138, PAUSE_UP_WIDTH, PAUSE_UP_HEIGHT, PAUSE_UP_DATA, PLAY_DOWN_DATA)
};

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
  gfx.fillBuffer(COLOR_BLACK);
  gfx.commit();

  center_width = gfx.getWidth() >> 1;
  center_height = gfx.getHeight() >> 1;
}

void drawClearScreen() {
  gfx.fillBuffer(COLOR_BLACK);

  // icons
  drawXPM(49, 17, CLOCK_ICON_WIDTH, CLOCK_ICON_HEIGHT, CLOCK_ICON_DATA);
  drawXPM(49, 138, TARGET_ICON_WIDTH, TARGET_ICON_HEIGHT, TARGET_ICON_DATA);
  drawXPM(164, 138, SENSOR_ICON_WIDTH, SENSOR_ICON_HEIGHT, SENSOR_ICON_DATA);

  // labels
  drawXPM(278, 84, MIN_LABEL_WIDTH, MIN_LABEL_HEIGHT, MIN_LABEL_DATA);
  drawXPM(168, 205, C_LABEL_WIDTH, C_LABEL_HEIGHT, C_LABEL_DATA);
  drawXPM(278, 205, C_LABEL_WIDTH, C_LABEL_HEIGHT, C_LABEL_DATA);

  // controls
  drawControls(true);
  drawIcons(false, false, true);
}

void drawControls(bool forceDraw) {
  for (int i=0; i<CONTROL_COUNT; i++) {
    controls[i].draw(forceDraw);
  }
}

void drawCommit() {
  gfx.commit();
}

void drawCalibrationCallback(int16_t x, int16_t y) {
  gfx.setColor(1);
  gfx.fillCircle(x, y, 10);
}

void drawCalibrationScreen() {
  gfx.fillBuffer(COLOR_BLACK);
  gfx.setColor(COLOR_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setFont(ArialMT_Plain_10);
  gfx.drawString(120, 160, "Please calibrate\ntouch screen by\ntouch point");
}

void drawNumber(int n, int posX, int posY, bool bForceAllDigits = false) {
  // '4' needs additional cleanup
  gfx.setColor(COLOR_BLACK);
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
}

bool g_bCurHeater = false;
bool g_bCurAgitator = false;
void drawIcons(bool bHeater, bool bAgitator, bool forceDraw) {
    if (bHeater != g_bCurHeater || forceDraw) {
      drawXPM(51, 169, bHeater ? FIRE_ON_WIDTH : FIRE_OFF_WIDTH, bHeater ? FIRE_ON_HEIGHT : FIRE_OFF_HEIGHT, bHeater ? FIRE_ON_DATA : FIRE_OFF_DATA);
      g_bCurHeater = bHeater;
    }
    if (bAgitator != g_bCurAgitator || forceDraw) {
      drawXPM(51, 169, bAgitator ? FIRE_ON_WIDTH : FIRE_OFF_WIDTH, bAgitator ? FIRE_ON_HEIGHT : FIRE_OFF_HEIGHT, bAgitator ? FIRE_ON_DATA : FIRE_OFF_DATA);
      g_bCurAgitator = bAgitator;
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

  gfx.setColor(bAnimation ? COLOR_GREY : COLOR_WHITE);
  gfx.fillRect(170, 66, 10, 10);
  gfx.fillRect(170, 92, 10, 10);
}

void drawTouch(int x, int y) {
  gfx.setColor(COLOR_BLACK);
  gfx.fillRect(0, 0, 320, 10);

  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setColor(COLOR_WHITE);
  gfx.setFont(ArialMT_Plain_10);

  char tmp_str[180];
  sprintf(tmp_str, "Touch: %d/%d", x, y);

  gfx.drawString(0, 0, tmp_str);
}
