#pragma once

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

using namespace DirectX;
class Particle
{
public:
	Particle(XMFLOAT3 pos, XMFLOAT3 vel, XMFLOAT3 rot, XMFLOAT4 col, Mesh* m, Material* mat);
	~Particle();
	
	float getAge();
	void setAge(float a);

	void calcWorld();
	void offsetPosition(float, float, float);

	void update(float dt, XMFLOAT3 vel);
	void draw(ID3D11DeviceContext* context, Camera* cam);

private:
	XMFLOAT3 position;
	XMFLOAT3 velocity;
	XMFLOAT3 scale;
	XMFLOAT3 rotation;
	XMFLOAT4 rgbValue;
	float age;

	Mesh * mesh;
	Material * material;

	XMFLOAT4X4 world;
};

