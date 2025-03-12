//
// Created by alex on 3/10/25.
//

// Sphere.h
#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include "Entity.h"
#include "SpectralData.h"

// Sphere class representing a 3D sphere.
class Sphere : public Entity {
public:
    glm::vec3 center;
    float radius;
    Spectrum color;  // Changed from glm::vec3 to Spectrum

    Sphere();
    Sphere(const glm::vec3& center, float radius, const Spectrum& color);

    bool intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const override;
};

#endif // SPHERE_H
