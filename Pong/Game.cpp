#include <cmath>        // std::abs
#include <iostream>

#include "Game.h"
#include <time.h>

using namespace std;

// Global Settings
const int thickness = 15;
const float paddleH = 100.0f;
const int numBalls = 1;

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
	, mPaddleLeftDir(0)
	, mPaddleRightDir(0)
{
}

bool Game::Initialize()
{
	int result = SDL_Init(SDL_INIT_VIDEO);
	if (result != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError);
		return false;
	}

	mWindow = SDL_CreateWindow(
		"Pong",
		100,
		100,
		1024,
		768,
		0
	);
	if (!mWindow) {
		SDL_Log("Unable to create window: %s", SDL_GetError);
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mRenderer) {
		SDL_Log("Unable to create renderer: %s", SDL_GetError);
		return false;
	}

	// Game specific
	mPaddleLeftPos.x = 10.0f;
	mPaddleLeftPos.y = 768.0f / 2.0f;
	mPaddleRightPos.x = 1024.0f - 10.0f - thickness;
	mPaddleRightPos.y = 768.0f / 2.0f;

	srand((unsigned)time(0));

	for (int i = 0; i < numBalls; i++) {
		float ballPosX = 1024.0f / 2.0f;
		float ballPosY = 768.0f / 2.0f;

		int randomDirX = rand() % 2;
		randomDirX = randomDirX == 0 ? -1 : 1;

		int randomDirY = rand() % 2;
		randomDirY = randomDirY == 0 ? -1 : 1;

		float ballVelX = (float)(150 + rand() % 200) * (float)randomDirX;
		float ballVelY = (float)(150 + rand() % 235) * (float)randomDirY;

		Ball ball{
			Vector2{ballPosX, ballPosY},
			Vector2{ballVelX, ballVelY}
		};
		mBalls.push_back(ball);
	}

	return true;
}

void Game::RunGameLoop()
{
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// Update left paddle direction based on W/S keys
	mPaddleLeftDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleLeftDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleLeftDir += 1;
	}

	// Update right paddle direction based on I/K keys
	mPaddleRightDir = 0;
	if (state[SDL_SCANCODE_I])
	{
		mPaddleRightDir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mPaddleRightDir += 1;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();

	// Update left paddle position based on direction
	if (mPaddleLeftDir != 0)
	{
		mPaddleLeftPos.y += mPaddleLeftDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddleLeftPos.y < (paddleH / 2.0f + thickness))
		{
			mPaddleLeftPos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddleLeftPos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddleLeftPos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	// Update right paddle position based on direction
	if (mPaddleRightDir != 0)
	{
		mPaddleRightPos.y += mPaddleRightDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddleRightPos.y < (paddleH / 2.0f + thickness))
		{
			mPaddleRightPos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddleRightPos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddleRightPos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	for (Ball& ball : mBalls) {
		Vector2& mBallPos = ball.pos;
		Vector2& mBallVel = ball.vel;
		// Update ball position based on ball velocity
		mBallPos.x += mBallVel.x * deltaTime;
		mBallPos.y += mBallVel.y * deltaTime;

		// Bounce if needed
		// Did we intersect with the left paddle?
		if (
			// Our y-difference is small enough
			abs(mPaddleLeftPos.y - mBallPos.y) <= paddleH / 2.0f &&
			// We are in the correct x-position
			mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
			// The ball is moving to the left
			mBallVel.x < 0.0f)
		{
			mBallVel.x *= -1.0f;
		}
		// Did we intersect with the right paddle?
		else if (
			// Our y-difference is small enough
			abs(mPaddleRightPos.y - mBallPos.y) <= paddleH / 2.0f &&
			// We are in the correct x-position
			mBallPos.x <= (1024.0f - 10.0f) && mBallPos.x >= (1024.0f - 10.0f - thickness) &&
			// The ball is moving to the right
			mBallVel.x > 0.0f)
		{
			mBallVel.x *= -1.0f;
		}
		// Did the ball go off the screen? (if so, end game)
		else if (mBallPos.x <= 0.0f || mBallPos.x >= 1024.0f)
		{
			mIsRunning = false;
		}

		// Did the ball collide with the top wall?
		if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
		{
			mBallVel.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (mBallPos.y >= (768 - thickness) &&
			mBallVel.y > 0.0f)
		{
			mBallVel.y *= -1;
		}
	}
}

void Game::GenerateOutput()
{
	// Clear Screen
	SDL_SetRenderDrawColor(
		mRenderer,
		0, // R
		0, // G
		255, // B
		255 // A
	);
	SDL_RenderClear(mRenderer);

	// Draw Frame
	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw Left paddle
	SDL_Rect leftPaddle{
		static_cast<int>(mPaddleLeftPos.x),
		static_cast<int>(mPaddleLeftPos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &leftPaddle);

	// Draw Right paddle
	SDL_Rect rightPaddle{
		static_cast<int>(mPaddleRightPos.x),
		static_cast<int>(mPaddleRightPos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &rightPaddle);

	// Draw ball
	for (Ball ball : mBalls) {
		SDL_Rect ball_obj{
		static_cast<int>(ball.pos.x - thickness / 2),
		static_cast<int>(ball.pos.y - thickness / 2),
		thickness,
		thickness
		};
		SDL_RenderFillRect(mRenderer, &ball_obj);
	}

	// Swap buffers
	SDL_RenderPresent(mRenderer);
}