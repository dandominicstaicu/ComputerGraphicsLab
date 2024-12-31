#include "lab_m1/Tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    camera = new implemented::CameraT2();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, Z_NEAR, Z_FAR);

    // Drone initialization
    rotorAngle = 0; // Initialize rotor angle
    dronePosition = glm::vec3(0, 0, 0);
    droneScale = glm::vec3(.1f, 1.0f, .1f); // Adjust dimensions as needed
    rotorScale = glm::vec3(1.f, 2.f, 1.f);

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    

    // Define a test position
    dronePosition = glm::vec3(0, 0, 0);

    {
        Shader* droneShader = new Shader("DroneShader");
        droneShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "DroneVertexShader.glsl"), GL_VERTEX_SHADER);
        droneShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "DroneFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        droneShader->CreateAndLink();
        shaders[droneShader->GetName()] = droneShader;
    }

}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{


    // Render the 1st box (drone body - horizontal box)
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, dronePosition);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f, 0.25f, 0.15f));
    shaders["DroneShader"]->Use();
    glUniform3f(glGetUniformLocation(shaders["DroneShader"]->GetProgramID(), "object_color"), 0.5f, 0.5f, 0.5f); // Grey color
    RenderMesh(meshes["box"], shaders["DroneShader"], modelMatrix);

    // Render the second box (drone body - vertical box)
    glm::mat4 modelMatrix2 = glm::mat4(1);
    modelMatrix2 = glm::translate(modelMatrix2, dronePosition);
    modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(90.0f), glm::vec3(0, 1, 0));
    modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(3.0f, 0.25f, 0.15f));
    glUniform3f(glGetUniformLocation(shaders["DroneShader"]->GetProgramID(), "object_color"), 0.5f, 0.5f, 0.5f); // Grey color
    RenderMesh(meshes["box"], shaders["DroneShader"], modelMatrix2);

    // Render small cubes at the ends of the horizontal and vertical boxes
    std::vector<glm::vec3> smallCubePositions = {
        glm::vec3(1.5f, 0, 0),
        glm::vec3(-1.5f, 0, 0),
        glm::vec3(0, 0, 1.5f),
        glm::vec3(0, 0, -1.5f)
    };

    for (const auto& pos : smallCubePositions) {
        glm::mat4 smallCube = glm::mat4(1);
        smallCube = glm::translate(smallCube, dronePosition + pos);
        smallCube = glm::scale(smallCube, glm::vec3(0.3f, 0.3f, 0.3f));
        glUniform3f(glGetUniformLocation(shaders["DroneShader"]->GetProgramID(), "object_color"), 0.5f, 0.5f, 0.5f); // Grey color
        RenderMesh(meshes["box"], shaders["DroneShader"], smallCube);
    }

    // Render pillars above each small cube
    for (const auto& pos : smallCubePositions) {
        glm::mat4 pillar = glm::mat4(1);
        pillar = glm::translate(pillar, dronePosition + pos + glm::vec3(0, 0.3f, 0)); // Slightly above the small cubes
        pillar = glm::scale(pillar, glm::vec3(0.1f, 0.3f, 0.1f));
        glUniform3f(glGetUniformLocation(shaders["DroneShader"]->GetProgramID(), "object_color"), 0.5f, 0.5f, 0.5f); // Grey color
        RenderMesh(meshes["box"], shaders["DroneShader"], pillar);
    }

    // Render propellers above each pillar
    for (const auto& pos : smallCubePositions) {
        glm::mat4 propeller = glm::mat4(1);
        propeller = glm::translate(propeller, dronePosition + pos + glm::vec3(0, 0.475f, 0)); // Above the pillars
        propeller = glm::scale(propeller, glm::vec3(0.6f, 0.05f, 0.1f));
        glUniform3f(glGetUniformLocation(shaders["DroneShader"]->GetProgramID(), "object_color"), 0.0f, 0.0f, 0.0f); // Black color
        RenderMesh(meshes["box"], shaders["DroneShader"], propeller);
    }

    // Attention! The `RenderMesh()` function overrides the usual
    // `RenderMesh()` that we've been using up until now. This new
    // function uses the view matrix from the camera that you just
    // implemented, and the local projection matrix.

    {

    }

    // Render the camera target. This is useful for understanding where
    // the rotation point is, when moving in third-person camera mode.
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
        float cameraSpeed = 2.0f;

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
