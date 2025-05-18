//
// Created by Wiktor on 18.05.2025.
//

#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H
#include <GameState.h>
#include <vector>

#include "UiElement.h"

class GameOverState : public GameState {
	std::vector<UiElement*> _children;
	int _level;
public:
	explicit GameOverState(SDL_Renderer *renderer, int level);

	~GameOverState() override;

	static void rerunLevel(void* arg);

	void update() override;

	void render() override;

	void handleInput(SDL_Event &event, GameState *&nextState) override;
};
#endif //GAMEOVERSTATE_H
