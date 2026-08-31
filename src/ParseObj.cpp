#include "Model.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static Vec3 getFaceColor(unsigned int faceIndex)
{
	const float grays[] = {
		0.30f,
		0.36f,
		0.42f,
		0.48f,
		0.54f,
		0.60f,
		0.66f,
		0.72f
	};
	const unsigned int colorCount = sizeof(grays) / sizeof(grays[0]);
	float gray = grays[faceIndex % colorCount];
	return {
		gray,
		gray,
		gray
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

			if (faceElements.size() < 3)
				continue;

			if (faceElements.size() > 4)
			{
				std::cerr << "Cara con mas de 4 vertices no soportada: " << path << "\n";
				continue;
			}

			// --------------------------------
			// Color de la cara
			// --------------------------------
			Vec3 faceColor = getFaceColor(faceIndex);
			faceIndex++;

			// --------------------------------
			// Triangulación
			// --------------------------------
			size_t triangleCount = faceElements.size() - 2;

			for (size_t triangle = 0; triangle < triangleCount; ++triangle)
			{
				size_t indices[3] = {
					0,
					triangle + 1,
					triangle + 2
				};

				for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
				{
					std::stringstream faceStream(faceElements[indices[vertexIndex]]);

					std::string positionIndex;
					std::string texCoordIndex;
					std::string normalIndex;

					std::getline(faceStream, positionIndex, '/');
					std::getline(faceStream, texCoordIndex, '/');
					std::getline(faceStream, normalIndex, '/');

					if (positionIndex.empty())
						continue;

					int p = std::stoi(positionIndex);

					int positionCount = static_cast<int>(positions.size());

					// --------------------------------
					// Índices negativos OBJ
					// --------------------------------
					if (p < 0)
						p = positionCount + p + 1;

					if (p <= 0 || p > positionCount)
					{
						std::cerr << "Indice de posicion invalido " << "en el modelo: " << path << "\n";
						continue;
					}

					Vec3 position = positions[p - 1];

					// --------------------------------
					// Coordenadas de textura
					// --------------------------------
					Vec2 uv = {
						0.0f,
						0.0f
					};

					if (!texCoordIndex.empty())
					{
						int t = std::stoi(texCoordIndex);
						int texCoordCount = static_cast<int>(texCoords.size());
						if (t < 0)
							t = texCoordCount + t + 1;

						if (t > 0 && t <= texCoordCount)
							uv = texCoords[t - 1];
						else
							uv = generateTextureCoordinate(position);
					}
					else
						uv = generateTextureCoordinate(position);

					// --------------------------------
					// Vertex
					// --------------------------------
					vertices.push_back({
						position,
						faceColor,
						materialColor,
						uv
					});
				}
			}
		}
	}

	// --------------------------------
	// Centrar modelo
	// --------------------------------
	centerModel();
	vertexCount = static_cast<unsigned int>(vertices.size());
	std::cout << "Modelo cargado: " << positions.size() << " vertices originales, " << vertexCount << " vertices renderizables\n";
}