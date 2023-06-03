#pragma once

#include <raylib.h>
#include "raymath.h"
#include "PhysicsManager.h"

using namespace physx;

class Player {
public:
    Player(const Vector3& startPos = { 0.0f, 10.0f, 5.0f }, float playerSpeed = 5.0f, const Color& playerColor = RED, const Vector3& playerSize = { 0.5f, 0.5f, 0.5f });
    ~Player();
    void Update();
    void Draw();
    void Load();
    void Unload();

    Vector3 GetPosition() const; // Getter function for player position
    void SetRotation(const Vector3& rotation);

    void SetShader(Shader shader);
    void SetPhysicsManager(PhysicsManager* physicsMgr);
    void SetMapShape(PxShape* m_shape);

    PxRigidActor* GetActor();
private:
    Vector3 position;
    Vector3 rotation;
    float speed;
    Color color;
    Vector3 size;
    Model cube;
    PxRigidActor* actor;
    PhysicsManager* physicsManager;
    PxShape* shape;
    PxShape* collisionShape;
    PxController* characterController;
};
