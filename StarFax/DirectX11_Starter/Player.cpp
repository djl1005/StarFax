#include "Player.h"

Player::Player(Mesh* mesh, Material* pMaterial, Camera* cam) : GameEntity(mesh, pMaterial)
{
	speed = 3.0f;
	camera = cam;
}

Player::~Player()
{
}

void Player::Move(float dt)
{


	if (GetAsyncKeyState('W') & 0x8000)
	{
		offsetPosition(0.0f, speed * dt, 0.0f);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		offsetPosition(0.0f, -speed * dt, 0.0f);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		offsetPosition(-speed * dt, 0.0f, 0.0f);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		offsetPosition(speed * dt, 0.0f, 0.0f);
	}
}