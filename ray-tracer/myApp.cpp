#include "myApp.h"
#include <random>

MyApp::MyApp()
{
    m_isRunning = true;
    m_window = NULL;
    m_renderer = NULL;
}

bool MyApp::OnInit()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return false;
    }

    int WIDTH = 600;
    int HEIGHT = 600;

    m_window = SDL_CreateWindow("Hridiza's Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if (m_window != NULL)
    {
        m_renderer = SDL_CreateRenderer(m_window, -1, 0);

        m_image.initialize(WIDTH, HEIGHT, m_renderer);
    }
    else
    {
        return false;
    }

    return true;
}

int MyApp::onExecute()
{
    SDL_Event event;

    if (!OnInit())
    {
        return -1;
    }

    while (m_isRunning)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            OnEvent(&event);
        }

        OnLoop();
        OnRender();
    }

    return 0;
}


void MyApp::OnEvent(SDL_Event* event)
{
    if (event->type == SDL_QUIT)
    {
        m_isRunning = false;
    }
}

void MyApp::OnLoop()
{

}

void MyApp::OnRender()
{
    // Set BG Color
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 0);
    SDL_RenderClear(m_renderer);

    Sphere sphere1;
    sphere1.center = glm::vec3(0.9f, 0.4f, -1.5f);
    sphere1.radius = 0.3;
    sphere1.albedo = glm::vec3(0.0f, 1.0f, 0.0f);

    Sphere sphere2;
    sphere2.center = glm::vec3(0.2f, 0.6f, -1.5f);
    sphere2.radius = 0.4;
    sphere2.albedo = glm::vec3(0.0f, 1.0f, 1.0f);

    Scene scene;
    scene.spheres.push_back(sphere1);
    scene.spheres.push_back(sphere2);

    glm::vec3 camPos{ 0.5f, 0.8f, 5.0f };

    m_image.display(scene, camPos);

    SDL_RenderPresent(m_renderer);
}

void MyApp::OnExit()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_window = NULL;
    SDL_Quit();
}
