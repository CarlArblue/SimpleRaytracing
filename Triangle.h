//
// Created by alex on 3/10/25.
//

// Triangle.h
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "Entity.h"

// Triangle class representing a triangle in 3D space.
class Triangle : public Entity {
public:
    glm::vec3 v0, v1, v2;
    glm::vec3 color;

    Triangle();
    Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &color);

    // Override intersect to fill HitRecord.
    bool intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const override;
};

#endif // TRIANGLE_H


