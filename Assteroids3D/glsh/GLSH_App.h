#ifndef GLSH_APP_H_
#define GLSH_APP_H_

#include "GLSH_Event.h"

namespace glsh {

// forward declarations
class Window;
class Keyboard;
class Mouse;

/**
*/
class App {
public:
    virtual             ~App();

    //
    // the interface that each application must implement
    //

    virtual bool        initialize(int w, int h)    = 0;
    virtual void        shutdown()                  = 0;
    virtual void        resize(int w, int h)        = 0;
    virtual void        draw()                      = 0;
    virtual void        update(float dt)            = 0;

    //
    // event delivery interface
    //

    virtual void        onKeyDown(const KeyEvent& ke)   {}
    virtual void        onKeyUp(const KeyEvent& ke)     {}

    virtual void        onMouseDown(const MouseButtonEvent& mbe)    {}
    virtual void        onMouseUp(const MouseButtonEvent& mbe)      {}

    virtual void        onMouseMotion(const MouseMotionEvent& mbe)  {}
    virtual void        onMouseScroll(const MouseScrollEvent& mbe)  {}


    //
    // some useful stuff
    //

    const Window*       getWindow() const;

    const Keyboard*     getKeyboard() const;
    const Mouse*        getMouse() const;

    float               getTime() const;    // time since the app main loop started

    void                quit();             // request to quit

    bool                isRunning() const;
    bool                isQuitting() const;

    //
    // internals; regular clients shouldn't call these methods
    //

    void                _setWindow(const Window* wnd);

protected:
                        App();

private:
                        App(const App&);
                        App& operator= (const App&);

    const Window*       mWindow;
    bool                mShouldQuit;
};

}

#endif
