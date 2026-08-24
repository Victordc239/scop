#include "Model.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

Model::Model(const std::string& modelPath, const std::string& texturePath) : VAO(0), VBO(0), texture(0), vertexCount(0)
{
	loadOBJ(modelPath);
	createBuffers();
	loadTexture(texturePath);
}

Model::~Model()
{
	if (VAO != 0)
		glDeleteVertexArrays(1, &VAO);

	if (VBO != 0)
		glDeleteBuffers(1, &VBO);

	if (texture != 0)
		glDeleteTextures(1, &texture);
}

Vec3 Model::getFaceColor(unsigned int faceIndex) const
{
	const Vec3 colors[] = {
		{0.25f, 0.25f, 0.25f},
		{0.35f, 0.35f, 0.35f},
		{0.45f, 0.45f, 0.45f},
		{0.55f, 0.55f, 0.55f},
		{0.65f, 0.65f, 0.65f},
		{0.75f, 0.75f, 0.75f}
	};

	return colors[faceIndex % 6];
}

Model::Vec2 Model::generateTextureCoordinate(const Vec3& position) const
{
	/*
	* Some OBJ files, such as the 42 logo supplied
	* with the project, do not contain texture coordinates.
	*
	* We generate simple planar coordinates so that the
	* texture can still be displayed.
	*
	* X/Y are used because they give a good projection
	* for the 42 logo.
	*/
	float u = position.x * 0.5f + 0.5f;
	float v = position.y * 0.5f + 0.5f;

	return {
		u,
		v
	};
}

void Model::loadOBJ(const std::string& path)
{
	std::ifstream file(path);
	if (!file)
	{
		std::cerr << "No se pudo abrir el modelo: " << path << "\n";
		return;
	}

	std::string line;
	unsigned int faceIndex = 0;
	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		if (line[0] == '#')
			continue;

		std::stringstream stream(line);
		std::string type;
		stream >> type;

		// --------------------------------
		// Posición
		// --------------------------------
		if (type == "v")
		{
			Vec3 position;
			stream
			>> position.x
			>> position.y
			>> position.z;

			positions.push_back(position);
		}

		// --------------------------------
		// Coordenada de textura
		// --------------------------------
		else if (type == "vt")
		{
			Vec2 texCoord;
			stream
			>> texCoord.u
			>> texCoord.v;

			texCoords.push_back(texCoord);
		}

		// --------------------------------
		// Cara
		// --------------------------------
		else if (type == "f")
		{
			std::vector<std::string> faceElements;

			std::string element;

			while (stream >> element)
			{
			faceElements.push_back(element);
			}

			/*
			* A face must contain at least 3 vertices.
			*/
			if (faceElements.size() < 3)
			continue;

			Vec3 color = getFaceColor(faceIndex);

			/*
			* Triangulate the face using a triangle fan:
			*
			*      0
			*     / \
			*    1---2
			*     \ /
			*      3
			*
			* becomes:
			*
			* (0,1,2)
			* (0,2,3)
			* ...
			*
			* This allows us to handle triangles,
			* quads and larger polygons.
			*/
			for (size_t i = 1; i + 1 < faceElements.size(); ++i)
			{
				std::string triangleElements[3] = {
					faceElements[0],
					faceElements[i],
					faceElements[i + 1]
				};

				for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
				{
					std::stringstream faceStream(triangleElements[vertexIndex]);
					std::string positionIndex;
					std::string texCoordIndex;
					std::string normalIndex;

					std::getline(faceStream, positionIndex, '/');
					std::getline(faceStream, texCoordIndex, '/');
					std::getline(faceStream, normalIndex, '/');

					if (positionIndex.empty())
					{
						continue;
					}

					int p = std::stoi(positionIndex);

					/*
					* OBJ indices can be positive or negative.
					*
					* Positive:
					* 1 = first vertex
					*
					* Negative:
					* -1 = last vertex
					*/
					int positionCount = static_cast<int>(positions.size());

					if (p < 0)
						p = positionCount + p + 1;

					if (p <= 0 || p > positionCount)
					{
						std::cerr << "Indice de posicion invalido " << "en el modelo: " << path << "\n";
						continue;
					}

					Vec3 position = positions[p - 1];
					Vec2 uv = {
						0.0f,
						0.0f
					};

					/*
					* If the OBJ provides texture
					* coordinates, use them.
					*/
					if (!texCoordIndex.empty())
					{
						int t = std::stoi(texCoordIndex);
						int texCoordCount = static_cast<int>(texCoords.size());
						if (t < 0)
						{
							t = texCoordCount + t + 1;
						}
						if (t > 0 && t <= texCoordCount)
						{
							uv = texCoords[t - 1];
						}
						else
						{
							uv = generateTextureCoordinate(position);
						}
					}
					else
					{
						/*
						* The OBJ has no UV.
						* Generate one automatically.
						*/
						uv = generateTextureCoordinate(position);
					}

					vertices.push_back({position, color, uv});
				}
			}
			faceIndex++;
		}
	}

	// --------------------------------
	// Centrar modelo
	// --------------------------------
	centerModel();
	vertexCount = static_cast<unsigned int>(vertices.size());
	std::cout << "Modelo cargado: " << positions.size() << " vertices originales, " << vertexCount << " vertices renderizables\n";
}

