#include "Map.h"

Map::Map() : mapShape(nullptr)
{
    // Initialize any variables or resources needed for the map
    mapModel = { 0 };
}

Map::~Map()
{
    // Unload any resources used by the map
    //Unload();
}

void Map::GenerateMap(PhysicsManager* physicsManager)
{
    // TODO : Implement Perlin noise Generation 

    size.x = 10.0f;
    size.y = 1.0f;
    size.z = 10.0f;

    position = { 0.0f, -0.5f, 0.0f};
    mapModel = LoadModel("models/Game_Level_Scene_Low_Poly.obj");
    //mapModel = LoadModelFromMesh(GenMeshCube(10.0f, 1.0f, 10.0f));
    //mapShape = physicsManager->CreateBoxShape(PxVec3(size.x / 2, size.y / 2, size.z / 2), physicsManager->GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f));
    mapShapes = physicsManager->CreateMeshShapes(mapModel, physicsManager->GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f));

    
    PxVec3 positionVec(position.x, position.y, position.z);

    // Create a PxTransform object with the specified position
    PxTransform transform(positionVec);
    for (PxShape* shape : mapShapes)
    {
        shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        actor = physicsManager->CreateRigidStatic(shape, transform);
        actor->attachShape(*shape);
    }
}

PxShape* Map::GetCollisionShape() const
{
    return mapShape;
}

void Map::SetShader(Shader shader)
{
    for (int i = 0; i < sizeof(mapModel.materials); i++)
    {
        mapModel.materials[i].shader = shader;
    }  
    //Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
    //Texture2D texture = LoadTextureFromImage(checked);
    //UnloadImage(checked);
    //mapModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    //mapModel.materials[0].shader = shader;
}

void Map::Draw()
{
    // Draw the plane model
    DrawModel(mapModel, position, 1.0f, WHITE);
}

Model Map::GetModel()
{
    return mapModel;
}

void Map::Unload()
{
    for (int i = 0; i < sizeof(mapModel.materials); i++)
    {
        printf("Materials count : %d\n", i);
    }
    UnloadModel(mapModel);
}

