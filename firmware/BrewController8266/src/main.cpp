#include <Arduino.h>
#include <SPI.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "settings.h"
#include "rendering.h"
#include "touchscreen.h"

extern void setupRendering(int rotation);
extern void setupTouchScreen();
extern void setupWifi();
extern void setupTime();

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
unsigned long cntrl_wait_MS = 1000;
unsigned long cntrl_last_update = 0;
bool cntrl_ssr_state = false;
float cntrl_water_temp = 0.f;

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
  Serial.begin(115200);

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

  setupRendering(3);
  setupTouchScreen();
  setupTime();
}

int last_x = 0;
int last_y = 0;

bool time_style_12h = IS_STYLE_12HR;
void loop() {

  // update control states
  if (millis()-cntrl_last_update > cntrl_wait_MS) {
    cntrl_water_temp = getTemperature();
    cntrl_ssr_state = ! cntrl_ssr_state;
#ifdef ENABLE_SSR
    digitalWrite(SSR1_PIN, cntrl_ssr_state ? LOW : HIGH);
    digitalWrite(SSR2_PIN, cntrl_ssr_state ? HIGH : LOW);
#endif

#ifdef ENABLE_BUZZER
    digitalWrite(BUZZER_PIN, cntrl_ssr_state ? LOW : HIGH);
#endif

    cntrl_last_update = millis();
  }

  // rendering
  drawClearScreen();
  drawTime(time_style_12h);
  drawTemperatur(cntrl_water_temp);
  if (isTouched(500)) {
    TS_Point p = getTouchPoint();
    time_style_12h = !time_style_12h;
    last_x = p.x;
    last_y = p.y;
  }
  drawTouch(last_x, last_y);
  drawCommit();
}
