#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	// big static circle as "ground" in the middle of the screen
	int x = SCREEN_WIDTH / 2;
	int y = SCREEN_HEIGHT / 1.5f;
	int diameter = SCREEN_WIDTH / 2;

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* big_ball = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	// Pivot 0, 0
	int pinball[48] = {
	600, 763,
	719, 669,
	720, 459,
	727, 448,
	733, 460,
	733, 732,
	753, 745,
	768, 733,
	768, 205,
	743, 115,
	666, 51,
	581, 38,
	485, 36,
	394, 65,
	322, 133,
	291, 223,
	290, 384,
	290, 670,
	423, 766,
	264, 766,
	264, 0,
	783, 1,
	784, 766,
	600, 766
	};

	App->physics->CreateChain(0, 0, pinball, 48, b2_staticBody);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, b2BodyType type)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, b2BodyType type)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, b2BodyType type)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

b2RevoluteJoint* ModulePhysics::CreateRevoluteJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, float angle, bool collideConnected, bool enableLimit)
{
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = A->body;
	revoluteJointDef.bodyB = B->body;
	revoluteJointDef.collideConnected = collideConnected;
	revoluteJointDef.localAnchorA.Set(anchorA.x, anchorA.y);
	revoluteJointDef.localAnchorB.Set(anchorB.x, anchorB.y);
	revoluteJointDef.referenceAngle = 0;
	revoluteJointDef.enableLimit = enableLimit;
	revoluteJointDef.lowerAngle = -DEG_TO_RAD(angle);
	revoluteJointDef.upperAngle = DEG_TO_RAD(angle);

	return (b2RevoluteJoint*)world->CreateJoint(&revoluteJointDef);
}

b2PrismaticJoint* ModulePhysics::CreatePrismaticJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, b2Vec2 axys, float maxHeight, bool collideConnected, bool enableLimit)
{
	b2PrismaticJointDef prismaticJointDef;
	prismaticJointDef.bodyA = A->body;
	prismaticJointDef.bodyB = B->body;
	prismaticJointDef.collideConnected = collideConnected;
	prismaticJointDef.localAxisA.Set(axys.x, axys.y);
	prismaticJointDef.localAnchorA.Set(anchorA.x, anchorA.y);
	prismaticJointDef.localAnchorB.Set(anchorB.x, anchorB.y);
	prismaticJointDef.referenceAngle = 0;
	prismaticJointDef.enableLimit = enableLimit;
	prismaticJointDef.lowerTranslation = -0.01;
	prismaticJointDef.upperTranslation = maxHeight;

	return (b2PrismaticJoint*)world->CreateJoint(&prismaticJointDef);
}

// 
update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}

			// Variables for holding clicked body and mouse positions
			b2Body* clickedBody = nullptr;
			int mouseX, mouseY;
			float32 mouseX_meters, mouseY_meters;

			// TODO 1: If mouse button 1 is pressed ...
			// App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN
			// test if the current body contains mouse position
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				float32 mouseX_meters = PIXEL_TO_METERS(mouseX);
				float32 mouseY_meters = PIXEL_TO_METERS(mouseY);


				for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
            for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext()) {
                b2Transform transform = f->GetBody()->GetTransform();
                if (f->GetShape()->TestPoint(transform, b2Vec2(mouseX_meters, mouseY_meters))) {
                    clickedBody = b;
                    break;
                }
            }
            if (clickedBody) break;  // Exit the loop if a body is found
        }

				// If a body was selected we will attach a mouse joint to it
	            // so we can pull it around
	            // TODO 2: If a body was selected, create a mouse joint
	            // using mouse_joint class property
				if (clickedBody != nullptr) {
					int mouseX, mouseY;
					SDL_GetMouseState(&mouseX, &mouseY);
					float32 mouseX_meters = PIXEL_TO_METERS(mouseX);
					float32 mouseY_meters = PIXEL_TO_METERS(mouseY);
					// Create mouse joint
					b2MouseJointDef def;
					def.bodyA = ground; // Assuming 'ground' is already defined
					def.bodyB = clickedBody;
					def.target.Set(mouseX_meters, mouseY_meters); // Assuming mouse position in meters is available
					def.dampingRatio = 0.5f;
					def.frequencyHz = 2.0f;
					def.maxForce = 100.0f * clickedBody->GetMass();
					b2MouseJoint* mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);

				}

			}

			// New code for TODO 3
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				if (mouse_joint) {
					int mouseX, mouseY;
					SDL_GetMouseState(&mouseX, &mouseY);
					float32 mouseX_meters = PIXEL_TO_METERS(mouseX);
					float32 mouseY_meters = PIXEL_TO_METERS(mouseY);

					// Update the target position
					mouse_joint->SetTarget(b2Vec2(mouseX_meters, mouseY_meters));

					// Draw the line (convert back to pixels for rendering)
					b2Vec2 anchorA = mouse_joint->GetAnchorA();
					b2Vec2 anchorB = mouse_joint->GetAnchorB();
					int p1x = METERS_TO_PIXELS(anchorA.x);
					int p1y = METERS_TO_PIXELS(anchorA.y);
					int p2x = METERS_TO_PIXELS(anchorB.x);
					int p2y = METERS_TO_PIXELS(anchorB.y);

					App->renderer->DrawLine(p1x, p1y, p2x, p2y, 255, 0, 0);
				}
			}

			// New code for destroying the joint when the mouse button is released
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP) {
				if (mouse_joint) {
					world->DestroyJoint(mouse_joint);
					mouse_joint = nullptr; // Reset mouse_joint pointer to NULL
				}
			}

		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void ModulePhysics::DestroyObject(PhysBody* physBody)
{
	world->DestroyBody(physBody->body);
}