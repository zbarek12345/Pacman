//
// Created by Wiktor on 18.03.2025.
//

#ifndef STATE_H
#define STATE_H
#include <SDL_events.h>
#include <SDL_render.h>

class GameState {
public:
	explicit GameState(SDL_Renderer* renderer) : _renderer(renderer) {}
	virtual ~GameState() = default;

	// Pure virtual methods for each state to implement
	virtual void update() = 0;               // Update logic (e.g., animations)
	virtual void render() = 0;               // Draw the state to the screen
	virtual void handleInput(SDL_Event& event, GameState*& nextState) = 0; // Handle events and possibly switch states

	SDL_Renderer* _renderer; // Shared renderer
};

#endif //STATE_H
