#include <Arduino.h>
#include "button.h"
// #include "rendering.h"
extern void drawXPM(int16_t pos_x, int16_t pos_y, int16_t width, int16_t height, const char *xpm);

Button::Button(int x, int y, int width, int heigt, const char *xpm_up, const char *xpm_down):
  m_pXPM_Up(xpm_up), m_pXPM_Down(xpm_down)
{
  m_x = x;
  m_y = y;
  m_width = width;
  m_height = heigt;
  m_bEnabled = true;
  m_bStateUp = true;
}

void Button::enable() {
  m_bEnabled = true;
}

void Button::disable() {
  m_bEnabled = false;
}

void Button::draw() {
  if (m_bEnabled && m_bStateUp) {
    drawXPM(m_x, m_y, m_width, m_height, m_pXPM_Up);
  } else {
    drawXPM(m_x, m_y, m_width, m_height, m_pXPM_Down);
  }
}
