#include "tema1.h"
#include <vector>
#include <iostream>
#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

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

    // Initialize tank position (for now, static)
    tankX = resolution.x / 2.0f;
    tankY = resolution.y / 3.0f;

    // Generate terrain data
    GenerateTerrain();

    // Create a simple tank model (e.g., a square or rectangle)
    glm::vec3 corner = glm::vec3(0, 0, 0);
    float tankSize = 50.0f;

    // Create and add tank mesh
    Mesh* tank = object2D::CreateSquare("tank", corner, tankSize, glm::vec3(0.2f, 0.7f, 0.2f), true);
    AddMeshToList(tank);

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

    // Render the tank on top of the terrain
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(tankX, tankY);

    RenderMesh2D(meshes["tank"], shaders["VertexColor"], modelMatrix);
}


void Tema1::FrameEnd()
{
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
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
