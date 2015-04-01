#include "GameManager.h"

GameManager::GameManager()
{
	state = 0;
}

GameManager::~GameManager()
{
}

void GameManager::manageInput()
{
	switch (state)
	{
		case 0:
			if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			{
				state = 1;
			}
			break;

		case 1:
			if (GetAsyncKeyState('P') & 0x8000)
			{
				state = 2;
			}
			break;

		case 2:
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				state = 1;
			}
	}
}

int GameManager::getState()
{
	return state;
}