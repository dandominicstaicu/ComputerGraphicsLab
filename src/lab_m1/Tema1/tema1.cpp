#include "tema1.h"
#include <vector>
#include <cmath>
#include <iostream>
#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;

Tema1::Tema1()
{
    tank1TrajectoryMesh = nullptr;
    tank2TrajectoryMesh = nullptr;
}

Tema1::~Tema1()
{
    if (tank1TrajectoryMesh) {
        delete tank1TrajectoryMesh;
    }
    if (tank2TrajectoryMesh) {
        delete tank2TrajectoryMesh;
    }
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
    tank1TurretAngle = 1.0f;

    tank2Color = glm::vec3(0.62f, 0.73f, 0.45f); // Right tank
    tank2ColorBottom = glm::vec3(0.32f, 0.38f, 0.19f); // Right tank
    tank2TurretAngle = 5.5f;

    tank1Angle = 0.0f;
    tank2Angle = 0.0f;

    // Generate terrain data
    GenerateTerrain();

    AdjustTankPosition(tank1X, tank1Y);
    AdjustTankPosition(tank2X, tank2Y);

    glm::vec3 cannonColor = glm::vec3(0.22f, 0.22f, 0.23f);

    // Create meshes for Tank 1
    Mesh* trapezoid1_tank1 = object2D::CreateTrapezoid("trapezoid1_tank1",
        glm::vec3(-40, 0, 0),
        80, // widthTop
        100, // widthBottom
        20, // height
        tank1Color);
    AddMeshToList(trapezoid1_tank1);

    Mesh* trapezoid2_tank1 = object2D::CreateTrapezoid("trapezoid2_tank1",
        glm::vec3(-18, -12, 0),
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
        glm::vec3(-30, -2.5f, 0), // Start from -30 to point left
        30, // width (length of the cannon)
        5,  // height
        cannonColor, true);
    AddMeshToList(cannon_tank2);

    Mesh* projectileMesh = object2D::CreateCircle("projectile", glm::vec3(0, 0, 0), 3.0f,
                                              glm::vec3(1.0f, 1.0f, 1.0f), true, 20);
    AddMeshToList(projectileMesh);


    glm::vec3 groundColor = glm::vec3(0.83f, 0.87f, 0.12f);
    Mesh* terrainQuad = object2D::CreateSquare("terrainQuad", glm::vec3(0, 0, 0), 1.0f, groundColor, true);
    AddMeshToList(terrainQuad);

    maxHealth = 100.0f;
    tank1Health = maxHealth;
    tank2Health = maxHealth;
    tank1Alive = true;
    tank2Alive = true;

    // Initialize collision detection parameters
    projectileCollisionRadius = 3.0f;  // Same as projectile radius
    tankCollisionRadius = 30.0f;       // Adjust based on tank size
    damagePerHit = 20.0f;

    // Health bar dimensions
    healthBarWidth = 50.0f;
    healthBarHeight = 5.0f;

    // Create filled health bar mesh
    Mesh* healthBarFull = object2D::CreateRectangle("healthBarFull", glm::vec3(0, 0, 0),
                                                    healthBarWidth, healthBarHeight,
                                                    glm::vec3(0.0f, 1.0f, 0.0f), true);
    AddMeshToList(healthBarFull);

    // Create health bar border (wireframe)
    Mesh* healthBarBorder = object2D::CreateRectangle("healthBarBorder", glm::vec3(0, 0, 0),
                                                      healthBarWidth, healthBarHeight,
                                                      glm::vec3(0.0f, 0.0f, 0.0f), false);
    AddMeshToList(healthBarBorder);

    offsetY = 235.0f; // Ground level offset

    // Cloud properties
    numberOfClouds = 3; // Number of clouds
    cloudSpawnY = window->GetResolution().y - 100.0f; // Y position for clouds

    // Create a mesh for a cloud
    Mesh* cloudMesh = object2D::CreateCloud("cloud", glm::vec3(0, 0, 0), 50.0f, glm::vec3(1.0f));
    AddMeshToList(cloudMesh);

    // Initialize clouds with random positions and speeds
    for (int i = 0; i < numberOfClouds; ++i) {
        Cloud cloud;
        cloud.position = glm::vec2(rand() % window->GetResolution().x, cloudSpawnY + rand() % 100);
        cloud.speed = 20.0f + rand() % 30; // Random speed between 20 and 50
        cloud.scale = 0.5f + static_cast<float>(rand() % 10) / 10.0f; // Random scale between 0.5 and 1.5
        clouds.push_back(cloud);
    }
}

