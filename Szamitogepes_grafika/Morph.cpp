#include "Morph.hpp"

#include <ctime>

using namespace std;

void morphByStep(Mesh& pointCloud, Mesh& base)
{
	Mesh::VertexPC bunnyPoint = pointCloud.verticesPC[rand() % pointCloud.verticesPC.size()];

	float minDistance = 1000;
	int minDistanceIndex = 0;
	float distance;

	for (int i = 0; i < base.verticesSubdiv.size(); i++)
	{
		distance = glm::distance(base.verticesSubdiv[i].position, bunnyPoint.position);
		if (minDistance > distance)
		{
			minDistance = distance;
			minDistanceIndex = i;
		}
		base.verticesSubdiv[i].color = glm::vec3(0.2, 0.2, 1.0);
	}

	vector<float> distances;
	vector<int> verticesInRange;

	for (int i = 0; i < base.verticesSubdiv.size(); i++)
	{
		distance = glm::distance(base.verticesSubdiv[i].position, base.verticesSubdiv[minDistanceIndex].position);
		if (0.02 > distance)
		{
			distances.push_back(distance);
			verticesInRange.push_back(i);
		}
	}

	//base.verticesSubdiv[minDistanceIndex].position= base.verticesSubdiv[minDistanceIndex].position - (base.verticesSubdiv[minDistanceIndex].position - bunnyPoint.position)/20.0f;

	glm::vec3 delta = base.verticesSubdiv[minDistanceIndex].position - bunnyPoint.position;

	base.verticesSubdiv[minDistanceIndex].color = glm::vec3(3.0,0.4,-1);

	for (int i = 0; i < verticesInRange.size(); i++)
	{
		base.verticesSubdiv[verticesInRange[i]].color = base.verticesSubdiv[verticesInRange[i]].color + glm::vec3(3.0 * (1.0f - (distances[i] / 0.02f)), -0.2, -1);
		base.verticesSubdiv[verticesInRange[i]].position = base.verticesSubdiv[verticesInRange[i]].position - (delta * (1.0f - (distances[i] / 0.02f))) / 5.0f;
		//base.verticesSubdiv[i].position = base.verticesSubdiv[i].position + delta/10.f /(1.0f-(1.0f * 0.2f/distances[i])) ;
	}

	vector <float> areas;
	vector <float> areasIndex;



	for (int i = 0; i < base.faces.size(); i++)
	{

		// Csak olyan faceket szeretnénk megvizsgálni, amik tartalmaznak vertexet a most elmozgatott pontok közül. 
		// Ezt a teljesítmény javítása érdekében tesszük, hogy ne kelljen minden oldalra kiszámolni a területet.
		bool in = false;
		for (int j = 0; j < verticesInRange.size(); j++)
		{
			in = in || base.faces[i].vertices[0] == verticesInRange[j];
			in = in || base.faces[i].vertices[1] == verticesInRange[j];
			in = in || base.faces[i].vertices[2] == verticesInRange[j];
		}

		if (in)
		{
			float a = glm::distance(base.vertices[base.faces[i].vertices[0]].position, base.vertices[base.faces[i].vertices[1]].position);
			float b = glm::distance(base.vertices[base.faces[i].vertices[1]].position, base.vertices[base.faces[i].vertices[2]].position);;
			float c = glm::distance(base.vertices[base.faces[i].vertices[2]].position, base.vertices[base.faces[i].vertices[0]].position);;
			float s = (a + b + c) / 2;
			float area = sqrt((s * (s - a) * (s - b) * (s - c))); //Heron képletből jön
			areas.push_back(area);
			areasIndex.push_back(i);
			
		}
		
	}
	cout << endl << endl;

	for (size_t i = 0; i < areas.size(); i++)
	{
		if (areas[i] > 0.0001) {
			int j = areasIndex[i];
			cout << "SUBDIVIDE" << endl;
			base.verticesSubdiv[base.faces[j].vertices[0]].color = glm::vec3(0.0, 0.4, 0.0);
			base.verticesSubdiv[base.faces[j].vertices[1]].color = glm::vec3(0.0, 0.4, 0.0);
			base.verticesSubdiv[base.faces[j].vertices[2]].color = glm::vec3(0.0, 0.4, 0.0);
			
			Mesh::VertexSubdiv v0;
			Mesh::VertexSubdiv v1;
			Mesh::VertexSubdiv v2;
			
			v0.position = (base.verticesSubdiv[base.faces[j].vertices[0]].position + base.verticesSubdiv[base.faces[j].vertices[1]].position) / 2.0f;
			v1.position = (base.verticesSubdiv[base.faces[j].vertices[1]].position + base.verticesSubdiv[base.faces[j].vertices[2]].position) / 2.0f;
			v2.position = (base.verticesSubdiv[base.faces[j].vertices[2]].position + base.verticesSubdiv[base.faces[j].vertices[0]].position) / 2.0f;
			
			v0.color= glm::vec3(0.0, 0.4, 0.0);
			v1.color= glm::vec3(0.0, 0.4, 0.0);
			v2.color= glm::vec3(0.0, 0.4, 0.0);

			base.verticesSubdiv.push_back(v0);
			base.verticesSubdiv.push_back(v1);
			base.verticesSubdiv.push_back(v2);

			Mesh::Face f1;

			//=========================================
			//BELSŐ OLDALAK
			//=========================================

			f1.vertices[0] = base.faces[j].vertices[0];
			f1.vertices[1] = base.verticesSubdiv.size() - 3;
			f1.vertices[2] = base.verticesSubdiv.size() - 1;
			base.faces.push_back(f1);

			f1.vertices[0] = base.faces[j].vertices[1];
			f1.vertices[1] = base.verticesSubdiv.size() - 2;
			f1.vertices[2] = base.verticesSubdiv.size() - 3;
			base.faces.push_back(f1);

			f1.vertices[0] = base.faces[j].vertices[2];
			f1.vertices[1] = base.verticesSubdiv.size() - 3;
			f1.vertices[2] = base.verticesSubdiv.size() - 2;
			base.faces.push_back(f1);

			f1.vertices[0] = base.verticesSubdiv.size() - 3;
			f1.vertices[1] = base.verticesSubdiv.size() - 2;
			f1.vertices[2] = base.verticesSubdiv.size() - 1;
			base.faces.push_back(f1);

			//=========================================
			//KÜLSŐ OLDALAK
			//=========================================

			f1.vertices[0] = base.faces[j].vertices[0];
			f1.vertices[1] = base.verticesSubdiv[base.faces[j].vertices[0]].edge->pair->next->v2;
			f1.vertices[2] = base.verticesSubdiv.size() - 3;
			base.faces.push_back(f1);

			f1.vertices[0] = base.verticesSubdiv[base.faces[j].vertices[0]].edge->pair->next->v2; 
			f1.vertices[1] = base.faces[j].vertices[1];
			f1.vertices[2] = base.verticesSubdiv.size() - 3;
			base.faces.push_back(f1);

			f1.vertices[0] = base.faces[j].vertices[1];
			f1.vertices[1] = base.verticesSubdiv[base.faces[j].vertices[1]].edge->pair->next->v2;
			f1.vertices[2] = base.verticesSubdiv.size() - 2;
			base.faces.push_back(f1);

			f1.vertices[0] = base.verticesSubdiv[base.faces[j].vertices[1]].edge->pair->next->v2; 
			f1.vertices[1] = base.faces[j].vertices[2];
			f1.vertices[2] = base.verticesSubdiv.size() - 2;
			base.faces.push_back(f1);

			f1.vertices[0] = base.faces[j].vertices[2];
			f1.vertices[1] = base.verticesSubdiv[base.faces[j].vertices[2]].edge->pair->next->v2;
			f1.vertices[2] = base.verticesSubdiv.size() - 1;
			base.faces.push_back(f1);

			f1.vertices[0] = base.verticesSubdiv[base.faces[j].vertices[2]].edge->pair->next->v2; 
			f1.vertices[1] = base.faces[j].vertices[0];
			f1.vertices[2] = base.verticesSubdiv.size() - 1;
			base.faces.push_back(f1);


			//==================================================
			//RECALCULATING HALFEDGES
			//==================================================

		//	base.halfEdge.clear();

			for (int i = 0; i < base.faces.size(); i++)
			{
				Mesh::HalfEdge halfEdge;
				halfEdge.v1 = base.faces[i].vertices[0];
				halfEdge.v2 = base.faces[i].vertices[1];
				halfEdge.vertex = &base.verticesSubdiv[base.faces[i].vertices[1]];
				halfEdge.face = &base.faces[i];

				Mesh::HalfEdge halfEdge2find;
				halfEdge2find.v1 = base.faces[i].vertices[1];
				halfEdge2find.v2 = base.faces[i].vertices[0];
				halfEdge2find.pair = NULL;

				vector<Mesh::HalfEdge>::iterator pair = find(base.halfEdge.begin(), base.halfEdge.end(), halfEdge2find);

				if (pair != base.halfEdge.end())
				{
					halfEdge.pair = &(*pair);
				}

				base.halfEdge.push_back(halfEdge);
				base.verticesSubdiv[base.faces[i].vertices[0]].edge = &base.halfEdge[base.halfEdge.size() - 1];

				//============================================================================================

				halfEdge.v1 = base.faces[i].vertices[1];
				halfEdge.v2 = base.faces[i].vertices[2];
				halfEdge.vertex = &base.verticesSubdiv[base.faces[i].vertices[2]];
				halfEdge.face = &base.faces[i];
				halfEdge.pair = NULL;

				halfEdge2find.v1 = base.faces[i].vertices[2];
				halfEdge2find.v2 = base.faces[i].vertices[1];
				halfEdge2find.pair = NULL;

				pair = find(base.halfEdge.begin(), base.halfEdge.end(), halfEdge2find);

				if (pair != base.halfEdge.end())
				{
					halfEdge.pair = &(*pair);
				}

				base.halfEdge.push_back(halfEdge);
				base.verticesSubdiv[base.faces[i].vertices[1]].edge = &base.halfEdge[base.halfEdge.size() - 1];

				//============================================================================================

				halfEdge.v1 = base.faces[i].vertices[2];
				halfEdge.v2 = base.faces[i].vertices[0];
				halfEdge.vertex = &base.verticesSubdiv[base.faces[i].vertices[0]];
				halfEdge.face = &base.faces[i];
				halfEdge.pair = NULL;
				halfEdge2find.v1 = base.faces[i].vertices[0];
				halfEdge2find.v2 = base.faces[i].vertices[2];
				halfEdge2find.pair = NULL;

				pair = find(base.halfEdge.begin(), base.halfEdge.end(), halfEdge2find);

				if (pair != base.halfEdge.end())
				{
					halfEdge.pair = &(*pair);
				}

				base.halfEdge.push_back(halfEdge);
				base.verticesSubdiv[base.faces[i].vertices[2]].edge = &base.halfEdge[base.halfEdge.size() - 1];

				base.halfEdge[base.halfEdge.size() - 3].next = &base.halfEdge[base.halfEdge.size() - 2];
				base.halfEdge[base.halfEdge.size() - 2].next = &base.halfEdge[base.halfEdge.size() - 1];
				base.halfEdge[base.halfEdge.size() - 1].next = &base.halfEdge[base.halfEdge.size() - 3];

				if (base.halfEdge[base.halfEdge.size() - 3].pair != NULL)
				{
					base.halfEdge[base.halfEdge.size() - 3].pair->pair = &base.halfEdge[base.halfEdge.size() - 3];
				}

				if (base.halfEdge[base.halfEdge.size() - 2].pair != NULL)
				{
					base.halfEdge[base.halfEdge.size() - 2].pair->pair = &base.halfEdge[base.halfEdge.size() - 2];
				}

				if (base.halfEdge[base.halfEdge.size() - 1].pair != NULL)
				{
					base.halfEdge[base.halfEdge.size() - 1].pair->pair = &base.halfEdge[base.halfEdge.size() - 1];
				}

				base.faces[i].edge = &base.halfEdge[base.halfEdge.size() - 3];
			}
		}

			
	}

	


}