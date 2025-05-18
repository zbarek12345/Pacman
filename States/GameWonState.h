//
// Created by Wiktor on 18.05.2025.
//

#ifndef GAMEWONSTATE_H
#define GAMEWONSTATE_H
#include <GameState.h>
#include <vector>

#include "UiElement.h"


class GameWonState : public GameState{
	std::vector<UiElement*> _children;
	int32_t _level;
public:
	GameWonState(SDL_Renderer *renderer, int32_t level);

	static void rerunLevel(void *arg);

	~GameWonState() override;

	void update() override;

	void render() override;

	void handleInput(SDL_Event &event, GameState *&nextState) override;
};



#endif //GAMEWONSTATE_H
