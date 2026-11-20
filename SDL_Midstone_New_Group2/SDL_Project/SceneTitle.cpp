#include "SceneTitle.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <MMath.h>
#include <iostream>
#include "Entity.h"



SceneTitle::SceneTitle(SDL_Window* sdlWindow_) :
	window(sdlWindow_)
	, renderer(nullptr)
	, ball(nullptr)
	, ballScale(2.0f)
	, xAxis(30.0f)
	, yAxis(15.0f)
{

}

SceneTitle::~SceneTitle(){

}

bool SceneTitle::OnCreate() {
	// Create a project matrix that moves positions from physics/world space 
	// to screen/pixel space
	int w, h;
	SDL_GetWindowSize(window,&w,&h);
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

	ball = new Entity();
	ball->pos = Vec3(5.0f, 8.0f, 0.0f);
	ball->SetImage("textures/bowlingBall.png", renderer);

	Entity* ballEntity = new Entity();
	ballEntity->pos = Vec3(5.0f, 15.0f, 0.0f);
	ballEntity->SetImage("textures/bowlingBall.png", renderer);
	entities.push_back(ballEntity);

	hoopBack = new Entity();
	hoopBack->pos = Vec3(24.0f, 10.0f, 0.0f);
	hoopBack->SetImage("textures/hoopBack.png", renderer);

	hoopFront = new Entity();
	hoopFront->pos = Vec3(24.0f, 10.0f, 0.0f);
	hoopFront->SetImage("textures/hoopFront.png", renderer);

	hoopEdgeL = new Entity(); 
	hoopEdgeL->pos = Vec3(22.65f, 8.65f, 0.0f);
	//	hoopEdgeL->SetImage("centerer.png", renderer);

	hoopEdgeR = new Entity();
	hoopEdgeR->pos = Vec3(26.0f, 8.65f, 0.0f);
	//	hoopEdgeR->SetImage("centerer.png", renderer);

	ballInDetect = new Entity();
	ballInDetect->pos = Vec3(24.325f, 7.75f, 0.0f);
	//	ballInDetect->SetImage("centerer.png", renderer);

	SDL_Init(SDL_INIT_AUDIO);
	MIX_Init();

	mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

	if (!mixer)
	{
		std::cout << "Failed to create mixer: %s\n", SDL_GetError();
		return 0;
	}

	//// Load and play music
	MIX_Audio* Music = MIX_LoadAudio(mixer, "Audio/CrabRave.wav", true);
	MIX_SetMasterGain(mixer, master_volume);
	MIX_PlayAudio(mixer, Music);
	MIX_DestroyAudio(Music);


	hoopFront = new Entity();
	hoopFront->pos = Vec3(24.0f, 10.0f, 0.0f);
	hoopFront->SetImage("textures/hoopFront.png", renderer);

	
	//background image
	backgroundSurface = IMG_Load("textures/Title.png"); // path to your background image


	backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
	if (!backgroundTexture) {
		std::cerr << "Failed to create background texture: " << SDL_GetError() << std::endl;
		return false;
	}

	backgroundSurface = nullptr;
	

	return true;
}

void SceneTitle::OnDestroy() {
	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}

	if (mixer) {
		MIX_DestroyMixer(mixer);
		MIX_Quit(); 
	}

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

	/*if (backgroundTexture) {
		SDL_DestroyTexture(backgroundTexture);
		backgroundTexture = nullptr;
	}
	if (backgroundSurface) {
		SDL_DestroySurface(backgroundSurface);
		backgroundSurface = nullptr;
	}*/

}



void SceneTitle::HandleEvents(const SDL_Event& event)
{

}

