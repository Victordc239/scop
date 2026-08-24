#include "Shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

std::string Shader::readFile(const std::string& path) const
{
	std::ifstream file(path);

	if (!file)
	{
		std::cerr << "No se pudo abrir " << path << "\n";
		return "";
	}

	std::stringstream stream;
	stream << file.rdbuf();
	return stream.str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) const
{
	unsigned int shader = glCreateShader(type);
	const char* shaderSource = source.c_str();
	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);
	int success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[1024];
		glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
		std::cerr << "Error compilando Shader:\n" << infoLog << "\n";
	}

	return shader;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) : program(0)
{
	std::string vertexCode = readFile(vertexPath);
	std::string fragmentCode = readFile(fragmentPath);
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	int success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[1024];
		glGetProgramInfoLog(program, 1024, nullptr, infoLog);
		std::cerr << "Error enlazando Shader Program:\n" << infoLog << "\n";
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
	if (program != 0)
		glDeleteProgram(program);
}

void Shader::use() const
{
	glUseProgram(program);
}

void Shader::setMatrix(const std::string& name, const Mat4& matrix) const
{
	int location = glGetUniformLocation(program, name.c_str());
	if (location == -1)
	{
		std::cerr << "No se encontró el uniform: " << name << "\n";
		return;
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix.data);
}

void Shader::setFloat(const std::string& name, float value) const
{
	int location = glGetUniformLocation(program, name.c_str());
	if (location == -1)
	{
		std::cerr << "No se encontró el uniform: " << name << "\n";
		return;
	}

	glUniform1f(location, value);
}

void Shader::setInt(const std::string& name, int value) const
{
	int location = glGetUniformLocation(program, name.c_str());
	if (location == -1)
	{
		std::cerr << "No se encontró el uniform: " << name << "\n";
		return;
	}

	glUniform1i(location, value);
}