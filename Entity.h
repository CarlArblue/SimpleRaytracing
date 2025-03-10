//
// Created by alex on 3/10/25.
//

// Entity.h
#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>

// Structure to store intersection details.
struct HitRecord {
    float t = 0.0f;
    glm::vec3 hitPoint;
    glm::vec3 normal;
    glm::vec3 color;
};

// Abstract base class for all scene entities.
class Entity {
public:
    // Test if the ray (origin, dir) intersects the entity.
    // If it does, fill in the hit record and return true.
    virtual bool intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const = 0;

    // Virtual destructor for proper cleanup.
    virtual ~Entity() = default;
};

#endif // ENTITY_H

