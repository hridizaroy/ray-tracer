#ifndef MYIMAGE_H
#define MYIMAGE_H

#include "config.h"
#include <SDL.h>
#include <SDL_main.h>
#include "scene.h"
#include "ray.h"



class Renderer
{
public:
	struct Settings
	{
		bool accumulate = true;
	};
public:
	Renderer();

	~Renderer();

	void initialize(const int xSize, const int ySize, SDL_Renderer* pRenderer);

	void display(const Scene& scene, glm::vec3 camPos = { 0.5f, 0.5f, 5.0f });

private:
	struct HitPayload
	{
		float hitDistance;
		glm::vec3 worldPosition;
		glm::vec3 worldNormal;

		int objectIndex;
	};

	uint32_t convertColor(glm::vec4 color);
	void initTexture();

	glm::vec4 perPixel(glm::vec2 coord);
	HitPayload traceRay(const Ray& ray);
	HitPayload closestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload miss(const Ray& ray);

	glm::vec3 randomVec3(float min, float max);
	
	int m_xSize, m_ySize;

	SDL_Renderer* m_pRenderer;
	SDL_Texture* m_pTexture;

	const Scene* m_scene = nullptr;
	glm::vec3 m_camPos;

	glm::vec4* m_accumulationData = nullptr;
	uint32_t m_frameIndex = 1;

	Settings m_settings;

	std::vector<uint32_t> m_imageHorizontalIter, m_imageVerticalIter;
};

#endif