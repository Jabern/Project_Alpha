#pragma once

#include <PxPhysicsAPI.h>

#include <raylib.h>
#include <vector>

using namespace physx;

struct RaycastResult {
    Vector3 position;
    Vector3 normal;
    float distance;
    PxRigidActor* actor;
};

class PhysicsManager {
private:
    PxDefaultErrorCallback gDefaultErrorCallback;
    PxDefaultAllocator gDefaultAllocatorCallback;

    PxFoundation* foundation;
    PxPhysics* physics;
    PxCooking* cooking;
    PxScene* scene;
    PxPvd* mPvd;
    PxRigidActor* pActor;
    PxControllerManager* manager;

public:
    PhysicsManager();
    ~PhysicsManager();
    void Initialize();
    void Cleanup();
    void Update();
    void SetPlayer(PxRigidActor* playerA);
    
    PxControllerManager* GetControllerManager(); 

    PxShape* CreateBoxShape(const PxVec3& dimensions, PxMaterial* material);
    PxRigidDynamic* CreateRigidDynamic(PxShape* shape, const PxTransform& transform);
    PxRigidStatic* CreateRigidStatic(PxShape* shape, const PxTransform& transform);

    std::vector<PxShape*> CreateMeshShapes(Model model, const PxMaterial* material); // Create Colision Mesh from Raylib Model

    PxPhysics* GetPhysics() const; // Getter function for the physics object
};