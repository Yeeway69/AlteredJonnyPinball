#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	// Flippers
	b2Vec2 pAngle = { -0.44, -0 };
	b2Vec2 b = { -0, 0 };
	Flipper* f = new Flipper;
	f->Circle = App->physics->CreateCircle(440, 700, 4, b2_staticBody);
	f->Rect = App->physics->CreateRectangle(430 + rectSect.w / 2, 690 + rectSect.h / 2, rectSect.w, rectSect.h - 10, b2_dynamicBody);
	f->rightSide = false;
	App->physics->CreateRevoluteJoint(f->Rect, pAngle, f->Circle, b, 35.0f);
	flippers.add(f);

	pAngle = { 0.44,0 };

	Flipper* f2 = new Flipper;
	f2->Circle = App->physics->CreateCircle(570, 700, 4, b2_staticBody);
	f2->Rect = App->physics->CreateRectangle(560 - rectSect.w / 2, 690 + rectSect.h / 2, rectSect.w, rectSect.h - 10, b2_dynamicBody);
	f2->rightSide = true;
	App->physics->CreateRevoluteJoint(f2->Rect, pAngle, f2->Circle, b, 35.0f);
	flippers.add(f2);

	//Kicker
	kicker.pivot = App->physics->CreateRectangle(750, 700, 20, 8, b2_staticBody);
	kicker.mobile = App->physics->CreateRectangle(750, 650, 22, 8, b2_dynamicBody);
	App->physics->CreatePrismaticJoint(kicker.mobile, { 0,0 }, kicker.pivot, { 0,0 }, { 0,1 }, 1.9f);

	//Ball
	onceInit = true;
	isDead = false;

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{

	if (onceInit)
	{
		circles.add(App->physics->CreateCircle(750, 550, 10));
		circles.getLast()->data->listener = this;
		currentScore = 0;
		onceInit = false;
		isDead = false;
		onceBall = true;
	}

	// Flippers --------------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		p2List_item<Flipper*>* f = flippers.getFirst();
		while (f != NULL)
		{
			if (f->data->rightSide == false)
			{
				f->data->Rect->body->ApplyForce({ -3,0 }, { 0,0 }, true);
			}
			f = f->next;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		p2List_item<Flipper*>* f = flippers.getFirst();
		while (f != NULL)
		{
			if (f->data->rightSide == true)
			{
				f->data->Rect->body->ApplyForce({ 3,0 }, { 0,0 }, true);
			}
			f = f->next;
		}
	}

	// Kicker --------------------------------------------------------
	kicker.mobile->body->ApplyForce({ 0,-18 }, { 0,0 }, true);
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		kicker.mobile->body->ApplyForce({ 0,18 }, { 0,0 }, true);
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		kicker.mobile->body->ApplyForce({ 0,-36 }, { 0,0 }, true);
		App->audio->PlayFx(kickerFx);
	}


	return UPDATE_CONTINUE;
}



