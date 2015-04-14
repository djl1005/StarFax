#include "Terrain.h"


Terrain::Terrain(ID3D11Device * theDevice, Material * m, float dist) : GameEntity(gentrateMesh(theDevice, dist, 5), m)
{
}

Mesh * Terrain::gentrateMesh(ID3D11Device * theDevice, float dist, int size)
{

	//Vertex a[25];
	//unsigned int in[96];

	int curentIndex = 0;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			Vertex temp;
			temp.Position = XMFLOAT3(dist * i, 0, dist * j);
			temp.Normal = XMFLOAT3(0, 1, 0);

			float u = float(i) / float(size);
			float v = float(j) / float(size);

			temp.UV = XMFLOAT2(u, v );
			a[curentIndex] = temp;

			curentIndex++;
		}
	}

	int index = 0;

	for (int i = 0; i < size * size; i += 1)
	{
		if (i + size < size * size)
		{
			in[index] = i;
			index++;
			in[index] = i + size;
			index++;
			in[index] = i + 1;
			index++;
		}
		if (i - size > 0)
		{
			in[index] = i;
			index++;
			in[index] = i + 1;
			index++;
			in[index] = i + 1 - size;
			index++;
		}
	}

	mesh = new Mesh(a, size* size, in, index, theDevice);

	return mesh;

	

}

Terrain::~Terrain()
{
}
