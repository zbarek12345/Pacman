//
// Created by Wiktor on 18.03.2025.
//

#include "Game.h"

#include <MenuState.h>

#include "GameOverState.h"
#include "LevelSelectState.h"
#include "PlayState.h"
#include "TestState.cpp"

Game::GameStates Game::_state = Game::MENU;  // Initialize to MENU or any other default state
GameState* Game::_currentState = nullptr;   // Initialize to nullptr
int Game::_speed = 5 * 1e9/SDL_GetPerformanceFrequency();// Default value for _speed
int Game::_volume = 0;                      // Default value for _volume
SDL_Window* Game::_window = nullptr;        // Initialize to nullptr
SDL_Renderer* Game::_renderer = nullptr;
TTF_Font* Game::_font = nullptr;
DatabaseController* Game::_databaseController = nullptr;
int Game::_fps = 100;
bool Game::_render = true;
SDL_bool Game::_running = SDL_TRUE;

pthread_mutex_t stateLock = PTHREAD_MUTEX_INITIALIZER;
Game::Game(SDL_Window *window, SDL_Renderer *renderer) {
	_window = window;
	_renderer = renderer;
	_font = TTF_OpenFont("../Fonts/varsity_regular.ttf", 25);
	_databaseController = new DatabaseController("../Database/database.sqlite");
	_currentState = new MenuState(_renderer);
	//pthread_mutex_init(&stateLock, nullptr);
	//pthread_create(&_renderThread, nullptr, Renderer, nullptr);
}

Game::~Game() {
	delete _currentState;
	TTF_CloseFont(_font);
}

void Game::run() {
	SDL_Event event;
	uint32_t start = SDL_GetTicks();
	//GameState::_next = nullptr;
	while (_running) {
		uint32_t delta = ceil(1e3/_fps);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) _running = SDL_FALSE;
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
}
