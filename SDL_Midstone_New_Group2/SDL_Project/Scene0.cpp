#include "Scene0.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <MMath.h>
#include <iostream>
#include "Entity.h"



Scene0::Scene0(SDL_Window* sdlWindow_) :
	window(sdlWindow_)
	, renderer(nullptr)
	, cliff(nullptr)
	, ball(nullptr)
	, crate(nullptr)
	, diver(nullptr)
	, ballScale(0.25f)
	, xAxis(30.0f)
	, yAxis(15.0f)
{

}

Scene0::~Scene0() {

}

bool Scene0::OnCreate() {
	// Create a project matrix that moves positions from physics/world space 
	// to screen/pixel space
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	Matrix4 ndc = MMath::viewportNDC(w, h);
	Matrix4 ortho = MMath::orthographic(0.0f, xAxis, 0.0f, yAxis, -1.0f, 1.0f);
	projectionMatrix = ndc * ortho;

	//Create screen renderer
	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		std::cerr << "SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	//Initialize renderer color (black)
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	// Create the objects that will be rendered on the screen
	cliff = new Entity();
	cliff->pos = Vec3(0.0f, 5.0f, 0.0f);
	cliff->SetImage("textures/cliff.png", renderer);

	ball = new Entity();
	ball->pos = Vec3(5.0f, 8.0f, 0.0f);
	ball->SetImage("textures/ball.png", renderer);
	
	crate = new Entity();
	crate->pos = Vec3(18.0f, 13.0f, 0.0f);
	crate->SetImage("textures/crate.png", renderer);

	rock = new Entity();
	rock->pos = Vec3(15.0f, 8.0f, 0.0f);
	rock->SetImage("textures/rock.png", renderer);

	Entity* ballEntity = new Entity();
	ballEntity->pos = Vec3(6.0f, 8.0f, 0.0f);
	ballEntity->SetImage("textures/bowlingBall.png", renderer);
	entities.push_back(ballEntity);

	hoopBack = new Entity();
	hoopBack->pos = Vec3(24.0f, 10.0f, 0.0f);
	hoopBack->SetImage("textures/hoopBack.png", renderer);
	hoopBack->angleDeg = -20.0f;

	hoopFront = new Entity();
	hoopFront->pos = Vec3(24.0f, 10.0f, 0.0f);
	hoopFront->SetImage("textures/hoopFront.png", renderer);
	hoopFront->angleDeg = -20.0f;

	hoopEdgeL = new Entity();
	hoopEdgeL->pos = Vec3(23.5f, 8.7f, 0.0f);
	hoopEdgeL->SetImage("textures/centerer.png", renderer);

	hoopEdgeR = new Entity();
	hoopEdgeR->pos = Vec3(25.3f, 9.5f, 0.0f);
	hoopEdgeR->SetImage("textures/centerer.png", renderer);

	ballInDetect = new Entity();
	ballInDetect->pos = Vec3(24.5f, 8.5f, 0.0f);
	ballInDetect->SetImage("textures/centerer.png", renderer);

	//disable the last three comments to view debug hitboxes

	diver = new Entity();
	diver->pos = Vec3(5.0f, 8.0f, 0.0f);
	diver->SetImage("textures/diver_windup_spritesheet.png", renderer);

	powerBar = new Entity();
	powerBar->pos = Vec3(6.0f, 8.0f, 0.0f);
	powerBar->angleDeg = 90.0f;
	powerBar->SetImage("textures/PowerBar_spritesheet.png", renderer);

	SDL_Init(SDL_INIT_AUDIO);
	MIX_Init();

	mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

	if (!mixer)
	{
		std::cout << "Failed to create mixer: %s\n", SDL_GetError();
		return 0;
	}

	//// Load and play music
	MIX_Audio* Music = MIX_LoadAudio(mixer, "Audio/Wii Sports - Boxing Results Music.mp3", true);
	MIX_SetMasterGain(mixer, master_volume);
	MIX_PlayAudio(mixer, Music);
	MIX_DestroyAudio(Music);


	//background image
	backgroundSurface = IMG_Load("textures/BG_Underwater.png"); // path to your background image


	backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
	if (!backgroundTexture) {
		std::cerr << "Failed to create background texture: " << SDL_GetError() << std::endl;
		return false;
	}

	backgroundSurface = nullptr;



	return true;
}

