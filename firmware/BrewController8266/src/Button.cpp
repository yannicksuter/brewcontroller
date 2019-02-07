#include <Arduino.h>
#include "button.h"
// #include "rendering.h"
extern void drawXPM(int16_t pos_x, int16_t pos_y, int16_t width, int16_t height, const char *xpm);

Button::Button(int id, int x, int y, int width, int heigt, const char *xpm_up, const char *xpm_down):
  Button(id, x, y, width, heigt, xpm_up, xpm_down, NULL, NULL)
{
  m_bToggleButton = false;
}

Button::Button(int id, int x, int y, int width, int heigt, const char *state1_up, const char *state1_down, const char *state2_up, const char *state2_down)
{
  m_id = id;
  m_x = x;
  m_y = y;
  m_width = width;
  m_height = heigt;
  m_bEnabled = true;
  m_bIsPressedState = false;
  m_bDirty = true;
  m_pCallbackPressed = NULL;
  m_pCallbackReleased = NULL;

  m_bToggleButton = true;
  m_nToggleButtonState = 0;
  m_pStateImagesUp[0] = state1_up;
  m_pStateImagesUp[1] = state2_up;
  m_pStateImagesDown[0] = state1_down;
  m_pStateImagesDown[1] = state2_down;
}

void Button::setToggleState(uint8_t state) {
  if (m_bToggleButton) {
    m_nToggleButtonState = state;
    m_bDirty = true;
  }
}

void Button::enable() {
  m_bEnabled = true;
}

void Button::disable() {
  m_bEnabled = false;
}

void Button::draw(bool forceDraw) {
  if (m_bDirty || forceDraw) {
    if (m_bEnabled && !m_bIsPressedState) {
      drawXPM(m_x, m_y, m_width, m_height, m_pStateImagesUp[m_nToggleButtonState]);
    } else {
      drawXPM(m_x, m_y, m_width, m_height, m_pStateImagesDown[m_nToggleButtonState]);
    }
    m_bDirty = false;
  }
}

void Button::changeState(bool bPressed) {
  m_bDirty = (m_bIsPressedState != bPressed);
  m_bIsPressedState = bPressed;
}

void Button::registerCallbacks(eventCallbackPtr pressed, eventCallbackPtr released) {
  m_pCallbackPressed = pressed;
  m_pCallbackReleased = released;
}

bool Button::verifyPressed(int x, int y) {
 if (m_bEnabled && x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y+m_height) {
    if (m_pCallbackPressed) {
      m_pCallbackPressed(m_id, this);
    }
    changeState(true);
    return true;
 }
 changeState(false);
 return false;
}

bool Button::verifyReleased(int x, int y) {
 if (m_bEnabled && x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y+m_height) {
   if (m_pCallbackReleased) {
     m_pCallbackReleased(m_id, this);
   }
   changeState(false);
    return true;
 }
 return false;
}
