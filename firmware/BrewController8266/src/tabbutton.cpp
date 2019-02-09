#include <Arduino.h>
#include "rendering.h"
#include "tabbutton.h"

TabButton::TabButton(int id, int x, int y, int width, int height, int numTabs):
  Button(id, x, y, width, height)
{
  m_nTabCount = numTabs;
  m_nCurTab = 0;
  m_nTabWidth = width/numTabs;
}

void TabButton::draw(bool forceDraw) {
  gfx.setColor(COLOR_WHITE);
  for (int i=0; i<m_nTabCount;i++) {
    gfx.fillRect(m_x, m_y, (i==(m_nTabCount-1)) ? m_width : m_width-1, (i==m_nCurTab) ? m_height : 10);
  }
}

bool TabButton::verifyPressed(int x, int y, long timestamp) {
  for (int i=0; i<m_nTabCount;i++) {
    if (x<=(m_x+i*m_nTabWidth) && x>=(m_x+(i+1)*m_nTabWidth) && y >= m_y && y <= m_y+m_height) {
      m_nCurTab = i;
      return Button::verifyPressed(x, y, timestamp);
    }
  }
  return false;
}

bool TabButton::verifyReleased(int x, int y, long timestamp) {
  for (int i=0; i<m_nTabCount;i++) {
    if (x<=(m_x+i*m_nTabWidth) && x>=(m_x+(i+1)*m_nTabWidth) && y >= m_y && y <= m_y+m_height) {
      m_nCurTab = i;
      return Button::verifyReleased(x, y, timestamp);
    }
  }
  return false;
}
