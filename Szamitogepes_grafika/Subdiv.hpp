#pragma once

#include "Common.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <array>

using namespace std;

struct Mesh {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
	};

	struct VertexSubdiv;
	struct Face;
	struct HalfEdge;

	struct HalfEdge {
		HalfEdge* pair = NULL;
		HalfEdge* next = NULL;
		Face* face;
		VertexSubdiv* vertex; //VÉGPONT
		int v1, v2;
		bool operator==(HalfEdge b)
		{
			return b.v1 == this->v1 && b.v2 == this->v2;
		}
		bool used = false;
	};

	struct VertexSubdiv {
		glm::vec3 position;
		glm::vec3 normal;
		HalfEdge* edge; //KIINDULÓ
		bool operator==(VertexSubdiv b)
		{
			return b.position == this->position;
		}
	};

	struct Face {
		int vertices[3];
		HalfEdge* edge;
	};

	GLuint vbo, vao, ibo;

	vector<glm::vec3> normals;
	vector<GLuint> Normalindices;

	vector<GLuint> indices;

	vector<Vertex> vertices;

	vector<VertexSubdiv> verticesSubdiv;
	vector<Face> faces;
	vector<HalfEdge> halfEdge;

	void loadSubdivData()
	{
		cout << faces.size() << " " << verticesSubdiv.size() << endl;
		indices.clear();
		for (int i = 0; i < faces.size(); i++)
		{
			indices.push_back(faces[i].vertices[0]);
			indices.push_back(faces[i].vertices[1]);
			indices.push_back(faces[i].vertices[2]);
			cout << faces[i].vertices[0] << endl;
			glm::vec3 normal = glm::normalize(glm::cross(verticesSubdiv[faces[i].vertices[0]].position - verticesSubdiv[faces[i].vertices[1]].position, verticesSubdiv[faces[i].vertices[0]].position - verticesSubdiv[faces[i].vertices[2]].position));
			cout << "megy" << endl;
			verticesSubdiv[faces[i].vertices[0]].normal = normal;
			verticesSubdiv[faces[i].vertices[1]].normal = normal;
			verticesSubdiv[faces[i].vertices[2]].normal = normal;
		}
		cout << "megy" << endl;
		vertices.clear();
		for (int i = 0; i < verticesSubdiv.size(); i++)
		{
			Vertex vertex;
			vertex.position = verticesSubdiv[i].position;
			vertex.normal = verticesSubdiv[i].normal;
			vertices.push_back(vertex);
		}
	}
};

Mesh loadMesh(string fileName);

void subdivide();