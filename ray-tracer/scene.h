#pragma once

#include "config.h"


struct Material
{
	glm::vec3 albedo{ 1.0f }; // diffuse color
	float roughness = 1.0f;
	float metallic = 0.0f;
};

struct Sphere
{
	glm::vec3 center{ 0.0f };
	float radius = 0.5f;

	Material mat;
};

struct Scene
{
	std::vector<Sphere> spheres;
};