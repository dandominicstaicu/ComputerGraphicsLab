#include "lab_m1/Tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>


using namespace std;
using namespace m1;


static Mesh* CreateCube(const std::string& name)
{
    // A simple unit cube [-0.5..0.5]^3 for each axis
    std::vector<VertexFormat> vertices =
    {
        // Position            // Normal      // Texture
        VertexFormat(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 1)), 
        VertexFormat(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 1)), 
        VertexFormat(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0, 0, 1)), 
        VertexFormat(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0, 0, 1)), 

        VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1)), 
        VertexFormat(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1)), 
        VertexFormat(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0, 0, -1)), 
        VertexFormat(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0, 0, -1))  
    };

    // 12 triangles (36 indices), or as quads grouped by faces
    std::vector<unsigned int> indices =
    {
        // front
        0, 1, 2,   2, 3, 0,
        // right
        1, 5, 6,   6, 2, 1,
        // back
        7, 6, 5,   5, 4, 7,
        // left
        4, 0, 3,   3, 7, 4,
        // top
        3, 2, 6,   6, 7, 3,
        // bottom
        4, 5, 1,   1, 0, 4
    };

    Mesh* cube = new Mesh(name);
    cube->InitFromData(vertices, indices);
    return cube;
}

Tema2::Tema2()
{
}


Tema2::~Tema2()
{
    // Clean up obstacles
    for(auto& obstacle : obstacles)
    {
        delete obstacle;
    }
    obstacles.clear();

    // Clean up camera
    delete camera;
}


void Tema2::Init()
{
    camera = new implemented::CameraT2();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, Z_NEAR, Z_FAR);

    // Create our own procedural cube geometry
    {
        Mesh* cube = CreateCube("cube");
        meshes[cube->GetMeshID()] = cube;
    }

    // todo remove
    aabbMesh = meshes["cube"];

    // Load Box Mesh for Buildings
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Load Sphere Mesh for Drone hitbox
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Load Cylinder Mesh for Trunk
    {
        Mesh* cylinder = new Mesh("cylinder");
        cylinder->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "custom"), "cylinder.obj");
        meshes[cylinder->GetMeshID()] = cylinder;
    }

    // Load Cone Mesh for Foliage
    {
        Mesh* cone = new Mesh("cones");
        cone->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "custom"), "cones.obj");
        meshes[cone->GetMeshID()] = cone;
    }

    // Load Shaders
    {
        Shader* droneShader = new Shader("DroneShader");
        droneShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "DroneVertexShader.glsl"), GL_VERTEX_SHADER);
        droneShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "DroneFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        if (!droneShader->CreateAndLink())
        {
            std::cerr << "Failed to create and link DroneShader.\n";
        }
        shaders[droneShader->GetName()] = droneShader;
    }

    {
        Shader* terrainShader = new Shader("TerrainShader");
        terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TerrainVertexShader.glsl"), GL_VERTEX_SHADER);
        terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TerrainFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        if (!terrainShader->CreateAndLink())
        {
            std::cerr << "Failed to create and link TerrainShader.\n";
        }
        shaders[terrainShader->GetName()] = terrainShader;
    }

    {
        Shader* obstacleShader = new Shader("ObstacleShader");
        obstacleShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "ObstacleVertexShader.glsl"), GL_VERTEX_SHADER);
        obstacleShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "ObstacleFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        if (!obstacleShader->CreateAndLink())
        {
            std::cerr << "Failed to create and link ObstacleShader.\n";
        }
        shaders[obstacleShader->GetName()] = obstacleShader;
    }

    { // todo remove after
        Shader* aabbShader = new Shader("AABBShader");
        aabbShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "AABBVertexShader.glsl"), GL_VERTEX_SHADER);
        aabbShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "AABBFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        if (!aabbShader->CreateAndLink())
        {
            std::cerr << "Failed to create and link AABBShader.\n";
        }
        shaders[aabbShader->GetName()] = aabbShader;
    }

    int gridResolution = 86;

    // Initialize the Drone class
    drone.Init(meshes, shaders, shaders["DroneShader"], this);

    // Initialize the terrain
    terrain.GenerateGrid(gridResolution, gridResolution); 

    meshes["terrain"] = terrain.GetMesh();

    // Procedurally generate obstacles
    const float minDistance = 7.0f;    
    const float terrainHalfSize = gridResolution / 2.0f;
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_real_distribution<float> distPos(-terrainHalfSize + 10.0f, terrainHalfSize - 10.0f);

    // Separate scale distributions for trees and buildings
    std::uniform_real_distribution<float> distScaleTrees(10.0f, 25.0f);      // For trees
    std::uniform_real_distribution<float> distScaleBuildings(10.0f, 15.0f);  // For buildings

    std::vector<glm::vec3> placedPositions;

    const int numTrees = 30;
    const int numBuildings = 10;

    const int maxPlacementAttempts = 100; // Maximum attempts to place an obstacle

    // Generate Trees
    GenerateTrees(numTrees,
                  minDistance,
                  terrainHalfSize,
                  maxPlacementAttempts,
                  rng,
                  distPos,
                  distScaleTrees,
                  placedPositions);

    // Generate Buildings
    GenerateBuildings(numBuildings,
                      minDistance,
                      terrainHalfSize,
                      maxPlacementAttempts,
                      rng,
                      distPos,
                      distScaleBuildings,
                      placedPositions);
}

