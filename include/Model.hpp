#ifndef MODEL_HPP
#define MODEL_HPP

#include <glad/gl.h>
#include <string>
#include <vector>

#include "Math.hpp"

class Model
{
	public:
		Model(const std::string& modelPath, const std::string& texturePath);
		~Model();

		void draw() const;

	private:
		struct Vec2
		{
			float u;
			float v;
		};

		struct Vertex
		{
			Vec3 position;
			Vec3 color;
			Vec2 texCoord;
		};

		unsigned int VAO;
		unsigned int VBO;
		unsigned int texture;
		unsigned int vertexCount;
		std::vector<Vec3> positions;
		std::vector<Vec2> texCoords;
		std::vector<Vertex> vertices;
		void loadOBJ(const std::string& path);
		void createBuffers();
		void loadTexture(const std::string& path);
		void centerModel();
		Vec2 generateTextureCoordinate(const Vec3& position) const;
		Vec3 getFaceColor(unsigned int faceIndex) const;
};

#endif