//
// Created by alex on 3/10/25.
//

#include <random>
#include <glm/glm.hpp>
#include "SamplingHelpers.h"

// Returns a random float in [0, 1).
float random_float() {
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    static std::mt19937 generator(std::random_device{}());
    return distribution(generator);
}

// Generates a random unit vector in the hemisphere defined by the normal.
glm::vec3 random_in_hemisphere(const glm::vec3 &normal) {
    float u = random_float();
    float v = random_float();
    float theta = 2.0f * M_PI * u;
    float phi = acos(2.0f * v - 1.0f);

    glm::vec3 randomDir(
        sin(phi) * cos(theta),
        sin(phi) * sin(theta),
        cos(phi)
    );
    // Ensure the sample is in the same hemisphere as the normal.
    if (glm::dot(randomDir, normal) < 0.0f)
        randomDir = -randomDir;
    return glm::normalize(randomDir);
}
