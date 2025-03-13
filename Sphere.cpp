//
// Created by alex on 3/10/25.
//

// Sphere.cpp
#include "Entity.h"
#include <cmath>

bool Sphere::intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const {
    // Existing implementation is fine
    glm::vec3 oc = origin - center;
    // In Sphere.cpp
    float a = glm::dot(dir, dir);  // Should be 1.0 if dir is normalized
    float b = 2.0f * glm::dot(oc, dir);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0)
        return false;

    // Consider both intersection points
    float sqrtd = std::sqrt(discriminant);
    float t1 = (-b - sqrtd) / (2.0f * a);
    float t2 = (-b + sqrtd) / (2.0f * a);

    // Use nearest positive intersection
    float t = (t1 > 0) ? t1 : t2;
    if (t <= 0)
        return false;

    rec.t = t;
    rec.hitPoint = origin + dir * t;
    rec.normal = glm::normalize(rec.hitPoint - center);
    rec.color = color;
    rec.emission = emission;
    rec.isEmissive = isEmissive();
    return true;
}