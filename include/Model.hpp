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

		bool hasMaterial() const;

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
			Vec3 materialColor;
			Vec2 texCoord;
		};

		unsigned int VAO;
		unsigned int VBO;
		unsigned int texture;
		unsigned int vertexCount;

		Vec3 materialColor;
		bool materialValid;

		std::vector<Vec3> positions;
		std::vector<Vec2> texCoords;
		std::vector<Vertex> vertices;

		void loadOBJ(const std::string& path);
		void createBuffers();
		void loadTexture(const std::string& path);
		void centerModel();
		Vec2 generateTextureCoordinate(const Vec3& position) const;
};

bool getMaterialColorFromOBJ(const std::string& objPath, Vec3& materialColor);

#endif