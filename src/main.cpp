#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Error al inicializar GLFW\n";
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "SCOP", nullptr, nullptr);

	if (!window)
	{
		std::cerr << "No se pudo crear la ventana\n";
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	if (gladLoadGL(glfwGetProcAddress) == 0)
	{
		std::cerr << "Error al inicializar GLAD\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}