#include "tema1.h"
#include <vector>
#include <iostream>
#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

float clamp(float value, float min, float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // Initialize tank properties
    tank1X = resolution.x * 0.25f;
    tank2X = resolution.x * 0.75f;

    tankSpeed = 100.0f;

    tank1Color = glm::vec3(0.8f, 0.68f, 0.53f); // Left tank
    tank1ColorBottom = glm::vec3(0.45f, 0.39f, 0.30f); // Left tank
    tank1TurretAngle = 0.0f;

    tank2Color = glm::vec3(0.62f, 0.73f, 0.45f); // Right tank
    tank2ColorBottom = glm::vec3(0.32f, 0.38f, 0.19f); // Right tank
    tank2TurretAngle = 0.0f;

    // Generate terrain data
    GenerateTerrain();

    AdjustTankPosition(tank1X, tank1Y);
    AdjustTankPosition(tank2X, tank2Y);

    glm::vec3 cannonColor = glm::vec3(0.22f, 0.22f, 0.23f);

    // Create meshes for Tank 1
    // Mesh* trapezoid1_tank1 = object2D::CreateTrapezoid("trapezoid1_tank1", glm::vec3(-15, -10, 0), 80, 100, 20, tank1Color);
    // AddMeshToList(trapezoid1_tank1);
    // Mesh* trapezoid2_tank1 = object2D::CreateTrapezoid("trapezoid2_tank1", glm::vec3(0, 10, 0), 60, 70, 12, tank1ColorBottom);
    // AddMeshToList(trapezoid2_tank1);
    // Mesh* turret_tank1 = object2D::CreateCircle("turret_tank1", glm::vec3(35, 10, 0), 15, tank1Color, true, 30);
    // AddMeshToList(turret_tank1);
    // Mesh* cannon_tank1 = object2D::CreateRectangle("cannon_tank1", glm::vec3(0, 0, 0), 20, 5, cannonColor, true);
    // AddMeshToList(cannon_tank1);

    Mesh* trapezoid1_tank1 = object2D::CreateTrapezoid("trapezoid1_tank1",
        glm::vec3(-40, 0, 0), // Adjusted so that the tank's base is at (0, 0)
        80, // widthTop
        100, // widthBottom
        20, // height
        tank1Color);
    AddMeshToList(trapezoid1_tank1);

    Mesh* trapezoid2_tank1 = object2D::CreateTrapezoid("trapezoid2_tank1",
        glm::vec3(-18, -12, 0), // Adjusted position
        70, // widthTop
        60, // widthBottom
        12, // height
        tank1ColorBottom);
    AddMeshToList(trapezoid2_tank1);

    Mesh* turret_tank1 = object2D::CreateCircle("turret_tank1",
        glm::vec3(11, 20, 0), // Turret center relative to tank's base
        15, // radius
        tank1Color, true, 30);
    AddMeshToList(turret_tank1);

    // Create the cannon mesh centered at the origin
    Mesh* cannon_tank1 = object2D::CreateRectangle("cannon_tank1",
        glm::vec3(0, -2.5f, 0), // Centered horizontally at origin, vertically from -2.5 to +2.5
        30, // width (length of the cannon)
        5,  // height
        cannonColor, true);
    AddMeshToList(cannon_tank1);

    // Create meshes for Tank 2
    // Mesh* trapezoid1_tank2 = object2D::CreateTrapezoid("trapezoid1_tank2", glm::vec3(-15, -10, 0), 80, 100, 20, tank2Color);
    // AddMeshToList(trapezoid1_tank2);
    // Mesh* trapezoid2_tank2 = object2D::CreateTrapezoid("trapezoid2_tank2", glm::vec3(0, 10, 0), 60, 70, 12, tank2ColorBottom);
    // AddMeshToList(trapezoid2_tank2);
    // Mesh* turret_tank2 = object2D::CreateCircle("turret_tank2", glm::vec3(35, 10, 0), 15, tank2Color, true, 30);
    // AddMeshToList(turret_tank2);
    // Mesh* cannon_tank2 = object2D::CreateRectangle("cannon_tank2", glm::vec3(0, 0, 0), 20, 5, cannonColor, true);
    // AddMeshToList(cannon_tank2);

        Mesh* trapezoid1_tank2 = object2D::CreateTrapezoid("trapezoid1_tank2",
        glm::vec3(-40, 0, 0), // Adjusted so that the tank's base is at (0, 0)
        80, // widthTop
        100, // widthBottom
        20, // height
        tank2Color);
    AddMeshToList(trapezoid1_tank2);

    Mesh* trapezoid2_tank2 = object2D::CreateTrapezoid("trapezoid2_tank2",
        glm::vec3(-18, -12, 0), // Adjusted position
        70, // widthTop
        60, // widthBottom
        12, // height
        tank2ColorBottom);
    AddMeshToList(trapezoid2_tank2);

    Mesh* turret_tank2 = object2D::CreateCircle("turret_tank2",
        glm::vec3(11, 20, 0), // Turret center relative to tank's base
        15, // radius
        tank2Color, true, 30);
    AddMeshToList(turret_tank2);

    Mesh* cannon_tank2 = object2D::CreateRectangle("cannon_tank2",
        glm::vec3(0, 0, 0), // Centered around the turret center
        30, // width
        5, // height
        cannonColor, true);
    AddMeshToList(cannon_tank2);



    glm::vec3 groundColor = glm::vec3(0.83f, 0.87f, 0.12f);
    Mesh* terrainQuad = object2D::CreateSquare("terrainQuad", glm::vec3(0, 0, 0), 1.0f, groundColor, true);
    AddMeshToList(terrainQuad);
}

