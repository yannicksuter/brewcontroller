#include <Arduino.h>
#include "button.h"

extern void drawXPM(int16_t pos_x, int16_t pos_y, int16_t width, int16_t height, const char *xpm);

Button::Button(int id, int x, int y, int width, int height):
  Button(id, x, y, width, height, NULL, NULL, NULL, NULL)
{
  m_bToggleButton = false;
}

Button::Button(int id, int x, int y, int width, int height, const char *state1_up, const char *state1_down):
  Button(id, x, y, width, height, state1_up, state1_down, NULL, NULL)
{
  m_bToggleButton = false;
}

Button::Button(int id, int x, int y, int width, int height, const char *state1_up, const char *state1_down, const char *state2_up, const char *state2_down)
{
  m_id = id;
  m_x = x;
  m_y = y;
  m_width = width;
  m_height = height;
  m_bEnabled = true;
  m_bDirty = true;
  m_pCallbackPressed = NULL;
  m_pCallbackReleased = NULL;
  m_pCallbackLongPressed = NULL;
  m_bPressedEvent = false;
  m_bLongPressedEvent = false;
  m_bReleasedEvent = false;

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

void Button::draw(bool forceDraw) {
  if (m_bDirty || forceDraw) {
    if (m_bEnabled && !m_bPressedEvent) {
      drawXPM(m_x, m_y, m_width, m_height, m_pStateImagesUp[m_nToggleButtonState]);
    } else {
      drawXPM(m_x, m_y, m_width, m_height, m_pStateImagesDown[m_nToggleButtonState]);
    }
    m_bDirty = false;
  }
}

void Button::changePressedState(bool bIsPressed, long timestamp) {
  m_bDirty = (m_bPressedEvent != bIsPressed);

  // if new press state, we have to reset long press counter
  if (bIsPressed && !m_bPressedEvent) {
    m_nLongPressStartTime = timestamp;
    m_nLongPressCounter = 0;
  }

  m_bPressedEvent = bIsPressed;
}

bool Button::verifyPressed(int x, int y, long timestamp) {
  m_bLongPressedEvent = false;
  if (m_bEnabled && x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y+m_height) {
    changePressedState(true, timestamp);
    if (m_pCallbackPressed) {
      m_bPressedEvent = true;
      m_pCallbackPressed(m_id, this);
    }
    if ((timestamp-m_nLongPressStartTime) > BUTTON_LONG_PRESS_DELAY && m_pCallbackLongPressed) {
      m_bLongPressedEvent = true;
      m_pCallbackLongPressed(m_id, this);
    }
    return true;
  }
  changePressedState(false, timestamp);
  return false;
}

bool Button::verifyReleased(int x, int y, long timestamp) {
  m_bReleasedEvent = false;
  if (m_bEnabled && x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y+m_height) {
    m_bReleasedEvent = true;
    changePressedState(false, timestamp);
    return true;
  }
  return false;
}
