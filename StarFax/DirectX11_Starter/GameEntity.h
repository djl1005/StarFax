#pragma once

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "OBB.h"

using namespace DirectX;



class GameEntity
{
public:
	GameEntity(Mesh*, Material *);
	~GameEntity();

	void setPosition(float, float, float);
	void offsetPosition(float, float, float);
	void setScale(float, float, float);
	void offsetScale(float, float, float);
	void setRotation(float, float, float);
	void offsetRotation(float, float, float);
	void setVelocity(float, float, float);
	void setAcceleration(float, float, float);
	void setRotVelocity(float, float, float);
	void setRotAcceleration(float, float, float);
	void offsetVelocity(float, float, float);
	void offsetAcceleration(float, float, float);
	void offsetRotVelocity(float, float, float);
	void offsetRotAcceleration(float, float, float);

	void calcWorld();

	void draw(ID3D11DeviceContext * deviceContext, Camera * cam);

	XMFLOAT3 getPosition();
	XMFLOAT3 getScale();
	XMFLOAT3 getRotation();
	XMFLOAT3 getVelocity();
	XMFLOAT3 getAcceleration();
	XMFLOAT3 getRotVelocity();
	XMFLOAT3 getRotAcceleration();
	
	
	XMFLOAT4X4 getWorld();
	Mesh * getMesh();
	Material * getMaterial();
	OBB GetColider();

private:

	bool hasChanged = false;
	

	XMFLOAT3 position;
	XMFLOAT3 velocity;
	XMFLOAT3 acceleration;
	XMFLOAT3 scale;
	XMFLOAT3 rotiation;
	XMFLOAT3 rotVelocity;
	XMFLOAT3 rotAcceleration;

	OBB colider;

	Mesh * mesh;
	Material * material;

	XMFLOAT4X4 world;
};

