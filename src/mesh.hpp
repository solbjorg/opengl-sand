#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh {
public:
	std::string name;
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> colours;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoordinates;
	std::vector<unsigned int> indices;

	Mesh(std::string vname = "<missing>") : name(vname) {
		// Trading memory usage for speed
		vertices.reserve(200000);
		normals.reserve(200000);
		indices.reserve(200000);
	}

	bool hasNormals;
	bool hasTexture;

	unsigned long faceCount() {
		return (this->vertices.size() / 3);
	}
};

/*
class Mesh {
public:
	std::string name;
	std::vector<float> vertices;
	std::vector<float> colours;
	std::vector<float> normals;
	std::vector<float> textureCoordinates;
	std::vector<unsigned int> indices;

	Mesh(std::string vname) : name(vname) { }
	Mesh() : name("<missing>") { }
	Mesh(VectorMesh &mesh) {
		name = mesh.name;
	    vertices.reserve(mesh.vertices.size() * 3);
		for (unsigned int i = 0; i < mesh.vertices.size(); i++) {
			float4 vertex = mesh.vertices.at(i);
			vertices.emplace_back(vertex.x);
			vertices.emplace_back(vertex.y);
			vertices.emplace_back(vertex.z);
		}
        normals.resize(mesh.normals.size() * 3);
        indices.resize(mesh.indices.size());
		std::memcpy(normals.data(),  mesh.normals.data(),  mesh.normals.size() * 3 * sizeof(float));
		std::memcpy(indices.data(),  mesh.indices.data(),  mesh.indices.size() * sizeof(unsigned int));
	}

	unsigned int vertexCount() {
		return (this->vertices.size()) / 3;
	}

};
*/