void Scene0::OnDestroy() {
	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}

	if (mixer) {
		MIX_DestroyMixer(mixer);
		MIX_Quit();
	}
	
	delete cliff;
	cliff = nullptr;

	delete diver;
	diver = nullptr;
	
	delete powerBar;
	powerBar = nullptr;

	delete ball;
	ball = nullptr;

	for (auto* entity : entities) {
		delete entity;
	}
	entities.clear();

	delete ballInDetect;
	ballInDetect = nullptr;

	hoopBack = nullptr;
	hoopFront = nullptr;
	hoopEdgeL = nullptr;
	hoopEdgeR = nullptr;

	delete crate;
	crate = nullptr;

	delete rock;
	rock = nullptr;
	/*
	if (backgroundTexture) {
		SDL_DestroyTexture(backgroundTexture);
		backgroundTexture = nullptr;
	}
	if (backgroundSurface) {
		SDL_DestroySurface(backgroundSurface);
		backgroundSurface = nullptr;
	}
	*/
}



void Scene0::HandleEvents(const SDL_Event& event) {

	switch (event.type) {
	case SDL_EVENT_KEY_DOWN:
		// Change angle of the ball
		if (event.key.scancode == SDL_SCANCODE_O) {
			if (running == 0) {

				if (!entities.empty()) {
					auto ball = entities.back();
					ball->angleDeg -= 10.0f;
					std::cout << "Ball angle: " << ball->angleDeg << std::endl;
				}
				powerBar->angleDeg -= 10.0f;
			}
			else {}
		}
		if (event.key.scancode == SDL_SCANCODE_P) {
			if (running == 0) {
				if (!entities.empty()) {


					auto ball = entities.back();
					ball->angleDeg += 10.0f;
					std::cout << "Ball angle: " << ball->angleDeg << std::endl;
				}
				powerBar->angleDeg += 10.0f;
			}
			else {}
		}
		if (event.key.scancode == SDL_SCANCODE_G) {
			if (running == 0) {

				if (!entities.empty()) {
					auto ball = entities.back();
					ball->ballPower += 5.0f;
					std::cout << "Ball power: " << ball->ballPower << std::endl;

				}
			}
			else {}
		}
		if (event.key.scancode == SDL_SCANCODE_H) {
			if (running == 0) {

				if (!entities.empty()) {
						auto ball = entities.back();
						if (ball->ballPower <= 0) {
							ball->ballPower = 0;
						}
						else {
						ball->ballPower -= 5.0f;
						}
						std::cout << "Ball power: " << ball->ballPower << std::endl;
				}
			}
			else {}
		}
		//launch the ball 
		if (event.key.scancode == SDL_SCANCODE_SPACE) {
			if (running == 0) {

				if (!entities.empty()) {
					auto ball = entities.back();
					float angleRad = ball->angleDeg * (3.14159f / 180.0f);
					std::cout << "Ball power: " << ball->ballPower << std::endl;
					ball->vel.y -= sin(angleRad) * ball->ballPower;
					ball->vel.x += cos(angleRad) * ball->ballPower;
					running = 1;
				}
			}
			else{}
		}
		if (event.key.scancode == SDL_SCANCODE_R) {
			entities.clear();
		}
		if (event.key.scancode == SDL_SCANCODE_E) {
			Entity* newBall = new Entity();
			powerBar->angleDeg = 90.0f;
			newBall->radius = 0.15f;  // radius
			newBall->mass = 1.0f;
			newBall->pos = Vec3(6.0f, 8.0f, 0.0f);
			newBall->SetImage("textures/bowlingBall.png", renderer);
			entities.push_back(newBall);
			running = 0;
		}
		break;

		/*
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		isDiverWinding = true;
		// Start the diver animation
		clickTimeDown = SDL_GetTicks();
		Vec3 mousePosWorld = Vec3(event.button.x, event.button.y, 0.0f);
		previousMousePosition = mousePosWorld;
		clickTimeDown = SDL_GetTicks();
		isMouseDown = true;

		if (!entities.empty()) {
			auto ball = entities.back();
			ball->vel = Vec3(0.0f, 0.0f, 0.0f);
			ball->gravForce = Vec3(0.0f, 0.0f, 0.0f);
			ball->pos = mousePosWorld;
		}
		break;
	}

	case SDL_EVENT_MOUSE_BUTTON_UP: {
		// Stop and reset animation
		isDiverWinding = false;


		isMouseDown = false;
		clickTimeUp = SDL_GetTicks();
		mousePos = invProjectionMatrix * Vec3(event.button.x, event.button.y, 0.0f);
		float timeDelta = (clickTimeUp - clickTimeDown) / 1000.0f;
		if (timeDelta > 0.0f && !entities.empty()) {
			Vec3 flickVel = (mousePos - previousMousePosition) / timeDelta;
			auto ball = entities.back();
			ball->vel = flickVel;
		}
		break;
	}

	case SDL_EVENT_MOUSE_MOTION:
		if (isMouseDown && !entities.empty()) {
			Vec3 mouseWorldPos = invProjectionMatrix * Vec3(event.motion.x, event.motion.y, 0.0f);
			auto ball = entities.back();
			ball->pos = mouseWorldPos;
			ball->vel = Vec3(0.0f, 0.0f, 0.0f);
		}
		break;
*/
	}

}

