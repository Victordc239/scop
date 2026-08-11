#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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

	// --------------------------------
	// Leer Vertex Shader
	// --------------------------------

	std::ifstream vertexFile("shaders/vertex.glsl");

	if (!vertexFile)
	{
		std::cerr << "No se pudo abrir shaders/vertex.glsl\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}

	std::stringstream vertexStream;
	vertexStream << vertexFile.rdbuf();

	std::string vertexCode = vertexStream.str();

	// --------------------------------
	// Crear Vertex Shader
	// --------------------------------

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexSource = vertexCode.c_str();
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);

	// --------------------------------
	// Comprobar compilación
	// --------------------------------

	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cerr << "Error compilando Vertex Shader:\n" << infoLog << std::endl;
	}

	// --------------------------------
	// Leer Fragment Shader
	// --------------------------------

	std::ifstream fragmentFile("shaders/fragment.glsl");
	if (!fragmentFile)
	{
		std::cerr << "No se pudo abrir shaders/fragment.glsl\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}

	std::stringstream fragmentStream;
	fragmentStream << fragmentFile.rdbuf();

	std::string fragmentCode = fragmentStream.str();

	// --------------------------------
	// Crear Fragment Shader
	// --------------------------------

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentSource = fragmentCode.c_str();

	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);

	// --------------------------------
	// Comprobar Fragment Shader
	// --------------------------------

	int fragmentSuccess;

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);

	if (!fragmentSuccess)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cerr << "Error compilando Fragment Shader:\n" << infoLog << std::endl;
	}

	// --------------------------------
	// Crear Shader Program
	// --------------------------------

	unsigned int shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	// --------------------------------
	// Comprobar linking
	// --------------------------------

	int programSuccess;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programSuccess);

	if (!programSuccess)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cerr << "Error enlazando Shader Program:\n" << infoLog << std::endl;
	}

	// Ya no necesitamos los shaders individuales
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// --------------------------------
	// Vértices
	// --------------------------------

	float vertices[] = {
		0.0f,  0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);

	// --------------------------------
	// Bucle principal
	// --------------------------------

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}