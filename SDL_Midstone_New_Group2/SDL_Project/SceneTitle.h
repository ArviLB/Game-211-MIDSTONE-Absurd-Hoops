#ifndef SCENETITLE_H
#define SCENETITLE_H
#include <SDL3/SDL.h>
#include <Matrix.h>
#include "Scene.h"
#include "Entity.h"
#include <vector>

#include <SDL3/SDL_mixer.h>

using namespace MATH;
class SceneTitle : public Scene {
private:
	SDL_Window *window;
	float xAxis;
	float yAxis;

	Matrix4 projectionMatrix;
	Matrix4 invProjectionMatrix;

	SDL_Renderer* renderer;

	Entity* ball;

	std::vector<Entity*> entities;

	Entity* hoopBack;
	Entity* hoopFront;

	Entity* hoopEdgeL;
	Entity* hoopEdgeR;
	Entity* ballInDetect;
	bool isMouseDown = false;
	Vec3 previousMousePosition;
	Vec3 mousePos;
	bool cursorFlicking;
	Uint32 clickTimeDown; //SDL_MOUSEBUTTONDOWN
	Uint32 clickTimeUp;   //SDL_MOUSEBUTTONUP
	int swishCounter;
	bool collided;
	
	bool running = 0;

	float ballScale; // a scale to make the texture smaller or larger


	//for background image
	SDL_Texture* backgroundTexture;
	SDL_Surface* backgroundSurface;

	
	MIX_Mixer* mixer;
	float master_volume = 0.25f;
public:
	SceneTitle(SDL_Window* sdlWindow);
	~SceneTitle();
	bool OnCreate() override;
	void OnDestroy() override;
	void HandleEvents(const SDL_Event& event) override;
	void Update(const float time) override;
	void Render() const override;
};

#endif
