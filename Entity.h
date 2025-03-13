//
// Created by alex on 3/10/25.
//

// Entity.h
#ifndef ENTITY_H
#define ENTITY_H

#include "SpectralData.h"
#include "BSDF.h"
#include "AABB.h"
#include <glm/glm.hpp>
#include <memory>

class Entity;

struct HitRecord {
    float t = 0.0f;
    glm::vec3 hitPoint;
    glm::vec3 normal;
    Spectrum color;
    Spectrum emission;
    bool isEmissive = false;
    std::shared_ptr<Entity> hitEntity;
};

// Abstract base class for all scene entities.
class Entity {
public:

    // AABB
    virtual AABB getBounds() const = 0;

    // Test if the ray (origin, dir) intersects the entity.
    virtual bool intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const = 0;

    // Add emission getter
    virtual Spectrum getEmission() const {
        return Spectrum(0.0f);  // Default is non-emissive
    }

    // Add emissive check
    virtual bool isEmissive() const {
        return false;  // Default is non-emissive
    }

    // New method for emissive entities
    virtual void sampleLight(const glm::vec3& refPoint, glm::vec3& samplePoint, glm::vec3& lightNormal, float& pdf) const {
        // Default: do nothing if not emissive.
        pdf = 0.0f;
    }

    // Virtual method for retrieving BSDF
    virtual BSDF* getBSDF() const { return nullptr; }

    // Virtual destructor for proper cleanup.
    virtual ~Entity() = default;

};

// Triangle class representing a triangle in 3D space.
class Triangle : public Entity {
public:
    glm::vec3 v0, v1, v2;
    Spectrum color;
    Spectrum emission;
    BSDF* bsdf;

    Triangle()
        : v0(0.0f), v1(0.0f), v2(0.0f), color(1.0f), emission(0.0f), bsdf(nullptr) {}

    Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const Spectrum &color, const Spectrum &emission, BSDF* bsdf = nullptr)
        : v0(v0), v1(v1), v2(v2), color(color), emission(emission), bsdf(bsdf) {}

    bool intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const override;

    Spectrum getEmission() const override {
        return emission;
    }

    bool isEmissive() const override {
        return emission > 0.0f;
    }

    void sampleLight(const glm::vec3& refPoint, glm::vec3& samplePoint, glm::vec3& lightNormal, float& pdf) const override;

    BSDF* getBSDF() const override {
        return bsdf;
    }

    AABB getBounds() const override {
        glm::vec3 min = glm::min(glm::min(v0, v1), v2);
        glm::vec3 max = glm::max(glm::max(v0, v1), v2);
        // Add a small epsilon to ensure the AABB has volume even for flat triangles
        const float eps = 0.0001f;
        if (min.x == max.x) max.x += eps;
        if (min.y == max.y) max.y += eps;
        if (min.z == max.z) max.z += eps;
        return AABB(min, max);
    }

};

// Sphere class representing a 3D sphere.
class Sphere : public Entity {
public:
    glm::vec3 center;
    float radius;
    Spectrum color;
    Spectrum emission;
    BSDF* bsdf;

    Sphere()
    : center(0.0f, 0.0f, 0.0f), radius(1.0f), color(1.0f), emission(0.0f), bsdf(nullptr) {}

    Sphere(const glm::vec3& center, float radius, const Spectrum& color, const Spectrum& emission, BSDF* bsdf = nullptr)
        : center(center), radius(radius), color(color), emission(emission), bsdf(bsdf) {}

    bool intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const override;

    Spectrum getEmission() const override {
        return emission;
    }

    bool isEmissive() const override {
        return emission > 0.0f;
    }

    BSDF* getBSDF() const override {
        return bsdf;
    }

    AABB getBounds() const override {
        return AABB(center - glm::vec3(radius), center + glm::vec3(radius));
    }

};

#endif // ENTITY_H

