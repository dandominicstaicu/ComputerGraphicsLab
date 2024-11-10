#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateCircle(
    const std::string& name,
    glm::vec3 center,      // Center position for the circle
    float radius,
    glm::vec3 color,
    bool fill,
    int numSegments)  //  = 30 default number of segments for smoothness
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Center of the circle
    vertices.push_back(VertexFormat(center, color));  // Center vertex at specified position

    // Create vertices along the circumference
    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * PI * i / numSegments;
        float x = center.x + radius * cos(angle);  // Offset by center
        float y = center.y + radius * sin(angle);  // Offset by center

        vertices.push_back(VertexFormat(glm::vec3(x, y, center.z), color));
        if (i > 0) {
            indices.push_back(0);        // Center vertex
            indices.push_back(i);        // Current vertex
            indices.push_back(i + 1);    // Next vertex
        }
    }

    // Close the circle by connecting the last segment to the first vertex on the circumference
    indices.push_back(0);
    indices.push_back(numSegments);
    indices.push_back(1);

    Mesh* circle = new Mesh(name);
    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* object2D::CreateTrapezoid(const std::string& name, glm::vec3 bottomLeft, float widthTop, float widthBottom, float height, glm::vec3 color)
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(bottomLeft, color),                         // Bottom-left
        VertexFormat(bottomLeft + glm::vec3(widthBottom, 0, 0), color),   // Bottom-right
        VertexFormat(bottomLeft + glm::vec3((widthBottom - widthTop) / 2, height, 0), color), // Top-left
        VertexFormat(bottomLeft + glm::vec3((widthBottom + widthTop) / 2, height, 0), color)  // Top-right
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 2, 1, 3 };

    Mesh* trapezoid = new Mesh(name);
    trapezoid->InitFromData(vertices, indices);
    return trapezoid;
}

#include "object2D.h"
#include <vector>
#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object2D::CreateRectangle(
    const std::string &name,
    glm::vec3 bottomLeftCorner,
    float width,
    float height,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(bottomLeftCorner, color),
        VertexFormat(bottomLeftCorner + glm::vec3(width, 0, 0), color),
        VertexFormat(bottomLeftCorner + glm::vec3(width, height, 0), color),
        VertexFormat(bottomLeftCorner + glm::vec3(0, height, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (fill) {
        // Draw 2 triangles for a filled rectangle
        indices.push_back(0);
        indices.push_back(2);
    }

    Mesh* rectangle = new Mesh(name);
    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

