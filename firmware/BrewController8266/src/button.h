#ifndef _BUTTON_
#define _BUTTON_

class Button;
typedef void (*eventCallbackPtr)(int, Button*);

class Button {
  public:
    Button(int id, int x, int y, int width, int heigt, const char *xpm_up, const char *xpm_down);
    Button(int id, int x, int y, int width, int heigt, const char *xpm_up_state1, const char *xpm_down_state1, const char *xpm_up_state2, const char *xpm_down_state2);

    void enable();
    void disable();
    void draw(bool forceDraw);

    void setToggleState(uint8_t state);

    void registerCallbacks(eventCallbackPtr pressed, eventCallbackPtr released);
    bool verifyPressed(int x, int y);
    bool verifyReleased(int x, int y);

  private:
    void changeState(bool bPressed);

    int m_id;
    int m_x, m_y;
    int m_width, m_height;

    bool m_bToggleButton;
    uint8_t m_nToggleButtonState;
    const char* m_pStateImagesUp[2];
    const char* m_pStateImagesDown[2];
    // const char* m_pXPM_Up;
    // const char* m_pXPM_Down;
    bool m_bEnabled;
    bool m_bIsPressedState;
    bool m_bDirty;

    eventCallbackPtr m_pCallbackPressed;
    eventCallbackPtr m_pCallbackReleased;
};

#endif
