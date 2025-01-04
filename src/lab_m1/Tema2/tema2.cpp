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

    // Clean up checkpoints
    for(auto& checkpoint : checkpoints)
    {
        delete checkpoint;
    }
    checkpoints.clear();
}


void Tema2::Init()
{
    camera = new implemented::CameraT2();
    // Initial Camera Setup Based on Default Mode (First-Person)**
    float eyeHeight = 0.5f; // Adjust as needed
    glm::vec3 initialCameraPos = drone.GetPosition() + glm::vec3(0, eyeHeight, 0);
    glm::vec3 initialCameraTarget = initialCameraPos + drone.GetForwardVector();
    camera->Set(initialCameraPos, initialCameraTarget, glm::vec3(0, 1, 0));


    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, Z_NEAR, Z_FAR);

    // Create our own procedural cube geometry
    {
        Mesh* cube = CreateCube("cube");
        meshes[cube->GetMeshID()] = cube;
    }

    // Load Box Mesh for Buildings
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
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

    // Load Ring Mesh for Checkpoints
    {
        Mesh* ring = new Mesh("ring");
        ring->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "custom"), "ring.obj");
        meshes[ring->GetMeshID()] = ring;
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

    // Load AABB Shaders for hitboxes
    {
        Shader* aabbShader = new Shader("AABBShader");
        aabbShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "AABBVertexShader.glsl"), GL_VERTEX_SHADER);
        aabbShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "AABBFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        if (!aabbShader->CreateAndLink())
        {
            std::cerr << "Failed to create and link AABBShader.\n";
        }
        shaders[aabbShader->GetName()] = aabbShader;
    }

    {
        Shader* uiShader = new Shader("UIShader");
        uiShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "UIVertexShader.glsl"), GL_VERTEX_SHADER);
        uiShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "UIFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        if (!uiShader->CreateAndLink())
        {
            std::cerr << "Failed to create and link UIShader.\n";
        }
        shaders[uiShader->GetName()] = uiShader;
    }

    // Initialize UI Manager
    uiManager.Init(window, shaders);


    // Set up orthographic projection for UI
    {
        float orthoLeft = 0.0f;
        float orthoRight = static_cast<float>(window->GetResolution().x);
        float orthoBottom = 0.0f;
        float orthoTop = static_cast<float>(window->GetResolution().y);
        float orthoNear = -1.0f;
        float orthoFar = 1.0f;

        Shader* uiShader = shaders["UIShader"];
        uiShader->Use();
        glm::mat4 uiProjection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);
        glUniformMatrix4fv(glGetUniformLocation(uiShader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(uiProjection));
    }

    {
        // Define positions for 6 checkpoints above the trees (y > 10)
        std::vector<glm::vec3> checkpointPositions = {
            glm::vec3(10.0f, 15.0f, 10.0f),
            glm::vec3(-20.0f, 15.0f, 30.0f),
            glm::vec3(25.0f, 15.0f, -15.0f),
            glm::vec3(-30.0f, 15.0f, -25.0f),
            glm::vec3(35.0f, 15.0f, 40.0f),
            glm::vec3(-40.0f, 15.0f, 35.0f)
        };

        // Define default color (e.g., blue) and highlight color (e.g., red)
        glm::vec3 defaultColor(0.0f, 0.0f, 1.0f); // Blue
        glm::vec3 highlightColor(1.0f, 0.0f, 0.0f); // Red

        // Create and store checkpoints
        for (size_t i = 0; i < checkpointPositions.size(); ++i)
        {
            glm::vec3 color = (i == 0) ? highlightColor : defaultColor; // Highlight the first checkpoint initially
            Checkpoint* checkpoint = new Checkpoint(checkpointPositions[i], color);
            checkpoints.push_back(checkpoint);
        }
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

    const int numTrees = 35;
    const int numBuildings = 8;

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

    // Highlight the first checkpoint
    UpdateCheckpoint();

    // Enable blending for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    // Update Camera Position and Orientation Based on Camera Mode
    if (currentCameraMode == FIRST_PERSON) {
        float eyeHeight = 0.5f; // Adjust as needed
        glm::vec3 cameraPos = drone.GetPosition() + glm::vec3(0, eyeHeight, 0);
        glm::vec3 cameraTarget = cameraPos + drone.GetForwardVector();
        camera->Set(cameraPos, cameraTarget, glm::vec3(0, 1, 0));
    }
    else if (currentCameraMode == THIRD_PERSON) {
        // Calculate camera position behind and above the drone
        glm::vec3 forward = drone.GetForwardVector();
        glm::vec3 cameraPos = drone.GetPosition() - forward * thirdPersonDistance + glm::vec3(0, thirdPersonHeight, 0);
        glm::vec3 cameraTarget = drone.GetPosition() + glm::vec3(0, 1.0f, 0); // Look at the drone's eye level

        camera->Set(cameraPos, cameraTarget, glm::vec3(0, 1, 0));
    }

    // Render Checkpoints
    {
        for (size_t i = 0; i < checkpoints.size(); ++i)
        {
            Checkpoint* checkpoint = checkpoints[i];

            // Retrieve the shader used for checkpoints
            Shader* checkpointShader = shaders["ObstacleShader"]; // Using ObstacleShader for rendering

            // Set up the model matrix
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, checkpoint->GetPosition());

            // Scale the checkpoint
            modelMatrix = glm::scale(modelMatrix, glm::vec3(7.0f));

            // **Update Hitbox based on the model matrix**
            checkpoint->GetHitbox().Update(modelMatrix);

            // Set the color uniform
            checkpointShader->Use(); // Activate the shader before setting uniforms
            GLint colorLocation = glGetUniformLocation(checkpointShader->GetProgramID(), "objectColor");
            if(colorLocation == -1) {
                std::cerr << "Uniform 'objectColor' not found in ObstacleShader!\n";
            }
            else {
                glUniform3fv(colorLocation, 1, glm::value_ptr(checkpoint->GetColor()));
            }

            // Render the ring mesh
            if (meshes.find("ring") != meshes.end())
            {
                RenderMesh(meshes["ring"], checkpointShader, modelMatrix);
            }

            // **Render Hitbox for Debugging**
            Shader* aabbShader = shaders["AABBShader"];
            if (aabbShader && aabbShader->program && meshes.find("cube") != meshes.end())
            {
                checkpoint->DrawHitbox(camera->GetViewMatrix(), projectionMatrix, meshes, aabbShader);
            }
        }
    }

    // Render the drone
    drone.Render(camera->GetViewMatrix(), projectionMatrix);

    // Render the terrain
    RenderMesh(terrain.GetMesh(), shaders["TerrainShader"], glm::mat4(1));

    // Initialize collision flag
    bool collisionDetected = false;

    // Iterate through all obstacles to render them and check for collisions
    for (auto& obstacle : obstacles)
    {
        // Render the obstacle
        obstacle->Render(camera->GetViewMatrix(), projectionMatrix);

        // Perform collision checks
        if (auto tree = dynamic_cast<Tree*>(obstacle))
        {
            // Check collision with trunk
            if (CheckCollision(drone.droneBox, tree->trunkBox)) {
                std::cout << "Collision with tree trunk!\n";
                collisionDetected = true;
            }
            // Check collision with foliage
            if (CheckCollision(drone.droneBox, tree->foliageBox)) {
                std::cout << "Collision with tree foliage!\n";
                collisionDetected = true;
            }

            // Draw hitboxes for debugging
            tree->DrawHitboxes(camera->GetViewMatrix(), projectionMatrix);
        }
        else if (auto building = dynamic_cast<Building*>(obstacle))
        {
            if (CheckCollision(drone.droneBox, building->buildingBox)) {
                std::cout << "Collision with building!\n";
                collisionDetected = true;
            }

            // Draw hitbox for debugging
            building->DrawHitbox(camera->GetViewMatrix(), projectionMatrix);
        }
    }

    // **Check Collision with Checkpoints**
    CheckCheckpointCollisions();

    // Respond to collision
    if (collisionDetected) {
        // Restore the drone's previous position to prevent passing through
        drone.SetPosition(dronePrevPos);
        std::cout << "Drone position reset to prevent passing through obstacle.\n";
    }

    // Update Timer
    remainingTime -= deltaTimeSeconds;
    if (remainingTime < 0.0f) {
        remainingTime = 0.0f;
        // Handle game over logic here
        std::cout << "Time's up! Game Over.\n";
        // Example: Reset drone position, reset checkpoints, reset timer, etc.
    }

    // **Render UI Elements**
    {
        // Render Indicator Arrow
        if (currentCheckpointIndex < checkpoints.size())
        {
            Checkpoint* nextCheckpoint = checkpoints[currentCheckpointIndex];
            glm::vec3 checkpointPos = nextCheckpoint->GetPosition();

            glm::vec3 cameraPos = drone.GetPosition(); // Assuming drone's position represents the camera
            glm::vec3 cameraForward = drone.GetForwardVector(); // Drone's forward direction

            // Use UIShader
            Shader* uiShader = shaders["UIShader"];

            // Render the arrow using UIManager
            uiManager.RenderArrow(uiShader, projectionMatrix, cameraPos, cameraForward, checkpointPos,
                static_cast<float>(window->GetResolution().x), static_cast<float>(window->GetResolution().y));
        }

        // Render Remaining Time as Text
        {
            std::string timeText = "Time Remaining: " + std::to_string(static_cast<int>(remainingTime)) + "s";
            GLfloat x = 25.0f; // 25 pixels from left
            GLfloat y = 50.0f; // 50 pixels from top
            GLfloat scale = 1.0f; // Adjust scale as needed
            glm::vec3 textColor(1.0f, 1.0f, 1.0f); // White

            uiManager.RenderText(timeText, x, y, scale, textColor);
        }
    }

    // Re-enable depth testing if needed
    // glEnable(GL_DEPTH_TEST);
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


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float droneSpeed = 6.0f;
    float rotationSpeed = 90.0f; // Degrees per second

    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        // **Save the drone's current position before applying movement**
        dronePrevPos = drone.GetPosition();

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
    // Switch to First-Person Camera when '1' is pressed
    if (key == GLFW_KEY_1) {
        currentCameraMode = FIRST_PERSON;
        std::cout << "Switched to First-Person Camera.\n";
    }

    // Switch to Third-Person Camera when '3' is pressed
    if (key == GLFW_KEY_3) {
        currentCameraMode = THIRD_PERSON;
        std::cout << "Switched to Third-Person Camera.\n";
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
        float sensivityOX = 0.1f; // Adjust sensitivity as needed
        float sensivityOY = 0.1f;

        if (currentCameraMode == FIRST_PERSON) {
            // Rotate the drone's yaw based on horizontal mouse movement
            float yawChange = -sensivityOY * deltaX;
            drone.RotateRight(yawChange);
        }
        else if (currentCameraMode == THIRD_PERSON) {
            // Orbit the camera around the drone
            // Calculate horizontal and vertical angles based on mouse movement
            float horizontalAngle = -sensivityOY * deltaX;
            float verticalAngle = -sensivityOX * deltaY;

            // Rotate the camera's position around the drone
            // Convert current camera position to spherical coordinates relative to the drone
            glm::vec3 direction = camera->GetTargetPosition() - drone.GetPosition();
            float distance = glm::length(direction);
            float theta = atan2(direction.x, direction.z);
            float phi = acos(direction.y / distance);

            // Update angles based on mouse movement
            theta += glm::radians(horizontalAngle);
            phi += glm::radians(verticalAngle);

            // Clamp phi to avoid flipping
            phi = glm::clamp(phi, 0.1f, glm::pi<float>() - 0.1f);

            // Convert back to Cartesian coordinates
            direction.x = distance * sin(phi) * sin(theta);
            direction.y = distance * cos(phi);
            direction.z = distance * sin(phi) * cos(theta);

            // Update camera position
            glm::vec3 newCameraPos = drone.GetPosition() + direction;
            glm::vec3 newCameraTarget = drone.GetPosition() + glm::vec3(0, 1.0f, 0); // Look at drone's eye level

            camera->Set(newCameraPos, newCameraTarget, glm::vec3(0, 1, 0));
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
    // Update projection matrix for 3D rendering
    projectionMatrix = glm::perspective(RADIANS(60), static_cast<float>(width) / height, Z_NEAR, Z_FAR);

    // Update orthographic projection for UI via UIManager
    Shader* uiShader = shaders["UIShader"];
    Shader* textShader = shaders["TextShader"];
    uiManager.OnWindowResize(width, height, uiShader, textShader);
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

void Tema2::UpdateCheckpoint()
{
    // Reset all checkpoints to default color
    glm::vec3 defaultColor(0.0f, 0.0f, 1.0f); // Blue
    glm::vec3 highlightColor(1.0f, 0.0f, 0.0f); // Red

    for (size_t i = 0; i < checkpoints.size(); ++i)
    {
        if (i == currentCheckpointIndex)
            checkpoints[i]->SetColor(highlightColor);
        else
            checkpoints[i]->SetColor(defaultColor);
    }
}

void Tema2::CheckCheckpointCollisions()
{
    if (currentCheckpointIndex >= checkpoints.size())
        return; // All checkpoints have been passed

    Checkpoint* currentCheckpoint = checkpoints[currentCheckpointIndex];
    Hitbox& checkpointHitbox = currentCheckpoint->GetHitbox();

    if (CheckCollision(drone.droneBox, checkpointHitbox))
    {
        std::cout << "Checkpoint " << currentCheckpointIndex + 1 << " reached!\n";
        // Increment the checkpoint index
        currentCheckpointIndex++;
        if (currentCheckpointIndex >= checkpoints.size())
        {
            std::cout << "All checkpoints passed!\n";
            // Optionally, handle game completion
            currentCheckpointIndex = 0; // Example: Loop back to first checkpoint
        }
        // Update checkpoint colors
        UpdateCheckpoint();
    }
}
