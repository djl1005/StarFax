#pragma once
#include "GameEntity.h"
class Bullet :
	public GameEntity
{
public:
	Bullet(Mesh*, Material*);
	~Bullet();

private:
	float speed;
};

