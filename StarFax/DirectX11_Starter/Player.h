#pragma once
#include "GameEntity.h"
#include "Bullet.h"
#include "Camera.h"

class Player :
	public GameEntity
{
public:
	Player(Mesh*, Material*, Camera*);
	~Player();

	void Move(float);
	bool Fire();

private:
	float speed;
	float timer;
	float SHOTTIME;
	Camera* camera;
	float camPosition;
};

