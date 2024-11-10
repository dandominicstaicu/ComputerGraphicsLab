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
    tank1Color = glm::vec3(0.8f, 0.68f, 0.53f); // Red tank
    tank1ColorBottom = glm::vec3(0.45f, 0.39f, 0.30f); // Red tank
    
    tank2Color = glm::vec3(0.0f, 0.0f, 0.8f); // Blue tank
    tank1TurretAngle = 0.0f;
    tank2TurretAngle = 0.0f;

    // Generate terrain data
    GenerateTerrain();

    // Initial tank positions based on terrain
    // Create tank meshes and add them to the list once
    // Mesh* trapezoid1 = object2D::CreateSquare("trapezoid1", glm::vec3(-20, -10, 0), 40, tank1Color, true);
    // AddMeshToList(trapezoid1);

    // Mesh* trapezoid2 = object2D::CreateSquare("trapezoid2", glm::vec3(-30, -10, 0), 60, tank1Color, true);
    // AddMeshToList(trapezoid2);

    Mesh* trapezoid1 = object2D::CreateTrapezoid("trapezoid1", glm::vec3(-15, -10, 0), 80, 100, 20, tank1Color);  // Bottom trapezoid
    AddMeshToList(trapezoid1);
    Mesh* trapezoid2 = object2D::CreateTrapezoid("trapezoid2", glm::vec3(0, 10, 0), 60, 70, 12, tank1ColorBottom);
    AddMeshToList(trapezoid2);


    glm::vec3 turretCenter = glm::vec3(35, 10, 0);  // Replace with desired center
    Mesh* turret = object2D::CreateCircle("turret", turretCenter, 15, tank1Color, true, 30);
    AddMeshToList(turret);

    // Create a cannon rectangle mesh
    glm::vec3 cannonCenter = glm::vec3(0, 0, 0);  // Replace with desired center
    Mesh* cannon = object2D::CreateRectangle("cannon", cannonCenter, 20, 5, glm::vec3(0.22f, 0.22f, 0.23f), true);
    AddMeshToList(cannon);



    glm::vec3 groundColor = glm::vec3(0.83f, 0.87f, 0.12f);
    Mesh* terrainQuad = object2D::CreateSquare("terrainQuad", glm::vec3(0, 0, 0), 1.0f, groundColor, true);
    AddMeshToList(terrainQuad);
}

void Tema1::GenerateTerrain()
{
    // Simple height map generation for the terrain
    int terrainWidth = 1280; // 1280;
    float amplitude1 = -15.0f; // 50.0f;
    float frequency1 = 0.02f; // 0.005f;
    float amplitude2 = 30.0f; // 30.0f;
    float frequency2 = 0.04f; // 0.02f;
    float amplitude3 = -10.0f;
    float frequency3 = 0.05f;

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

void Tema1::RenderTank(float x, float y, float turretAngle, const glm::vec3 &color)
{
    glm::mat3 modelMatrix = glm::mat3(1);

    // Render the top trapezoid (larger part of the tank base)
    modelMatrix *= transform2D::Translate(x, y);
    RenderMesh2D(meshes["trapezoid1"], shaders["VertexColor"], modelMatrix);

    // Render the bottom trapezoid (smaller, flipped, and centered with respect to the top trapezoid)
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x, y);    // Position it below trapezoid1
    modelMatrix *= transform2D::Scale(1, -1);       // Flip vertically
    RenderMesh2D(meshes["trapezoid2"], shaders["VertexColor"], modelMatrix);

    // Render the turret
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x, y);  // Adjust y-position for turret placement
    RenderMesh2D(meshes["turret"], shaders["VertexColor"], modelMatrix);

    // Render the cannon with rotation around the turret
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x + 35, y + 11);  // Place cannon at the top of the turret (same as turret center)
    modelMatrix *= transform2D::Rotate(turretAngle);   // Apply turret angle rotation
    modelMatrix *= transform2D::Translate(15, 0);      // Offset cannon outward from turret center
    RenderMesh2D(meshes["cannon"], shaders["VertexColor"], modelMatrix);
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

    // Ensure tank is positioned above the terrain
    AdjustTankPosition(tank1X, tank1Y);

    // Render tanks
    RenderTank(tank1X, tank1Y, tank1TurretAngle, tank1Color);
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

    // Tank 2 movement (Arrow Keys)
    if (window->KeyHold(GLFW_KEY_LEFT)) MoveTank(tank2X, tank2Y, -1, deltaTime);
    if (window->KeyHold(GLFW_KEY_RIGHT)) MoveTank(tank2X, tank2Y, 1, deltaTime);
    if (window->KeyHold(GLFW_KEY_UP)) tank2TurretAngle += deltaTime;
    if (window->KeyHold(GLFW_KEY_DOWN)) tank2TurretAngle -= deltaTime;

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
