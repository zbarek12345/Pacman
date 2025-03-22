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

Game::Game(SDL_Window *window, SDL_Renderer *renderer) {
	_window = window;
	_renderer = renderer;
	_currentState = new TestState(renderer);
	_font = TTF_OpenFont("../Fonts/varsity_regular.ttf", 25);
}

Game::~Game() {
	delete _currentState;
	TTF_CloseFont(_font);
}


void Game::run() {
	SDL_Event event;
	SDL_bool running = SDL_TRUE;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = SDL_FALSE;
			_currentState->handleInput(event, _currentState);
		}
		_currentState->update();
		SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255); // Black background
		SDL_RenderClear(_renderer);
		_currentState->render();
		SDL_RenderPresent(_renderer);
	}
}
