#ifndef _TIMER_
#define _TIMER_

class Timer {
public:
  Timer();

  void reset(int seconds);
  int update(bool countElapsedTime, unsigned long ts);

  inline unsigned long getElapsedTime() { return m_nElapsedTimeMS; }

protected:
  unsigned long m_nElapsedTimeMS;
  unsigned long m_nLastUpdateMS;
};

#endif
