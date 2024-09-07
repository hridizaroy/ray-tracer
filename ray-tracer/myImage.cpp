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
	uint32_t *tempPixels = new uint32_t[m_xSize * m_ySize]();

	for (int y = 0; y < m_xSize; y++)
	{
		for (int x = 0; x < m_ySize; x++)
		{
			glm::vec2 coord;
			coord.x = static_cast<double>(x) / m_xSize;
			coord.y = static_cast<double>(y) / m_ySize;

			glm::vec4 color = perPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0), glm::vec4(1.0));
			tempPixels[(y * m_xSize) + x] = convertColor(color);

			//tempPixels[(y * m_xSize) + x] = convertColor(m_rChannel.at(x).at(y), m_gChannel.at(x).at(y), m_bChannel.at(x).at(y));
		}
	}

	// NULL makes sure that the entire texture is updated
	// Last param is number of bytes in a row
	SDL_UpdateTexture(m_pTexture, NULL, tempPixels, m_xSize * sizeof(uint32_t));

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


uint32_t MyImage::convertColor(glm::vec4 color)
{
	color = glm::clamp(color, glm::vec4(0.0), glm::vec4(1.0));

	uint8_t r = (uint8_t)(color.r * 255.0);
	uint8_t g = (uint8_t)(color.g * 255.0);
	uint8_t b = (uint8_t)(color.b * 255.0);
	uint8_t a = (uint8_t)(color.a * 255.0);

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	uint32_t pixelColor = (r << 24) | (g << 16) | (b << 8) | a;
	#else
	uint32_t pixelColor = (a << 24) | (r << 16) | (g << 8) | b;
	#endif

	return pixelColor;
}


glm::vec4 MyImage::perPixel(glm::vec2 coord)
{
	// TODO: Check if hit is behind camera
	// TODO: Check if camera is "inside" the sphere, i.e., ts are of opposite signs
	// TODO: Create a loop to check closest hit
	// Sphere-ray intersection
	glm::vec3 color;

	glm::vec3 center(0.5, 0.5, 0.0);
	glm::vec3 center2(0.7, 0.9, 0.2);

	glm::vec3 light(0.5, 0.7, 1.0);

	glm::vec3 coord_3d(coord, 0.0);

	glm::vec3 rayDir = glm::normalize(coord_3d - light);

	double radius = 0.3;
	double radius2 = 0.25;

	double a = glm::dot(rayDir, rayDir);
	double b = 2.0 * glm::dot(rayDir, light - center);
	double c = glm::dot(light - center, light - center) - radius * radius;
	double discriminant = b * b - 4 * a * c;

	double a2 = glm::dot(rayDir, rayDir);
	double b2 = 2.0 * glm::dot(rayDir, light - center2);
	double c2 = glm::dot(light - center2, light - center2) - radius2 * radius2;
	double discriminant2 = b2 * b2 - 4 * a2 * c2;

	// Generating color based on normal at hit point
	if (discriminant >= 0 && discriminant2 < 0)
	{
		// Point of intersection - center gives normal vector direction
		float t = (-b - std::sqrt(discriminant)) / (2 * a);

		if (t > 0)
		{
			glm::vec3 normal = glm::normalize(light + (t * rayDir) - center);

			color = 0.5f * (normal + glm::vec3(1));

			// Calculate color based on how much the point is facing the light
			// get cos of angle between normal and light
			float cosTheta = glm::max(glm::dot(normal, -rayDir), 0.0f);

			color = cosTheta * glm::vec3(1, 0, 0);
		}
		else
		{
			color = glm::vec3(0.0);
		}
	}
	else if (discriminant2 >= 0 && discriminant < 0)
	{
		// Point of intersection - center gives normal vector direction
		float t = (-b2 - std::sqrt(discriminant2)) / (2 * a2);

		if (t > 0)
		{
			glm::vec3 normal = glm::normalize(light + (t * rayDir) - center2);

			color = 0.5f * (normal + glm::vec3(1));

			// Calculate color based on how much the point is facing the light
			// get cos of angle between normal and light
			float cosTheta = glm::max(glm::dot(normal, -rayDir), 0.0f);

			color = cosTheta * glm::vec3(1, 0, 1);
		}
		else
		{
			color = glm::vec3(0.0);
		}
	}
	else if (discriminant >= 0 && discriminant2 >= 0)
	{
		// Point of intersection - center gives normal vector direction
		float t = (-b - std::sqrt(discriminant)) / (2 * a);
		float t2 = (-b2 - std::sqrt(discriminant2)) / (2 * a2);

		if (t < t2)
		{
			glm::vec3 normal = glm::normalize(light + (t * rayDir) - center);

			color = 0.5f * (normal + glm::vec3(1));

			// Calculate color based on how much the point is facing the light
			// get cos of angle between normal and light
			float cosTheta = glm::max(glm::dot(normal, -rayDir), 0.0f);

			color = cosTheta * glm::vec3(1, 0, 0);
		}
		else
		{
			glm::vec3 normal = glm::normalize(light + (t2 * rayDir) - center2);

			color = 0.5f * (normal + glm::vec3(1));

			// Calculate color based on how much the point is facing the light
			// get cos of angle between normal and light
			float cosTheta = glm::max(glm::dot(normal, -rayDir), 0.0f);

			color = cosTheta * glm::vec3(1, 0, 1);
		}
	}
	else
	{
		color = glm::vec3(0.0);
	}

	return glm::vec4(color, 1);
}