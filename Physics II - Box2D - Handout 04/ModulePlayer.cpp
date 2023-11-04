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

	//Kicker -----------------------------------------------------------------------------------------
	kicker.pivot = App->physics->CreateRectangle(1700, 140, 20, 8, b2_staticBody);
	kicker.mobile = App->physics->CreateRectangle(350, 160, 22, 8, b2_dynamicBody);
	App->physics->CreatePrismaticJoint(kicker.mobile, { 0,0 }, kicker.pivot, { 0,0 }, { 0,1 }, 1.9f);

	//Ball -------------------------------------------------------------------------------------------
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
		circles.add(App->physics->CreateCircle(350, 150, 10));
		circles.getLast()->data->listener = this;
		currentScore = 0;
		onceInit = false;
		isDead = false;
		onceBall = true;
	}

	/*if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && App->physics->debug)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 10));
		circles.getLast()->data->listener = this;
	}*/

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
	//kicker.mobile->body->ApplyForce({ 0,-18 }, { 0,0 }, true);
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		kicker.mobile->body->ApplyForce({ 0,18 }, { 0,0 }, true);
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		kicker.mobile->body->ApplyForce({ 0,-110 }, { 0,0 }, true);
		App->audio->PlayFx(kickerFx);
	}

	



	return UPDATE_CONTINUE;
}



