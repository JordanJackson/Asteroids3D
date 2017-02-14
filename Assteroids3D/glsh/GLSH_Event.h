#ifndef GLSH_EVENT_H_
#define GLSH_EVENT_H_

namespace glsh {
/**
    \enum KeyCode
    \brief Enumeration of common key codes.
*/
// Yikes... http://en.wikipedia.org/wiki/Keyboard_layout
enum KeyCode {
    KC_UNKNOWN,

    // common ASCII keys
    KC_SPACE, KC_TAB, KC_RETURN, KC_ESCAPE, KC_BACKSPACE, KC_DELETE,
    KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
    KC_A, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G, KC_H, KC_I, KC_J, KC_K, KC_L, KC_M,
    KC_N, KC_O, KC_P, KC_Q, KC_R, KC_S, KC_T, KC_U, KC_V, KC_W, KC_X, KC_Y, KC_Z,

    // common special keys
    KC_UP, KC_DOWN, KC_RIGHT, KC_LEFT, // arrows
    KC_PAGE_UP, KC_PAGE_DOWN, KC_HOME, KC_END, KC_INSERT, // misc navigation
    KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, // function keys
    
    // modifier keys
    KC_SHIFT, KC_SHIFT_L, KC_SHIFT_R, 
    KC_CTRL, KC_CTRL_L, KC_CTRL_R,
    KC_ALT, KC_ALT_L, KC_ALT_R,

    KC_NUM_KEYS
};


/**
    \enum MouseButton
    \brief Enumeration of common mouse buttons.
*/
enum MouseButton {
    MOUSE_BUTTON_UNKNOWN,

    MOUSE_BUTTON_LEFT, MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_RIGHT,

    MOUSE_NUM_BUTTONS
};


enum EventType {
    KEY_UP_EVENT,
    KEY_DOWN_EVENT,
    MOUSE_UP_EVENT,
    MOUSE_DOWN_EVENT,
    MOUSE_MOTION_EVENT,
    MOUSE_SCROLL_EVENT,
};

struct KeyEvent {
    KeyCode key;            // key code
};

struct MouseButtonEvent {
    MouseButton button;     // button id
    int x, y;               // cursor coordinates
};

struct MouseMotionEvent {
    int dx, dy;             // cursor displacement
};

struct MouseScrollEvent {
    int delta;              // wheel displacement
};

struct InputEvent {
    EventType type;
    union {
        KeyEvent ke;
        MouseButtonEvent mbe;
        MouseMotionEvent mme;
        MouseScrollEvent mse;
    };
};

} // end of namespace glsh

#endif
