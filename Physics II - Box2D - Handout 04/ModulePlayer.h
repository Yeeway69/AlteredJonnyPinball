#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "p2List.h"

struct Flipper
{
	PhysBody* Rect;
	PhysBody* Circle;
	bool rightSide;
};

struct Kicker
{
	PhysBody* pivot;
	PhysBody* mobile;
};


class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:


	p2List<PhysBody*> circles;
	SDL_Rect circleSect = { 127, 317, 19, 21 };
	int ballCount = 2;
	bool onceInit = true;
	bool isDead = false;

	SDL_Texture* fliperTexture;
	SDL_Texture* kickerTexture;

	PhysBody* playerBall;

	p2List<Flipper*> flippers;
	SDL_Rect rectSect = { 48, 250, 64, 20 };

	Kicker kicker;
	SDL_Rect kickerSect = { 302,706,22,96 };
	SDL_Rect kickerSectBonus = { 1002,706,22,96 };

	uint clickFx;
	uint kickerFx;

	bool onceBall = true;
	int currentScore = 0;
	int previousScore;
	int highScore = 0;
};