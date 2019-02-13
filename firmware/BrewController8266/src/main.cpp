#include <Arduino.h>
#include <SPI.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "settings.h"
#include "images.h"
#include "rendering.h"
#include "touchscreen.h"

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
#endif

ADC_MODE(ADC_VCC);

// controler states
static const int UPDATE_INTERVAL = 1000;
static const int UI_ROTATION = 3;

static int g_nCurTab = 0;
static long g_curTimestamp;
static long g_lastUpdate = 0;
static bool g_updateTick = false;

bool g_bTimerEnabled = false;
long g_targetTimeSeconds = DEFAULT_TIME_SEC;

bool g_bHeaterEnabled = false;
bool g_bAgitatorEnabled = true;
uint8_t g_targetTemperatur = DEFAULT_TEMPERATUR;
float g_currentTemperatur = 0;

const char *TABCONFIG_FILENAME = "/tabconfigs";

int g_loadedTime = 0;
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
      g_bTimerEnabled = !g_bTimerEnabled;
      controls[CNTL_TIMER]->setToggleState(g_bTimerEnabled ? TOGGLE_STATE_PAUSE : TOGGLE_STATE_PLAY);
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
  pinMode(SSR_HEATER_PIN, OUTPUT);
  digitalWrite(SSR_HEATER_PIN, LOW);
  pinMode(SSR_AGITATOR_PIN, OUTPUT);
  digitalWrite(SSR_AGITATOR_PIN, LOW);
#endif

pinMode(BUZZER_PIN, OUTPUT);
digitalWrite(BUZZER_PIN, LOW);

#ifdef ENABLE_TEMPERATUR
  DS18B20.begin();
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
  g_curTimestamp = millis();
  updateTouchScreen(g_curTimestamp);

  // update control states
  if ( (g_curTimestamp-g_lastUpdate) > UPDATE_INTERVAL ) {
    g_currentTemperatur = getTemperature();
    g_updateTick = ! g_updateTick;

#ifdef ENABLE_SSR
    digitalWrite(SSR_HEATER_PIN, g_bHeaterEnabled && (g_currentTemperatur < g_targetTemperatur) ? HIGH : LOW);
    digitalWrite(SSR_AGITATOR_PIN, g_bAgitatorEnabled ? HIGH : LOW);
#endif

    if (g_bTimerEnabled) {
      g_targetTimeSeconds -= 1;
      if (g_targetTimeSeconds <= 0) {
        disableTimer(0);
      }
    }
    g_lastUpdate = g_curTimestamp;
  }

  // rendering
  drawTimer((int)(g_targetTimeSeconds/60.f), g_targetTimeSeconds%60, g_bTimerEnabled?g_updateTick:false);
  drawTemperatur(g_currentTemperatur, (float)g_targetTemperatur, g_updateTick);
  drawIcons(g_bHeaterEnabled, g_bAgitatorEnabled);
  drawControls();
  drawCommit();
}
