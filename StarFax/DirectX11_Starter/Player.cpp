#include "Player.h"

Player::Player(Mesh* mesh, Material* pMaterial, Camera* cam) : GameEntity(mesh, pMaterial)
{
	speed = 3.0f;
	camera = cam;
	SHOTTIME = .35f;
	timer = SHOTTIME;
}

Player::~Player()
{
}

void Player::Move(float dt)
{

	if (timer > 0) timer -= dt;

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


	// stuff to lock player to camera

	XMFLOAT4 sp;

	XMVECTOR secrenPos;
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX project;

	secrenPos = XMVectorSet(position.x, position.y, position.z, 1);

	world = XMLoadFloat4x4(&this->world);
	view = XMLoadFloat4x4(&camera->getViewMat());
	project = XMLoadFloat4x4(&camera->getProjection());

	world *= view;

	world *= project;

	secrenPos = XMVector4Transform(secrenPos, world);

	XMStoreFloat4(&sp, secrenPos);


	if (sp.x > 5 && velocity.x > 0) setVelocity(0, velocity.y, velocity.z);
	if (sp.x < -5 && velocity.x < 0) setVelocity(0, velocity.y, velocity.z);
	if (sp.y > 5 && velocity.y > 0) setVelocity(velocity.x, 0, velocity.z);
	if (sp.y < -5 && velocity.y < 0) setVelocity(velocity.x, 0, velocity.z);
}

bool Player::Fire()
{
	if (GetAsyncKeyState(' ') & 0x8000 && timer <= 0)
	{
		timer = SHOTTIME;
		return true;
	}

	else
	{
		return false;
	}
}