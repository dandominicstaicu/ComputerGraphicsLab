#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1():colorCase(0), objectCase(0)
{
    // TODO(student): Never forget to initialize class variables!
    changeClearColor();
	changeObject();
}


Lab1::~Lab1()
{
}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): Load some more meshes. The value of RESOURCE_PATH::MODELS
    // is actually a path on disk, go there and you will find more meshes.
    {
        Mesh* bunnyMesh = new Mesh("bunny");
        bunnyMesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "animals"), "bunny.obj");
        meshes[bunnyMesh->GetMeshID()] = bunnyMesh;
    }

    {
        Mesh* sphereMesh = new Mesh("sphere");
        sphereMesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[sphereMesh->GetMeshID()] = sphereMesh;
    }

    {
        Mesh* teapotMesh = new Mesh("teapot");
        teapotMesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
        meshes[teapotMesh->GetMeshID()] = teapotMesh;
    }

}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer

    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
    glClearColor(clearRed, clearGreen, clearBlue, 1);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    RenderMesh(meshes["box"], glm::vec3(-1, 0.1f, 0), glm::vec3(0.5f));

    // Render the object again but with different properties
    RenderMesh(meshes["box"], glm::vec3(-2, 0.5f, 0));

    // TODO(student): We need to render (a.k.a. draw) the mesh that
    // was previously loaded. We do this using `RenderMesh`. Check the
    // signature of this function to see the meaning of its parameters.
    // You can draw the same mesh any number of times.
    RenderMesh(meshes[objModel],  glm::vec3(posX, posY, posZ), glm::vec3(0.1f));

}


void Lab1::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // TODO(student): Add some key hold events that will let you move
    // a mesh instance on all three axes. You will also need to
    // generalize the position used by `RenderMesh`.

    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {

		// treat continuous update based on input
		if (window->KeyHold(GLFW_KEY_W))
		{
			posZ -= deltaTime * SPEED_FACTOR;
		}
		if (window->KeyHold(GLFW_KEY_S))
		{
			posZ += deltaTime * SPEED_FACTOR;
		}
		if (window->KeyHold(GLFW_KEY_A))
		{
			posX -= deltaTime * SPEED_FACTOR;
		}
		if (window->KeyHold(GLFW_KEY_D))
		{
			posX += deltaTime * SPEED_FACTOR;
		}
        if (window->KeyHold(GLFW_KEY_E))
		{
			posY += deltaTime * SPEED_FACTOR;
		}
        if (window->KeyHold(GLFW_KEY_Q))
		{
			posY += deltaTime * SPEED_FACTOR;
		}
    }

}



void Lab1::changeClearColor()
{
    switch (colorCase)
    {
    case 1:
        clearRed	= 0.286f;
		clearGreen	= 0.00f;
		clearBlue	= 0.0f;
        break;
    case 2:
        clearRed	= 1.0f;
		clearGreen	= 1.0f;
		clearBlue	= 1.0f;
        break;
    default:
        break;
    }
}

void Lab1::changeObject()
{
	switch (objectCase)
	{
	case 1:
		objModel		= "sphere";
		objScale	= glm::vec3(5.f);
		break;

	case 2:
		objModel		= "teapot";
		objScale	= glm::vec3(5.f);
		break;

	default:
		objModel		= "bunny";
		objScale	= glm::vec3(0.01f);
		break;
	}
}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F) {
        // TODO(student): Change the values of the color components.
        colorCase = (colorCase + 1) % NUM_COLORS;

		changeClearColor();
    }


    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.

        if (key == GLFW_KEY_X) {
		objectCase = (objectCase + 1) % NUM_OBJECTS;

		changeObject();
	}


}



void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}

