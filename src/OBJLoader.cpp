#include "OBJLoader.hpp"
#include <algorithm>
#include <exception>
#include "sceneGraph.hpp"
#include "utilities/toolbox.hpp"

void split(std::string &target, const char delimiter, std::vector<std::string> &res, unsigned int* outLength)
{
	size_t pos = 0;
	unsigned int count = 0;
	while ((pos = target.find(delimiter)) != std::string::npos) {
		res[count] = target.substr(0, pos);
		target.erase(0, pos + 1);
		count++;
	}
	res[count] = target;
	count++;
	*outLength = count;
}

std::vector<Mesh> loadWavefront(std::string const srcFile, bool quiet)
{
	std::vector<Mesh> meshes;
	std::ifstream objFile(srcFile);
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;

	// Defined here for performance reasons
	unsigned int parts_main_length;
	std::vector<std::string> parts_main;
	parts_main.resize(64);
	
	unsigned int parts_1_length;
	std::vector<std::string> parts_1;
	parts_1.resize(64);
	
	unsigned int parts_2_length;
	std::vector<std::string> parts_2;
	parts_2.resize(64);
	
	unsigned int parts_3_length;
	std::vector<std::string> parts_3;
	parts_3.resize(64);
	
	unsigned int parts_4_length;
	std::vector<std::string> parts_4;
	parts_4.resize(64);

	if (objFile.is_open()) {
		std::string line;
		while (std::getline(objFile, line)) {

			split(line, ' ', parts_main, &parts_main_length);

			if (parts_main_length > 0) {
				// New Mesh object
				if (parts_main.at(0) == "o" && parts_main_length >= 2) {
					meshes.emplace_back(parts_main[1]);
				} else if (parts_main[0] == "v" && parts_main_length >= 4) {
					vertices.emplace_back(
						std::stof(parts_main[1]),
						std::stof(parts_main[2]),
						std::stof(parts_main[3]),
						(parts_main_length >= 5) ? std::stof(parts_main[4]) : 1.0f
					);
				} else if (parts_main[0] == "vn" && parts_main_length >= 4) {
				   normals.emplace_back(
					   std::stof(parts_main[1]),
					   std::stof(parts_main[2]),
					   std::stof(parts_main[3])
				   );
			   } else if (parts_main[0] == "f" && parts_main_length >= 4) {
				   if (meshes.size() == 0) {
					   if (!quiet) {
						   	std::cout << "[WARNING] face definition found, but no object" << std::endl;
							std::cout << "[WARNING] creating object 'noname'" << std::endl;
					   }
					   meshes.emplace_back("noname");
					   //continue;
				   }

				   	Mesh &Mesh = meshes.back();

					bool quadruple = parts_main_length >= 5;

					split(parts_main[1],'/', parts_1, &parts_1_length);
					split(parts_main[2],'/', parts_2, &parts_2_length);
					split(parts_main[3],'/', parts_3, &parts_3_length);

					if (quadruple) {
						split(parts_main[4],'/', parts_4, &parts_4_length);
					}

					if (parts_1_length < 1 || parts_1_length != parts_2_length || parts_2_length != parts_3_length || (quadruple && parts_4_length != parts_1_length)) {
						if (!quiet)
							std::cout << "[WARNING] invalid face defintion '" << line << "'" << std::endl;
						continue;
					}

					Mesh.hasNormals = parts_1_length >= 3;

					size_t n1_index, n2_index, n3_index, n4_index;
					size_t v4_index;
					size_t v1_index = std::stoi(parts_1[0]) - 1;
					size_t v2_index = std::stoi(parts_2[0]) - 1;
					size_t v3_index = std::stoi(parts_3[0]) - 1;

					if (quadruple) {
						v4_index = std::stoi(parts_4[0]) - 1;
					}

					if (v1_index >= vertices.size() ||
						v2_index >= vertices.size() ||
						v3_index >= vertices.size() ||
						(quadruple && v4_index >= vertices.size())) {
								if (!quiet) {
									std::cout << "[WARNING] Mesh " << Mesh.name << " faces vertices(" << v1_index << ", " << v2_index << ", " << v3_index;
									if (quadruple)
										std::cout << ", " << v4_index;
									std::cout << ") do not exist!" << std::endl;
								}
								continue;
					}


					if (Mesh.hasNormals) {
						n1_index = std::stoi(parts_1[2]) - 1;
						n2_index = std::stoi(parts_2[2]) - 1;
						n3_index = std::stoi(parts_3[2]) - 1;
						if (quadruple) {
							n4_index = std::stoi(parts_4[2]) - 1;
						}
						if (n1_index >= normals.size() ||
							n2_index >= normals.size() ||
							n3_index >= normals.size() ||
							(quadruple && n4_index >= normals.size())) {
									if (!quiet) {
										std::cout << "[WARNING] Mesh " << Mesh.name << " faces normals(" << n1_index << ", " << n2_index << ", " << n3_index;
										if (quadruple)
											std::cout << ", " << n4_index;
										std::cout << ") do not exist!" << std::endl;
									}
									continue;
						}
					}

					if (quadruple) {
						Mesh.vertices.push_back(vertices[v1_index]);
						Mesh.vertices.push_back(vertices[v3_index]);
						Mesh.vertices.push_back(vertices[v4_index]);

						if (Mesh.hasNormals) {
							Mesh.normals.push_back(normals[n1_index]);
							Mesh.normals.push_back(normals[n3_index]);
							Mesh.normals.push_back(normals[n4_index]);
						} else {
							Mesh.normals.insert(Mesh.normals.end(), { 0.0f, 0.0f, 0.0f });
						}

						Mesh.indices.push_back(unsigned(Mesh.indices.size()));
						Mesh.indices.push_back(unsigned(Mesh.indices.size()));
						Mesh.indices.push_back(unsigned(Mesh.indices.size()));
					}

					Mesh.vertices.push_back(vertices[v1_index]);
					Mesh.vertices.push_back(vertices[v2_index]);
					Mesh.vertices.push_back(vertices[v3_index]);
					if (Mesh.hasNormals){
						Mesh.normals.push_back(normals[n1_index]);
						Mesh.normals.push_back(normals[n2_index]);
						Mesh.normals.push_back(normals[n3_index]);
					} else {
						Mesh.normals.insert(Mesh.normals.end(), { 0.0f, 0.0f, 0.0f });
					}

					Mesh.indices.push_back(unsigned(Mesh.indices.size()));
					Mesh.indices.push_back(unsigned(Mesh.indices.size()));
					Mesh.indices.push_back(unsigned(Mesh.indices.size()));
				}
			}
		}
	} else {
		throw std::runtime_error("Reading OBJ file failed. This is usually because the operating system can't find it. Check if the relative path (to your terminal's working directory) is correct.");
	}

	return meshes;
}

void colourVertices(Mesh &Mesh, glm::vec4 colour) {
	Mesh.colours = std::vector<glm::vec4>();
	Mesh.colours.resize(Mesh.vertices.size());
	for (unsigned int i = 0; i < Mesh.vertices.size(); i++) {
		Mesh.colours[i] = colour;
	}
}

Mesh loadTerrainMesh(std::string const srcFile) {
	std::vector<Mesh> fileContents = loadWavefront(srcFile, true);
	Mesh terrainMesh = Mesh(fileContents.at(0));
	colourVertices(terrainMesh, glm::vec4(1, 1, 1, 1));

	return terrainMesh;
}
