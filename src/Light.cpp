/*******************************************************************************************
*
*   FROM ::: raylib [shaders] example - basic lighting
*
*   Copyright (c) 2019-2023 Chris Camacho (@codifies) and Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "Light.h"

int Light::lightsCount = 0;

Light::Light()
{

}

void Light::CreateLight(LightType type, Vector3 position, Vector3 target, Color color, Shader shader)
{
    this->type = type;
    this->position = position;
    this->target = target;
    this->color = color;
    this->shader = shader;

    if (lightsCount < MAX_LIGHTS)
    {
        // NOTE: Lighting shader naming must be the provided ones
        enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", lightsCount));
        typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", lightsCount));
        positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", lightsCount));
        targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", lightsCount));
        colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", lightsCount));

        UpdateValues();

        lightsCount++;
    }
}

void Light::DrawLightSphere()
{
    if (enabled) DrawSphereEx(this->position, 0.2f, 8, 8, this->color);
    else DrawSphereWires(this->position, 0.2f, 8, 8, ColorAlpha(this->color, 0.3f));
}

void Light::TEST_Input()
{
    if (IsKeyDown(KEY_UP))
        position.z += 1.5f * GetFrameTime();

    if (IsKeyDown(KEY_DOWN))
        position.z -= 1.5f * GetFrameTime();

    if (IsKeyDown(KEY_LEFT))
        position.x += 1.5f * GetFrameTime();

    if (IsKeyDown(KEY_RIGHT))
        position.x -= 1.5f * GetFrameTime();
    if (IsKeyPressed(KEY_N))
        enabled =! enabled;
}

void Light::UpdateValues()
{
    // Send to shader light enabled state and type
    SetShaderValue(shader, enabledLoc, &enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, typeLoc, &type, SHADER_UNIFORM_INT);

    // Send to shader light position values
    float positionArray[3] = { position.x, position.y, position.z };
    SetShaderValue(shader, positionLoc, positionArray, SHADER_UNIFORM_VEC3);

    // Send to shader light target position values
    float targetArray[3] = { target.x, target.y, target.z };
    SetShaderValue(shader, targetLoc, targetArray, SHADER_UNIFORM_VEC3);

    // Send to shader light color values
    float colorArray[4] = { static_cast<float>(color.r) / 255.0f, static_cast<float>(color.g) / 255.0f,
                            static_cast<float>(color.b) / 255.0f, static_cast<float>(color.a) / 255.0f };
    SetShaderValue(shader, colorLoc, colorArray, SHADER_UNIFORM_VEC4);
}
