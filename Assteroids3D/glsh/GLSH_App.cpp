#include "GLSH_App.h"
#include "GLSH_System.h"

#include <cstddef>

namespace glsh {

App::App() 
    : mWindow(NULL)
    , mShouldQuit(false)
{
}

App::~App()
{
}

void App::_setWindow(const Window* window)
{
    mWindow = window;
}

const Window* App::getWindow() const
{
    return mWindow;
}

const Keyboard* App::getKeyboard() const
{
    return mWindow ? &mWindow->getKeyboard() : NULL;
}

const Mouse* App::getMouse() const
{
    return mWindow ? &mWindow->getMouse() : NULL;
}

float App::getTime() const
{
    return mWindow ? mWindow->getTime() : 0.0f;
}

bool App::isRunning() const
{
    return mWindow != NULL && !mShouldQuit;
}

bool App::isQuitting() const
{
    return mShouldQuit;
}

void App::quit()
{
    mShouldQuit = true;
}

} // end of namespace glsh
