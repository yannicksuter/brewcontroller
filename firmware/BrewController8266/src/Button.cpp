#include <Arduino.h>
#include "button.h"
// #include "rendering.h"
extern void drawXPM(int16_t pos_x, int16_t pos_y, int16_t width, int16_t height, const char *xpm);

Button::Button(int id, int x, int y, int width, int heigt, const char *xpm_up, const char *xpm_down):
  m_pXPM_Up(xpm_up), m_pXPM_Down(xpm_down)
{
  m_id = id;
  m_x = x;
  m_y = y;
  m_width = width;
  m_height = heigt;
  m_bEnabled = true;
  m_bIsPressedState = false;
  m_bDirty = true;
  m_pFncPressed = NULL;
  m_pFncReleased = NULL;
}

void Button::enable() {
  m_bEnabled = true;
}

void Button::disable() {
  m_bEnabled = false;
}

void Button::draw(bool forceDraw) {
  if (m_bDirty || forceDraw) {
    if (m_bEnabled && m_bIsPressedState) {
      drawXPM(m_x, m_y, m_width, m_height, m_pXPM_Up);
    } else {
      drawXPM(m_x, m_y, m_width, m_height, m_pXPM_Down);
    }
    m_bDirty = false;
  }
}

void Button::changeState(bool bPressed) {
  m_bDirty = (m_bIsPressedState != bPressed);
  m_bIsPressedState = bPressed;
}

void Button::registerCallbacks(callbackPtr pressed, callbackPtr released) {
  m_pFncPressed = pressed;
  m_pFncReleased = released;
}

bool Button::verifyPressed(int x, int y) {
 if (m_bEnabled && x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y+m_height) {
    if (m_pFncPressed) {
      m_pFncPressed(m_id);
    }
    changeState(true);
    return true;
 }
 changeState(false);
 return false;
}

bool Button::verifyReleased(int x, int y) {
 if (m_bEnabled && x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y+m_height) {
   if (m_pFncReleased) {
     m_pFncReleased(m_id);
   }
   changeState(false);
    return true;
 }
 return false;
}
