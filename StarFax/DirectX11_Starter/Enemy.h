#pragma once
#include "GameEntity.h"
class Enemy :
	public GameEntity
{
public:
	Enemy(Mesh*, Material*);
	~Enemy();

	void Move(float);

private:
	float speed;
	float time;
};