#include "myImage.h"

MyImage::MyImage()
{
	m_xSize = 0;
	m_ySize = 0;
	m_pTexture = NULL;
	m_pRenderer = NULL;
}

MyImage::~MyImage()
{
	if (m_pTexture != NULL)
	{
		SDL_DestroyTexture(m_pTexture);
	}
}

void MyImage::initialize(const int xSize, const int ySize, SDL_Renderer* pRenderer)
{
	m_rChannel.resize(xSize, std::vector<double>(ySize, 0.0));
	m_gChannel.resize(xSize, std::vector<double>(ySize, 0.0));
	m_bChannel.resize(xSize, std::vector<double>(ySize, 0.0));

	m_xSize = xSize;
	m_ySize = ySize;

	m_pRenderer = pRenderer;

	initTexture();
}

void MyImage::setPixel(const int x, const int y, const double r, const double g, const double b)
{
	m_rChannel.at(x).at(y) = r;
	m_gChannel.at(x).at(y) = g;
	m_bChannel.at(x).at(y) = b;
}

void MyImage::display()
{
	// initialize tempPixels to 0
	Uint32 *tempPixels = new Uint32[m_xSize * m_ySize]();

	for (int y = 0; y < m_xSize; y++)
	{
		for (int x = 0; x < m_ySize; x++)
		{
			glm::vec2 coord;
			coord.x = static_cast<double>(x) / m_xSize;
			coord.y = static_cast<double>(y) / m_ySize;

			tempPixels[(y * m_xSize) + x] = perPixel( coord );

			//tempPixels[(y * m_xSize) + x] = convertColor(m_rChannel.at(x).at(y), m_gChannel.at(x).at(y), m_bChannel.at(x).at(y));
		}
	}

	// NULL makes sure that the entire texture is updated
	// Last param is number of bytes in a row
	SDL_UpdateTexture(m_pTexture, NULL, tempPixels, m_xSize * sizeof(Uint32));

	delete[] tempPixels;

	SDL_Rect srcRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = m_xSize;
	srcRect.h = m_ySize;

	SDL_RenderCopy(m_pRenderer, m_pTexture, &srcRect, &srcRect);
}


void MyImage::initTexture()
{
	Uint32 rMask, gMask, bMask, aMask;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rMask = 0xff000000;
	gMask = 0x00ff0000;
	bMask = 0x0000ff00;
	aMask = 0x000000ff;
	#else
	rMask = 0x000000ff;
	gMask = 0x0000ff00;
	bMask = 0x00ff0000;
	aMask = 0xff000000;
	#endif

	if (m_pTexture != NULL)
	{
		SDL_DestroyTexture(m_pTexture);
	}

	// 32 = number of bits per pixel
	SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, m_xSize, m_ySize, 32, rMask, gMask, bMask, aMask);

	m_pTexture = SDL_CreateTextureFromSurface(m_pRenderer, tempSurface);

	SDL_FreeSurface(tempSurface);
}


Uint32 MyImage::convertColor(const double r, const double g, const double b)
{
	unsigned char r_char = static_cast<unsigned char>(r);
	unsigned char g_char = static_cast<unsigned char>(g);
	unsigned char b_char = static_cast<unsigned char>(b);

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 pixelColor = (r_char << 24) + (g_char << 16) + (b_char << 8) + 0xFF;
	#else
	Uint32 pixelColor = (0xFF << 24) + (r_char << 16) + (g_char << 8) + b_char;
	#endif

	return pixelColor;
}


Uint32 MyImage::perPixel(glm::vec2 coord)
{
	double red, green, blue;

	// Sphere-ray intersection

	glm::vec3 center(0.5, 0.5, 0.0);

	glm::vec3 light(0.5, 0.5, 2.0);

	glm::vec3 coord_3d(coord, 0.0);

	glm::vec3 rayDir = coord_3d - light;

	double radius = 0.3;

	double a = glm::dot(rayDir, rayDir);
	double b = 2.0 * glm::dot(rayDir, light - center);
	double c = glm::dot(light - center, light - center) - radius * radius;
	double ans = b * b - 4 * a * c;

	// Generating color based on normal at hit point
	if (ans >= 0)
	{
		// Point of intersection - center gives normal vector direction
		float t = (-b - std::sqrt(ans)) / (2 * a);
		glm::vec3 normal = glm::normalize(light + (t * rayDir) - center);

		red = 0.5 * (normal.x + 1) * 255.0;
		green = 0.5 * (normal.y + 1) * 255.0;
		blue = 0.5 * (normal.z + 1) * 255.0;
	}
	else
	{
		red = 255;
		green = 255;
		blue = 255;
	}

	return convertColor(red, green, blue);
}