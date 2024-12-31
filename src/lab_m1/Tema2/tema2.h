#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/t2_camera.h"
#include "lab_m1/Tema2/drone.h" 

#define Z_FAR		(200.f)
#define Z_NEAR		(.01f)

namespace m1
{
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

        Drone drone;

     protected:
        implemented::CameraT2 *camera;
        glm::mat4 projectionMatrix;

   public:
      void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

    };
}   // namespace m1