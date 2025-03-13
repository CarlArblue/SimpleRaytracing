//
// Created by alex on 3/13/25.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "SpectralData.h"
#include <glm/glm.hpp>
#include <cmath>

// Ray
static constexpr int maxDepth = 5;              // Reflection recursion depth.
static constexpr int samplesPerPixel = 64;      // Increase for less noise.
static constexpr float shadowBias = 1e-4f;      // To avoid self-intersection.

// Scene
static const Spectrum backgroundSpectrum = Spectrum::fromRGB(glm::vec3(0.0f, 0.0f, 0.0f)); // Black background

// Camera
static constexpr float fov = M_PI / 3.0f; // 60° field of view.
static const float scale = std::tan(fov / 2.0f);

#endif //CONSTANTS_H
