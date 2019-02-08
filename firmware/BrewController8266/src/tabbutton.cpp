#include <Arduino.h>
#include "rendering.h"
#include "tabbutton.h"

TabButton::TabButton(int id, int x, int y, int width, int height, int numTabs):
  Button(id, x, y, width, height)
{
  m_nNumTabs = numTabs;
}

void TabButton::draw(bool forceDraw) {
  gfx.setColor(COLOR_WHITE);
  gfx.fillRect(m_x, m_y, m_width, m_height);
}