void Scene0::Update(const float deltaTime) {
	/// Physics goes here	
//	if (ball->ballPower != 0.0f) {
//		isDiverWinding = true;
//	}
//	else {
//		isDiverWinding = false;
//	}
	// Check crate collision first
	if (running) {
		Vec3 gravity = Vec3(0.0f, -9.8f, 0.0f); // F = m * a
		Vec3 drag = -0.2f * ball->vel; // F = -c * v

		//for now, wind is negligible
		Vec3 wind = Vec3(0.0f, 0.0f, 0.0f); // F = constant
		Vec3 netForce = gravity + drag + wind;
		if (!entities.empty()) {
			auto ball = entities.back();
			ball->ApplyForce(netForce); // gravity
			ball->Update(deltaTime);
		}
	}

	if (ball->ballPower != 0.0f) {
		isDiverWinding = true;
	}
	else {
		isDiverWinding = false;
	}


	//arvi's sem 1 ball physics
	float energyLoss = 0.9f; //for bumping off walls
	float EPSILON = 1e-6f;
	Vec3 gravAccel(0.0f, -9.8f, 0.0f); //gravity



	for (size_t i = 0; i < entities.size(); ++i) { //for every ball
		Entity* ball = entities[i];
		Vec3 gravForce = ball->mass * gravAccel;
		//ball->ApplyForce(gravForce);
		//ball->Update(deltaTime);


		//ground and wall collision
		if (ball->pos.y < 2.0f) {
			if (!entities.empty()) {
				entities.pop_back();
				ball->pos.y = 2.0f;
				ball->vel.y *= -energyLoss;
			}
		}
		if (ball->pos.y > 20.0f) {
			ball->pos.y = 20.0f;
			ball->vel.y *= -energyLoss;
		}
		if (ball->pos.x < 0.0f) {
			ball->pos.x = 0.0f;
			ball->vel.x *= -energyLoss / 1.5f;
		}
		if (ball->pos.x > 27.0f) {
			ball->pos.x = 27.0f;
			ball->vel.x *= -energyLoss / 1.5f;
		}

		//for stopping the ball from bouncing little off the ground
		if (ball->pos.y < 2.001f && ball->vel.y < 0.1f) {
			ball->pos.y = 2.0f;
			ball->vel.y = 0.0f;
		}
		/*
		//PREVIOUS HOOP COLLISION CODE

		for (size_t i = 0; i < entities.size(); ++i) { //for every ball
			Entity* ball = entities[i];

			float distanceBallInDetect = VMath::distance(ball->pos, ballInDetect->pos);
			//for detecting if the ball went in the hoop

			if (distanceBallInDetect <= ball->radius + 0.20 && ball->vel.y < -5.0f) {
				ball->SetImage("textures/boom.png", renderer); //blows up the ball lol
				std::cout << "SCORE!!!!!";
			}
		}
		for (size_t i = 0; i < entities.size(); ++i) { //for every ball
			Entity* ball = entities[i];
			float distanceEdgeL = VMath::distance(ball->pos, hoopEdgeL->pos);
			//left hoop edge collision
			if (distanceEdgeL < ball->radius + 0.25f) {
				collided = true;
				if (collided) {
					Vec3 normal = VMath::normalize(ball->pos - hoopEdgeL->pos);

					if (VMath::dot(ball->vel, normal) < 0) {
						ball->vel = VMath::reflect(ball->vel, normal);
						ball->pos += normal * 0.01f;  //displacement for overlapping hitboxes
					}
					collided = false;
				}
			}
		}
		for (size_t i = 0; i < entities.size(); ++i) { //for every ball
			Entity* ball = entities[i];
			float distanceEdgeR = VMath::distance(ball->pos, hoopEdgeR->pos);
			//right hoop edge collision
			if (distanceEdgeR < ball->radius + 0.25) {
				collided = true;
				if (collided) {
					Vec3 normal = VMath::normalize(ball->pos - hoopEdgeR->pos);

					if (VMath::dot(ball->vel, normal) < 0) {
						ball->vel = VMath::reflect(ball->vel, normal);
						ball->pos += normal * 0.01f;  //displacement for overlapping hitboxes
					}
					collided = false;
				}
			}
		}
		 */

		Entity* obj = ballInDetect;
		for (auto* ball : entities)
		{
			//Distance between ball and HoopR
			float dist = VMath::distance(ball->pos, obj->pos);

			// ballInDetectL collision radius
			float ballInDetectRadius = std::max(obj->GetSurface()->w, obj->GetSurface()->h) * 0.005f;

			// if they overlap:
			if (dist < ball->radius + ballInDetectRadius)
			{
				ball->SetImage("textures/boom.png", renderer); //blows up the ball lol
				std::cout << "SCORE!!!!! \n";
				std::cout << "You move onto the next level! \n";
				//this is where the nexr scene loading code goes -arvi
			}
		}

		obj = hoopEdgeL;
		for (auto* ball : entities)
		{
			//Distance between ball and HoopR
			float dist = VMath::distance(ball->pos, obj->pos);

			// hoopEdgeL collision radius
			float hoopEdgeLRadius = std::max(obj->GetSurface()->w, obj->GetSurface()->h) * 0.005f;

			// if they overlap:
			if (dist < ball->radius + hoopEdgeLRadius)
			{
				// Get collision normal
				Vec3 normal = VMath::normalize(ball->pos - obj->pos);

				// Push ball out so they don't overlap
				float overlap = (ball->radius + hoopEdgeLRadius) - dist;
				ball->pos += normal * overlap;

				// Bounce ball off hoopEdgeL
				if (VMath::dot(ball->vel, normal) < 0.0f)
				{
					ball->vel = VMath::reflect(ball->vel, normal);

					// Ball slows down bouncing
					ball->vel *= 0.8f;
				}
			}
		}

		obj = hoopEdgeR;
		for (auto* ball : entities)
		{
			//Distance between ball and HoopR
			float dist = VMath::distance(ball->pos, obj->pos);

			// hoopEdgeR collision radius
			float hoopEdgeRRadius = std::max(obj->GetSurface()->w, obj->GetSurface()->h) * 0.005f;

			// if they overlap:
			if (dist < ball->radius + hoopEdgeRRadius)
			{
				// Get collision normal
				Vec3 normal = VMath::normalize(ball->pos - obj->pos);

				// Push ball out so they don't overlap
				float overlap = (ball->radius + hoopEdgeRRadius) - dist;
				ball->pos += normal * overlap;

				// Bounce ball off hoopEdgeR
				if (VMath::dot(ball->vel, normal) < 0.0f)
				{
					ball->vel = VMath::reflect(ball->vel, normal);

					// Ball slows down bouncing
					ball->vel *= 0.8f;
				}
			}
		}

		obj = crate;
		//		// TODO IN THE MORNING : UPON COLLISION, THE CRATE SPRITE WILL UPDATE TO A BROKEN CRATE AND WILL NO LONGER HAVE COLLISION ENABLED
				//IM TIRED BOSS
		//		bool crateDestroyed = false;
		for (auto* ball : entities)
		{
			//Distance between ball and crate
			float dist = VMath::distance(ball->pos, obj->pos);

			// crate collision radius
			float crateRadius = std::max(obj->GetSurface()->w, obj->GetSurface()->h) * 0.005f;
			if (crate->destroyed == 0) {

				// if they overlap:
				if (dist < ball->radius + crateRadius)
				{
					// Get collision normal
					Vec3 normal = VMath::normalize(ball->pos - obj->pos);

					// Push ball out so they don't overlap
					float overlap = (ball->radius + crateRadius) - dist;
					ball->pos += normal * overlap;

					// Bounce ball off hoopEdgeR
					if (VMath::dot(ball->vel, normal) < 0.0f)
					{
						ball->vel = VMath::reflect(ball->vel, normal);

						// Ball slows down bouncing
						ball->vel *= 0.8f;
					}
					crate->destroyed = true;

				}
			}

		}
		obj = rock;
		for (auto* ball : entities)
		{
			//Distance between ball and crate
			float dist = VMath::distance(ball->pos, obj->pos);

			// crate collision radius
			float rockRadius = std::max(obj->GetSurface()->w, obj->GetSurface()->h) * 0.005f;

				// if they overlap:
				if (dist < ball->radius + rockRadius)
				{
					// Get collision normal
					Vec3 normal = VMath::normalize(ball->pos - obj->pos);

					// Push ball out so they don't overlap
					float overlap = (ball->radius + rockRadius) - dist;
					ball->pos += normal * overlap;

					// Bounce ball off rock
					if (VMath::dot(ball->vel, normal) < 0.0f)
					{
						ball->vel = VMath::reflect(ball->vel, normal);

						// Ball slows down bouncing
						ball->vel *= 0.8f;
					}

			}

		}


		/*
		*/





		/*
			Entity* ball = entities[i];
			//for ball-ball collisions
			for (size_t j = i + 1; j < entities.size(); j++) { //for every other ball
				//uses vmath::distance to calc dist. b/n two balls
				float dist = VMath::distance(entities[i]->pos, entities[j]->pos);
				if (dist <= entities[i]->radius + entities[j]->radius) {
					Vec3 diff = entities[i]->pos - entities[j]->pos;

					//distance between ball centers; use dot product
					float distSq = VMath::dot(diff, diff);
					//avoids the dividing by zero error
					if (dist < EPSILON) continue;
					//calculates normal
					Vec3 normal = diff / dist;
					//relative velocity b/n two balls
					Vec3 relativeVel = entities[i]->vel - entities[j]->vel;
					//velocity along normal
					float velAlongNormal = VMath::dot(relativeVel, normal);
					//skips if balls are separating
					if (velAlongNormal > EPSILON) continue;
					//coeff. of restitution
					float restitutionCoeff = 1.0f;
					//scalar of impulse
					float impulseScalar = -(1 + restitutionCoeff) * velAlongNormal;
					//add masses of balls
					impulseScalar /= (1 / entities[i]->mass) + (1 / entities[j]->mass);
					//apply impulse from impulse scalar to balls' velocity
					Vec3 impulse = impulseScalar * normal;
					entities[i]->vel += impulse / entities[i]->mass;
					entities[j]->vel -= impulse / entities[j]->mass;
					//separates the balls so overlap doesn't happen
					float overlap = entities[i]->radius + entities[j]->radius - dist;
					if (overlap > 0.0f) {
						Vec3 overlapFix = (overlap / 2.0f) * normal;
						entities[i]->pos += overlapFix;
						entities[j]->pos -= overlapFix;
					}
				}
			}
		*/

	}
}
	


