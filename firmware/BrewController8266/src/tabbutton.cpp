#include <Arduino.h>
#include "rendering.h"
#include "tabbutton.h"

TabButton::TabButton(int id, int x, int y, int width, int height, int numTabs):
  Button(id, x, y, width, height)
{
  m_nTabCount = numTabs;
  m_nPrevTab = 0;
  m_nCurTab = 0;
  m_nTabWidth = width/numTabs;
}

void TabButton::draw(bool forceDraw) {
  if (m_bDirty || forceDraw) {
    // clean previous active tab
    gfx.setColor(COLOR_BLACK);
    gfx.fillRect(m_x + m_nPrevTab*m_nTabWidth, m_y, m_nTabWidth - ((m_nPrevTab==(m_nTabCount-1)) ? 0: 1), m_height);

    gfx.setColor(COLOR_WHITE);
    for (int i=0; i<m_nTabCount;i++) {
      gfx.fillRect(m_x + i*m_nTabWidth, m_y, m_nTabWidth - ((i==(m_nTabCount-1)) ? 0: 1), (i==m_nCurTab) ? m_height : 10);
    }
    m_bDirty = false;
  }
}

bool TabButton::verifyPressed(int x, int y, long timestamp) {
  for (int i=0; i<m_nTabCount;i++) {
    if (x>=(m_x+i*m_nTabWidth) && x<=(m_x+(i+1)*m_nTabWidth) && y >= m_y && y <= m_y+m_height) {
      if (m_nCurTab != i) {
        m_nPrevTab = m_nCurTab;
        m_nCurTab = i;
        m_bDirty = true;
      }
      return Button::verifyPressed(x, y, timestamp);
    }
  }
  return false;
}

bool TabButton::verifyReleased(int x, int y, long timestamp) {
  for (int i=0; i<m_nTabCount;i++) {
    if (x>=(m_x+i*m_nTabWidth) && x<=(m_x+(i+1)*m_nTabWidth) && y >= m_y && y <= m_y+m_height) {
      if (m_nCurTab != i) {
        m_nPrevTab = m_nCurTab;
        m_nCurTab = i;
        m_bDirty = true;
      }
      return Button::verifyReleased(x, y, timestamp);
    }
  }
  return false;
}
