#include "Model.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static bool parseMTL(const std::string& path, const std::string& materialName, Vec3& materialColor)
{
	std::ifstream file(path);

	if (!file)
	{
		std::cerr << "No se pudo abrir el material: " << path << "\n";
		return false;
	}

	std::string line;
	bool materialFound = false;

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
		// Nombre del material
		// --------------------------------
		if (type == "newmtl")
		{
			std::string name;
			stream >> name;

			materialFound = (name == materialName);
		}

		// --------------------------------
		// Color difuso
		// --------------------------------
		else if (type == "Kd" && materialFound)
		{
			stream
				>> materialColor.x
				>> materialColor.y
				>> materialColor.z;

			return true;
		}
	}

	return false;
}

bool getMaterialColorFromOBJ(const std::string& objPath, Vec3& materialColor)
{
	std::ifstream file(objPath);

	if (!file)
	{
		std::cerr << "No se pudo abrir el OBJ para buscar el material: " << objPath << "\n";
		return false;
	}

	std::string line;
	std::string mtlFileName;
	std::string materialName;

	// --------------------------------
	// Buscar mtllib y usemtl
	// --------------------------------
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
		// Archivo MTL
		// --------------------------------
		if (type == "mtllib")
		{
			stream >> mtlFileName;
		}

		// --------------------------------
		// Material utilizado
		// --------------------------------
		else if (type == "usemtl")
		{
			stream >> materialName;
		}

		// --------------------------------
		// Ya tenemos lo necesario
		// --------------------------------
		if (!mtlFileName.empty() && !materialName.empty())
			break;
	}

	// --------------------------------
	// El OBJ no tiene material
	// --------------------------------
	if (mtlFileName.empty())
		return false;

	if (materialName.empty())
		return false;

	// --------------------------------
	// Construir ruta del MTL
	// --------------------------------
	std::string mtlPath;

	std::size_t slash = objPath.find_last_of("/\\");

	if (slash == std::string::npos)
		mtlPath = mtlFileName;
	else
		mtlPath = objPath.substr(0, slash + 1) + mtlFileName;

	// --------------------------------
	// Parsear MTL
	// --------------------------------
	if (!parseMTL(mtlPath, materialName, materialColor))
		return false;

	std::cout << "Material encontrado: " << materialName << "\n";
	std::cout << "MTL: " << mtlPath << "\n";
	std::cout << "Kd: " << materialColor.x << " " << materialColor.y << " " << materialColor.z << "\n";

	return true;
}