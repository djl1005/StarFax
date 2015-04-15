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
		setVelocity(0.0f, speed, 0.0f);
	}

	else if (GetAsyncKeyState('S') & 0x8000)
	{
		setVelocity(0.0f, -speed, 0.0f);
	}

	else if (GetAsyncKeyState('A') & 0x8000)
	{
		setVelocity(-speed, 0.0f, 0.0f);
	}

	else if (GetAsyncKeyState('D') & 0x8000)
	{
		setVelocity(speed, 0.0f, 0.0f);
	}

	else
	{
		setVelocity(0, 0, 0);
	}
}