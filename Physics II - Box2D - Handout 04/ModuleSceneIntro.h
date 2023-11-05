#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

enum Scene {
	START,
	PINBALL,
	GAMEOVER
};
struct Bumper {
	PhysBody* bumpy;
};
class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;

	PhysBody* sensor;
	bool sensed;
	Scene currentScene;
	p2List<Bumper*> bumpers;

	//Pantalla inicio
	SDL_Texture* backgroundTexture = nullptr;
	bool skibidi;
	//

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
};
