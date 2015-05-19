#pragma once
#include "GameEntity.h"
class Enemy :
	public GameEntity
{
public:
	Enemy(Mesh*, Material*);
	~Enemy();

	void Move(float);
	void hit();
private:

	float invinciblityTimer;
	float speed;
	float time;
	float health;
};