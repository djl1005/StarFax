#include "Enemy.h"


Enemy::Enemy(Mesh* mesh, Material* pMaterial) : GameEntity(mesh, pMaterial)
{
	speed = 3.0f;
	time = 0.0f;
	
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