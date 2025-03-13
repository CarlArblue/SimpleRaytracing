//
// Created by alex on 3/10/25.
//

// Scene.h
#ifndef SCENE_H
#define SCENE_H

// Scene.h modification
#include <vector>
#include <memory>
#include "Entity.h"
#include "Light.h"
#include "Triangle.h"
#include "EmissiveSurface.h"

class Scene {
public:
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Light>> lights;  // Add lights container

    // Add a new entity to the scene
    void addEntity(const std::shared_ptr<Entity>& entity) {
        entities.push_back(entity);
    }

    // Add a new light to the scene
    void addLight(const std::shared_ptr<Light>& light) {
        lights.push_back(light);
    }

    // Add an emissive triangle which acts both as geometry and light
    void addEmissiveTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                           const Spectrum& color, const Spectrum& emission) {
        auto triangle = std::make_shared<Triangle>(v0, v1, v2, color);
        auto emissive = std::make_shared<EmissiveTriangle>(triangle, emission);
        entities.push_back(emissive);

        // Also add as an area light
        auto light = std::make_shared<AreaLight>(v0, v1 - v0, v2 - v0, emission);
        lights.push_back(light);
    }
};

#endif // SCENE_H