void Tema1::GenerateTerrain()
{
    // Simple height map generation for the terrain
    int terrainWidth = 1280; // 1280;

    // Desired number of cycles over the terrain width
    float cycles1 = 2.7f;  // Gentle slope
    float cycles2 = 4.5f;  // Moderate variation
    float cycles3 = 5.5f;  // Smaller details

    // Calculate angular frequencies
    float frequency1 = 2 * PI * cycles1 / terrainWidth;
    float frequency2 = 2 * PI * cycles2 / terrainWidth;
    float frequency3 = 2 * PI * cycles3 / terrainWidth;

    // Set amplitudes for each sine function
    float amplitude1 = 50.0f;
    float amplitude2 = 30.0f;
    float amplitude3 = 20.0f;

    heightMap.resize(terrainWidth);

    // Populate the height map with combined sinusoidal functions
    for (int x = 0; x < terrainWidth; ++x) {
        float t = static_cast<float>(x);
        heightMap[x] = amplitude1 * sin(frequency1 * t) +
                       amplitude2 * sin(frequency2 * t) +
                       amplitude3 * sin(frequency3 * t);
    }
}

void Tema1::AdjustTankPosition(float &x, float &y)
{
    int ix = static_cast<int>(x);
    if (ix >= 0 && ix < heightMap.size()) {
        float terrainHeight = heightMap[ix] + 235.0f;  // Use terrain height + offset
        y = terrainHeight + 25.0f;  // Add a small offset to position tank above the terrain
    }
}

void Tema1::MoveTank(float &x, float &y, int direction, float deltaTimeSeconds)
{
    x += direction * tankSpeed * deltaTimeSeconds;
    AdjustTankPosition(x, y);
}

void Tema1::RenderTank(float x, float y, float turretAngle, float bodyAngle,
                       const std::string& trapezoid1Name,
                       const std::string& trapezoid2Name,
                       const std::string& turretName,
                       const std::string& cannonName,
                       bool mirrored)
{
    glm::mat3 modelMatrix = glm::mat3(1);

    // Apply translation to tank position
    modelMatrix *= transform2D::Translate(x, y);

    // Apply body rotation around the tank's base (which is at (0, 0) in local coordinates)
    modelMatrix *= transform2D::Rotate(bodyAngle);

    // Apply mirroring if needed
    if (mirrored) {
        modelMatrix *= transform2D::Scale(-1, 1);
    }

    // Render the tank body (top trapezoid)
    RenderMesh2D(meshes[trapezoid1Name], shaders["VertexColor"], modelMatrix);

    // Render the tank lower part (bottom trapezoid)
    RenderMesh2D(meshes[trapezoid2Name], shaders["VertexColor"], modelMatrix);

    // Render the turret
    glm::mat3 modelMatrixTurret = modelMatrix;
    RenderMesh2D(meshes[turretName], shaders["VertexColor"], modelMatrixTurret);

    // Render the cannon with rotation around the turret center
    glm::mat3 modelMatrixCannon = modelMatrix;
    modelMatrixCannon *= transform2D::Translate(11, 20);     // Move to the turret center
    modelMatrixCannon *= transform2D::Rotate(turretAngle);   // Rotate around the turret center
    modelMatrixCannon *= transform2D::Translate(0, 0);       // No additional translation needed
    RenderMesh2D(meshes[cannonName], shaders["VertexColor"], modelMatrixCannon);
}