float Tema1::GetTerrainHeightAt(float x) {
    int x1 = static_cast<int>(floor(x));
    int x2 = x1 + 1;
    if (x1 < 0 || x2 >= heightMap.size()) {
        return 0.0f; // Default terrain height if out of bounds
    }

    float y1 = heightMap[x1];
    float y2 = heightMap[x2];
    float t = x - x1;
    float yTerrain = y1 * (1 - t) + y2 * t;
    return yTerrain + offsetY;
}

void Tema1::ComputeProjectileTrajectory(float startX, float startY, float angle, std::vector<glm::vec2>& trajectoryPoints) {
    float t = 0.0f;
    float dt = 0.05f; // Adjust time step as needed
    glm::vec2 position = glm::vec2(startX, startY);
    glm::vec2 velocity = glm::vec2(cos(angle), sin(angle)) * projectileSpeed;

    float maxTime = 10.0f; // Max simulation time to prevent infinite loops
    float terrainWidth = static_cast<float>(heightMap.size());

    while (position.y >= GetTerrainHeightAt(position.x) && t < maxTime) {
        trajectoryPoints.push_back(position);

        position += velocity * dt;
        velocity += gravity * dt;

        t += dt;

        // Break if position.x is out of terrain bounds
        if (position.x < 0 || position.x >= terrainWidth)
            break;
    }
}

void Tema1::UpdateTrajectoryMesh(Mesh*& mesh, const std::vector<glm::vec2>& trajectoryPoints, const std::string& meshName) {
    if (trajectoryPoints.empty()) {
        // If there is an existing mesh, delete it
        if (mesh != nullptr) {
            delete mesh;
            mesh = nullptr;
        }
        return;
    }

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < trajectoryPoints.size(); ++i) {
        const glm::vec2& point = trajectoryPoints[i];
        vertices.emplace_back(glm::vec3(point, 0.0f), glm::vec3(1.0f)); // White color
        indices.push_back(i);
    }

    if (mesh == nullptr) {
        mesh = new Mesh(meshName);
        mesh->SetDrawMode(GL_LINE_STRIP);
    }

    mesh->InitFromData(vertices, indices);
}

void Tema1::GenerateTerrain()
{
    // Simple height map generation for the terrain
    int terrainWidth = 1280;

    // Number of cycles over the terrain width
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
        y = terrainHeight + 11.0f;  // Add a small offset to position tank above the terrain
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
                       const std::string& cannonName)
{
    glm::mat3 modelMatrix = glm::mat3(1);

    // Apply translation to tank position
    modelMatrix *= transform2D::Translate(x, y);

    // Apply body rotation around the tank's base
    modelMatrix *= transform2D::Rotate(bodyAngle);

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

    modelMatrixCannon *= transform2D::Rotate(turretAngle);

    RenderMesh2D(meshes[cannonName], shaders["VertexColor"], modelMatrixCannon);
}

glm::vec2 Tema1::GetCannonTipPosition(float tankX, float tankY, float tankAngle, float turretAngle, bool cannonPointsLeft)
{
    glm::vec2 localCannonTip = glm::vec2(30.0f, 0.0f);

    glm::mat3 modelMatrix = glm::mat3(1);

    // Apply transformations
    modelMatrix *= transform2D::Translate(tankX, tankY);
    modelMatrix *= transform2D::Rotate(tankAngle);
    modelMatrix *= transform2D::Translate(11, 20);

    if (cannonPointsLeft) {
        modelMatrix *= transform2D::Rotate(PI + turretAngle); // Adjust rotation for cannon pointing left
    } else {
        modelMatrix *= transform2D::Rotate(turretAngle);
    }

    glm::vec3 worldCannonTip = modelMatrix * glm::vec3(localCannonTip, 1.0f);

    return glm::vec2(worldCannonTip.x, worldCannonTip.y);
}


