// drone.cpp
#include "lab_m1/Tema2/drone.h"
#include "lab_m1/Tema2/tema2.h" // Include Tema2 definition
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace m1
{
    Drone::Drone()
        : rotorAngle(0.0f), position(0.0f), scale(0.1f, 1.0f, 0.1f), rotationY(0.0f), shader(nullptr), parent(nullptr)
    {
    }

    Drone::~Drone()
    {
        // Assuming Tema2 manages the lifetime of meshes and shaders
    }

    void Drone::Init(std::unordered_map<std::string, Mesh*>& meshesMap,
                     std::unordered_map<std::string, Shader*>& shadersMap,
                     Shader* droneShader,
                     Tema2* parentInstance) // Receive parent pointer
    {
        // Initialize meshes and shaders
        meshes = meshesMap;
        shaders = shadersMap;
        shader = droneShader;
        parent = parentInstance;

        // Additional initialization if necessary
    }

    void Drone::Update(float deltaTimeSeconds)
    {
        // Update rotor angle for animation
        rotorAngle += deltaTimeSeconds * 360.0f; // Adjust speed if needed
        if (rotorAngle >= 360.0f)
            rotorAngle -= 360.0f;
    }

    void Drone::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        if (!shader || !shader->program)
            return;

        shader->Use();
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        // Create the model matrix
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationY), glm::vec3(0, 1, 0));

        // Render drone components
        RenderBody(modelMatrix);
        RenderPropellers(modelMatrix);
        // RenderAxes(viewMatrix, projectionMatrix);
    }

    void Drone::RenderBody(const glm::mat4& modelMatrix)
    {
        if (parent)
        {
            // Render the horizontal and vertical bodies
            glm::mat4 bodyMatrix = glm::scale(modelMatrix, glm::vec3(3.0f, 0.25f, 0.15f));
            glUniform3f(glGetUniformLocation(shader->GetProgramID(), "object_color"), 0.5f, 0.5f, 0.5f); // Grey color
            parent->RenderMesh(meshes["cube"], shader, bodyMatrix);

            glm::mat4 verticalBodyMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
            verticalBodyMatrix = glm::scale(verticalBodyMatrix, glm::vec3(3.0f, 0.25f, 0.15f));
            glUniform3f(glGetUniformLocation(shader->GetProgramID(), "object_color"), 0.5f, 0.5f, 0.5f); // Grey color
            parent->RenderMesh(meshes["cube"], shader, verticalBodyMatrix);

            // Render small cubes at the ends
            std::vector<glm::vec3> smallCubePositions = {
                glm::vec3(1.5f, 0, 0),
                glm::vec3(-1.5f, 0, 0),
                glm::vec3(0, 0, 1.5f),
                glm::vec3(0, 0, -1.5f)
            };

            for (const auto& pos : smallCubePositions)
            {
                glm::mat4 smallCube = glm::translate(modelMatrix, pos);
                smallCube = glm::scale(smallCube, glm::vec3(0.3f, 0.3f, 0.3f));
                glUniform3f(glGetUniformLocation(shader->GetProgramID(), "object_color"), 0.5f, 0.5f, 0.5f); // Grey color
                parent->RenderMesh(meshes["cube"], shader, smallCube);
            }

            // Render pillars above each small cube
            for (const auto& pos : smallCubePositions)
            {
                glm::mat4 pillar = glm::translate(modelMatrix, pos + glm::vec3(0, 0.3f, 0));
                pillar = glm::scale(pillar, glm::vec3(0.1f, 0.3f, 0.1f));
                glUniform3f(glGetUniformLocation(shader->GetProgramID(), "object_color"), 0.5f, 0.5f, 0.5f); // Grey color
                parent->RenderMesh(meshes["cube"], shader, pillar);
            }
        }
    }

    void Drone::RenderPropellers(const glm::mat4& modelMatrix)
    {
        if (parent)
        {
            // Render propellers above each pillar
            std::vector<glm::vec3> smallCubePositions = {
                glm::vec3(1.5f, 0, 0),
                glm::vec3(-1.5f, 0, 0),
                glm::vec3(0, 0, 1.5f),
                glm::vec3(0, 0, -1.5f)
            };

            for (const auto& pos : smallCubePositions)
            {
                glm::mat4 propeller = glm::translate(modelMatrix, pos + glm::vec3(0, 0.475f, 0));
                propeller = glm::rotate(propeller, glm::radians(rotorAngle), glm::vec3(0, 1, 0)); // Rotate propellers
                propeller = glm::scale(propeller, glm::vec3(0.6f, 0.05f, 0.1f));
                glUniform3f(glGetUniformLocation(shader->GetProgramID(), "object_color"), 0.0f, 0.0f, 0.0f); // Black color
                parent->RenderMesh(meshes["cube"], shader, propeller);
            }
        }
    }

    void Drone::RenderAxes(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        if (parent)
        {
            // Render local coordinate system axes
            glm::mat4 axisMatrix = glm::mat4(1);
            axisMatrix = glm::translate(axisMatrix, position);
            axisMatrix = glm::rotate(axisMatrix, glm::radians(rotationY), glm::vec3(0, 1, 0)); // Align with drone's rotation

            // Ox (Red)
            glm::mat4 oxMatrix = glm::scale(axisMatrix * glm::translate(glm::mat4(1), glm::vec3(1.0f, 0, 0)), glm::vec3(1.0f, 0.05f, 0.05f));
            glUniform3f(glGetUniformLocation(shader->GetProgramID(), "object_color"), 1.0f, 0.0f, 0.0f); // Red
            parent->RenderMesh(meshes["cube"], shader, oxMatrix);

            // Oy (Green)
            glm::mat4 oyMatrix = glm::scale(axisMatrix * glm::translate(glm::mat4(1), glm::vec3(0, 1.0f, 0)), glm::vec3(0.05f, 1.0f, 0.05f));
            glUniform3f(glGetUniformLocation(shader->GetProgramID(), "object_color"), 0.0f, 1.0f, 0.0f); // Green
            parent->RenderMesh(meshes["cube"], shader, oyMatrix);

            // Oz (Blue)
            glm::mat4 ozMatrix = glm::scale(axisMatrix * glm::translate(glm::mat4(1), glm::vec3(0, 0, 1.0f)), glm::vec3(0.05f, 0.05f, 1.0f));
            glUniform3f(glGetUniformLocation(shader->GetProgramID(), "object_color"), 0.0f, 0.0f, 1.0f); // Blue
            parent->RenderMesh(meshes["cube"], shader, ozMatrix);
        }
    }

    // Input handling methods
    void Drone::MoveForward(float distance)
    {
        position += glm::vec3(-sin(glm::radians(rotationY)), 0, -cos(glm::radians(rotationY))) * distance;
    }

    void Drone::MoveBackward(float distance)
    {
        position -= glm::vec3(-sin(glm::radians(rotationY)), 0, -cos(glm::radians(rotationY))) * distance;
    }

    void Drone::MoveLeft(float distance)
    {
        position -= glm::vec3(cos(glm::radians(rotationY)), 0, -sin(glm::radians(rotationY))) * distance;
    }

    void Drone::MoveRight(float distance)
    {
        position += glm::vec3(cos(glm::radians(rotationY)), 0, -sin(glm::radians(rotationY))) * distance;
    }

    void Drone::MoveUp(float distance)
    {
        position += glm::vec3(0, 1, 0) * distance;
    }

    void Drone::MoveDown(float distance)
    {
        position -= glm::vec3(0, 1, 0) * distance;
    }

    void Drone::RotateLeft(float angle)
    {
        rotationY += angle;
        if (rotationY >= 360.0f)
            rotationY -= 360.0f;
    }

    void Drone::RotateRight(float angle)
    {
        rotationY -= angle;
        if (rotationY < 0.0f)
            rotationY += 360.0f;
    }

    // Getters and Setters
    glm::vec3 Drone::GetPosition() const
    {
        return position;
    }

    void Drone::SetPosition(const glm::vec3& pos)
    {
        position = pos;
    }
}
