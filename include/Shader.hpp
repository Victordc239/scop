#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/gl.h>
#include <string>

#include "Math.hpp"

class Shader
{
	public:
		Shader(const std::string& vertexPath, const std::string& fragmentPath);
		~Shader();

		void use() const;
		void setMatrix(const std::string& name, const Mat4& matrix) const;
		void setFloat(const std::string& name, float value) const;
		void setInt(const std::string& name, int value) const;

	private:
		unsigned int program;
		std::string readFile(const std::string& path) const;
		unsigned int compileShader(unsigned int type, const std::string& source) const;
};

#endif