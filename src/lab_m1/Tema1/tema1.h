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
        
        

        void GenerateTerrain();  // Function to initialize terrain data
      //   void RenderTank(float x, float y, float turretAngle, const glm::vec3 &color);
      //   void RenderTank(float x, float y, float turretAngle,
      //                  const std::string& trapezoid1Name,
      //                  const std::string& trapezoid2Name,
      //                  const std::string& turretName,
      //                  const std::string& cannonName,
      //                  bool mirrored);
      void RenderTank(float x, float y, float turretAngle, float bodyAngle,
                       const std::string& trapezoid1Name,
                       const std::string& trapezoid2Name,
                       const std::string& turretName,
                       const std::string& cannonName,
                       bool mirrored);

        void MoveTank(float &x, float &y, int direction, float deltaTimeSeconds);
        void AdjustTankPosition(float &x, float &y);

    };
}   // namespace m1
