#pragma once

#include <d3d11.h>
#include "Vertex.h"
#include <vector>
#include "Mesh.h"
#include "Material.h"
#include <DirectXMath.h>
#include "GameEntity.h"


// For DirectX Math
using namespace DirectX;

class Terrain :
	public GameEntity
{
public:
	Terrain(ID3D11Device * theDevice, Material * m, float dist);
	~Terrain();

private:

	Vertex a[25];
	unsigned int in[96];

	Mesh * mesh;

	Mesh * gentrateMesh(ID3D11Device * theDevice, float dist, int size);
};

