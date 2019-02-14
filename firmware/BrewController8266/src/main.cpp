#include <Arduino.h>
#include <SPI.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "settings.h"
#include "images.h"
#include "rendering.h"
#include "touchscreen.h"
#include "timer.h"

#define ENABLE_SSR
// #define ENABLE_TEMPERATUR
#define ENABLE_BUZZER

#define SSR_AGITATOR_PIN 1
#define SSR_HEATER_PIN 3
#define TEMP_PIN 10
#define BUZZER_PIN 9

#ifdef ENABLE_TEMPERATUR
OneWire oneWire(TEMP_PIN);
DallasTemperature DS18B20(&oneWire);
int g_tempDelayInMillis;
unsigned long g_lastTempRequest;
#endif

ADC_MODE(ADC_VCC);

// controler states
Timer g_timer;
static const int UI_ROTATION = 3;
static const int TEMPERATUR_RESOLUTION = 12;

static int g_nCurTab = 0;
static bool g_updateTick = false;

bool g_bTimerEnabled = false;
unsigned long g_nElapsedTimeSeconds = 0;
unsigned long g_targetTimeSeconds = DEFAULT_TIME_SEC;

bool g_bHeaterEnabled = false;
bool g_bAgitatorEnabled = true;
uint8_t g_targetTemperatur = DEFAULT_TEMPERATUR;
float g_currentTemperatur = 0;

const char *TABCONFIG_FILENAME = "/tabconfigs";

unsigned long g_loadedTime = 0;
int g_loadedTemp = 0;
void loadConfig(int id) {
  if (SPIFFS.begin()) {
    File f = SPIFFS.open(String(TABCONFIG_FILENAME) + id, "r");
    if (f) {
      String time = f.readStringUntil('\n');
      String targetTemp = f.readStringUntil('\n');
      g_targetTimeSeconds = time.toInt();
      g_targetTemperatur = targetTemp.toInt();
      f.close();
      g_loadedTime = g_targetTimeSeconds;
      g_loadedTemp = g_targetTemperatur;
    } else {
      g_targetTimeSeconds = DEFAULT_TIME_SEC;
      g_targetTemperatur = DEFAULT_TEMPERATUR;
    }
    SPIFFS.end();
  }
}

void saveConfig(int id) {
  // only save if config changed
  if ((g_loadedTime != g_targetTimeSeconds || g_loadedTemp != g_targetTemperatur) && SPIFFS.begin()) {
    File f = SPIFFS.open(String(TABCONFIG_FILENAME) + id, "w");
    if (f) {
      f.println(g_targetTimeSeconds);
      f.println(g_targetTemperatur);
      f.close();
#ifdef ENABLE_BUZZER
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
#endif
    }
    SPIFFS.end();
  }
}

void disableTimer(long remainingSeconds) {
  g_targetTimeSeconds = remainingSeconds;
  g_bTimerEnabled = false;
  controls[CNTL_TIMER]->setToggleState(0);

  if (remainingSeconds == 0) {
#ifdef ENABLE_BUZZER
    for (int i=0; i<4; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);
    }
#endif
  }
}

bool g_bBlockNextTimerRelease = false;
void callbackLongPressed(int id, Button *src) {
  switch(id) {
    case CNTL_TIME_PLUS: {
      if (src->getLongPressCounter()%2 == 0) {
        g_targetTimeSeconds += 60;
      }
      break;
    }
    case CNTL_TIME_MINUS: {
      if (src->getLongPressCounter()%2 == 0) {
        if (g_targetTimeSeconds > 60) {
          g_targetTimeSeconds -= 60;
        } else {
          disableTimer(0);
        }
      }
      break;
    }
    case CNTL_TIMER: {
      if (src->getLongPressCounter() == 0) {
        disableTimer(DEFAULT_TIME_SEC);
        g_bBlockNextTimerRelease = true;
      }
      break;
    }
    case CNTL_TEMP_PLUS: {
      if (src->getLongPressCounter()%2 == 0) {
        g_targetTemperatur += 1;
      }
      break;
    }
    case CNTL_TEMP_MINUS: {
      if (src->getLongPressCounter()%2 == 0) {
        g_targetTemperatur = (g_targetTemperatur > 1) ? g_targetTemperatur-1 : 0;
      }
      break;
    }
    case CNTL_TAB: {
      saveConfig(((TabButton*)src)->getCurTab());
      break;
    }
  }
}

