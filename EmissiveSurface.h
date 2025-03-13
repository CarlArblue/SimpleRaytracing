//
// Created by alex on 3/13/25.
//

// EmissiveSurface.h
#ifndef EMISSIVESURFACE_H
#define EMISSIVESURFACE_H

#include "Entity.h"
#include "Triangle.h"

// Decorator pattern - wraps an existing entity to make it emissive
class EmissiveTriangle : public Entity {
private:
    std::shared_ptr<Triangle> triangle;
    Spectrum emission;

public:
    EmissiveTriangle(const std::shared_ptr<Triangle>& triangle, const Spectrum& emission)
        : triangle(triangle), emission(emission) {}

    bool intersect(const glm::vec3& origin, const glm::vec3& dir, HitRecord& rec) const override {
        bool hit = triangle->intersect(origin, dir, rec);
        if (hit) {
            rec.emission = emission;
            rec.isEmissive = true;
        }
        return hit;
    }

    Spectrum getEmission() const override {
        return emission;
    }

    bool isEmissive() const override {
        return true;
    }
};

#endif // EMISSIVESURFACE_H