//
// Created by alex on 3/10/25.
//

// Scene.h
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include "Entity.h"
#include "BVHNode.h"

class Scene {
public:
    std::vector<std::shared_ptr<Entity>> entities;
    std::shared_ptr<BVHNode> bvh;

    void buildBVH() {
        bvh = std::make_shared<BVHNode>(entities);
    }

    // Add a new entity to the scene
    void addEntity(const std::shared_ptr<Entity>& entity) {
        entities.push_back(entity);
    }
};

#endif // SCENE_H
