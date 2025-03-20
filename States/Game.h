//
// Created by Wiktor on 18.03.2025.
//

#ifndef GAME_H
#define GAME_H
#include <GameState.h>


class Game {
	enum GameStates {
		MENU,
		LEVEL_SELECT,
		SETTINGS,
		PAUSE,
		GAME,
		GAME_OVER,
	};
	static GameStates _state;
	static GameState* _currentState;
	static int _speed;
	static int _volume;
public:
	Game(SDL_Window* window, SDL_Renderer* renderer);

	static SDL_Window* _window;
	static SDL_Renderer* _renderer;

	static void run();

	Game();
	~Game();
};



#endif //GAME_H
