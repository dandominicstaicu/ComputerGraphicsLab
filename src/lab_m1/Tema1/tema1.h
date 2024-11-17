#pragma once

#include "components/simple_scene.h"

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        glm::mat3 modelMatrix;
        std::vector<float> heightMap; // Terrain height map

        float tank1X, tank1Y, tank2X, tank2Y;
        float tank1TurretAngle, tank2TurretAngle;
        float tankSpeed;
        glm::vec3 tank1Color, tank2Color;
        glm::vec3 tank1ColorBottom, tank2ColorBottom;
        float tank1Angle, tank2Angle; // Angles to align tanks with terrain

        // Gravity and projectile parameters
        float projectileSpeed = 500.0f;       // Adjust as needed
        float projectileLifespan = 5.0f;      // seconds
        glm::vec2 gravity = glm::vec2(0.0f, -400.0f); // Adjust as needed

        // Projectile struct and list
        struct Projectile {
            glm::vec2 position;
            glm::vec2 velocity;
            float lifespan;
        };
        std::vector<Projectile> projectiles;  // List to store active projectiles

        // Cloud struct and variables
        struct Cloud {
            glm::vec2 position;
            float speed;
            float scale;
        };
        
        std::vector<Cloud> clouds;
        int numberOfClouds;
        float cloudSpawnY;

        float healthBarWidth;
        float healthBarHeight;

        float maxHealth;
        float tank1Health;
        float tank2Health;
        bool tank1Alive;
        bool tank2Alive;
     
        // Collision detection parameters
        float projectileCollisionRadius;
        float tankCollisionRadius;
        float damagePerHit;

        float craterRadius = 35.0f;       // Radius of the crater
        float craterDepth = 10.0f;        // Depth of the crater
        float minTerrainHeight = -200.0f; // Minimum allowed terrain height

        float heightDifferenceThreshold = 0.1f; 
        float transferRate = 100.0f;             // units per second

        // For trajectory guide
        float offsetY; // Ground level offset
        void ComputeProjectileTrajectory(float startX, float startY, float angle, std::vector<glm::vec2>& trajectoryPoints);
        float GetTerrainHeightAt(float x);
        void UpdateTrajectoryMesh(Mesh*& mesh, const std::vector<glm::vec2>& trajectoryPoints, const std::string& meshName);

        // Trajectory meshes for each tank
        Mesh* tank1TrajectoryMesh;
        Mesh* tank2TrajectoryMesh;

        // Methods
        void GenerateTerrain();  // Function to initialize terrain data
        void RenderTank(float x, float y, float turretAngle, float bodyAngle,
                       const std::string& trapezoid1Name,
                       const std::string& trapezoid2Name,
                       const std::string& turretName,
                       const std::string& cannonName);

        void MoveTank(float &x, float &y, int direction, float deltaTimeSeconds);
        void AdjustTankPosition(float &x, float &y);
        void FireProjectile(float startX, float startY, float angle);
        glm::vec2 GetCannonTipPosition(float tankX, float tankY, float tankAngle, float turretAngle, bool cannonPointsLeft);

    };
}   // namespace m1
