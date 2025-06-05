//
// Created by Wiktor on 18.05.2025.
//

#ifndef GAMEWONSTATE_H
#define GAMEWONSTATE_H
#include <GameState.h>
#include <vector>

#include "Label.h"
#include "UiElement.h"


class GameWonState : public GameState{
	std::vector<UiElement*> _children;
	int32_t _level;

	class HighScore : public UiElement {
		friend class GameWonState;

		Label* _text;
		int _updatetime;

		explicit HighScore();

		~HighScore() override;

		void update() override;

		void render(SDL_Renderer* renderer) override;

		void handleInput(SDL_Event &event) override;
	};

	HighScore* hs;

public:
	GameWonState(SDL_Renderer *renderer, int32_t level);

	static void rerunLevel(void *arg);

	GameWonState(SDL_Renderer *renderer, int32_t level, float bestTime, int bestScore);

	~GameWonState() override;

	void update() override;

	void render() override;

	void handleInput(SDL_Event &event, GameState *&nextState) override;
};



#endif //GAMEWONSTATE_H
