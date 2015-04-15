#include "Bullet.h"


Bullet::Bullet(Mesh* mesh, Material* pMaterial) : GameEntity(mesh, pMaterial)
{
	speed = 3.0f;
	setVelocity(0.0f, 0.0f, speed);
}

Bullet::~Bullet()
{
}