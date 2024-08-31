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

        // Seed with a real random value, if available
        std::random_device rd;

        // Initialize the random number generator with a seed
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> distr(0, 255);

        for (int x = 0; x < WIDTH; x++)
        {
            for (int y = 0; y < HEIGHT; y++)
            {
                double r = (static_cast<double>(x) / WIDTH) * 255;
                double g = (static_cast<double>(y) / HEIGHT) * 255;
                
                m_image.setPixel(x, y, distr(gen), distr(gen), distr(gen));
            }
        }
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

    m_image.display();

    SDL_RenderPresent(m_renderer);
}

void MyApp::OnExit()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_window = NULL;
    SDL_Quit();
}
