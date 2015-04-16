#include "OBB.h"



OBB::OBB(XMFLOAT3 phalfwidth)
{
	halfWidth = phalfwidth;

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

bool OBB::Sat(OBB * second)
{
	XMVECTOR axisToTest[15];

	int indexToadd = 0;

	for (int i = 0; i < 3; i++)
	{
		axisToTest[indexToadd] = XMVectorSet(rotatedNormals[i].x, rotatedNormals[i].y, rotatedNormals[i].z, 0);
		indexToadd++;
	}
	
	for (int i = 0; i < 3; i++)
	{
		axisToTest[indexToadd] = XMVectorSet(second->rotatedNormals[i].x,
										second->rotatedNormals[i].y, 
										second->rotatedNormals[i].z, 0);
		indexToadd++;
	}


	XMVECTOR zero = XMVectorSet(0, 0, 0, 0);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			XMVECTOR temp = XMVector3Cross(axisToTest[i], axisToTest[j]);
			if (!XMVector3Equal(temp, zero))
			{
				axisToTest[indexToadd] = temp;
				indexToadd++;
			}
		}

	}



	for (int i = 0; i < indexToadd; i++)
	{

		//min max along axis for a object a 
		float mina = 9999;
		float maxa = -9999;

		float minb = 9999;
		float maxb = -9999;

		for (int j = 0; j < 8; j++)
		{
			XMVECTOR temp = XMVectorSet(worldVertices[j].x, worldVertices[j].y,  worldVertices[j].z, 0);
			XMVECTOR a = XMVector3Dot(axisToTest[i],temp);
			float dot; 
			XMStoreFloat(&dot, a);
			
			if (dot <mina) { mina = dot; }
			if (dot > maxa) { maxa = dot; }
		}

		for (int j = 0; j < 8; j++)
		{
			XMVECTOR temp = XMVectorSet(second->worldVertices[j].x, second->worldVertices[j].y, second->worldVertices[j].z, 0);
			XMVECTOR a = XMVector3Dot(axisToTest[i], temp);
			float dot;
			XMStoreFloat(&dot, a);

			if (dot <minb) { minb = dot; }
			if (dot > maxb) { maxb = dot; }
		}

		//if they don't over lap return
		if (!(maxb >= mina &&  maxb <= maxa))
		{
			if (!(minb >= mina &&  minb <= maxa))
			{
				if (!(maxa >= minb &&  maxa <= maxb))
				{
					if (!(mina >= minb &&  mina <= maxb))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}
