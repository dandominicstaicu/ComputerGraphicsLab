#pragma once

#include "components/simple_scene.h"


namespace m1
{
    #define NUM_COLORS		(3)
    #define NUM_OBJECTS		(3)

    #define SPEED_FACTOR	(2.f)

    

    class Lab1 : public gfxc::SimpleScene
    {
     public:
        Lab1();
        ~Lab1();

        void Init() override;

     private:
        GLuint colorCase, objectCase;
		GLfloat clearRed, clearGreen, clearBlue;

        std::string objModel;
        glm::vec3 objScale;

        GLfloat posX, posY, posZ;

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
        void changeClearColor();
        void changeObject();

        // TODO(student): Class variables go here

    };
}   // namespace m1
