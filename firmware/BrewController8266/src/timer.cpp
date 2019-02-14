#include "timer.h"

Timer::Timer() {
  reset();
}

void Timer::reset() {
  m_nElapsedTime = 0;
  m_nLastUpdateMS = 0;
}

int Timer::update(bool countElapsedTime, unsigned long ts) {
  if (countElapsedTime && m_nLastUpdateMS != 0) {
    m_nElapsedTime += (ts - m_nLastUpdateMS);
  }
  m_nLastUpdateMS = ts;
  return m_nElapsedTime;
}
