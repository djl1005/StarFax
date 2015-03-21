#pragma once

#include <d3d11.h>
#include "Vertex.h"
#include <vector>
#include <fstream>


class Mesh
{
public:


	
	Mesh(Vertex * vertices, int numVertices, unsigned int* indices, int numIndices, ID3D11Device *);
	Mesh(char*, ID3D11Device *);
	~Mesh();

	void draw(ID3D11DeviceContext * deviceContext);

	ID3D11Buffer * GetVertexBuffer();
	ID3D11Buffer * GetIndexBuffer();
	int GetNumIndices();

private:
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;

	int numIndices;
};

