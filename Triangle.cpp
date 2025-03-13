//
// Created by alex on 3/10/25.
//

// Triangle.cpp
#include "Entity.h"
#include "SamplingHelpers.h"
#include <glm/glm.hpp>
#include <cmath>

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
    rec.emission = emission;
    rec.isEmissive = isEmissive();
    return true;
}

void Triangle::sampleLight(const glm::vec3& refPoint, glm::vec3& samplePoint, glm::vec3& lightNormal, float& pdf) const {
    // Uniformly sample a point on the triangle
    float r1 = random_float();
    float r2 = random_float();
    if (r1 + r2 > 1.0f) { // Ensure uniformity over the triangle
        r1 = 1.0f - r1;
        r2 = 1.0f - r2;
    }
    samplePoint = v0 + r1 * (v1 - v0) + r2 * (v2 - v0);
    // The normal of the triangle (assumed constant)
    lightNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    // PDF is the reciprocal of the triangle’s area:
    float area = 0.5f * glm::length(glm::cross(v1 - v0, v2 - v0));
    pdf = 1.0f / area;
}

