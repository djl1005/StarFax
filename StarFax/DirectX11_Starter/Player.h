#pragma once
#include "GameEntity.h"
#include "Camera.h"

class Player :
	public GameEntity
{
public:
	Player(Mesh*, Material*, Camera*);
	~Player();

	void Move(float);

private:
	float speed;
	Camera* camera;
	float camPosition;
};