void Tema1::FrameStart()
{
    glClearColor(0.57f, 0.74f, 0.89f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
    int terrainWidth = static_cast<int>(heightMap.size());
    float offsetY = 235.0f;  // Raise the ground level by this offset

    for (int x = 0; x < terrainWidth - 1; ++x) {
        float y1 = heightMap[x] + offsetY;
        float y2 = heightMap[x + 1] + offsetY;

        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(x, 0);  // Start from the bottom of the screen

        // Adjust the quad height to fill from the bottom to the terrain height
        float quadWidth = 1.0f;
        float quadHeight = y1;

        // Render a filled quad from the bottom up to y1
        modelMatrix *= transform2D::Scale(quadWidth, quadHeight);

        RenderMesh2D(meshes["terrainQuad"], shaders["VertexColor"], modelMatrix);
    }

    AdjustTankPosition(tank1X, tank1Y);
    AdjustTankPosition(tank2X, tank2Y);

    // Compute tank1 angle based on terrain slope
    int ix1 = static_cast<int>(tank1X);
    if (ix1 > 0 && ix1 < terrainWidth - 1) {
        float deltaHeight = heightMap[ix1 + 1] - heightMap[ix1 - 1];
        float deltaX = 2.0f;  // Difference in x positions
        float slope = deltaHeight / deltaX;
        tank1Angle = atan(slope);
    }

    // Compute tank2 angle based on terrain slope
    int ix2 = static_cast<int>(tank2X);
    if (ix2 > 0 && ix2 < terrainWidth - 1) {
        float deltaHeight = heightMap[ix2 + 1] - heightMap[ix2 - 1];
        float deltaX = 2.0f;
        float slope = deltaHeight / deltaX;
        tank2Angle = atan(slope);
    }


    // Render Tank 1
    RenderTank(tank1X, tank1Y, tank1TurretAngle, tank1Angle,
               "trapezoid1_tank1", "trapezoid2_tank1",
               "turret_tank1", "cannon_tank1", false);

    // Render Tank 2 (mirrored)
    RenderTank(tank2X, tank2Y, tank2TurretAngle, tank2Angle,
               "trapezoid1_tank2", "trapezoid2_tank2",
               "turret_tank2", "cannon_tank2", true);
}


void Tema1::FrameEnd()
{
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Tank 1 movement (WASD)
    if (window->KeyHold(GLFW_KEY_A)) MoveTank(tank1X, tank1Y, -1, deltaTime);
    if (window->KeyHold(GLFW_KEY_D)) MoveTank(tank1X, tank1Y, 1, deltaTime);
    if (window->KeyHold(GLFW_KEY_W)) tank1TurretAngle += deltaTime;
    if (window->KeyHold(GLFW_KEY_S)) tank1TurretAngle -= deltaTime;

    // Clamp turret angle between 0 and PI (0 to 180 degrees)
    tank1TurretAngle = clamp(tank1TurretAngle, 0.0f, PI);

    // Tank 2 movement (Arrow Keys)
    if (window->KeyHold(GLFW_KEY_LEFT)) MoveTank(tank2X, tank2Y, -1, deltaTime);
    if (window->KeyHold(GLFW_KEY_RIGHT)) MoveTank(tank2X, tank2Y, 1, deltaTime);
    if (window->KeyHold(GLFW_KEY_UP)) tank2TurretAngle += deltaTime;
    if (window->KeyHold(GLFW_KEY_DOWN)) tank2TurretAngle -= deltaTime;

    // Clamp turret angle between 0 and PI (0 to 180 degrees)
    tank2TurretAngle = clamp(tank2TurretAngle, 0.0f, PI);

    // Keep tanks on terrain
    AdjustTankPosition(tank1X, tank1Y);
    AdjustTankPosition(tank2X, tank2Y);
}

void Tema1::OnKeyPress(int key, int mods)
{
}

void Tema1::OnKeyRelease(int key, int mods)
{
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}
