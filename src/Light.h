/*******************************************************************************************
*
*   FROM ::: raylib [shaders] example - basic lighting
*
*   Copyright (c) 2019-2023 Chris Camacho (@codifies) and Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#pragma once

#include "raylib.h"

#define MAX_LIGHTS  4     

// Light type
enum class LightType
{
    Directional = 0,
    Point
};

class Light
{
public:
    Light();
    void CreateLight(LightType type, Vector3 position, Vector3 target, Color color , Shader shader);
    void DrawLightSphere();
    void UpdateValues();

    void TEST_Input();

private:
    LightType type;
    bool enabled = true;
    Vector3 position;
    Vector3 target;
    Color color;
    float attenuation;

    // Shader locations
    int enabledLoc;
    int typeLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int attenuationLoc;

    Shader shader;
    static int lightsCount;
};