void SceneTitle::Update(const float deltaTime) {
	/// Physics goes here	
	if (running) {
		Vec3 gravity = Vec3(0.0f, -9.8f, 0.0f); // F = m * a
		Vec3 drag = -0.2f * ball->vel; // F = -c * v
		
		//for now, wind is negligible
		Vec3 wind = Vec3(0.0f, 0.0f, 0.0f); // F = constant
		Vec3 netForce = gravity + drag + wind;
		ball->ApplyForce(netForce); // gravity
		ball->Update(deltaTime);
	}

	//arvi's sem 1 ball physics
	float energyLoss = 0.9f; //for bumping off walls
	float EPSILON = 1e-6f;
	Vec3 gravAccel(0.0f, -9.8f, 0.0f); //gravity

	for (size_t i = 0; i < entities.size(); ++i) { //for every ball
		Entity* ball = entities[i];
		Vec3 gravForce = ball->mass * gravAccel;
		ball->ApplyForce(gravForce);
		ball->Update(deltaTime);


		//ground and wall collision
		if (ball->pos.y < 2.0f) {
			ball->pos.y = 2.0f;
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
	for (size_t i = 0; i < entities.size(); ++i) { //for every ball
		Entity* ball = entities[i];
		float distanceBallInDetect = VMath::distance(ball->pos, ballInDetect->pos);
		//for detecting if the ball went in the hoop
		if (distanceBallInDetect <= ball->radius + 0.20 && ball->vel.y < -5.0f) {
			ball->SetImage("boom.png", renderer); //blows up the ball lol
		}
	}
	for (size_t i = 0; i < entities.size(); ++i) { //for every ball
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
	}


}

void SceneTitle::Render() const {
	SDL_RenderClear(renderer);
	SDL_RenderTextureRotated(renderer, backgroundTexture, nullptr, nullptr, SDL_FLIP_NONE, nullptr, SDL_FLIP_NONE);

	// Convert from world coordinates to pixel coordinates using Scott's magical matrix
	Vec3 screenCoords = projectionMatrix * Vec3(0.0f, 0.0f, 0.0f);
	// Set up sprite's position and size
	SDL_FRect square{};
	square.x = screenCoords.x;
	square.y = screenCoords.y;

	// Rinse and repeat for the ball
	screenCoords = projectionMatrix * ball->pos;
	square.x = screenCoords.x;
	square.y = screenCoords.y;
	square.w = ball->GetSurface()->w * ballScale; // we will scale the ball 
	square.h = ball->GetSurface()->h * ballScale;
	//SDL_RenderTextureRotated(renderer, ball->GetTexture(), nullptr, &square, ball->angleDeg, nullptr, SDL_FLIP_NONE);

	Vec3 screenCoordsHoopBack = projectionMatrix * hoopBack->pos;
	SDL_FRect squareHoopBack;
	squareHoopBack.x = static_cast<int>(screenCoordsHoopBack.x);
	squareHoopBack.y = static_cast<int>(screenCoordsHoopBack.y);
	squareHoopBack.w = hoopBack->GetSurface()->w;
	squareHoopBack.h = hoopBack->GetSurface()->h;
	SDL_RenderTextureRotated(renderer, hoopBack->GetTexture(), nullptr, &squareHoopBack, hoopBack->angleDeg, nullptr, SDL_FLIP_NONE);


	for (auto* entity : entities) {
		screenCoords = projectionMatrix * entity->pos;
		SDL_FRect square{};
		square.x = static_cast<int>(screenCoords.x);
		square.y = static_cast<int>(screenCoords.y);
		square.w = entity->GetSurface()->w;
		square.h = entity->GetSurface()->h;
		SDL_RenderTextureRotated(renderer, entity->GetTexture(), nullptr, &square, entity->angleDeg, nullptr, SDL_FLIP_NONE);
	}

	Vec3 screenCoordsHoopFront = projectionMatrix * hoopFront->pos;
	SDL_FRect squareHoopFront{};
	squareHoopFront.x = static_cast<int>(screenCoordsHoopFront.x);
	squareHoopFront.y = static_cast<int>(screenCoordsHoopFront.y);
	squareHoopFront.w = hoopBack->GetSurface()->w;
	squareHoopFront.h = hoopBack->GetSurface()->h;
	SDL_RenderTextureRotated(renderer, hoopFront->GetTexture(), nullptr, &squareHoopFront, hoopFront->angleDeg, nullptr, SDL_FLIP_NONE);



	//for debug
	//renderers for hoop edges and ballInDetector
	/*
	Vec3 screenCoordsHoopEdgeL = projectionMatrix * hoopEdgeL->pos;
	SDL_Rect squareHoopEdgeL;
	squareHoopEdgeL.x = static_cast<int>(screenCoordsHoopEdgeL.x);
	squareHoopEdgeL.y = static_cast<int>(screenCoordsHoopEdgeL.y);
	squareHoopEdgeL.w = hoopEdgeL->GetSurface()->w;
	squareHoopEdgeL.h = hoopEdgeL->GetSurface()->h;
	SDL_RenderCopyEx(renderer, hoopEdgeL->GetTexture(), nullptr, &squareHoopEdgeL, hoopEdgeL->angleDeg, nullptr, SDL_FLIP_NONE);

	Vec3 screenCoordsHoopEdgeR = projectionMatrix * hoopEdgeR->pos;
	SDL_Rect squareHoopEdgeR;
	squareHoopEdgeR.x = static_cast<int>(screenCoordsHoopEdgeR.x);
	squareHoopEdgeR.y = static_cast<int>(screenCoordsHoopEdgeR.y);
	squareHoopEdgeR.w = hoopEdgeR->GetSurface()->w;
	squareHoopEdgeR.h = hoopEdgeR->GetSurface()->h;
	SDL_RenderCopyEx(renderer, hoopEdgeR->GetTexture(), nullptr, &squareHoopEdgeR, hoopEdgeR->angleDeg, nullptr, SDL_FLIP_NONE);

	Vec3 screenCoordsBallInDetect = projectionMatrix * ballInDetect->pos;
	SDL_Rect squareBallInDetect;
	squareBallInDetect.x = static_cast<int>(screenCoordsBallInDetect.x);
	squareBallInDetect.y = static_cast<int>(screenCoordsBallInDetect.y);
	squareBallInDetect.w = ballInDetect->GetSurface()->w;
	squareBallInDetect.h = ballInDetect->GetSurface()->h;
	SDL_RenderCopyEx(renderer, ballInDetect->GetTexture(), nullptr, &squareBallInDetect, ballInDetect->angleDeg, nullptr, SDL_FLIP_NONE);
	*/


	// Update the screen
	SDL_RenderPresent(renderer);
}

