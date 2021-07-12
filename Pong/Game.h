#pragma once
#include <SDL.h>
#include <vector>

using namespace std;

struct Vector2 {
	float x;
	float y;
};

struct Ball {
	Vector2 pos;
	Vector2 vel;
};

class Game
{
public:
	// Constructor
	Game();

	// Initialize the game
	bool Initialize();

	// Run game loop
	void RunGameLoop();

	// Clean up after game
	void Shutdown();

private:
	// Game loop helper functions
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	// Window created by SDL
	SDL_Window* mWindow;
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	// Game should continue to run
	bool mIsRunning;

	// Pong specific
	// Direction of paddle
	int mPaddleLeftDir;
	int mPaddleRightDir;
	// Position of paddle
	Vector2 mPaddleLeftPos;
	Vector2 mPaddleRightPos;
	vector<Ball> mBalls;
};
