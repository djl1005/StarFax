#include "OBB.h"


OBB::OBB()
{
	halfWidth = XMFLOAT3(1,1,1);

	vertices[0] = XMFLOAT3(-1, -1, 1);
	vertices[1] = XMFLOAT3(1, -1, 1);
	vertices[2] = XMFLOAT3(-1, 1, 1);
	vertices[3] = XMFLOAT3(1, 1, 1);
	vertices[4] = XMFLOAT3(-1, 1, -1);
	vertices[5] = XMFLOAT3(1, 1, -1);
	vertices[6] = XMFLOAT3(-1, -1, -1);
	vertices[7] = XMFLOAT3(1, -1, -1);

	normals[0] = XMFLOAT3(1, 0, 0);
	normals[1] = XMFLOAT3(0, 1, 0);
	normals[2] = XMFLOAT3(0, 0, 1);
	normals[3] = XMFLOAT3(-1, 0, 0);
	normals[4] = XMFLOAT3(0, -1, 0);
	normals[5] = XMFLOAT3(0, 0, -1);


	dirty = true;
}


OBB::~OBB()
{
}

void OBB::updateSat(XMFLOAT3 translation, XMFLOAT3 rotation)
{

	XMMATRIX postion = XMMatrixTranslation(translation.x, translation.y, translation.z);
	XMMATRIX mRotation = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	XMMATRIX rt = mRotation * postion;
	
	for (int i = 0; i < 8; i++)
	{
		XMVECTOR temp = XMVectorSet(
			vertices[i].x * halfWidth.x, 
			vertices[i].y * halfWidth.y, 
			vertices[i].z * halfWidth.z, 0);

		 temp = XMVector3Transform(temp,rt);
		XMStoreFloat3(&worldVertices[i], temp);
		int k = 0;
	}

	for (int i = 0; i < 6; i++)
	{
		XMVECTOR temp = XMVectorSet(normals[i].x, normals[i].y, normals[i].z, 0);
		 temp =XMVector3Rotate(temp, quat);
		XMStoreFloat3(&rotatedNormals[i], temp);

	}


}
