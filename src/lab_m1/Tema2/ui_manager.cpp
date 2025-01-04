// lab_m1/Tema2/ui_manager.cpp
#include "lab_m1/Tema2/ui_manager.h"

#include <iostream>
#include "components/simple_scene.h"

using namespace std;
using namespace m1;

// Constructor
UIManager::UIManager()
    : VAO_text(0), VBO_text(0), arrowShader(nullptr), arrowMesh(nullptr), windowRef(nullptr), shadersRef(nullptr)
{
}

// Destructor
UIManager::~UIManager()
{
    // Cleanup text resources
    for (auto& c : Characters)
    {
        glDeleteTextures(1, &c.second.TextureID);
    }
    glDeleteVertexArrays(1, &VAO_text);
    glDeleteBuffers(1, &VBO_text);

    // Cleanup arrow resources
    if (arrowMesh)
    {
        delete arrowMesh;
    }
    if (arrowShader)
    {
        delete arrowShader;
    }

    // Cleanup FreeType
    FT_Done_FreeType(ft);
}

void UIManager::Init(WindowObject* window, std::unordered_map<std::string, Shader*>& shaders)
{
    windowRef = window;
    shadersRef = &shaders;

    // Initialize arrow
    InitArrow();

    // Initialize text rendering
    InitText();
}

void UIManager::InitArrow()
{
    // Create arrow mesh
    // Assuming the arrow is a simple 2D triangle as defined earlier
    std::vector<VertexFormat> vertices =
    {
        // Position            // Normal      // Texture (optional)
        VertexFormat(glm::vec3(0.0f,  0.1f, 0.0f), glm::vec3(0, 0, 1)),  // Tip
        VertexFormat(glm::vec3(-0.05f, -0.05f, 0.0f), glm::vec3(0, 0, 1)), // Left base
        VertexFormat(glm::vec3(0.05f, -0.05f, 0.0f), glm::vec3(0, 0, 1))  // Right base
    };

    std::vector<unsigned int> indices =
    {
        0, 1, 2
    };

    arrowMesh = new Mesh("arrow");
    arrowMesh->InitFromData(vertices, indices);

    // Initialize arrow shader
    arrowShader = new Shader("UIShader"); // Reusing UIShader
    arrowShader->AddShader(PATH_JOIN(windowRef->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "UIVertexShader.glsl"), GL_VERTEX_SHADER);
    arrowShader->AddShader(PATH_JOIN(windowRef->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "UIFragmentShader.glsl"), GL_FRAGMENT_SHADER);
    if (!arrowShader->CreateAndLink())
    {
        std::cerr << "Failed to create and link UIShader for Arrow.\n";
    }
}

void UIManager::InitText()
{
    // Initialize FreeType library
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
    std::string fontPath = PATH_JOIN(windowRef->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf");
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font at " << fontPath << std::endl;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48); // Font size 48, adjust as needed

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph for character '" << c << "'\n";
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // Destroy FreeType face
    FT_Done_Face(face);

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO_text);
    glGenBuffers(1, &VBO_text);
    glBindVertexArray(VAO_text);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Initialize TextShader
    Shader* textShader = new Shader("TextShader");
    textShader->AddShader(PATH_JOIN(windowRef->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TextVertexShader.glsl"), GL_VERTEX_SHADER);
    textShader->AddShader(PATH_JOIN(windowRef->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TextFragmentShader.glsl"), GL_FRAGMENT_SHADER);
    if (!textShader->CreateAndLink())
    {
        std::cerr << "Failed to create and link TextShader.\n";
    }

    // Set projection matrix for TextShader
    textShader->Use();
    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<GLfloat>(windowRef->GetResolution().x),
        static_cast<GLfloat>(windowRef->GetResolution().y), 0.0f,
        -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(textShader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    // Initialize model matrix to identity
    glm::mat4 textModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(textShader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(textModel));

    // Store the TextShader in the shaders map
    if (shadersRef)
    {
        (*shadersRef)["TextShader"] = textShader;
    }
}

void UIManager::RenderArrow(Shader* uiShader, const glm::mat4& projection, const glm::vec3& cameraPos,
    const glm::vec3& cameraForward, const glm::vec3& arrowPosition, float windowWidth, float windowHeight)
{
    // Compute direction from camera to arrow position
    glm::vec3 toArrow = arrowPosition - cameraPos;
    glm::vec2 toArrowXZ = glm::normalize(glm::vec2(toArrow.x, toArrow.z));

    glm::vec2 cameraForwardXZ = glm::normalize(glm::vec2(cameraForward.x, cameraForward.z));

    // Compute angle between camera forward and toArrow vectors
    float angleRad = atan2(toArrowXZ.y, toArrowXZ.x) - atan2(cameraForwardXZ.y, cameraForwardXZ.x);
    float angleDeg = glm::degrees(angleRad);

    // Normalize angle to [0, 360)
    if (angleDeg < 0)
        angleDeg += 360.0f;

    // Make arrow size responsive
    float arrowSize = windowHeight * WINDOW_PERCENT; // % of window height

    // Position the arrow at the bottom center, 50 pixels from the bottom
    glm::vec3 arrowPos = glm::vec3(windowWidth / 2.0f, windowHeight - 600.0f, 0.0f);

    // Create model matrix for the arrow
    glm::mat4 arrowModel = glm::mat4(1.0f);
    arrowModel = glm::translate(arrowModel, arrowPos);
    arrowModel = glm::rotate(arrowModel, glm::radians(angleDeg), glm::vec3(0, 0, 1));
    arrowModel = glm::scale(arrowModel, glm::vec3(arrowSize, arrowSize, 1.0f));

    // Use UIShader
    uiShader->Use();

    // Set color
    glm::vec3 arrowColor(1.0f, 1.0f, 0.0f); // Yellow
    glUniform3fv(glGetUniformLocation(uiShader->GetProgramID(), "objectColor"), 1, glm::value_ptr(arrowColor));

    // Set model matrix
    glUniformMatrix4fv(glGetUniformLocation(uiShader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(arrowModel));

    // Render the arrow
    if (arrowMesh)
    {
        arrowMesh->Render();
    }
}

void UIManager::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state	
    Shader* shader = (*shadersRef)["TextShader"];
    shader->Use();
    glUniform3f(glGetUniformLocation(shader->GetProgramID(), "textColor"), color.x, color.y, color.z);
    glUniform1i(glGetUniformLocation(shader->GetProgramID(), "text"), 0); // Set 'text' sampler to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_text);

    // Iterate through all characters
    std::string::const_iterator c_it;
    for (c_it = text.begin(); c_it != text.end(); c_it++)
    {
        if (Characters.find(*c_it) == Characters.end()) {
            std::cerr << "Character '" << *c_it << "' not found in Characters map.\n";
            continue;
        }

        Character ch = Characters[*c_it];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void UIManager::OnWindowResize(int width, int height, Shader* uiShader, Shader* textShader)
{
    // Update projection matrix for UIShader
    glm::mat4 uiProjection = glm::ortho(0.0f, static_cast<float>(width),
        static_cast<float>(height), 0.0f,
        -1.0f, 1.0f);

    uiShader->Use();
    glUniformMatrix4fv(glGetUniformLocation(uiShader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(uiProjection));

    // Update projection matrix for TextShader
    if (textShader) {
        textShader->Use();
        glm::mat4 textProjection = glm::ortho(0.0f, static_cast<GLfloat>(width),
            static_cast<GLfloat>(height), 0.0f,
            -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(textShader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));
    }
}
