#include "renderer.h"

Renderer::Renderer()
{
	m_xSize = 0;
	m_ySize = 0;
	m_pTexture = NULL;
	m_pRenderer = NULL;
}

Renderer::~Renderer()
{
	if (m_pTexture != NULL)
	{
		SDL_DestroyTexture(m_pTexture);
	}
}

void Renderer::initialize(const int xSize, const int ySize, SDL_Renderer* pRenderer)
{
	m_xSize = xSize;
	m_ySize = ySize;

	m_pRenderer = pRenderer;

	initTexture();
}

void Renderer::display(const Scene& scene, glm::vec3 camPos)
{
	m_scene = &scene;
	m_camPos = camPos;

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


void Renderer::initTexture()
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


uint32_t Renderer::convertColor(glm::vec4 color)
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


glm::vec4 Renderer::perPixel(glm::vec2 coord)
{
	glm::vec3 coord_3d(coord, 0.0);

	Ray ray;
	ray.origin = m_camPos;	
	ray.direction = glm::normalize(coord_3d - ray.origin);

	Renderer::HitPayload payload = traceRay(ray);

	// Return black if we didn't hit anything
	if (payload.hitDistance < 0.0f)
	{
		return glm::vec4(0.0, 0.0f, 0.0f, 1.0f);
	}

	// Calculate color based on how much the point is facing the light
	// get cos of angle between normal and light
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	float cosTheta = glm::max(glm::dot(payload.worldNormal, -lightDir), 0.0f);
	float lightIntensity = glm::max(cosTheta, 0.0f);

	const Sphere& sphere = m_scene->spheres[payload.objectIndex];
	glm::vec3 sphereColor = sphere.albedo;
	sphereColor *= lightIntensity;

	return glm::vec4(sphereColor, 1.0f);
}


Renderer::HitPayload Renderer::traceRay(const Ray& ray)
{
	// TODO: Check if camera is "inside" the sphere, i.e., ts are of opposite signs
	// TODO: Maybe check if a + tb - center is <= radius or something

	// Sphere-ray intersection
	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();

	// Check which sphere is the closest to the camera and at what distance
	for (size_t ii = 0; ii < m_scene->spheres.size(); ii++)
	{
		const Sphere& sphere = m_scene->spheres[ii];
		glm::vec3 raySphereToCam = ray.origin - sphere.center;

		double a = glm::dot(ray.direction, ray.direction);
		double b = 2.0 * glm::dot(ray.direction, raySphereToCam);
		double c = glm::dot(raySphereToCam, raySphereToCam) - sphere.radius * sphere.radius;
		double discriminant = b * b - 4 * a * c;

		if (discriminant < 0.0f)
		{
			continue;
		}

		float closestT = (-b - std::sqrt(discriminant)) / (2.0f * a);

		// Make sure that the hit is in front of the camera
		if (closestT < hitDistance && closestT > 0.0f)
		{
			hitDistance = closestT;
			closestSphere = (int)ii;
		}
	}

	// if we didn't hit anything
	if (closestSphere < 0)
	{
		return miss(ray);
	}

	return closestHit(ray, hitDistance, closestSphere);
}


Renderer::HitPayload Renderer::closestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.hitDistance = hitDistance;
	payload.objectIndex = objectIndex;


	const Sphere& closestSphere = m_scene->spheres[objectIndex];
	payload.worldPosition = ray.origin + ray.direction * hitDistance; // a + bt
	glm::vec3 sphereCenterToHitpoint = payload.worldPosition - closestSphere.center;
	payload.worldNormal = glm::normalize(sphereCenterToHitpoint);
	
	return payload;
}


Renderer::HitPayload Renderer::miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.hitDistance = -1.0f;
	return payload;
}