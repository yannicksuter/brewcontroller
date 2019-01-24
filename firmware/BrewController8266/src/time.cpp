#include <Arduino.h>
#include "settings.h"

#include <time.h>
#include <simpleDSTadjust.h>

struct dstRule StartRule = {"CEST", Last, Sun, Mar, 2, 3600};
struct dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0};

#define NTP_UPDATE_INTERVAL_SEC 5*3600
#define NTP_SERVERS "0.ch.pool.ntp.org", "1.ch.pool.ntp.org", "2.ch.pool.ntp.org"

simpleDSTadjust dstAdjusted(StartRule, EndRule);

void setupTime() {
  configTime(UTC_OFFSET * 3600, 0, NTP_SERVERS);
  delay(500);
  Serial.print("NTP Config: ");
  while (!time(nullptr)) {
    Serial.print("#");
    delay(1000);
  }
  Serial.print(" done.\n");
}

extern void getTimeDate(struct tm** timeinfo, String& date) {
  char *dstAbbrev;
  char time_str[11];
  time_t now = dstAdjusted.time(&dstAbbrev);
  *timeinfo = localtime(&now);
  date = ctime(&now);
}
