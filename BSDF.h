//
// Created by alex on 3/13/25.
//

// BSDF.h
#ifndef BSDF_H
#define BSDF_H

#include "SpectralData.h"
#include <glm/glm.hpp>

class BSDF {
public:
    virtual ~BSDF() = default;

    // Evaluate the BSDF for given incoming (wi) and outgoing (wo) directions.
    // normal is the surface normal at the hit point.
    virtual Spectrum evaluate(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& normal) const = 0;

    // Sample an outgoing direction (wo) given an incoming direction (wi) and surface normal.
    // Returns the sampled direction and sets pdf to the probability density.
    virtual glm::vec3 sample(const glm::vec3& wi, const glm::vec3& normal, float& pdf) const = 0;
};

#endif // BSDF_H
