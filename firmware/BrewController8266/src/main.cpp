#include <Arduino.h>
#include <SPI.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "settings.h"
#include "images.h"
#include "rendering.h"
#include "touchscreen.h"

#define ENABLE_SSR
#define ENABLE_TEMPERATUR
// #define ENABLE_BUZZER

#define SSR1_PIN 1
#define SSR2_PIN 3
#define TEMP_PIN 10
#define BUZZER_PIN 9

#ifdef ENABLE_TEMPERATUR
OneWire oneWire(TEMP_PIN);
DallasTemperature DS18B20(&oneWire);
#endif

ADC_MODE(ADC_VCC);

// controler states
static const int UPDATE_INTERVAL = 1000;
uint16_t g_lastUpdate = 0;
bool g_updateTick = false;

bool g_bTimerEnabled = false;
uint16_t g_targetTimeSeconds = 10*60;
unsigned long g_timerStartMS = 0;

bool g_bHeaterEnabled = false;
bool g_bAgitatorEnabled = true;
uint8_t g_targetTemperatur = 65;
float g_currentTemperatur = 0;

uint8_t g_rotation = 3;

void callbackPressed(int id, Button *button) {
  switch(id) {
    case CNTL_TIME_PLUS: {
      break;
    }
    case CNTL_TIME_MINUS: {
      break;
    }
    case CNTL_TEMP_PLUS: {
      break;
    }
    case CNTL_TEMP_MINUS: {
      break;
    }
  }
}

void callbackReleased(int id, Button *button) {
  switch(id) {
    case CNTL_TIME_PLUS: {
      g_targetTimeSeconds += 60;
      break;
    }
    case CNTL_TIME_MINUS: {
      g_targetTimeSeconds = (g_targetTimeSeconds > 60) ? g_targetTimeSeconds-60 : 0;
      break;
    }
    case CNTL_TIMER: {
      g_bTimerEnabled = !g_bTimerEnabled;
      if (g_bTimerEnabled) {
        g_timerStartMS = millis();
        button->setImages(PAUSE_UP_DATA, PAUSE_DOWN_DATA);
      } else {
        button->setImages(PLAY_UP_DATA, PLAY_DOWN_DATA);
      }
      break;
    }
    case CNTL_TEMP_PLUS: {
      g_targetTemperatur += 1;
      break;
    }
    case CNTL_TEMP_MINUS: {
      g_targetTemperatur = (g_targetTemperatur > 1) ? g_targetTemperatur-1 : 0;
      break;
    }
    case CNTL_HEATER: {
      break;
    }
  }
}

float getTemperature() {
  float tempC = 0.f;
#ifdef ENABLE_TEMPERATUR
  do {
    DS18B20.requestTemperatures();
    tempC = DS18B20.getTempCByIndex(0);
  } while (tempC == 85.0 || tempC == (-127.0));
#endif
  return tempC;
}

void setup() {
  // Serial.begin(115200);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  // SSR, TEMP setup
#ifdef ENABLE_SSR
  pinMode(SSR1_PIN, OUTPUT);
  digitalWrite(SSR1_PIN, LOW);
  pinMode(SSR2_PIN, OUTPUT);
  digitalWrite(SSR2_PIN, LOW);
#endif

pinMode(BUZZER_PIN, OUTPUT);
digitalWrite(BUZZER_PIN, LOW);

#ifdef ENABLE_TEMPERATUR
  DS18B20.begin();
#endif

  setupRendering(g_rotation);
  setupTouchScreen(g_rotation, true);

  // register callbacks
  for (int i=0; i<CONTROL_COUNT; i++) {
    controls[i].registerCallbacks(callbackPressed, callbackReleased);
  }

  drawClearScreen();
}

void loop() {
  updateTouchScreen();

  // update control states
  if ( (millis()-g_lastUpdate) > UPDATE_INTERVAL ) {
    g_currentTemperatur = getTemperature();
    g_updateTick = ! g_updateTick;
#ifdef ENABLE_SSR
    digitalWrite(SSR1_PIN, g_updateTick ? LOW : HIGH);
    digitalWrite(SSR2_PIN, g_updateTick ? HIGH : LOW);
#endif

#ifdef ENABLE_BUZZER
    digitalWrite(BUZZER_PIN, g_updateTick ? LOW : HIGH);
#endif

    if (g_bTimerEnabled) {
      g_targetTimeSeconds -= 1;
    }
    g_lastUpdate = millis();
  }

  // rendering
  drawTimer((int)(g_targetTimeSeconds/60.f), g_targetTimeSeconds%60, g_updateTick);
  drawTemperatur(g_currentTemperatur, (float)g_targetTemperatur, g_updateTick);
  drawIcons(g_bHeaterEnabled, g_bAgitatorEnabled);
  drawControls();
  drawCommit();
}
