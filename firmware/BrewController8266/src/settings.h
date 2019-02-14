#ifndef SETTINGS_H
#define SETTINGS_H

// ILI9341 hardware
#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8

#define HAVE_TOUCHPAD
#define TOUCH_CS D3
#define TOUCH_IRQ  D4

// display
#define TFT_WIDTH 320
#define TFT_HEIGHT 240
#define BITS_PER_PIXEL 4

#define NUM_TABS 4

#define DEFAULT_TIME_SEC 4*60
#define DEFAULT_TEMPERATUR 65

#define BEEP_SHORT 30
#define BEEP_LONG 100

#endif
