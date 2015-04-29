#include "Particle.h"


Particle::Particle(XMFLOAT3 pos, XMFLOAT3 vel, XMFLOAT3 rot, XMFLOAT3 col, Mesh* m, Material* mat)
{
	mesh = m;
	material = mat;

	position = pos;
	scale = XMFLOAT3(1, 1, 1);
	rotation = rot;
	velocity = vel;

	rgbValue = col;

	age = 0;

	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&world, XMMatrixTranspose(W));
}


Particle::~Particle()
{
}

float Particle::getAge()
{
	return age;
}

void Particle::setAge(float a)
{
	age = a;
}

void Particle::offsetPosition(float x, float y, float z)
{
	position = XMFLOAT3(position.x + x, position.y + y, position.z + z);
}


void Particle::calcWorld()
{

	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX rotationXMatrix = XMMatrixRotationX(rotation.x);
	XMMATRIX rotationYMatrix = XMMatrixRotationY(rotation.y);
	XMMATRIX rotationZMatrix = XMMatrixRotationZ(rotation.z);
	XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);

	XMMATRIX W = scaleMatrix * rotationXMatrix * rotationYMatrix * rotationZMatrix * translationMatrix;

	XMStoreFloat4x4(&world, XMMatrixTranspose(W));
}

void Particle::update(float dt)
{ 
	offsetPosition(velocity.x * dt, velocity.y * dt, velocity.z * dt);
	age += dt;
	calcWorld();
}
void Particle::draw(ID3D11DeviceContext* context, Camera* cam)
{
	material->getVerShader()->SetMatrix4x4("world", world);
	material->getVerShader()->SetMatrix4x4("view", cam->getViewMat());
	material->getVerShader()->SetMatrix4x4("projection", cam->getProjection());

	material->setShaders();
	mesh->draw(context);
}
