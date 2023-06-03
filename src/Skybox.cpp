/*******************************************************************************************
*
*   FROM :: raylib [models] example - Skybox loading and drawing
*
*   Copyright (c) 2017-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "Skybox.h"

TextureCubemap Skybox::GenTextureCubemap(Shader shader, Texture2D panorama, int size, int format)
{
    TextureCubemap cubemap = { 0 };

    rlDisableBackfaceCulling();     // Disable backface culling to render inside the cube

    // STEP 1: Setup framebuffer
    //------------------------------------------------------------------------------------------
    unsigned int rbo = rlLoadTextureDepth(size, size, true);
    cubemap.id = rlLoadTextureCubemap(0, size, format);

    unsigned int fbo = rlLoadFramebuffer(size, size);
    rlFramebufferAttach(fbo, rbo, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    rlFramebufferAttach(fbo, cubemap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X, 0);

    // Check if framebuffer is complete with attachments (valid)
    if (rlFramebufferComplete(fbo)) TraceLog(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", fbo);
    //------------------------------------------------------------------------------------------

    // STEP 2: Draw to framebuffer
    //------------------------------------------------------------------------------------------
    // NOTE: Shader is used to convert HDR equirectangular environment map to cubemap equivalent (6 faces)
    rlEnableShader(shader.id);

    // Define projection matrix and send it to shader
    Matrix matFboProjection = MatrixPerspective(90.0 * DEG2RAD, 1.0, RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);
    rlSetUniformMatrix(shader.locs[SHADER_LOC_MATRIX_PROJECTION], matFboProjection);

    // Define view matrix for every side of the cubemap
    Matrix fboViews[6] = {
        MatrixLookAt({ 0.0f, 0.0f, 0.0f }, { 1.0f,  0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f }),
        MatrixLookAt({ 0.0f, 0.0f, 0.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f }),
        MatrixLookAt({ 0.0f, 0.0f, 0.0f }, { 0.0f,  1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f }),
        MatrixLookAt({ 0.0f, 0.0f, 0.0f }, { 0.0f, -1.0f,  0.0f }, { 0.0f,  0.0f, -1.0f }),
        MatrixLookAt({ 0.0f, 0.0f, 0.0f }, { 0.0f,  0.0f,  1.0f }, { 0.0f, -1.0f,  0.0f }),
        MatrixLookAt({ 0.0f, 0.0f, 0.0f }, { 0.0f,  0.0f, -1.0f }, { 0.0f, -1.0f,  0.0f })
    };

    rlViewport(0, 0, size, size);   // Set viewport to current fbo dimensions

    // Activate and enable texture for drawing to cubemap faces
    rlActiveTextureSlot(0);
    rlEnableTexture(panorama.id);

    for (int i = 0; i < 6; i++)
    {
        // Set the view matrix for the current cube face
        rlSetUniformMatrix(shader.locs[SHADER_LOC_MATRIX_VIEW], fboViews[i]);

        // Select the current cubemap face attachment for the fbo
        // WARNING: This function by default enables->attach->disables fbo!!!
        rlFramebufferAttach(fbo, cubemap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X + i, 0);
        rlEnableFramebuffer(fbo);

        // Load and draw a cube, it uses the current enabled texture
        rlClearScreenBuffers();
        rlLoadDrawCube();

        // ALTERNATIVE: Try to use internal batch system to draw the cube instead of rlLoadDrawCube
        // for some reason this method does not work, maybe due to cube triangles definition? normals pointing out?
        // TODO: Investigate this issue...
        //rlSetTexture(panorama.id); // WARNING: It must be called after enabling current framebuffer if using internal batch system!
        //rlClearScreenBuffers();
        //DrawCubeV(Vector3Zero(), Vector3One(), WHITE);
        //rlDrawRenderBatchActive();
    }
    //------------------------------------------------------------------------------------------

    // STEP 3: Unload framebuffer and reset state
    //------------------------------------------------------------------------------------------
    rlDisableShader();          // Unbind shader
    rlDisableTexture();         // Unbind texture
    rlDisableFramebuffer();     // Unbind framebuffer
    rlUnloadFramebuffer(fbo);   // Unload framebuffer (and automatically attached depth texture/renderbuffer)

    // Reset viewport dimensions to default
    rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
    rlEnableBackfaceCulling();
    //------------------------------------------------------------------------------------------

    cubemap.width = size;
    cubemap.height = size;
    cubemap.mipmaps = 1;
    cubemap.format = format;

    return cubemap;
}

Skybox::Skybox()
{

}

Skybox::~Skybox()
{

}

void Skybox::Load(const char* HDR_FileName, const char* IMG_FileName, bool hdr)
{

    useHDR = hdr;
    Mesh cube = GenMeshCube(100.0f, 100.0f, 100.0f);
    this->skybox = LoadModelFromMesh(cube);


    // Load skybox shader and set required locations
    // NOTE: Some locations are automatically set at shader loading
    skybox.materials[0].shader = LoadShader("skybox.vs", "skybox.fs");

    int environmentMapValue = MATERIAL_MAP_CUBEMAP;
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "environmentMap"), &environmentMapValue, SHADER_UNIFORM_INT);

    int doGammaValue = useHDR ? 1 : 0;
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "doGamma"), &doGammaValue, SHADER_UNIFORM_INT);

    int vflippedValue = useHDR ? 1 : 0;
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "vflipped"), &vflippedValue, SHADER_UNIFORM_INT);


    // Load cubemap shader and setup required shader locations
    this->shdrCubemap = LoadShader("cubemap.vs", "cubemap.fs");

    int* value = 0;
    SetShaderValue(shdrCubemap, GetShaderLocation(shdrCubemap, "equirectangularMap"), &value, SHADER_UNIFORM_INT);

    this->panorama;

    if (useHDR)
    {
        TextCopy(skyboxFileName, HDR_FileName);

        // Load HDR panorama (sphere) texture
        panorama = LoadTexture(skyboxFileName);

        // Generate cubemap (texture with 6 quads-cube-mapping) from panorama HDR texture
        // NOTE 1: New texture is generated rendering to texture, shader calculates the sphere->cube coordinates mapping
        // NOTE 2: It seems on some Android devices WebGL, fbo does not properly support a FLOAT-based attachment,
        // despite texture can be successfully created.. so using PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 instead of PIXELFORMAT_UNCOMPRESSED_R32G32B32A32
        skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = GenTextureCubemap(shdrCubemap, panorama, 1024, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

        //UnloadTexture(panorama);    // Texture not required anymore, cubemap already generated
    }
    else
    {
        Image img = LoadImage(IMG_FileName);
        skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);    // CUBEMAP_LAYOUT_PANORAMA
        UnloadImage(img);
    }
}

void Skybox::Unload()
{
    UnloadTexture(skyboxTexture);
    UnloadShader(skyboxShader);
}

void Skybox::Draw()
{
    // We are inside the cube, we need to disable backface culling!
    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(skybox, { 0, 0, 0 }, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();
    //DrawTextureEx(panorama, (Vector2){ 0, 0 }, 0.0f, 0.5f, WHITE);
    //if (useHDR) DrawText(TextFormat("Panorama image from hdrihaven.com: %s", GetFileName(skyboxFileName)), 10, GetScreenHeight() - 20, 10, BLACK);
    //else DrawText(TextFormat(": %s", GetFileName(skyboxFileName)), 10, GetScreenHeight() - 20, 10, BLACK);

}
