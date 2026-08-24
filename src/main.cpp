#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Shader.hpp"
#include "Model.hpp"
#include "Math.hpp"

#include <iostream>
#include <cmath>
#include <string>

void processInput(GLFWwindow* window, Vec3& position, Vec3& rotation, bool& textureEnabled)
{
	const float moveSpeed = 0.02f;
	const float rotationSpeed = 0.02f;

	// --------------------------------
	// Salir
	// --------------------------------
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// --------------------------------
	// Traslación X
	// --------------------------------
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position.x -= moveSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position.x += moveSpeed;
	}

	// --------------------------------
	// Traslación Y
	// --------------------------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position.y += moveSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position.y -= moveSpeed;
	}

	// --------------------------------
	// Traslación Z
	// --------------------------------
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		position.z += moveSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		position.z -= moveSpeed;
	}

	// --------------------------------
	// Rotación X
	// --------------------------------
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		rotation.x += rotationSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		rotation.x -= rotationSpeed;
	}

	// --------------------------------
	// Rotación Y
	// --------------------------------
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		rotation.y += rotationSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		rotation.y -= rotationSpeed;
	}

	// --------------------------------
	// Rotación Z
	// --------------------------------
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		rotation.z += rotationSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		rotation.z -= rotationSpeed;
	}

	// --------------------------------
	// Reset
	// --------------------------------
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		position = {
			0.0f,
			0.0f,
			0.0f
		};

		rotation = {
			0.0f,
			0.0f,
			0.0f
		};
	}

	// --------------------------------
	// Toggle textura
	// --------------------------------
	static bool previousT = false;
	bool currentT = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
	if (currentT && !previousT)
	{
		textureEnabled = !textureEnabled;
	}
	previousT = currentT;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	(void)window;

	if (height == 0)
		return;

	glViewport(0, 0, width, height);
}

void runOpenGL(GLFWwindow* window, const std::string& modelPath, const std::string& texturePath)
{
	// --------------------------------
	// Shader
	// --------------------------------
	Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

	// --------------------------------
	// Modelo
	// --------------------------------
	Model model(modelPath, texturePath);

	// --------------------------------
	// Depth testing
	// --------------------------------
	glEnable(GL_DEPTH_TEST);

	// --------------------------------
	// Viewport
	// --------------------------------
	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// --------------------------------
	// Modelo
	// --------------------------------
	Vec3 objectPosition = {
		0.0f,
		0.0f,
		0.0f
	};

	Vec3 objectRotation = {
		0.0f,
		0.0f,
		0.0f
	};

	// --------------------------------
	// Textura
	// --------------------------------
	bool textureEnabled = false;
	float textureMix = 0.0f;

	// --------------------------------
	// Cámara
	// --------------------------------
	Vec3 cameraPosition = {
		0.0f,
		0.0f,
		3.0f
	};

	Vec3 cameraTarget = {
		0.0f,
		0.0f,
		0.0f
	};

	Mat4 view = viewMatrix(cameraPosition, cameraTarget);

	// --------------------------------
	// Bucle principal
	// --------------------------------
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// --------------------------------
		// Input
		// --------------------------------
		processInput(window, objectPosition, objectRotation, textureEnabled);

		// --------------------------------
		// Textura suave
		// --------------------------------
		const float textureSpeed = 0.03f;

		if (textureEnabled)
		{
			textureMix += textureSpeed;
			if (textureMix > 1.0f)
			textureMix = 1.0f;
		}
		else
		{
			textureMix -= textureSpeed;
			if (textureMix < 0.0f)
			textureMix = 0.0f;
		}

		// --------------------------------
		// Clear
		// --------------------------------
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// --------------------------------
		// MODEL MATRIX
		// --------------------------------
		Mat4 rotationX = rotationXMatrix(objectRotation.x);
		Mat4 rotationY = rotationYMatrix(objectRotation.y);
		Mat4 rotationZ = rotationZMatrix(objectRotation.z);
		Mat4 rotationXY = multiplyMatrix(rotationY, rotationX);
		Mat4 rotation = multiplyMatrix(rotationZ, rotationXY);
		Mat4 modelMatrix = multiplyMatrix(translationMatrix(objectPosition), rotation);

		// --------------------------------
		// PROJECTION MATRIX
		// --------------------------------
		glfwGetFramebufferSize(window, &width, &height);

		if (height == 0)
			height = 1;
		float aspect = static_cast<float>(width) / static_cast<float>(height);
		float fov = 45.0f * 3.14159265359f / 180.0f;
		Mat4 projection = perspectiveMatrix(fov, aspect, 0.1f, 100.0f);

		// --------------------------------
		// Shader
		// --------------------------------
		shader.use();
		shader.setMatrix("model", modelMatrix);
		shader.setMatrix("view", view);
		shader.setMatrix("projection", projection);

		// --------------------------------
		// Textura
		// --------------------------------
		shader.setFloat("textureMix", textureMix);
		shader.setInt("textureSampler", 0);

		// --------------------------------
		// Draw
		// --------------------------------
		model.draw();

		// --------------------------------
		// Present
		// --------------------------------
		glfwSwapBuffers(window);
	}
}

int main(int argc, char** argv)
{
	// --------------------------------
	// Paths
	// --------------------------------
	std::string modelPath = "models/42.obj";
	std::string texturePath = "textures/texture.ppm";

	if (argc >= 2)
	{
		modelPath = argv[1];
	}

	if (argc >= 3)
	{
		texturePath = argv[2];
	}

	// --------------------------------
	// GLFW
	// --------------------------------
	if (!glfwInit())
	{
		std::cerr << "Error al inicializar GLFW\n";
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// --------------------------------
	// Window
	// --------------------------------
	GLFWwindow* window = glfwCreateWindow(800, 600, "SCOP", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "No se pudo crear la ventana\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	// --------------------------------
	// Resize callback
	// --------------------------------
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// --------------------------------
	// GLAD
	// --------------------------------
	if (gladLoadGL(glfwGetProcAddress) == 0)
	{
		std::cerr << "Error al inicializar GLAD\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}

	// --------------------------------
	// Info
	// --------------------------------
	std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
	std::cout << "Modelo: " << modelPath << "\n";
	std::cout << "Textura: " << texturePath << "\n";

	// --------------------------------
	// Run
	// --------------------------------
	runOpenGL(window, modelPath, texturePath);

	// --------------------------------
	// Cleanup
	// --------------------------------
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}