void Tema1::FireProjectile(float startX, float startY, float angle) {
    Projectile p;
    p.position = glm::vec2(startX, startY);
    p.velocity = glm::vec2(cos(angle), sin(angle)) * projectileSpeed;
    p.lifespan = projectileLifespan;
    projectiles.push_back(p);
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
    } else {
        tank1Angle = 0.0f; // Default angle when out of bounds
    }

    // Compute tank2 angle based on terrain slope
    int ix2 = static_cast<int>(tank2X);
    if (ix2 > 0 && ix2 < terrainWidth - 1) {
        float deltaHeight = heightMap[ix2 + 1] - heightMap[ix2 - 1];
        float deltaX = 2.0f;
        float slope = deltaHeight / deltaX;
        tank2Angle = atan(slope);
    } else {
        tank2Angle = 0.0f; // Default angle when out of bounds
    }


    if (tank1Alive) {
        // Render Tank 1
        RenderTank(tank1X, tank1Y, tank1TurretAngle, tank1Angle,
                   "trapezoid1_tank1", "trapezoid2_tank1",
                   "turret_tank1", "cannon_tank1");
    }

    if (tank2Alive) {
        // Render Tank 2
        RenderTank(tank2X, tank2Y, tank2TurretAngle, tank2Angle,
               "trapezoid1_tank2", "trapezoid2_tank2",
               "turret_tank2", "cannon_tank2");
    }

    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        it->position += it->velocity * deltaTimeSeconds;
        it->velocity += gravity * deltaTimeSeconds; // Apply gravity
        it->lifespan -= deltaTimeSeconds;

        bool collisionDetected = false;

        // **Collision Detection with Terrain**
        float x = it->position.x;
        if (x >= 0 && x < terrainWidth - 1) {
            int x1 = static_cast<int>(floor(x));
            int x2 = x1 + 1;
            float t = (x - x1) / (x2 - x1);
            float y1 = heightMap[x1];
            float y2 = heightMap[x2];
            float yTerrain = y1 * (1 - t) + y2 * t;
            float terrainHeight = yTerrain + offsetY;

            // If projectile is below or touching the terrain
            if (it->position.y <= terrainHeight) {
                collisionDetected = true;

                // **Modify the terrain to create a circular crater**
                float x0 = x;
                float y0 = it->position.y;

                int startX = std::max(0, static_cast<int>(x0 - craterRadius));
                int endX = std::min(terrainWidth - 1, static_cast<int>(x0 + craterRadius));

                for (int i = startX; i <= endX; ++i) {
                    float dx = i - x0;
                    if (abs(dx) <= craterRadius) {
                        float dySquared = craterRadius * craterRadius - dx * dx;
                        if (dySquared >= 0.0f) { // Ensure non-negative argument for sqrt
                            float dy = sqrt(dySquared);
                            float craterBottom = y0 - dy;

                            // Adjust terrain height
                            float newHeight = craterBottom - offsetY;

                            // Set the terrain height to the lower of the current height and the crater's bottom
                            heightMap[i] = std::min(heightMap[i], newHeight);

                            // Ensure the height doesn't go below a minimum value
                            heightMap[i] = std::max(heightMap[i], minTerrainHeight);
                        }
                    }
                }
            }
        }

        // **Collision Detection with Tanks**
        if (!collisionDetected) {
            // Check collision with Tank 1
            if (tank1Alive) {
                float dx1 = it->position.x - tank1X;
                float dy1 = it->position.y - tank1Y;
                float distance1 = sqrt(dx1 * dx1 + dy1 * dy1);
                if (distance1 < (projectileCollisionRadius + tankCollisionRadius)) {
                    tank1Health -= damagePerHit;
                    collisionDetected = true;

                    if (tank1Health <= 0) {
                        tank1Health = 0;
                        tank1Alive = false;
                    }
                }
            }

            // Check collision with Tank 2
            if (tank2Alive) {
                float dx2 = it->position.x - tank2X;
                float dy2 = it->position.y - tank2Y;
                float distance2 = sqrt(dx2 * dx2 + dy2 * dy2);
                if (distance2 < (projectileCollisionRadius + tankCollisionRadius)) {
                    tank2Health -= damagePerHit;
                    collisionDetected = true;

                    if (tank2Health <= 0) {
                        tank2Health = 0;
                        tank2Alive = false;
                    }
                }
            }
        }

        // **Remove Projectile if Collision Occurred or Lifespan Expired**
        if (it->lifespan <= 0 || collisionDetected) {
            it = projectiles.erase(it);
        } else {
            // Render the projectile
            glm::mat3 modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(it->position.x, it->position.y);
            RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);
            ++it;
        }
    }

    // **Terrain Landslide Animation**
    int neighborhoodRadius = 5; // Smoothness

    // Create a copy of the heightMap to store updated heights
    std::vector<float> newHeightMap = heightMap;

    // Precompute Gaussian weights
    float sigma = neighborhoodRadius / 2.0f;
    std::vector<float> weights(2 * neighborhoodRadius + 1);
    for (int offset = -neighborhoodRadius; offset <= neighborhoodRadius; ++offset) {
        float distance = abs(offset);
        weights[offset + neighborhoodRadius] = exp(-(distance * distance) / (2 * sigma * sigma));
    }

    for (size_t i = 0; i < heightMap.size(); ++i) {
        float totalDifference = 0.0f;
        float totalWeight = 0.0f;

        // Calculate the weighted sum of height differences with neighbors
        for (int offset = -neighborhoodRadius; offset <= neighborhoodRadius; ++offset) {
            int neighborIndex = i + offset;

            if (neighborIndex >= 0 && neighborIndex < heightMap.size() && offset != 0) {
                float weight = weights[offset + neighborhoodRadius];
                float heightDifference = heightMap[i] - heightMap[neighborIndex];
                totalDifference += heightDifference * weight;
                totalWeight += weight;
            }
        }

        // Calculate the average weighted height difference
        if (totalWeight > 0.0f) {
            float averageDifference = totalDifference / totalWeight;

            // Apply the landslide effect if the average difference exceeds the threshold
            if (abs(averageDifference) > heightDifferenceThreshold) {
                // Calculate the amount of height to transfer based on deltaTime
                float epsilon = transferRate * deltaTimeSeconds;

                // Limit the transfer amount to prevent over-correction
                float transferAmount = std::min(epsilon, abs(averageDifference) - heightDifferenceThreshold);

                // Adjust the current point's height
                if (averageDifference > 0) {
                    newHeightMap[i] -= transferAmount;
                } else {
                    newHeightMap[i] += transferAmount;
                }
            }
        }
    }

    // After updating newHeightMap
    for (size_t i = 0; i < newHeightMap.size(); ++i) {
        if (std::isnan(newHeightMap[i]) || std::isinf(newHeightMap[i])) {
            newHeightMap[i] = minTerrainHeight;
        }
    }

    // Update heightMap with the new values
    heightMap = newHeightMap;

    // Ensure terrain heights are valid
    for (size_t i = 0; i < heightMap.size(); ++i) {
        if (std::isnan(heightMap[i]) || std::isinf(heightMap[i])) {
            heightMap[i] = minTerrainHeight;
        } else {
            heightMap[i] = std::max(heightMap[i], minTerrainHeight);
        }
    }

    // Render Tank 1 and its health bar
    if (tank1Alive) {
        RenderTank(tank1X, tank1Y, tank1TurretAngle, tank1Angle,
                   "trapezoid1_tank1", "trapezoid2_tank1",
                   "turret_tank1", "cannon_tank1");

        // Render health bar for Tank 1
        float healthPercentage = tank1Health / maxHealth;
        glm::mat3 healthBarModelMatrix = glm::mat3(1);
        healthBarModelMatrix *= transform2D::Translate(tank1X - healthBarWidth / 2.0f, tank1Y + 50.0f);

        // Scale the filled part according to the health
        glm::mat3 healthBarFullMatrix = healthBarModelMatrix;
        healthBarFullMatrix *= transform2D::Scale(healthPercentage, 1.0f);

        RenderMesh2D(meshes["healthBarFull"], shaders["VertexColor"], healthBarFullMatrix);
        RenderMesh2D(meshes["healthBarBorder"], shaders["VertexColor"], healthBarModelMatrix);
    }

    // Render Tank 2 and its health bar
    if (tank2Alive) {
        RenderTank(tank2X, tank2Y, tank2TurretAngle, tank2Angle,
                   "trapezoid1_tank2", "trapezoid2_tank2",
                   "turret_tank2", "cannon_tank2");

        // Render health bar for Tank 2
        float healthPercentage = tank2Health / maxHealth;
        glm::mat3 healthBarModelMatrix = glm::mat3(1);
        healthBarModelMatrix *= transform2D::Translate(tank2X - healthBarWidth / 2.0f, tank2Y + 50.0f);

        // Scale the filled part according to the health
        glm::mat3 healthBarFullMatrix = healthBarModelMatrix;
        healthBarFullMatrix *= transform2D::Scale(healthPercentage, 1.0f);

        RenderMesh2D(meshes["healthBarFull"], shaders["VertexColor"], healthBarFullMatrix);
        RenderMesh2D(meshes["healthBarBorder"], shaders["VertexColor"], healthBarModelMatrix);
    }

    // For Tank 1 trajectory guide
    if (tank1Alive) {
        // Compute trajectory
        std::vector<glm::vec2> tank1TrajectoryPoints;
        glm::vec2 cannonTip = GetCannonTipPosition(tank1X, tank1Y, tank1Angle, tank1TurretAngle, false);
        float totalAngle = tank1Angle + tank1TurretAngle;

        ComputeProjectileTrajectory(cannonTip.x, cannonTip.y, totalAngle, tank1TrajectoryPoints);
        UpdateTrajectoryMesh(tank1TrajectoryMesh, tank1TrajectoryPoints, "tank1Trajectory");

        // Render trajectory
        RenderMesh2D(tank1TrajectoryMesh, shaders["VertexColor"], glm::mat3(1));
    }

    // For Tank 2 trajectory guide
    if (tank2Alive) {
        // Compute trajectory
        std::vector<glm::vec2> tank2TrajectoryPoints;
        glm::vec2 cannonTip = GetCannonTipPosition(tank2X, tank2Y, tank2Angle, tank2TurretAngle, true);
        float totalAngle = tank2Angle + PI + tank2TurretAngle;

        ComputeProjectileTrajectory(cannonTip.x, cannonTip.y, totalAngle, tank2TrajectoryPoints);
        UpdateTrajectoryMesh(tank2TrajectoryMesh, tank2TrajectoryPoints, "tank2Trajectory");

        // Render trajectory
        RenderMesh2D(tank2TrajectoryMesh, shaders["VertexColor"], glm::mat3(1));
    }

    // Update and render clouds
    for (Cloud& cloud : clouds) {
        // Update cloud position
        cloud.position.x += cloud.speed * deltaTimeSeconds;

        // Wrap around if cloud moves off-screen
        if (cloud.position.x > window->GetResolution().x) {
            cloud.position.x = -100.0f; // Move to the left off-screen
            cloud.position.y = cloudSpawnY + rand() % 100; // Randomize Y position
        }

        // Render cloud
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(cloud.position.x, cloud.position.y);
        modelMatrix *= transform2D::Scale(cloud.scale, cloud.scale);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);
    }
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
    if (window->KeyHold(GLFW_KEY_UP)) tank2TurretAngle -= deltaTime;     // Adjusted for left-pointing cannon
    if (window->KeyHold(GLFW_KEY_DOWN)) tank2TurretAngle += deltaTime;

    // Clamp turret angle between -PI/2 and PI/2
    tank2TurretAngle = clamp(tank2TurretAngle, PI, 2.0 * PI);

    // Keep tanks on terrain
    AdjustTankPosition(tank1X, tank1Y);
    AdjustTankPosition(tank2X, tank2Y);
}

void Tema1::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE && tank1Alive) {  // Fire from Tank 1
        glm::vec2 cannonTip = GetCannonTipPosition(tank1X, tank1Y, tank1Angle, tank1TurretAngle, false);
        float totalAngle = tank1Angle + tank1TurretAngle;
        FireProjectile(cannonTip.x, cannonTip.y, totalAngle);
    } else if (key == GLFW_KEY_ENTER && tank2Alive) {  // Fire from Tank 2
        glm::vec2 cannonTip = GetCannonTipPosition(tank2X, tank2Y, tank2Angle, tank2TurretAngle, true);
        float totalAngle = tank2Angle + PI + tank2TurretAngle;
        FireProjectile(cannonTip.x, cannonTip.y, totalAngle);
    }
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
