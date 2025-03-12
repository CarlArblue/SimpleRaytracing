//
// Created by alex on 3/10/25.
//

// Triangle.cpp
#include "Triangle.h"
#include <glm/glm.hpp>
#include <cmath>

Triangle::Triangle()
    : v0(0.0f), v1(0.0f), v2(0.0f), color(1.0f) {} // Use Spectrum(1.0f)

Triangle::Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const Spectrum &color)
    : v0(v0), v1(v1), v2(v2), color(color) {}

bool Triangle::intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const {
    // Existing implementation is fine
    const float EPSILON = 1e-8f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);
    if (std::abs(a) < EPSILON)
        return false;  // Ray is parallel to triangle.

    float f = 1.0f / a;
    glm::vec3 s = origin - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0f || (u + v) > 1.0f)
        return false;

    float t = f * glm::dot(edge2, q);
    if (t <= EPSILON)
        return false;

    rec.t = t;
    rec.hitPoint = origin + dir * t;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    // Ensure normal faces toward the ray origin
    if (glm::dot(normal, dir) > 0.0f) {
        normal = -normal; // Flip normal if it's facing away from the ray
    }
    rec.normal = normal;
    rec.color = color;
    return true;
}

