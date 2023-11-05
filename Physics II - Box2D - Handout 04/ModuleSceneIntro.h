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
	p2List<PhysBody*> ricks1;

	PhysBody* sensor;
	bool sensed;
	Scene currentScene;
	p2List<Bumper*> bumpers;
	bool fondocargado;
	bool fondocargado2;
	int bolas;
	bool isded;
	//Pantalla inicio
	SDL_Texture* backgroundTexture = nullptr;
	bool skibidi;
	//

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* player;
	SDL_Texture* rick;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;

	bool isInPinballScene = false;
	int counterTextures = 0;
	SDL_Texture* bumperTexture;

	SDL_Texture* bola1;
	SDL_Texture* bola2;
	SDL_Texture* bola3;
};
