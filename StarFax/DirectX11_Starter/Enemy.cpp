#include "Enemy.h"


Enemy::Enemy(Mesh* mesh, Material* pMaterial) : GameEntity(mesh, pMaterial)
{
	speed = 3.0f;
	time = 0.0f;

	colider = OBB(XMFLOAT3(1, .5, .5));

	invinciblityTimer = 1;
	
	calcWorld();

	setVelocity(speed, 0.0f, 0.0f);
}


Enemy::~Enemy()
{
}

void Enemy::hit()
{
	if (invinciblityTimer <= 0)
	{
		invinciblityTimer = 1;
		speed += (1.0f * (speed/ fabsf(speed)));
		speed *= -1;
		setVelocity(speed, 0.0f, 0.0f);
	}
	
}

void Enemy::Move(float dt)
{
	time += dt;

	if (invinciblityTimer >= 0)
	{
		invinciblityTimer -= dt;
	}

	if (!(position.x >= -10 && position.x <= 10))
	{
		speed *= -1;
	}


	setVelocity(speed, 0.0f, 0.0f);
}