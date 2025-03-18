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
	SDL_Renderer* renderer;
	GameStates _state;
	GameState* _currentState;
	static int _speed;
	static int _volume;

	void run();

	Game();
	~Game();
};



#endif //GAME_H
