#ifndef _TAB_BUTTON_
#define _TAB_BUTTON_

#include "button.h"

class TabButton : public Button {
public:
  TabButton(int id, int x, int y, int width, int height, int numTabs);

  inline int getCurTab() { return m_nCurTab; };

  virtual void draw(bool forceDraw);
  virtual bool verifyPressed(int x, int y, long timestamp);
  virtual bool verifyReleased(int x, int y, long timestamp);

private:
  int m_nTabCount;
  int m_nCurTab;
  int m_nTabWidth;
};

#endif
