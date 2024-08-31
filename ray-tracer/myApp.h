#ifndef MYAPP_H
#define MYAPP_H

#include <SDL.h>
#include <SDL_main.h>
#include "myImage.h"

class MyApp
{
public:
    MyApp();

    int onExecute();
    bool OnInit();
    void OnEvent(SDL_Event* event);
    void OnLoop();
    void OnRender();
    void OnExit();

private:
    bool m_isRunning;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    MyImage m_image;
};
#endif