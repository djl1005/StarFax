#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class OBB
{
public:

	
	OBB();
	OBB(XMFLOAT3 halfwidth);
	~OBB();

	void updateSat(XMFLOAT3 tras, XMFLOAT3 rotation);
	bool Sat(OBB second);

private:

	bool dirty;

	XMFLOAT3 halfWidth;


	XMFLOAT3 normals[6];
	XMFLOAT3 vertices[8];

	XMFLOAT3 worldVertices[8];
	XMFLOAT3 rotatedNormals[6];
};

