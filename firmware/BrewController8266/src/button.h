#ifndef _BUTTON_
#define _BUTTON_

#define BUTTON_LONG_PRESS_DELAY 3000

class Button;
typedef void (*eventCallbackPtr)(int, Button*);

class Button {
  public:
    Button(int id, int x, int y, int width, int height);
    Button(int id, int x, int y, int width, int height, const char *xpm_up, const char *xpm_down);
    Button(int id, int x, int y, int width, int height, const char *xpm_up_state1, const char *xpm_down_state1, const char *xpm_up_state2, const char *xpm_down_state2);

    void enable();
    void disable();
    virtual void draw(bool forceDraw);

    void setToggleState(uint8_t state);

    inline long getLongPressCounter() { return m_nLongPressCounter; }

    inline void registerPressCallback(eventCallbackPtr pPressedCB) { m_pCallbackPressed = pPressedCB; };
    inline void registerLongPressCallback(eventCallbackPtr pLongPressedCB) { m_pCallbackLongPressed = pLongPressedCB; };
    inline void registerReleaseCallback(eventCallbackPtr pReleasedCB) { m_pCallbackReleased = pReleasedCB; };

    virtual bool verifyPressed(int x, int y, long timestamp);
    virtual bool verifyReleased(int x, int y, long timestamp);

  protected:
    void changeState(bool bIsPressed, long timestamp);

    int m_id;
    int m_x, m_y;
    int m_width, m_height;

    bool m_bToggleButton;
    uint8_t m_nToggleButtonState;
    const char* m_pStateImagesUp[2];
    const char* m_pStateImagesDown[2];

    bool m_bEnabled;
    bool m_bIsPressedState;
    bool m_bDirty;

    long m_nLongPressStartTime;
    long m_nLongPressCounter;

    eventCallbackPtr m_pCallbackPressed;
    eventCallbackPtr m_pCallbackLongPressed;
    eventCallbackPtr m_pCallbackReleased;
};

#endif
