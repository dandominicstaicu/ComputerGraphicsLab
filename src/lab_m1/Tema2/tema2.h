#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/t2_camera.h"
#include "lab_m1/Tema2/drone.h"
#include "lab_m1/Tema2/terrain.h"
#include "lab_m1/Tema2/obstacle.h"
#include "lab_m1/Tema2/building.h"
#include "lab_m1/Tema2/tree.h"
#include "lab_m1/Tema2/checkpoint.h"


#include <random>
#include <ctime>

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#define Z_FAR		(200.f)
#define Z_NEAR		(.01f)

namespace m1
{
   // Structure to hold character information for text rendering
    struct Character {
        GLuint TextureID;   // Texture ID handle
        glm::ivec2 Size;    // Size of glyph
        glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
        GLuint Advance;     // Offset to advance to next glyph
    };

   class Obstacle; 

    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

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

         float CalculateDistance(const glm::vec3& a, const glm::vec3& b);

         void GenerateBuildings(int numBuildings,
                              float minDistance,
                              float terrainHalfSize,
                              int maxPlacementAttempts,
                              std::mt19937 &rng,
                              std::uniform_real_distribution<float> &distPos,
                              std::uniform_real_distribution<float> &distScaleBuildings,
                              std::vector<glm::vec3> &placedPositions);

         void GenerateTrees(int numTrees,
                          float minDistance,
                          float terrainHalfSize,
                          int maxPlacementAttempts,
                          std::mt19937 &rng,
                          std::uniform_real_distribution<float> &distPos,
                          std::uniform_real_distribution<float> &distScaleTrees,
                          std::vector<glm::vec3> &placedPositions);


         Drone drone;
         Terrain terrain;

         glm::vec3 dronePrevPos;

         std::vector<Obstacle*> obstacles; // Vector to store obstacles

         std::vector<Checkpoint*> checkpoints;

         size_t currentCheckpointIndex = 0; 

      // **Camera Mode Enumeration**
        enum CameraMode { FIRST_PERSON, THIRD_PERSON };
        CameraMode currentCameraMode = FIRST_PERSON; // Default mode

        // **Third-Person Camera Offsets**
        float thirdPersonDistance = 5.0f; // Distance behind the drone
        float thirdPersonHeight = 2.0f;   // Height above the drone

         // **Text Rendering Components**
        std::map<GLchar, Character> Characters; // Holds all characters
        GLuint VAO_text, VBO_text;              // Vertex Array Object and Vertex Buffer Object for text rendering
        Shader* textShader;                     // Shader used for text rendering

        // **Timer Variables**
        float totalTime = 120.0f;   // Total time in seconds (e.g., 2 minutes)
        float remainingTime = 120.0f; // Remaining time in seconds

     protected:
        implemented::CameraT2 *camera;
        glm::mat4 projectionMatrix;

   public:
      void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
      void UpdateCheckpoint();
      void CheckCheckpointCollisions();

      // RenderText Function
        void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

    };
}   // namespace m1