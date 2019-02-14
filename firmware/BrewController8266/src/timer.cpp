#include "timer.h"

Timer::Timer() {
  reset(0);
}

void Timer::reset(int seconds) {
  m_nElapsedTimeMS = seconds*1000;
  m_nLastUpdateMS = 0;
}

int Timer::update(bool countElapsedTime, unsigned long ts) {
  if (countElapsedTime && m_nLastUpdateMS != 0) {
    m_nElapsedTimeMS += (ts - m_nLastUpdateMS);
  }
  m_nLastUpdateMS = ts;
  return m_nElapsedTimeMS;
}
