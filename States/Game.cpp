//
// Created by Wiktor on 18.03.2025.
//

#include "Game.h"

#include <MenuState.h>

#include "LevelSelectState.h"
#include "TestState.cpp"

Game::GameStates Game::_state = Game::MENU;  // Initialize to MENU or any other default state
GameState* Game::_currentState = nullptr;   // Initialize to nullptr
int Game::_speed = 0;                       // Default value for _speed
int Game::_volume = 0;                      // Default value for _volume
SDL_Window* Game::_window = nullptr;        // Initialize to nullptr
SDL_Renderer* Game::_renderer = nullptr;
TTF_Font* Game::_font = nullptr;
DatabaseController* Game::_databaseController = nullptr;
int Game::_fps = 60;
bool Game::_render = true;
pthread_t Game::_renderThread;
pthread_mutex_t stateLock = PTHREAD_MUTEX_INITIALIZER;
Game::Game(SDL_Window *window, SDL_Renderer *renderer) {
	_window = window;
	_renderer = renderer;
	_currentState = new MenuState(renderer);
	_font = TTF_OpenFont("../Fonts/varsity_regular.ttf", 25);
	_databaseController = new DatabaseController("../Database/database.sqlite");
	//pthread_mutex_init(&stateLock, nullptr);
	//pthread_create(&_renderThread, nullptr, Renderer, nullptr);
}

Game::~Game() {
	delete _currentState;
	TTF_CloseFont(_font);
}



void* Game::Renderer(void* arg) {


	int sleepNs = ceil(1e6/_fps);
	uint64_t start = SDL_GetPerformanceCounter(), end;
	timespec sleepTime = {0,0};
	while (_render) {
		pthread_mutex_lock(&stateLock);
		SDL_RenderClear(_renderer);
		_currentState->render();
		SDL_RenderPresent(_renderer);
		pthread_mutex_unlock(&stateLock);

		end = SDL_GetPerformanceCounter();
		sleepTime.tv_nsec = end - start;
		if (sleepTime.tv_nsec < sleepNs) {
			nanosleep(&sleepTime, nullptr);
		}
		start = SDL_GetPerformanceCounter();
	}
	return nullptr;
}

void Game::run() {
	SDL_Event event;
	SDL_bool running = SDL_TRUE;
	uint32_t start = SDL_GetTicks();
	uint32_t delta = ceil(1e3/_fps);
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = SDL_FALSE;
			//pthread_mutex_lock(&stateLock);
			_currentState->handleInput(event, _currentState);
			//pthread_mutex_unlock(&stateLock);
		}
		_currentState->update();
		SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255); // Black background
		if (SDL_GetTicks() - start > delta) {
			SDL_RenderClear(_renderer);
			_currentState->render();
			SDL_RenderPresent(_renderer);
			start = SDL_GetTicks();
		}
		SDL_Delay(1);
	}

	_render = false;
	pthread_join(_renderThread, nullptr);
}
