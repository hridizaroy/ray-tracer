#include "myApp.h"

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

    Scene scene;

    // Materials
    Material& greenSphere = scene.materials.emplace_back();
    greenSphere.albedo = { 0.0f, 1.0f, 0.0f };
    greenSphere.roughness = 0.0f;

    Material& redSphere = scene.materials.emplace_back();
    redSphere.albedo = { 1.0f, 0.0f, 0.0f };
    redSphere.roughness = 0.8f;

    Material& emissiveSphere = scene.materials.emplace_back();
    emissiveSphere.albedo = { 0.8f, 0.5f, 0.2f };
    emissiveSphere.roughness = 0.1f;
    emissiveSphere.emissionColor = emissiveSphere.albedo;
    emissiveSphere.emissionPower = 2.0f;

    // Spheres
    Sphere sphere1;
    sphere1.center = glm::vec3(0.4f, 0.55f, 0.0f);
    sphere1.radius = 0.15f;
    sphere1.materialIndex = 0;

    Sphere sphere2;
    sphere2.center = glm::vec3(0.5f, 4.7f, 0.0f);
    sphere2.radius = 4.0f;
    sphere2.materialIndex = 1;

    Sphere sphere3;
    sphere3.center = glm::vec3(0.7f, 0.55f, 0.0f);
    sphere3.radius = 0.14f;
    sphere3.materialIndex = 2;

    
    scene.spheres.push_back(sphere1);
    scene.spheres.push_back(sphere2);
    scene.spheres.push_back(sphere3);

    glm::vec3 camPos{ 0.5f, 0.5f, 5.0f };

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
