#pragma once

#include "config.h"

struct Sphere
{
	glm::vec3 center{ 0.0f };
	float radius = 0.5f;

	glm::vec3 albedo{ 1.0f }; // Diffuse color
};

struct Scene
{
	std::vector<Sphere> spheres;
};