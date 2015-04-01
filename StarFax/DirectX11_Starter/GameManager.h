#pragma once
#pragma once
#include <Windows.h>
#include "Player.h"

class GameManager
{
public:
	GameManager();
	~GameManager();

	void manageInput();
	int getState();
	
private:
	int state;
};

