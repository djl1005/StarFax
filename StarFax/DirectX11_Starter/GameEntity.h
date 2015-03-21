#pragma once

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

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

	void calcWorld();

	void draw(ID3D11DeviceContext * deviceContext, Camera * cam);

	XMFLOAT3 getPosition();
	XMFLOAT3 getScale();
	XMFLOAT3 getRotation();
	XMFLOAT4X4 getWorld();
	Mesh * getMesh();
	Material * getMaterial();

private:

	bool hasChanged = false;
	

	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT3 rotiation;

	Mesh * mesh;
	Material * material;

	XMFLOAT4X4 world;
};

