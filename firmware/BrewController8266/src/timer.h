#ifndef _TIMER_
#define _TIMER_

class Timer {
public:
  Timer();

  void reset();
  int update(bool countElapsedTime, unsigned long ts);

  inline unsigned long getElapsedTime() { return m_nElapsedTime; }

protected:
  unsigned long m_nElapsedTime;
  unsigned long m_nLastUpdateMS;
};

#endif
