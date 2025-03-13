//
// Created by alex on 3/10/25.
//

// Entity.h
#ifndef ENTITY_H
#define ENTITY_H

#include "SpectralData.h"
#include <glm/glm.hpp>

struct HitRecord {
    float t = 0.0f;
    glm::vec3 hitPoint;
    glm::vec3 normal;
    Spectrum color;
    Spectrum emission;  // Add emission property
    bool isEmissive = false;  // Flag to quickly check emissive surfaces
};

// Abstract base class for all scene entities.
class Entity {
public:
    // Test if the ray (origin, dir) intersects the entity.
    // If it does, fill in the hit record and return true.
    virtual bool intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const = 0;

    // Virtual destructor for proper cleanup.
    virtual ~Entity() = default;

    // Add emission getter
    virtual Spectrum getEmission() const {
        return Spectrum(0.0f);  // Default is non-emissive
    }

    // Add emissive check
    virtual bool isEmissive() const {
        return false;  // Default is non-emissive
    }

};

#endif // ENTITY_H

