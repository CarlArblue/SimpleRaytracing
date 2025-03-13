//
// Created by alex on 3/13/25.
//

// LambertianBSDF.h
#ifndef LAMBERTIANBSDF_H
#define LAMBERTIANBSDF_H

#include "BSDF.h"
#include "SamplingHelpers.h" // For random_in_hemisphere
#include <glm/glm.hpp>
#include <algorithm>

class LambertianBSDF : public BSDF {
public:
    Spectrum diffuseColor;

    LambertianBSDF(const Spectrum& color) : diffuseColor(color) {}

    // For a Lambertian surface, the BSDF is diffuseColor / pi.
    Spectrum evaluate(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& normal) const override {
        float cosine = std::max(0.0f, glm::dot(normal, wo));
        // Divide by PI (assumes cosine weighting in the integration)
        return diffuseColor * (1.0f / M_PI) * cosine;
    }

    // Sample a new direction uniformly over the hemisphere
    glm::vec3 sample(const glm::vec3& wi, const glm::vec3& normal, float& pdf) const override {
        glm::vec3 sampledDir = random_in_hemisphere(normal);
        // For a uniform hemisphere, pdf is 1/(2*pi)
        pdf = 1.0f / (2.0f * M_PI);
        return sampledDir;
    }
};

#endif // LAMBERTIANBSDF_H
