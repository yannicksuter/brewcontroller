#ifndef _TAB_BUTTON_
#define _TAB_BUTTON_

#include "button.h"

class TabButton : public Button {
public:
  TabButton(int id, int x, int y, int width, int height, int numTabs);

  virtual void draw(bool forceDraw);

private:
  int m_nNumTabs;
};

#endif
