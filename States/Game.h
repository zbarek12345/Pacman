//
// Created by Wiktor on 18.03.2025.
//

#ifndef GAME_H
#define GAME_H
#include <GameState.h>
#include <SDL_ttf.h>
#include <DatabaseController.h>
#include <pthread.h>
#include <unistd.h>


class Game {
public:
	enum GameStates {
		MENU,
		LEVEL_SELECT,
		SETTINGS,
		PAUSE,
		GAME,
		GAME_OVER,
	};
	static DatabaseController* _databaseController;
	static GameStates _state;
	static GameState* _currentState;
	static int _speed;
	static int _volume;
	static TTF_Font* _font;
	static int _fps;
	static bool _render;
	static pthread_t _renderThread;

	Game(SDL_Window* window, SDL_Renderer* renderer);

	static void* Renderer(void* arg);
	static SDL_Window* _window;
	static SDL_Renderer* _renderer;

	static void run();

	Game();
	~Game();
};



#endif //GAME_H
