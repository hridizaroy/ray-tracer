#ifndef MYIMAGE_H
#define MYIMAGE_H

#include "config.h"
#include <SDL.h>
#include <SDL_main.h>



class MyImage
{
public:
	MyImage();

	~MyImage();

	void initialize(const int xSize, const int ySize, SDL_Renderer* pRenderer);

	void setPixel(const int x, const int y, const double r, const double g, const double b);

	void display();

	glm::vec4 perPixel(glm::vec2 coord);

private:
	uint32_t convertColor(glm::vec4 color);
	void initTexture();

	std::vector<std::vector<double>> m_rChannel;
	std::vector<std::vector<double>> m_gChannel;
	std::vector<std::vector<double>> m_bChannel;


	int m_xSize, m_ySize;

	SDL_Renderer* m_pRenderer;
	SDL_Texture* m_pTexture;
};

#endif