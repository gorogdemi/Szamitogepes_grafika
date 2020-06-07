#include "Subdiv.hpp"

using namespace std;

Mesh s_mesh;

Mesh loadMesh(string fileName)
{
	Mesh mesh;
	mesh.vertices.reserve(2000);
	mesh.verticesSubdiv.reserve(2000);
	mesh.faces.reserve(2000);
	mesh.halfEdge.reserve(2000);
	ifstream be(fileName);
	string x;

	getline(be, x);
	getline(be, x);
	getline(be, x);
	getline(be, x);
	be >> x;

	while (x == "v")
	{
		Mesh::Vertex temp;
		GLfloat xx, y, z;
		be >> xx >> y >> z;
		temp.position = glm::vec3(xx, y, z);
		mesh.vertices.push_back(temp);
		be >> x;
	}

	while (x == "vn")
	{
		glm::vec3 temp;
		GLfloat xx, y, z;
		be >> xx >> y >> z;
		temp = glm::vec3(xx, y, z);
		mesh.normals.push_back(temp);
		be >> x;
	}

	getline(be, x);
	getline(be, x);
	be >> x;

	while (x == "f")
	{
		be >> x;
		mesh.indices.push_back(atoi(x.substr(0, x.find_first_of('/')).c_str()) - 1);
		mesh.vertices[mesh.indices[mesh.indices.size() - 1]].normal = mesh.normals[atoi(x.substr(x.find_last_of('/') + 1, x.length() - 1).c_str()) - 1];
		be >> x;
		mesh.indices.push_back(atoi(x.substr(0, x.find_first_of('/')).c_str()) - 1);
		mesh.vertices[mesh.indices[mesh.indices.size() - 1]].normal = mesh.normals[atoi(x.substr(x.find_last_of('/') + 1, x.length() - 1).c_str()) - 1];
		be >> x;
		mesh.indices.push_back(atoi(x.substr(0, x.find_first_of('/')).c_str()) - 1);
		mesh.vertices[mesh.indices[mesh.indices.size() - 1]].normal = mesh.normals[atoi(x.substr(x.find_last_of('/') + 1, x.length() - 1).c_str()) - 1];
		be >> x;
	}

	be.close();

	for (int i = 0; i < mesh.vertices.size(); i++)
	{
		Mesh::VertexSubdiv vertexSubdiv;
		vertexSubdiv.position = mesh.vertices[i].position;
		mesh.verticesSubdiv.push_back(vertexSubdiv);
	}

	for (int i = 0; i < mesh.indices.size(); i += 3)
	{
		Mesh::Face face;
		face.vertices[0] = mesh.indices[i];
		face.vertices[1] = mesh.indices[i + 1];
		face.vertices[2] = mesh.indices[i + 2];
		mesh.faces.push_back(face);
		cout << face.vertices[0] << " " << face.vertices[1] << " " << face.vertices[2] << endl;
	}

	mesh.halfEdge.reserve(mesh.faces.size()*3);

	for (int i = 0; i < mesh.faces.size(); i++)
	{
		Mesh::HalfEdge halfEdge;
		halfEdge.v1 = mesh.faces[i].vertices[0];
		halfEdge.v2 = mesh.faces[i].vertices[1];
		halfEdge.vertex = &mesh.verticesSubdiv[mesh.faces[i].vertices[1]];
		halfEdge.face = &mesh.faces[i];

		Mesh::HalfEdge halfEdge2find;
		halfEdge2find.v1 = mesh.faces[i].vertices[1];
		halfEdge2find.v2 = mesh.faces[i].vertices[0];
		halfEdge2find.pair = NULL;

		vector<Mesh::HalfEdge>::iterator pair = find(mesh.halfEdge.begin(), mesh.halfEdge.end(), halfEdge2find);

		if (pair != mesh.halfEdge.end())
		{
			halfEdge.pair = &(*pair);
		}
		mesh.halfEdge.push_back(halfEdge);
		mesh.verticesSubdiv[mesh.faces[i].vertices[0]].edge = &mesh.halfEdge[mesh.halfEdge.size() - 1];

		//============================================================================================

		halfEdge.v1 = mesh.faces[i].vertices[1];
		halfEdge.v2 = mesh.faces[i].vertices[2];
		halfEdge.vertex = &mesh.verticesSubdiv[mesh.faces[i].vertices[2]];
		halfEdge.face = &mesh.faces[i];

		halfEdge2find.v1 = mesh.faces[i].vertices[2];
		halfEdge2find.v2 = mesh.faces[i].vertices[1];
		halfEdge2find.pair = NULL;

		pair = find(mesh.halfEdge.begin(), mesh.halfEdge.end(), halfEdge2find);

		if (pair != mesh.halfEdge.end())
		{
			halfEdge.pair = &(*pair);
		}

		mesh.halfEdge.push_back(halfEdge);
		mesh.verticesSubdiv[mesh.faces[i].vertices[1]].edge = &mesh.halfEdge[mesh.halfEdge.size() - 1];

		//============================================================================================

		halfEdge.v1 = mesh.faces[i].vertices[2];
		halfEdge.v2 = mesh.faces[i].vertices[0];
		halfEdge.vertex = &mesh.verticesSubdiv[mesh.faces[i].vertices[0]];
		halfEdge.face = &mesh.faces[i];

		halfEdge2find.v1 = mesh.faces[i].vertices[0];
		halfEdge2find.v2 = mesh.faces[i].vertices[2];
		halfEdge2find.pair = NULL;

		pair = find(mesh.halfEdge.begin(), mesh.halfEdge.end(), halfEdge2find);

		if (pair != mesh.halfEdge.end())
		{
			halfEdge.pair = &(*pair);
		}

		mesh.halfEdge.push_back(halfEdge);
		mesh.verticesSubdiv[mesh.faces[i].vertices[2]].edge = &mesh.halfEdge[mesh.halfEdge.size() - 1];

		mesh.halfEdge[mesh.halfEdge.size() - 3].next = &mesh.halfEdge[mesh.halfEdge.size() - 2];
		mesh.halfEdge[mesh.halfEdge.size() - 2].next = &mesh.halfEdge[mesh.halfEdge.size() - 1];
		mesh.halfEdge[mesh.halfEdge.size() - 1].next = &mesh.halfEdge[mesh.halfEdge.size() - 3];

		if (mesh.halfEdge[mesh.halfEdge.size() - 3].pair != NULL)
		{
			mesh.halfEdge[mesh.halfEdge.size() - 3].pair->pair = &mesh.halfEdge[mesh.halfEdge.size() - 3];
		}

		if (mesh.halfEdge[mesh.halfEdge.size() - 2].pair != NULL)
		{
			mesh.halfEdge[mesh.halfEdge.size() - 2].pair->pair = &mesh.halfEdge[mesh.halfEdge.size() - 2];
		}

		if (mesh.halfEdge[mesh.halfEdge.size() - 1].pair != NULL)
		{
			mesh.halfEdge[mesh.halfEdge.size() - 1].pair->pair = &mesh.halfEdge[mesh.halfEdge.size() - 1];
		}

		mesh.faces[i].edge = &mesh.halfEdge[mesh.halfEdge.size() - 3];
	}

	for (int i = 0; i < mesh.verticesSubdiv.size(); i++)
	{
		cout << mesh.verticesSubdiv[i].edge->vertex << endl;
	}

	glGenBuffers(1, &mesh.vbo);
	glGenBuffers(1, &mesh.ibo);
	glGenVertexArrays(1, &mesh.vao);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Mesh::Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint), mesh.indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/** Konfiguráljuk a VAO-t. */
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (const void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (const void*)(sizeof(glm::vec3)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return mesh;
}

void subdivide()
{
	//unsigned long int V = s_mesh.verticesSubdiv.size(), F = s_mesh.faces.size();
	//	unsigned long int H = 3 * F;
	//	unsigned long int E = H / 2;
	//	vector<Mesh::VertexSubdiv> NV = s_mesh.verticesSubdiv;
	//	vector<int> EM(H);
	//	vector<Mesh::HalfEdge> NE(3 * 4 * F);
	//	vector<Mesh::Face> NF{ 4 * F };
	
	vector<Mesh::VertexSubdiv> newVertices;
	newVertices = s_mesh.verticesSubdiv;
	newVertices.reserve(300);
	vector<Mesh::HalfEdge> newEdges;
	newEdges.reserve(2000);
	
	vector<Mesh::Face> newFaces;

	for (int h = 0; h < s_mesh.halfEdge.size(); h++)
	{
		if (!s_mesh.halfEdge[h].used)
		{
			//glm::vec3 u = (glm::vec3(s_mesh.halfEdge[h].vertex->position) + glm::vec3(s_mesh.halfEdge[h].pair->vertex->position)) * 3.0f / 8.0f + (glm::vec3(s_mesh.halfEdge[h].next->vertex->position) + glm::vec3(s_mesh.halfEdge[h].pair->next->vertex->position)) / 8.0f;
			glm::vec3 u = (glm::vec3(s_mesh.halfEdge[h].vertex->position) + glm::vec3(s_mesh.halfEdge[h].pair->vertex->position))/2.0f;
			//glm::vec3 u = (glm::vec3(s_mesh.halfEdge[h].next->vertex->position));
			newVertices.push_back(Mesh::VertexSubdiv{ u,glm::vec3(0,0,0),s_mesh.halfEdge[h].pair });
			
			s_mesh.halfEdge[h].pair->vertex = &newVertices[newVertices.size() - 1];
			s_mesh.halfEdge[h].used = true;
			s_mesh.halfEdge[h].pair->used = true;
		}
	}


	/*for (int i = 0; i < s_mesh.verticesSubdiv.size(); i++)
	{
		Mesh::HalfEdge* edge = newVertices[i].edge->next;
		glm::vec3 sum(0, 0, 0);

		int n = 0;
		do {
			// végezzünk valamilyen műveletet az edge->vertex csúccsal
			sum+=edge->vertex->position;
			n++;
			edge = edge->pair->next;
		}
		while (edge->pair != newVertices[i].edge);

		float beta = pow((3.0f / 8.0f + cos(2 * glm::pi<float>() / n) / 4.0f), 2);
		float alpha = (5.0f / 8.0f - beta) / n;

		newVertices[i].position = glm::vec3(0, 0, 0);

		newVertices[i] = Mesh::VertexSubdiv{ (1 - n * alpha) * s_mesh.verticesSubdiv[i].position + alpha * sum,glm::vec3(0,0,0),NULL };
	}
	*/
	newFaces.clear();
	newFaces.reserve(200);
	cout << endl;
	for (int i = 0; i < newVertices.size(); i++)
	{
		cout << newVertices[i].edge<< endl;
		cout << newVertices[i].position.x << " " << newVertices[i].position.y << " " << newVertices[i].position.z << endl;
	}
	cout << endl;
	for (int i = 0; i < s_mesh.vertices.size(); i++)
	{
		cout << &s_mesh.vertices[i] << endl;
	}
	cout << "faceképzés" << endl;
	for (int i = 0; i < 1; i++)
	{
		Mesh::Face face;
		Mesh::VertexSubdiv* v0 = &newVertices[s_mesh.faces[i].vertices[0]];
		Mesh::VertexSubdiv* v1 = &newVertices[s_mesh.faces[i].vertices[1]];
		Mesh::VertexSubdiv* v2 = &newVertices[s_mesh.faces[i].vertices[2]];
		Mesh::VertexSubdiv* j0 = v0->edge->vertex;
		Mesh::VertexSubdiv* j1 = v1->edge->vertex;
		Mesh::VertexSubdiv* j2 = v2->edge->vertex;
		cout << j0->position.x << " " << j0->position.y << " " << j0->position.z << endl;
		cout << s_mesh.faces[i].vertices[0] << " " << s_mesh.faces[i].vertices[1] << " " << s_mesh.faces[i].vertices[2] << endl;
		//cout << abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *v0)) << " " << abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *v1)) << " " << abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *v2)) << endl;
		face.vertices[0] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *j0));
		face.vertices[1] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *j1));
		face.vertices[2] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *j1));
		
		newFaces.push_back(face);
		//cout << "megy" << endl;
		//face.vertices[0] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *v0));
		//face.vertices[1] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *j0));
		//face.vertices[2] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *j2));

		//newFaces.push_back(face);

		//face.vertices[0] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *v1));
		//face.vertices[1] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *j1));
		//face.vertices[2] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *j0));

		//newFaces.push_back(face);

		//face.vertices[0] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *v2));
		//face.vertices[1] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *j2));
		//face.vertices[2] = abs(newVertices.begin() - find(newVertices.begin(), newVertices.end(), *j1));

		//newFaces.push_back(face);
	}
	/*
	for (int i = 0; i < newFaces.size(); i++)
	{
		Mesh::HalfEdge halfEdge;
		halfEdge.v1 = newFaces[i].vertices[0];
		halfEdge.v2 = newFaces[i].vertices[1];
		halfEdge.vertex = &newVertices[newFaces[i].vertices[1]];
		halfEdge.face = &newFaces[i];

		Mesh::HalfEdge halfEdge2find;
		halfEdge2find.v1 = newFaces[i].vertices[1];
		halfEdge2find.v2 = newFaces[i].vertices[0];
		halfEdge2find.pair = NULL;

		vector<Mesh::HalfEdge>::iterator pair = find(newEdges.begin(), newEdges.end(), halfEdge2find);
		if (pair != newEdges.end())
		{
			halfEdge.pair = &(*pair);
		}
		newEdges.push_back(halfEdge);
		newVertices[newFaces[i].vertices[0]].edge = &newEdges[newEdges.size() - 1];
		//============================================================================================
		halfEdge.v1 = newFaces[i].vertices[0];
		halfEdge.v2 = newFaces[i].vertices[1];
		halfEdge.vertex = &newVertices[newFaces[i].vertices[1]];
		halfEdge.face = &newFaces[i];

		halfEdge2find.v1 = newFaces[i].vertices[1];
		halfEdge2find.v2 = newFaces[i].vertices[0];
		halfEdge2find.pair = NULL;

		pair = find(newEdges.begin(), newEdges.end(), halfEdge2find);
		if (pair != newEdges.end())
		{
			halfEdge.pair = &(*pair);
		}
		newEdges.push_back(halfEdge);
		newVertices[newFaces[i].vertices[0]].edge = &newEdges[newEdges.size() - 1];
		//============================================================================================
		halfEdge.v1 = newFaces[i].vertices[0];
		halfEdge.v2 = newFaces[i].vertices[1];
		halfEdge.vertex = &newVertices[newFaces[i].vertices[1]];
		halfEdge.face = &newFaces[i];

		halfEdge2find.v1 = newFaces[i].vertices[1];
		halfEdge2find.v2 = newFaces[i].vertices[0];
		halfEdge2find.pair = NULL;

		pair = find(newEdges.begin(), newEdges.end(), halfEdge2find);
		if (pair != newEdges.end())
		{
			halfEdge.pair = &(*pair);
		}
		newEdges.push_back(halfEdge);
		newVertices[newFaces[i].vertices[0]].edge = &newEdges[newEdges.size() - 1];

		newEdges[newEdges.size() - 3].next = &newEdges[newEdges.size() - 2];
		newEdges[newEdges.size() - 2].next = &newEdges[newEdges.size() - 1];
		newEdges[newEdges.size() - 1].next = &newEdges[newEdges.size() - 3];
		newFaces[i].edge = &newEdges[newEdges.size() - 3];
	}*/

	s_mesh.verticesSubdiv = newVertices;
	s_mesh.faces = newFaces;
	s_mesh.halfEdge = newEdges;
}

