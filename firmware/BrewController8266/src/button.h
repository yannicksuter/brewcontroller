#ifndef _BUTTON_
#define _BUTTON_

typedef void (*callbackPtr)(int);

class Button {
  public:
    Button(int id, int x, int y, int width, int heigt, const char *xpm_up, const char *xpm_down);

    void enable();
    void disable();
    void draw(bool forceDraw);

    void registerCallbacks(callbackPtr pressed, callbackPtr released);
    bool verifyPressed(int x, int y);
    bool verifyReleased(int x, int y);

  private:
    void changeState(bool bPressed);

    int m_id;
    int m_x, m_y;
    int m_width, m_height;
    const char* m_pXPM_Up;
    const char* m_pXPM_Down;
    bool m_bEnabled;
    bool m_bIsPressedState;
    bool m_bDirty;

    callbackPtr m_pFncPressed;
    callbackPtr m_pFncReleased;
};

#endif
