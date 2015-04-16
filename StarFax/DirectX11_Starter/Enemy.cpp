#include "Enemy.h"


Enemy::Enemy(Mesh* mesh, Material* pMaterial) : GameEntity(mesh, pMaterial)
{
	speed = 3.0f;
	time = 0.0f;

	colider = OBB(XMFLOAT3(1, .5, .5));
	
	calcWorld();
}


Enemy::~Enemy()
{
}

void Enemy::Move(float dt)
{
	time += dt;
	setVelocity(sin(time) * speed, 0.0f, 0.0f);
}