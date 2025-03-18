//
// Created by Wiktor on 18.03.2025.
//

#include "Game.h"

void Game::run() {
	SDL_Event event;
	SDL_bool running = SDL_TRUE;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = SDL_FALSE;
			_currentState->handleInput(event, _currentState);
		}
		_currentState->update();
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
		SDL_RenderClear(renderer);
		_currentState->render();
		SDL_RenderPresent(renderer);
	}
}
