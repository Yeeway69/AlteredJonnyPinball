#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{


	LOG("Loading Intro assets");
	bola1 = App->textures->Load("pinball/bola1.png");
	bola2 = App->textures->Load("pinball/bola2.png");
	bola3 = App->textures->Load("pinball/bola3.png");



	int triangulo[8] = {
	292, 489,
	292, 599,
	440, 605,
	299, 487
	};

	ricks.add(App->physics->CreateChain(0, 0, triangulo, 8, b2_staticBody));

	int triangulo1[8] = {
	714, 600,
	715, 489,
	581, 601,
	706, 606
	};

	ricks1.add(App->physics->CreateChain(0, 0, triangulo1, 8, b2_staticBody));


	bool ret = true;
	currentScene = START;
	App->renderer->camera.x = App->renderer->camera.y = 0;

	Bumper* b = new Bumper;
	b->bumpy = App->physics->CreateCircle(418, 332, 40, b2_staticBody);
	b->bumpy->listener = this;
	bumpers.add(b);

	Bumper* b1 = new Bumper;
	b1->bumpy = App->physics->CreateCircle(594, 332, 40, b2_staticBody);
	b1->bumpy->listener = this;
	bumpers.add(b1);

	//Duplica estas lineas 
	Bumper* b2 = new Bumper;
	b2->bumpy = App->physics->CreateCircle(508, 436, 40, b2_staticBody);
	b2->bumpy->listener = this;
	bumpers.add(b2);

	Bumper* b3 = new Bumper;
	b3->bumpy = App->physics->CreateCircle(508, 436 - 208, 40, b2_staticBody);
	b3->bumpy->listener = this;
	bumpers.add(b3);


	backgroundTexture = App->textures->Load("pinball/start.png");
	circle = App->textures->Load("pinball/BocchiBall.png");
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/triangle1.png");

	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	currentScene = START;
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	switch (currentScene) 
	{


	case START:
		fondocargado = true;
		fondocargado2 = true;
		
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)

		{
			currentScene = PINBALL;
			
		}
		if (isded) {
			backgroundTexture = App->textures->Load("pinball/start.png");
			App->player->fliperTexture = App->textures->Load("pinball/flipperL.png");
			App->player->kickerTexture = App->textures->Load("pinball/flipperL.png");
			isded = false;
		}
		
		App->renderer->Blit(backgroundTexture, 0, 0);
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			currentScene = GAMEOVER;

		}
		
		break;
	case PINBALL:
	{
		
		if (fondocargado) {
			backgroundTexture = App->textures->Load("pinball/Fondo1.png");
			fondocargado = false;
			circles.add(App->physics->CreateCircle(750, 550, 10));
			circles.getLast()->data->listener = this;
			App->player->playerBall = circles.getLast()->data;
		}
		int x, y;
		App->player->playerBall->GetPosition(x, y);
		if (y > 1000) 
		{
			App->physics->DestroyObject(App->player->playerBall);
			bolas++;
			if (bolas == 3) {
				currentScene = GAMEOVER;
				bolas = 0;
			}
			else {
				circles.add(App->physics->CreateCircle(750, 550, 10));
				circles.getLast()->data->listener = this;
				App->player->playerBall = circles.getLast()->data;
			}
			
			
			
		}
		
		App->renderer->Blit(backgroundTexture, 0, 0);
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			currentScene = GAMEOVER;

		}
		/*if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			ray_on = !ray_on;
			ray.x = App->input->GetMouseX();
			ray.y = App->input->GetMouseY();
		}*/

		//if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		//{
		//	circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 25));
		//	circles.getLast()->data->listener = this;
		//}

		//if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		//{
		//	boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
		//}

		//if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		//{
		//	// Pivot 0, 0
		//	int rick_head[64] = {
		//		14, 36,
		//		42, 40,
		//		40, 0,
		//		75, 30,
		//		88, 4,
		//		94, 39,
		//		111, 36,
		//		104, 58,
		//		107, 62,
		//		117, 67,
		//		109, 73,
		//		110, 85,
		//		106, 91,
		//		109, 99,
		//		103, 104,
		//		100, 115,
		//		106, 121,
		//		103, 125,
		//		98, 126,
		//		95, 137,
		//		83, 147,
		//		67, 147,
		//		53, 140,
		//		46, 132,
		//		34, 136,
		//		38, 126,
		//		23, 123,
		//		30, 114,
		//		10, 102,
		//		29, 90,
		//		0, 75,
		//		30, 62
		//	};

		//	//ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
		//}

		// Prepare for raycast ------------------------------------------------------

		iPoint mouse;
		mouse.x = App->input->GetMouseX();
		mouse.y = App->input->GetMouseY();
		int ray_hit = ray.DistanceTo(mouse);

		fVector normal(0.0f, 0.0f);

		// All draw functions ------------------------------------------------------
		p2List_item<PhysBody*>* c = circles.getFirst();

		while (c != NULL)
		{
			int x, y;
			c->data->GetPosition(x, y);
			App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
			c = c->next;
		}

		c = boxes.getFirst();

		while (c != NULL)
		{
			int x, y;
			c->data->GetPosition(x, y);
			App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
			if (ray_on)
			{
				int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
				if (hit >= 0)
					ray_hit = hit;
			}
			c = c->next;
		}

		c = ricks.getFirst();

		while (c != NULL)
		{
			int x, y;
			c->data->GetPosition(x, y);
			App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
			c = c->next;
		}

		// ray -----------------
		if (ray_on == true)
		{
			fVector destination(mouse.x - ray.x, mouse.y - ray.y);
			destination.Normalize();
			destination *= ray_hit;

			App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

			if (normal.x != 0.0f)
				App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
			
		}
		p2List_item<Bumper*>* b = bumpers.getFirst();
		while (b != NULL)
		{
			int x, y;
			b->data->bumpy->GetPosition(x, y);
			if (counterTextures == 0) 
			{
				App->renderer->Blit(bola1, x, y + 2, false, false, 1.0f, b->data->bumpy->GetRotation());
			}
			if (counterTextures == 1) 
			{
				App->renderer->Blit(bola2, x, y + 2, false, false, 1.0f, b->data->bumpy->GetRotation());
			}
			switch (counterTextures)
			{
			case 0:
				
				break;
			case 1:
			
				break;
			case 2:
				App->renderer->Blit(bola3, x, y + 2, false, false, 1.0f, b->data->bumpy->GetRotation());
				break;
			default:
				App->renderer->Blit(bola1, x, y + 2, false, false, 1.0f, b->data->bumpy->GetRotation());
				break;
			}
			counterTextures++;
			b = b->next;
		}
		
			
	}
	break;

		case GAMEOVER:
			if (fondocargado2) {
				backgroundTexture = App->textures->Load("pinball/game_over.png");
				fondocargado2 = false;
			}
			
			circles.clear();
			boxes.clear();
			ricks.clear();
			App->player->flippers.clear();
			App->renderer->Blit(backgroundTexture, 0, 0);
			if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)

			{
				isded = true;
				currentScene = START;
			}
			break;
			
		
	}
	


	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);

	p2List_item<Bumper*>* b = bumpers.getFirst();
	while (b != NULL)
	{
		if (bodyA == b->data->bumpy )
		{
			//App->audio->PlayFx(bumperFx);
			b2Vec2 force(bodyB->body->GetWorldCenter() - bodyA->body->GetWorldCenter());
			force *= 2;
			bodyB->body->ApplyLinearImpulse(force, bodyB->body->GetWorldCenter(), true);

			return;
		}
		b = b->next;
	}
	
}
