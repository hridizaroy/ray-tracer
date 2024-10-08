#pragma once

#include "config.h"


struct Material
{
	glm::vec3 albedo{ 1.0f }; // diffuse color
	float roughness = 1.0f;
	float metallic = 0.0f;

	glm::vec3 emissionColor{ 0.0f };
	float emissionPower = 0.0f;

	glm::vec3 getEmission() const { return emissionPower * emissionColor; };
};

struct Sphere
{
	glm::vec3 center{ 0.0f };
	float radius = 0.5f;

	uint32_t materialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> spheres;
	std::vector<Material> materials;
};