//
// Created by alex on 3/10/25.
//

// Sphere.cpp
#include "Sphere.h"
#include <cmath>

Sphere::Sphere()
    : center(0.0f, 0.0f, 0.0f), radius(1.0f), color(1.0f, 1.0f, 1.0f) {}

Sphere::Sphere(const glm::vec3& center, float radius, const glm::vec3& color)
    : center(center), radius(radius), color(color) {}

bool Sphere::intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const {
    glm::vec3 oc = origin - center;
    float b = 2.0f * glm::dot(oc, dir);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0f * c;
    if (discriminant < 0)
        return false;

    float t = (-b - std::sqrt(discriminant)) / 2.0f;
    if (t <= 0)
        return false;

    rec.t = t;
    rec.hitPoint = origin + dir * t;
    rec.normal = glm::normalize(rec.hitPoint - center);
    rec.color = color;
    return true;
}