void Tema2::FrameStart()
{
    glEnable(GL_DEPTH_TEST);
    

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.1f, 0.3f, 0.5f, 1); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    glm::vec3 oldDronePos = drone.GetPosition();

    // Update the drone state
    drone.Update(deltaTimeSeconds);

    // Collision check with the terrain
    GLfloat terrainTreshold = 1.0f;
    glm::vec3 dronePos = drone.GetPosition();
    if (dronePos.y < terrainTreshold) {     
        // Clamp the Y position to 0 so it never goes below ground
        dronePos.y = terrainTreshold;
        drone.SetPosition(dronePos);
    }

    // Check obstacle collisions
    float droneRadius = drone.GetCollisionRadius();
    for (auto& obstacle : obstacles)
    {
        if (obstacle->CheckCollisionWithSphere(dronePos, droneRadius))
        {
            // Collision found! Decide how to handle it:
            // Example: revert movement or push drone back up
            // For demonstration, just print a message:
            drone.SetPosition(oldDronePos);
            std::cout << "Drone collided with an obstacle!\n";

            // If you want to do something like push the drone away or revert the last movement
            // you'd need to store the old drone position before this frame, then restore it here.
            break; // If you only want to handle the first obstacle
        }
    }

    // Render the drone
    drone.Render(camera->GetViewMatrix(), projectionMatrix);

    // Render the terrain
    RenderMesh(terrain.GetMesh(), shaders["TerrainShader"], glm::mat4(1));

    // Render all obstacles
    for(auto& obstacle : obstacles) {
        obstacle->Render(camera->GetViewMatrix(), projectionMatrix);
    }

    // Render AABBs
    RenderAABBs();

    // Render Drone's Hitbox
    RenderDroneHitbox();
}

