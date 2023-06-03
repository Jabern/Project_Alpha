#pragma once

#include <raylib.h>
#include "PerlinNoise.hpp"
#include "PhysicsManager.h"

class Map
{
public:
    Map();
    ~Map();

    void GenerateMap(PhysicsManager* physicsManager); // Pass the PhysicsManager reference
    void Draw();
    void Unload();

    Model GetModel();
    void SetShader(Shader shader);
    PxShape* GetCollisionShape() const;
private:
    Vector3 position;
    Model mapModel;
    PxShape* mapShape; // New member to store the physics shape
    Vector3 size;
    PxRigidStatic* actor;
    std::vector<PxShape*> mapShapes;

};