//int nIndex(int ei, int type)
//{
//	// a párok meghatározásához szükséges offsetek
//	int offsets[] = { 3, 1, 6, 4, 0, 7 };
//	// az eredeti pár
//	int op = s_mesh.halfEdge[ei].pair;
//	// ez nem fordulhat elõ, mivel olyan meshekkel dolgozunk, ahol minden élnek van párja
//	if (op == -1)
//		return -1;
//	int bp = 12 * floor(op / 3);
//	return bp + offsets[2 * (op % 3) + type];
//}
//
//
//void subdivide()
//{
//	//===========================================================================
//	unsigned long int V = s_mesh.verticesSubdiv.size(), F = s_mesh.faces.size();
//	unsigned long int H = 3 * F;
//	unsigned long int E = H / 2;
//	vector<Mesh::VertexSubdiv> NV = s_mesh.verticesSubdiv;
//	vector<int> EM(H);
//	vector<Mesh::HalfEdge> NE(3 * 4 * F);
//	vector<Mesh::Face> NF{ 4 * F };
//	for (int i = 0; i < EM.size(); i++)
//	{
//		EM[i] = -1;
//	}
//	//===========================================================================
//	for (int h = 0; h < EM.size(); h++)
//	{
//		if (EM[h] == -1)
//		{
//			int v0 = s_mesh.faces[h / 3].vertices[h % 3];
//			int v1 = s_mesh.faces[s_mesh.halfEdge[h].pair / 3].vertices[s_mesh.halfEdge[h].pair % 3];
//			int sz0 = s_mesh.faces[h / 3].vertices[(h + 2) % 3];
//			int sz1 = s_mesh.faces[s_mesh.halfEdge[h].pair / 3].vertices[(s_mesh.halfEdge[h].pair + 2) % 3];
//			glm::vec3 u = (s_mesh.verticesSubdiv[v0].position + s_mesh.verticesSubdiv[v1].position) * 3.0f / 8.0f + (s_mesh.verticesSubdiv[sz0].position + s_mesh.verticesSubdiv[sz1].position) / 8.0f;
//			EM[s_mesh.halfEdge[h].pair] = EM[h] = NV.size();
//			NV.push_back(Mesh::VertexSubdiv{ u,glm::vec3(0,0,0),h });
//		}
//	}
//	//===========================================================================
//	for (int v = 0; v < V; v++)
//	{
//
//		float n = 0;
//		glm::vec3 sum{ 0,0,0 };
//		int eA = s_mesh.verticesSubdiv[v].edge;
//		int eB = s_mesh.halfEdge[eA].pair;
//		Mesh::Face face = s_mesh.faces[eB / 3];
//		int index_on_face = (eB + 1) % 3;
//		Mesh::VertexSubdiv w = s_mesh.verticesSubdiv[face.vertices[index_on_face]];
//		int f = floor(eB / 3);
//		int he = 3 * f;
//		int eC = he + ((eB + 2) % 3);
//		sum += w.position; n++;
//		while (eC != s_mesh.verticesSubdiv[v].edge)
//		{
//			eA = eC;
//			eB = s_mesh.halfEdge[eA].pair;
//			face = s_mesh.faces[eB / 3];
//			index_on_face = (eB + 1) % 3;
//			w = s_mesh.verticesSubdiv[face.vertices[index_on_face]];
//			f = eB / 3;
//			he = 3 * f;
//			eC = he + ((eB + 2) % 3);
//			sum += w.position; n++;
//		}
//		float beta = pow((3.0f / 8.0f + cos(2 * glm::pi<float>() / n) / 4.0f), 2);
//		float alpha = (5.0f / 8.0f - beta) / n;
//		NV[v] = Mesh::VertexSubdiv{ (1 - n * alpha) * s_mesh.verticesSubdiv[v].position + alpha * sum,glm::vec3(0,0,0),-1 };
//	}
//	//===========================================================================
//
//	for (int f = 0; f < s_mesh.faces.size(); f++)
//	{
//		int j0 = EM[3 * f];
//		int j1 = EM[3 * f + 1];
//		int j2 = EM[3 * f + 2];
//
//		int v0 = s_mesh.faces[f].vertices[0];
//		int v1 = s_mesh.faces[f].vertices[1];
//		int v2 = s_mesh.faces[f].vertices[2];
//
//		NF[4 * f].vertices[0] = j2;	NF[4 * f].vertices[1] = v0;	NF[4 * f].vertices[2] = j0;
//		NF[4 * f + 1].vertices[0] = j0;	NF[4 * f + 1].vertices[1] = v1;	NF[4 * f + 1].vertices[2] = j1;
//		NF[4 * f + 2].vertices[0] = j1;	NF[4 * f + 2].vertices[1] = v2;	NF[4 * f + 2].vertices[2] = j2;
//		NF[4 * f + 3].vertices[0] = j2;	NF[4 * f + 3].vertices[1] = j0;	NF[4 * f + 3].vertices[2] = j1;
//
//		NV[j0].edge = 3 * 4 * f + 1;
//		NV[j1].edge = 3 * (4 * f + 1) + 1;
//		NV[j2].edge = 3 * 4 * f + 2;
//
//		NV[v0].edge = 3 * 4 * f;
//		NV[v1].edge = 3 * (4 * f + 1);
//		NV[v2].edge = 3 * (4 * f + 2);
//	}
//	//===========================================================================
//	for (int f = 0; f < s_mesh.faces.size(); f++)
//	{
//		int b = 12 * f;
//		NE[b].pair = nIndex((f * 3) + 2, 1);
//		NE[b + 1].pair = nIndex((f * 3), 0);
//		NE[b + 2].pair = b + 9;
//
//		NE[b + 3].pair = nIndex((f * 3), 1);
//		NE[b + 4].pair = nIndex((f * 3) + 1, 0);
//		NE[b + 5].pair = b + 10;
//
//		NE[b + 6].pair = nIndex((f * 3) + 1, 1);
//		NE[b + 7].pair = nIndex((f * 3) + 2, 0);
//		NE[b + 8].pair = b + 11;
//
//		NE[b + 9].pair = b + 2;
//		NE[b + 10].pair = b + 5;
//		NE[b + 11].pair = b + 8;
//	}
//	//===========================================================================
//	s_mesh.verticesSubdiv = NV;
//	s_mesh.faces = NF;
//	s_mesh.halfEdge = NE;
//}