void callbackReleased(int id, Button *src) {
  switch(id) {
    case CNTL_TIME_PLUS: {
      g_targetTimeSeconds += 60;
      break;
    }
    case CNTL_TIME_MINUS: {
      if (g_targetTimeSeconds > 60) {
        g_targetTimeSeconds -= 60;
      } else {
        disableTimer(0);
      }
      break;
    }
    case CNTL_TIMER: {
      if (!g_bBlockNextTimerRelease) {
        g_bTimerEnabled = !g_bTimerEnabled;
        controls[CNTL_TIMER]->setToggleState(g_bTimerEnabled ? TOGGLE_STATE_PAUSE : TOGGLE_STATE_PLAY);
      }
      g_bBlockNextTimerRelease = false;
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
      g_bHeaterEnabled = !g_bHeaterEnabled;
      controls[CNTL_HEATER]->setToggleState(g_bHeaterEnabled ? TOGGLE_STATE_PAUSE : TOGGLE_STATE_PLAY);
      break;
    }
    case CNTL_AGITATOR: {
      g_bAgitatorEnabled = !g_bAgitatorEnabled;
      controls[CNTL_AGITATOR]->setToggleState(g_bAgitatorEnabled ? TOGGLE_STATE_PAUSE : TOGGLE_STATE_PLAY);
      break;
    }
    case CNTL_TAB: {
      int tab = ((TabButton*)src)->getCurTab();
      if (g_nCurTab != tab) {
        g_nCurTab = tab;
        loadConfig(g_nCurTab);
      }
      break;
    }
  }
}

float getTemperature() {
  float tempC = 0.f;
#ifdef ENABLE_TEMPERATUR
  if (millis() - g_lastTempRequest >= g_tempDelayInMillis) {
    tempC = DS18B20.getTempCByIndex(0);
    g_lastTempRequest = millis();
    DS18B20.requestTemperatures();
  }
#endif
  return tempC;
}

void setup() {
  // Serial.begin(115200);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  // SSR, TEMP setup
#ifdef ENABLE_SSR
  pinMode(SSR_HEATER_PIN, OUTPUT);
  digitalWrite(SSR_HEATER_PIN, LOW);
  pinMode(SSR_AGITATOR_PIN, OUTPUT);
  digitalWrite(SSR_AGITATOR_PIN, LOW);
#endif

pinMode(BUZZER_PIN, OUTPUT);
digitalWrite(BUZZER_PIN, LOW);

#ifdef ENABLE_TEMPERATUR
  DS18B20.begin();
  DS18B20.getAddress(tempDeviceAddress, 0);
  DS18B20.setResolution(tempDeviceAddress, TEMPERATUR_RESOLUTION);
  DS18B20.setWaitForConversion(false);
  DS18B20.requestTemperatures();
  g_tempDelayInMillis = 750 / (1 << (12 - TEMPERATUR_RESOLUTION));
  g_lastTempRequest = millis();
#endif

  loadConfig(g_nCurTab);
  setupRendering(UI_ROTATION);
  setupTouchScreen(UI_ROTATION, false);

  // register callbacks
  for (int i=0; i<CONTROL_COUNT; i++) {
    controls[i]->registerReleaseCallback(callbackReleased);
    controls[i]->registerLongPressCallback(callbackLongPressed);
  }
  controls[CNTL_AGITATOR]->setToggleState(g_bAgitatorEnabled ? TOGGLE_STATE_PAUSE : TOGGLE_STATE_PLAY);

  drawClearScreen();
}

void loop() {
  static unsigned long curTimestamp = millis();
  g_currentTemperatur = getTemperature();
  g_nElapsedTimeSeconds = g_timer.update(g_bTimerEnabled, curTimestamp) / 1000;
  g_updateTick = (g_nElapsedTimeSeconds%2) == 0;

  if (g_nElapsedTimeSeconds >= g_targetTimeSeconds) {
    disableTimer(0);
  }

  updateTouchScreen(curTimestamp);

  #ifdef ENABLE_SSR
      digitalWrite(SSR_HEATER_PIN, g_bHeaterEnabled && (g_currentTemperatur < g_targetTemperatur) ? HIGH : LOW);
      digitalWrite(SSR_AGITATOR_PIN, g_bAgitatorEnabled ? HIGH : LOW);
  #endif

  // rendering
  int timeRemaining = g_targetTimeSeconds-g_nElapsedTimeSeconds;
  drawTimer((int)(timeRemaining/60.f), timeRemaining%60, g_bTimerEnabled?g_updateTick:false);
  drawTemperatur(g_currentTemperatur, (float)g_targetTemperatur, g_updateTick);
  drawIcons(g_bHeaterEnabled, g_bAgitatorEnabled);
  drawControls();
  drawCommit();
}
