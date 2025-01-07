#include "checkpoint.h"


namespace m1
{
    Checkpoint::Checkpoint(glm::vec3 pos, glm::vec3 col)
        : position(pos), color(col)
    {
        // Define the size of the checkpoint
        glm::vec3 size = glm::vec3(7.0f, 1.0f, 7.0f); // Width, Height, Depth

        // Initialize Hitbox's localMin and localMax relative to model origin
        hitbox.localMin = glm::vec3(-0.04f, -0.18f, -0.18f);
        hitbox.localMax = glm::vec3( 0.04f,  0.18f,  0.18f);

        // Initially, worldMin and worldMax are same as local (will be updated)
        hitbox.worldMin = hitbox.localMin;
        hitbox.worldMax = hitbox.localMax;

    }

    Checkpoint::~Checkpoint()
    {
    }

     void Checkpoint::DrawHitbox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const Mesh* cubeMesh, Shader* shader)
    {
        // Ensure the AABBShader and hitbox mesh are available
        if (!shader || !shader->program || !cubeMesh)
            return;

        // Calculate size and center of the hitbox
        glm::vec3 size = hitbox.worldMax - hitbox.worldMin;
        glm::vec3 center = (hitbox.worldMax + hitbox.worldMin) * 0.5f;

        // Create the model matrix for the hitbox
        glm::mat4 modelBox = glm::mat4(1.0f);
        modelBox = glm::translate(modelBox, center);
        modelBox = glm::scale(modelBox, size);

        // Set shader uniforms
        shader->Use();
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelBox));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        // Set the hitbox color to red
        glUniform3f(glGetUniformLocation(shader->GetProgramID(), "objectColor"), 1.0f, 0.0f, 0.0f); // Red

        // Enable wireframe mode for hitbox visualization
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Render the cube hitbox
        cubeMesh->Render();

        // Restore to fill mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Checkpoint::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, 
                            Mesh* ringMesh, Shader* obstacleShader, Shader* aabbShader, Mesh* cubeMesh)
    {
        if (!ringMesh || !obstacleShader || !aabbShader || !cubeMesh)
            return;

        // Set up the model matrix
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(7.0f));

        // Update Hitbox based on the model matrix
        hitbox.Update(modelMatrix);

        // Set the color uniform
        obstacleShader->Use(); // Activate the shader before setting uniforms
        GLint colorLocation = glGetUniformLocation(obstacleShader->GetProgramID(), "objectColor");
        if(colorLocation == -1) {
            std::cerr << "Uniform 'objectColor' not found in ObstacleShader!\n";
        }
        else {
            glUniform3fv(colorLocation, 1, glm::value_ptr(color));
        }


        obstacleShader->Use();
        glUniformMatrix4fv(glGetUniformLocation(obstacleShader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(obstacleShader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(obstacleShader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        // Render the ring mesh
        ringMesh->Render();


    }
}