void Model::centerModel()
{
	if (positions.empty())
		return;

	// --------------------------------
	// Bounding box
	// --------------------------------
	Vec3 min = positions[0];
	Vec3 max = positions[0];

	for (const Vec3& position : positions)
	{
		if (position.x < min.x)
			min.x = position.x;
		if (position.y < min.y)
			min.y = position.y;
		if (position.z < min.z)
			min.z = position.z;
		if (position.x > max.x)
			max.x = position.x;
		if (position.y > max.y)
			max.y = position.y;
		if (position.z > max.z)
			max.z = position.z;
	}

	// --------------------------------
	// Centro
	// --------------------------------
	Vec3 center = {
		(min.x + max.x) / 2.0f,
		(min.y + max.y) / 2.0f,
		(min.z + max.z) / 2.0f
	};

	// --------------------------------
	// Centrar posiciones
	// --------------------------------
	for (Vec3& position : positions)
	{
		position.x -= center.x;
		position.y -= center.y;
		position.z -= center.z;
	}

	// --------------------------------
	// Centrar vertices
	// --------------------------------
	for (Vertex& vertex : vertices)
	{
		vertex.position.x -= center.x;
		vertex.position.y -= center.y;
		vertex.position.z -= center.z;
	}
	std::cout << "Modelo centrado en (0, 0, 0)\n";
}

void Model::createBuffers()
{
	if (vertices.empty())
	{
		std::cerr << "El modelo no tiene datos " << "para crear buffers\n";
		return;
	}

	// --------------------------------
	// VAO
	// --------------------------------
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// --------------------------------
	// VBO
	// --------------------------------
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// --------------------------------
	// Position
	// --------------------------------
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// --------------------------------
	// Color
	// --------------------------------
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// --------------------------------
	// Texture coordinates
	// --------------------------------
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// --------------------------------
	// Unbind
	// --------------------------------
	glBindVertexArray(0);
}

void Model::loadTexture(const std::string& path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
	{
		std::cerr << "No se pudo abrir la textura: " << path << "\n";
		return;
	}

	std::string format;
	file >> format;

	if (format != "P6")
	{
		std::cerr << "Formato de textura no soportado. " << "Usa PPM P6.\n";
		return;
	}

	int width;
	int height;
	int maxValue;

	file
		>> width
		>> height
		>> maxValue;

	file.get();
	if (width <= 0 || height <= 0 || maxValue != 255)
	{
		std::cerr << "Cabecera PPM invalida\n";
		return;
	}

	std::vector<unsigned char> pixels(width * height * 3);
	file.read(reinterpret_cast<char*>(pixels.data()), pixels.size());
	if (!file)
	{
		std::cerr << "Error leyendo textura\n";
		return;
	}

	// --------------------------------
	// OpenGL texture
	// --------------------------------
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	std::cout << "Textura cargada: " << path << " (" << width << "x" << height << ")\n";
}

void Model::draw() const
{
	if (VAO == 0 || vertexCount == 0)
	{
		return;
	}

	// --------------------------------
	// Texture
	// --------------------------------
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// --------------------------------
	// Model
	// --------------------------------
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}