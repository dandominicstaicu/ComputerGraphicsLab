// lab_m1/Tema2/ui_manager.h
#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "components/simple_scene.h"

#define WINDOW_PERCENT 1.0f

namespace m1
{
    struct Character {
        GLuint TextureID;   // Texture ID handle
        glm::ivec2 Size;    // Size of glyph
        glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
        GLuint Advance;     // Offset to advance to next glyph
    };

    class UIManager
    {
    public:
        UIManager();
        ~UIManager();

        // Initialize UI components with specific resources
        void Init(WindowObject* window, std::unordered_map<std::string, Shader*>& shaders);

        // Render the arrow indicator
        void RenderArrow(Shader* uiShader, const glm::mat4& projection, const glm::vec3& cameraPos,
                        const glm::vec3& cameraForward, const glm::vec3& arrowPosition, float windowWidth, float windowHeight);

        // Render text on the screen
        void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

        // Handle window resize for UI
        void OnWindowResize(int width, int height, Shader* uiShader, Shader* textShader);

    private:
        // Text rendering components
        std::map<GLchar, Character> Characters;
        GLuint VAO_text, VBO_text;

        // Arrow rendering components
        Shader* arrowShader;
        Mesh* arrowMesh;

        // Reference to shaders and window
        WindowObject* windowRef;
        std::unordered_map<std::string, Shader*>* shadersRef;

        // Initialize text rendering
        void InitText();

        // Initialize arrow rendering
        void InitArrow();

        // Load fonts and prepare characters
        void LoadFont();

        // FreeType library initialization
        FT_Library ft;
    };
}
