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
			
		}
		
	}
	cout << endl << endl;

	for (size_t i = 0; i < areas.size(); i++)
	{
		if (areas[i] > 0.0001) {
			cout << "SUBDIVIDE" << endl;
			base.verticesSubdiv[base.faces[i].vertices[0]].color = glm::vec3(0.0, 3.0, 0.0);
			base.verticesSubdiv[base.faces[i].vertices[1]].color = glm::vec3(0.0, 3.0, 0.0);
			base.verticesSubdiv[base.faces[i].vertices[2]].color = glm::vec3(0.0, 3.0, 0.0);

			Mesh::VertexSubdiv ujVertex;
			ujVertex.color = glm::vec3(0.0, 0.0, 0.0);
			ujVertex.position = (base.verticesSubdiv[base.faces[i].vertices[0]].position + base.verticesSubdiv[base.faces[i].vertices[1]].position + base.verticesSubdiv[base.faces[i].vertices[0]].position) / glm::vec3(3.0);
			base.verticesSubdiv.push_back(ujVertex);
			Mesh::Face f1;
			f1.vertices[0]= base.faces[i].vertices[0];
			f1.vertices[1]= base.faces[i].vertices[1];
			//f1.vertices[2]= base.verticesSubdiv.size() - 1;
			//base.faces.push_back(f1);
			f1.vertices[0] = base.faces[i].vertices[1];
			f1.vertices[1] = base.faces[i].vertices[2];
			//f1.vertices[2] = base.verticesSubdiv.size() - 1;
			//base.faces.push_back(f1);
			f1.vertices[0] = base.faces[i].vertices[2];
			f1.vertices[1] = base.faces[i].vertices[0];
			//f1.vertices[2] = base.verticesSubdiv.size() - 1;
			//base.faces.push_back(f1);
			//base.faces.erase(base.faces.begin()+i-1);
			
		}
	}

	


}