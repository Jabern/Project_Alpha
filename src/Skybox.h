/*******************************************************************************************
*
*   FROM :: raylib [models] example - Skybox loading and drawing
*
*   Copyright (c) 2017-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#pragma once

#include <raylib.h>

#include "rlgl.h"
#include "raymath.h" 

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

class Skybox
{
public:
    Skybox();
    ~Skybox();

    void Load(const char* HDR_FileName, const char * IMG_FileName, bool hdr);
    void Unload();
    void Draw();
    static TextureCubemap GenTextureCubemap(Shader shader, Texture2D panorama, int size, int format);

private:
    Texture2D skyboxTexture;
    Model skybox;
    Shader skyboxShader;
    Shader shdrCubemap;
    Image img;
    bool useHDR;
    char skyboxFileName[256] = { 0 };
    Texture2D panorama;
};
