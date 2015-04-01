#include "GameEntity.h"


GameEntity::GameEntity(Mesh* m, Material * Pmaterial)
{
	mesh = m;

	material = Pmaterial;

	position = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	rotiation = XMFLOAT3(0, 0, 0);

	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&world, XMMatrixTranspose(W));

	calcWorld();
}

#pragma region gets / sets
void GameEntity::setPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	hasChanged = true;
}

void GameEntity::offsetPosition(float x, float y, float z)
{
	position = XMFLOAT3(position.x + x, position.y + y, position.z + z);
	hasChanged = true;
}

void GameEntity::setScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	hasChanged = true;
}

void GameEntity::offsetScale(float x, float y, float z)
{
	scale = XMFLOAT3(scale.x + x, scale.y + y, scale.z + z);
	hasChanged = true;
}

void GameEntity::setRotation(float x, float y, float z)
{
	rotiation = XMFLOAT3(x, y, z);
	hasChanged = true;
}

void GameEntity::offsetRotation(float x, float y, float z)
{
	rotiation = XMFLOAT3(rotiation.x + x, rotiation.y + y, rotiation.z + z);
	hasChanged = true;
}

XMFLOAT3 GameEntity::getPosition()
{
	return position;
}

XMFLOAT3 GameEntity::getScale()
{
	return scale;
}

XMFLOAT3 GameEntity::getRotation()
{
	return rotiation;
}

XMFLOAT4X4 GameEntity::getWorld()
{
	return world;
}

Mesh * GameEntity::getMesh()
{
	return mesh;
}

Material * GameEntity::getMaterial()
{
	return material;
}

#pragma endregion


void GameEntity::calcWorld()
{
	if (hasChanged)
	{

		XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
		XMMATRIX rotationXMatrix = XMMatrixRotationX(rotiation.x);
		XMMATRIX rotationYMatrix = XMMatrixRotationY(rotiation.y);
		XMMATRIX rotationZMatrix = XMMatrixRotationZ(rotiation.z);
		XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);

		XMMATRIX W = scaleMatrix * rotationXMatrix * rotationYMatrix * rotationZMatrix * translationMatrix;

		XMStoreFloat4x4(&world, XMMatrixTranspose(W));

		colider.updateSat(this->position, this->rotiation);

		hasChanged = false;
	}

}

void GameEntity::draw(ID3D11DeviceContext * deviceContext, Camera * cam)
{

	getMaterial()->getVerShader()->SetMatrix4x4("world", getWorld());
	getMaterial()->getVerShader()->SetMatrix4x4("view", cam->getViewMat());
	getMaterial()->getVerShader()->SetMatrix4x4("projection", cam->getProjection());

	getMaterial()->setShaders();
	getMesh()->draw(deviceContext);


}

GameEntity::~GameEntity()
{
	//delete mesh;
	//delete material;
}
