#include "Morph.hpp"

#include <ctime>

using namespace std;

void morphByStep(Mesh& pointCloud, Mesh& base)
{
	
	Mesh::VertexPC bunnyPoint = pointCloud.verticesPC[rand() % pointCloud.verticesPC.size()];
	
	float minDistance=1000;
	int minDistanceIndex;
	float distance;

	for (int i = 0; i < base.verticesSubdiv.size(); i++)
	{
		distance = glm::distance(base.verticesSubdiv[i].position, bunnyPoint.position);
		if (minDistance > distance)
		{
			minDistance = distance;
			minDistanceIndex = i;
		}
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
	
	glm::vec3 delta = (base.verticesSubdiv[minDistanceIndex].position - bunnyPoint.position);

	for (int i = 0; i < verticesInRange.size(); i++)
	{
		base.verticesSubdiv[verticesInRange[i]].position = base.verticesSubdiv[verticesInRange[i]].position - (delta*(1.0f-(distances[i]/0.02f)))/5.0f;
		//base.verticesSubdiv[i].position = base.verticesSubdiv[i].position + delta/10.f /(1.0f-(1.0f * 0.2f/distances[i])) ;
	}
}