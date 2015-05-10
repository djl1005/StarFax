#include "GameEntity.h"


GameEntity::GameEntity(Mesh* m, Material * Pmaterial)
{
	mesh = m;

	material = Pmaterial;

	position = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	rotiation = XMFLOAT3(0, 0, 0);

	velocity = XMFLOAT3(0, 0, 0);
	acceleration = XMFLOAT3(0, 0, 0);

	rotVelocity = XMFLOAT3(0, 0, 0);
	rotAcceleration = XMFLOAT3(0, 0, 0);

	
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&world, XMMatrixTranspose(W));

	

	calcWorld();
}

#pragma region gets / sets
void GameEntity::setPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
}

void GameEntity::offsetPosition(float x, float y, float z)
{
	position = XMFLOAT3(position.x + x, position.y + y, position.z + z);
}

void GameEntity::setVelocity(float x, float y, float z)
{
	velocity = XMFLOAT3(x, y, z);
}

void GameEntity::offsetVelocity(float x, float y, float z)
{
	velocity = XMFLOAT3(velocity.x + x, velocity.y + y, velocity.z + z);
}

void GameEntity::setAcceleration(float x, float y, float z)
{
	acceleration = XMFLOAT3(x, y, z);
}

void GameEntity::offsetAcceleration(float x, float y, float z)
{
	acceleration = XMFLOAT3(acceleration.x + x, acceleration.y + y, acceleration.z + z);
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
}

void GameEntity::offsetRotation(float x, float y, float z)
{
	rotiation = XMFLOAT3(rotiation.x + x, rotiation.y + y, rotiation.z + z);
}

void GameEntity::setRotVelocity(float x, float y, float z)
{
	rotVelocity = XMFLOAT3(x, y, z);
}

void GameEntity::offsetRotVelocity(float x, float y, float z)
{
	rotVelocity = XMFLOAT3(rotVelocity.x + x, rotVelocity.y + y, rotVelocity.z + z);
}


void GameEntity::setRotAcceleration(float x, float y, float z)
{
	rotAcceleration = XMFLOAT3(x, y, z);
}

void GameEntity::offsetRotAcceleration(float x, float y, float z)
{
	rotAcceleration = XMFLOAT3(rotAcceleration.x + x, rotAcceleration.y + y, rotAcceleration.z + z);
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

OBB GameEntity::GetColider()
{
	return colider;
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

void GameEntity::update(float dt)
{
	XMFLOAT3 tempPos = position;
	XMFLOAT3 tempRot = rotiation;

	

	offsetVelocity(acceleration.x*dt, acceleration.y*dt, acceleration.z*dt);
	offsetPosition(velocity.x*dt, velocity.y*dt, velocity.z*dt);

	offsetRotVelocity(rotAcceleration.x*dt, rotAcceleration.y*dt, rotAcceleration.z*dt);
	offsetRotation(rotVelocity.x*dt, rotVelocity.y*dt, rotVelocity.z*dt);

	//check to see if pos changed
	if (!(position.x == tempPos.x && position.y == tempPos.y && position.z == tempPos.z) || 
		!(rotiation.x == tempRot.x && rotiation.y == tempRot.y && rotiation.z == tempRot.z))
	{
			calcWorld();
	}

}


void GameEntity::calcWorld()
{

		XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
		XMMATRIX rotationXMatrix = XMMatrixRotationX(rotiation.x);
		XMMATRIX rotationYMatrix = XMMatrixRotationY(rotiation.y);
		XMMATRIX rotationZMatrix = XMMatrixRotationZ(rotiation.z);
		XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);

		XMMATRIX W = scaleMatrix * rotationXMatrix * rotationYMatrix * rotationZMatrix * translationMatrix;

		XMStoreFloat4x4(&world, XMMatrixTranspose(W));

		colider.updateSat(this->position, this->rotiation);
}

void GameEntity::draw(ID3D11DeviceContext * deviceContext, Camera * cam, char * texture)
{

	getMaterial()->getVerShader()->SetMatrix4x4("world", getWorld());
	getMaterial()->getVerShader()->SetMatrix4x4("view", cam->getViewMat());
	getMaterial()->getVerShader()->SetMatrix4x4("projection", cam->getProjection());

	getMaterial()->setShaders(texture);
	getMesh()->draw(deviceContext);
}

GameEntity::~GameEntity()
{
	//delete mesh;
	//delete material;
}