void Scene0::Render() const {
	SDL_RenderClear(renderer);
	SDL_RenderTextureRotated(renderer, backgroundTexture, nullptr, nullptr, SDL_FLIP_NONE, nullptr, SDL_FLIP_NONE);

	// Convert from world coordinates to pixel coordinates using Scott's magical matrix
	Vec3 screenCoords = projectionMatrix * cliff->pos;
	// Set up sprite's position and size
	SDL_FRect square;
	square.x = screenCoords.x;
	square.y = screenCoords.y;
	square.w = cliff->GetSurface()->w;
	square.h = cliff->GetSurface()->h;
	// Display object on the screen
	SDL_RenderTextureRotated(renderer, cliff->GetTexture(), nullptr, &square, cliff->angleDeg, nullptr, SDL_FLIP_NONE);

	/*
	// Rinse and repeat for the ball
	screenCoords = projectionMatrix * ball->pos;
	square.x = screenCoords.x;
	square.y = screenCoords.y;
	square.w = ball->GetSurface()->w * ballScale; // we will scale the ball 
	square.h = ball->GetSurface()->h * ballScale;
	SDL_RenderTextureRotated(renderer, ball->GetTexture(), nullptr, &square, ball->angleDeg, nullptr, SDL_FLIP_NONE);
	 */



	Vec3 screenCoordsHoopBack = projectionMatrix * hoopBack->pos;
	SDL_FRect squareHoopBack;
	squareHoopBack.x = static_cast<int>(screenCoordsHoopBack.x);
	squareHoopBack.y = static_cast<int>(screenCoordsHoopBack.y);
	squareHoopBack.w = hoopBack->GetSurface()->w;
	squareHoopBack.h = hoopBack->GetSurface()->h;
	SDL_RenderTextureRotated(renderer, hoopBack->GetTexture(), nullptr, &squareHoopBack, hoopBack->angleDeg, nullptr, SDL_FLIP_NONE);

	for (auto* entity : entities) {
		screenCoords = projectionMatrix * entity->pos;
		SDL_FRect square;
		square.x = static_cast<int>(screenCoords.x);
		square.y = static_cast<int>(screenCoords.y);
		square.w = entity->GetSurface()->w;
		square.h = entity->GetSurface()->h;
		SDL_RenderTextureRotated(renderer, entity->GetTexture(), nullptr, &square, entity->angleDeg, nullptr, SDL_FLIP_NONE);
	}

	Vec3 screenCoordsHoopFront = projectionMatrix * hoopFront->pos;
	SDL_FRect squareHoopFront;
	squareHoopFront.x = static_cast<int>(screenCoordsHoopFront.x);
	squareHoopFront.y = static_cast<int>(screenCoordsHoopFront.y);
	squareHoopFront.w = hoopFront->GetSurface()->w;
	squareHoopFront.h = hoopFront->GetSurface()->h;
	SDL_RenderTextureRotated(renderer, hoopFront->GetTexture(), nullptr, &squareHoopFront, hoopFront->angleDeg, nullptr, SDL_FLIP_NONE);

	Vec3 screenCoordsRock = projectionMatrix * rock->pos;
	SDL_FRect squareRock;
	squareRock.x = static_cast<int>(screenCoordsRock.x);
	squareRock.y = static_cast<int>(screenCoordsRock.y);
	squareRock.w = rock->GetSurface()->w;
	squareRock.h = rock->GetSurface()->h;
	SDL_RenderTextureRotated(renderer, rock->GetTexture(), nullptr, &squareRock, rock->angleDeg, nullptr, SDL_FLIP_NONE);



	// !! ANIMATIONS for diver !!
	// Play nice with Scott, and change a local static variable

	Vec3 screenCoordsDiver = projectionMatrix * diver->pos;
	SDL_FRect squareDiver;
	squareDiver.x = static_cast<int>(screenCoordsDiver.x);
	squareDiver.y = static_cast<int>(screenCoordsDiver.y);
	float scale = 1.0f;
	squareDiver.w = diver->GetSurface()->w * scale / 8;
	squareDiver.h = diver->GetSurface()->h * scale;

	SDL_FRect diverSprite;
	diverSprite.w = diver->GetSurface()->w / 8;
	diverSprite.h = diver->GetSurface()->h;
	diverSprite.y = 0;

	// as Render is const
	static int diverCounter = 0;
	// Slow down our animation
	static int diverFrameCounter = 0;
	///sprite.x = diver->GetSurface()->w / 8; // Change this to 0, w/8, 2w/8, 3w/8, 4w/8, 5w/8, 6w/8, 7w/8, 8w/8
	diverSprite.x = diverCounter * diver->GetSurface()->w / 8;
	SDL_RenderTextureRotated(renderer, diver->GetTexture(), &diverSprite, &squareDiver, diver->angleDeg, nullptr, SDL_FLIP_NONE);

	// !! ANIMATIONS for powerbar !!
// Play nice with Scott, and change a local static variable
// as Render is const
	Vec3 screenCoordsPowerBar = projectionMatrix * powerBar->pos;
	SDL_FRect squarePowerBar;
	squarePowerBar.x = static_cast<int>(screenCoordsPowerBar.x);
	squarePowerBar.y = static_cast<int>(screenCoordsPowerBar.y);
	squarePowerBar.w = powerBar->GetSurface()->w * scale / 8;
	squarePowerBar.h = powerBar->GetSurface()->h * scale;
	SDL_FRect powerBarSprite;
	powerBarSprite.w = powerBar->GetSurface()->w / 8;
	powerBarSprite.h = powerBar->GetSurface()->h;
	powerBarSprite.y = 0;

	static int powerBarCounter = 0;
	// Slow down our animation
	static int powerBarFrameCounter = 0;
	if (!entities.empty()) {
		auto ball = entities.back();
		if (ball->ballPower == 0.0f) {
			powerBarCounter = 0;
			diverCounter = 0;
		}
		else if (ball->ballPower == 5.0f) {
			diverCounter = 1;
		}
		else if (ball->ballPower == 10.0f) {
			powerBarCounter = 1;
			diverCounter = 2;
		}
		else if (ball->ballPower == 20.0f) {
			powerBarCounter = 2;
			diverCounter = 3;
		}
		else if (ball->ballPower == 30.0f) {
			powerBarCounter = 3;
			diverCounter = 4;
		}
		else if (ball->ballPower == 40.0f) {
			powerBarCounter = 4;
			diverCounter = 5;
		}
		else if (ball->ballPower >= 45.0f) {
			diverCounter = 6;
		}
	}

	///sprite.x = diver->GetSurface()->w / 8; // Change this to 0, w/8, 2w/8, 3w/8, 4w/8, 5w/8, 6w/8, 7w/8, 8w/8
	powerBarSprite.x = powerBarCounter * powerBar->GetSurface()->w / 8;
	SDL_RenderTextureRotated(renderer, powerBar->GetTexture(), &powerBarSprite, &squarePowerBar, powerBar->angleDeg, nullptr, SDL_FLIP_NONE);


	// !! ANIMATIONS for crate !!
// Play nice with Scott, and change a local static variable
// as Render is const
	Vec3 screenCoordsCrate = projectionMatrix * crate->pos;
	SDL_FRect squareCrate;
	squareCrate.x = static_cast<int>(screenCoordsCrate.x);
	squareCrate.y = static_cast<int>(screenCoordsCrate.y);
	squareCrate.w = crate->GetSurface()->w * scale / 2;
	squareCrate.h = crate->GetSurface()->h * scale;
	SDL_FRect crateSprite;
	crateSprite.w = crate->GetSurface()->w / 2;
	crateSprite.h = crate->GetSurface()->h;
	crateSprite.y = 0;

	static int crateCounter = 0;
	// Slow down our animation
	static int crateFrameCounter = 0;

	if (crate->destroyed == true) {
		crateCounter = 1;
	}
	else
	{
		crateCounter = 0;
	}
	///sprite.x = crate->GetSurface()->w / 2; // Change this to 0, w/2, 2w/2
	crateSprite.x = crateCounter * crate->GetSurface()->w / 2;
	SDL_RenderTextureRotated(renderer, crate->GetTexture(), &crateSprite, &squareCrate, crate->angleDeg, nullptr, SDL_FLIP_NONE);




	


	//for debug
	//renderers for hoop edges and ballInDetector
	/*
	Vec3 screenCoordsHoopEdgeL = projectionMatrix * hoopEdgeL->pos;
	SDL_FRect squareHoopEdgeL;
	squareHoopEdgeL.x = static_cast<int>(screenCoordsHoopEdgeL.x);
	squareHoopEdgeL.y = static_cast<int>(screenCoordsHoopEdgeL.y);
	squareHoopEdgeL.w = hoopEdgeL->GetSurface()->w;
	squareHoopEdgeL.h = hoopEdgeL->GetSurface()->h;
	SDL_RenderTextureRotated(renderer, hoopEdgeL->GetTexture(), nullptr, &squareHoopEdgeL, hoopEdgeL->angleDeg, nullptr, SDL_FLIP_NONE);

	Vec3 screenCoordsHoopEdgeR = projectionMatrix * hoopEdgeR->pos;
	SDL_FRect squareHoopEdgeR;
	squareHoopEdgeR.x = static_cast<int>(screenCoordsHoopEdgeR.x);
	squareHoopEdgeR.y = static_cast<int>(screenCoordsHoopEdgeR.y);
	squareHoopEdgeR.w = hoopEdgeR->GetSurface()->w;
	squareHoopEdgeR.h = hoopEdgeR->GetSurface()->h;
	SDL_RenderTextureRotated(renderer, hoopEdgeR->GetTexture(), nullptr, &squareHoopEdgeR, hoopEdgeR->angleDeg, nullptr, SDL_FLIP_NONE);

	Vec3 screenCoordsBallInDetect = projectionMatrix * ballInDetect->pos;
	SDL_FRect squareBallInDetect;
	squareBallInDetect.x = static_cast<int>(screenCoordsBallInDetect.x);
	squareBallInDetect.y = static_cast<int>(screenCoordsBallInDetect.y);
	squareBallInDetect.w = ballInDetect->GetSurface()->w;
	squareBallInDetect.h = ballInDetect->GetSurface()->h;
	SDL_RenderTextureRotated(renderer, ballInDetect->GetTexture(), nullptr, &squareBallInDetect, ballInDetect->angleDeg, nullptr, SDL_FLIP_NONE);
	 */


	// Update the screen
	SDL_RenderPresent(renderer);
}
