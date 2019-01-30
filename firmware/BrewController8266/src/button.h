#ifndef _BUTTON_
#define _BUTTON_

class Button {
  public:
    Button(int x, int y, int width, int heigt, const char *xpm_up, const char *xpm_down);

    void enable();
    void disable();
    void draw();

  private:
    int m_x, m_y;
    int m_width, m_height;
    const char* m_pXPM_Up;
    const char* m_pXPM_Down;
    bool m_bEnabled;
    bool m_bStateUp;
};

#endif