void Tema2::RenderAABBs()
{
    Shader* aabbShader = shaders["AABBShader"];
    if (!aabbShader)
        return;

    aabbShader->Use();
    glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniform3fv(glGetUniformLocation(aabbShader->GetProgramID(), "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f))); // Red color

    // Enable wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for(auto& obstacle : obstacles)
    {
        std::vector<std::pair<glm::vec3, glm::vec3>> aabbs = obstacle->GetAABBs();
        for(const auto& aabb : aabbs)
        {
            glm::vec3 boxMin = aabb.first;
            glm::vec3 boxMax = aabb.second;

            // Calculate the center and scale
            glm::vec3 center = (boxMin + boxMax) / 2.0f;
            glm::vec3 size = boxMax - boxMin;

            // Create model matrix
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, center);
            model = glm::scale(model, size);

            // Render the AABB
            RenderMesh(aabbMesh, aabbShader, model);
        }
    }

    // Restore to fill mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));


    mesh->Render();
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 6.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            camera->TranslateForward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            camera->TranslateRight(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            camera->TranslateForward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            camera->TranslateRight(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            camera->TranslateUpward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera->TranslateUpward(-cameraSpeed * deltaTime);
        }
    }

    float droneSpeed = 2.0f;
    float rotationSpeed = 90.0f; // Degrees per second

    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        // Translate along local axes
        if (window->KeyHold(GLFW_KEY_W)) {
            // Move forward along Oz
            drone.MoveForward(droneSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // Move backward along Oz
            drone.MoveBackward(droneSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // Move right along Ox
            drone.MoveRight(droneSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // Move left along Ox
            drone.MoveLeft(droneSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // Move up along Oy
            drone.MoveUp(droneSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // Move down along Oy
            drone.MoveDown(droneSpeed * deltaTime);
        }

        // Rotate around Oy
        if (window->KeyHold(GLFW_KEY_LEFT)) {
            drone.RotateLeft(rotationSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_RIGHT)) {
            drone.RotateRight(rotationSpeed * deltaTime);

        }
    }

}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
    }

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
            camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {

        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

float Tema2::CalculateDistance(const glm::vec3& a, const glm::vec3& b)
{
    return glm::distance(glm::vec2(a.x, a.z), glm::vec2(b.x, b.z));
}

void Tema2::GenerateTrees(int numTrees,
                          float minDistance,
                          float terrainHalfSize,
                          int maxPlacementAttempts,
                          std::mt19937 &rng,
                          std::uniform_real_distribution<float> &distPos,
                          std::uniform_real_distribution<float> &distScaleTrees,
                          std::vector<glm::vec3> &placedPositions)
{
    for (int i = 0; i < numTrees; ++i)
    {
        bool positionOK = false;
        glm::vec3 pos;
        float scale;
        int attempts = 0;

        while (!positionOK && attempts < maxPlacementAttempts)
        {
            pos = glm::vec3(distPos(rng), 0.0f, distPos(rng));
            scale = distScaleTrees(rng);

            // Ensure no overlap with existing obstacles
            positionOK = true;
            for (const auto& existingPos : placedPositions)
            {
                if (CalculateDistance(pos, existingPos) < minDistance)
                {
                    positionOK = false;
                    break;
                }
            }

            attempts++;
        }

        if (positionOK)
        {
            placedPositions.emplace_back(pos);
            Tree* tree = new Tree(meshes, shaders, shaders["ObstacleShader"], pos, scale);
            obstacles.push_back(tree);
        }
        else
        {
            std::cerr << "Failed to place tree " << i + 1
                      << " after " << maxPlacementAttempts << " attempts.\n";
        }
    }
}

void Tema2::GenerateBuildings(int numBuildings,
                              float minDistance,
                              float terrainHalfSize,
                              int maxPlacementAttempts,
                              std::mt19937 &rng,
                              std::uniform_real_distribution<float> &distPos,
                              std::uniform_real_distribution<float> &distScaleBuildings,
                              std::vector<glm::vec3> &placedPositions)
{
    for (int i = 0; i < numBuildings; ++i)
    {
        bool positionOK = false;
        glm::vec3 pos;
        float scale;
        int attempts = 0;

        while (!positionOK && attempts < maxPlacementAttempts)
        {
            pos = glm::vec3(distPos(rng), 0.0f, distPos(rng));
            scale = distScaleBuildings(rng);

            // Ensure no overlap with existing obstacles
            positionOK = true;
            for (const auto& existingPos : placedPositions)
            {
                if (CalculateDistance(pos, existingPos) < minDistance)
                {
                    positionOK = false;
                    break;
                }
            }

            attempts++;
        }

        if (positionOK)
        {
            placedPositions.emplace_back(pos);
            Building* building = new Building(meshes, shaders, shaders["ObstacleShader"], pos, scale);
            obstacles.push_back(building);
        }
        else
        {
            std::cerr << "Failed to place building " << i + 1
                      << " after " << maxPlacementAttempts << " attempts.\n";
        }
    }
}

void Tema2::RenderDroneHitbox()
{
    Shader* aabbShader = shaders["AABBShader"];
    if (!aabbShader)
        return;

    aabbShader->Use();
    glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(aabbShader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniform3fv(glGetUniformLocation(aabbShader->GetProgramID(), "objectColor"), 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f))); // Green color for hitbox

    // Enable wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Get drone's position and collision radius
    glm::vec3 dronePos = drone.GetPosition();
    float droneRadius = drone.GetCollisionRadius();

    // Create model matrix for the collision sphere
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, dronePos);
    model = glm::scale(model, glm::vec3(droneRadius)); // Scale to collision radius

    // Render the drone's collision sphere
    RenderMesh(meshes["sphere"], aabbShader, model);

    // Restore to fill mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


