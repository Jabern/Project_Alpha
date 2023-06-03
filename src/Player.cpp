#include "player.h"
#include "Map.h"
#include <iostream>

Player::Player(const Vector3& startPos, float playerSpeed, const Color& playerColor, const Vector3& playerSize)
    : position(startPos), speed(playerSpeed), color(playerColor), size(playerSize)
{

}

Player::~Player()
{
    Unload();
}

void Player::Load()
{
    PxVec3 positionVec(position.x, position.y, position.z);

    // Create a PxTransform object with the specified position
    PxTransform transform(positionVec);

    cube = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));

    //collisionShape = physicsManager->CreateBoxShape(PxVec3(size.x / 2, size.y / 2, size.z / 2), physicsManager->GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f));
    //actor = physicsManager->CreateRigidDynamic(collisionShape, transform);
    //actor->attachShape(*collisionShape);

    // Create the Character Controller
    PxCapsuleControllerDesc controllerDesc;
    controllerDesc.height = size.y / 2;
    controllerDesc.radius = size.y / 2;
    controllerDesc.slopeLimit = 0.0f;
    controllerDesc.stepOffset = 0.1f;
    controllerDesc.material = physicsManager->GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f);
    controllerDesc.position = PxExtendedVec3(position.x, position.y, position.z);  // Initial position
    controllerDesc.upDirection = PxVec3(0.0f, 1.0f, 0.0f);  // Up direction for the controller

    characterController = physicsManager->GetControllerManager()->createController(controllerDesc);
}

void Player::Unload()
{

}

void Player::SetMapShape(PxShape* m_shape)
{
    shape = m_shape;
}

PxRigidActor* Player::GetActor()
{
    return actor;
}

void Player::Update()
{
    float moveSpeed = 0.1f;
    Vector3 movement = { 0.0f, 0.0f, 0.0f };

    if (IsKeyDown(KEY_W))
    {
        movement.x += sin(rotation.y) * speed * GetFrameTime();
        movement.z += cos(rotation.y) * speed * GetFrameTime();
    }

    if (IsKeyDown(KEY_S))
    {
        movement.x -= sin(rotation.y) * speed * GetFrameTime();
        movement.z -= cos(rotation.y) * speed * GetFrameTime();
    }

    if (IsKeyDown(KEY_A))
    {
        movement.x += cos(rotation.y) * speed * GetFrameTime();
        movement.z -= sin(rotation.y) * speed * GetFrameTime();
    }

    if (IsKeyDown(KEY_D))
    {
        movement.x -= cos(rotation.y) * speed * GetFrameTime();
        movement.z += sin(rotation.y) * speed * GetFrameTime();
    }

    // TODO : Add Jump 

    PxVec3 displacement(movement.x, movement.y, movement.z);
    PxVec3 gravity(0.0f, -9.81f, 0.0f);
    displacement += gravity * GetFrameTime();

    PxControllerFilters filters;

    characterController->move(displacement, 0.01f, GetFrameTime(), filters);
    physicsManager->Update();

    position.x = characterController->getPosition().x;
    position.y = characterController->getPosition().y;
    position.z = characterController->getPosition().z;
}

void Player::SetPhysicsManager(PhysicsManager* physicsMgr)
{
    physicsManager = physicsMgr;
}

void Player::SetShader(Shader shader)
{
    cube.materials[0].shader = shader;
}

void Player::Draw()
{
    cube.transform = MatrixRotateY(rotation.y);
    DrawModel(cube, position, 1.0f, BLUE);
}


void Player::SetRotation(const Vector3& rotation)
{
    this->rotation = rotation;
}

Vector3 Player::GetPosition() const
{
    return position;
}
