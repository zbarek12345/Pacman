//
// Created by Wiktor on 18.05.2025.
//

#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H
#include <GameState.h>
#include <Label.h>
#include <vector>

#include "UiElement.h"

class GameOverState : public GameState {
	std::vector<UiElement*> _children;
	int _level;

	class HighScore : public UiElement {
		friend class GameOverState;

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
	GameOverState(SDL_Renderer *renderer, int level, float time, int score);

	~GameOverState() override;

	static void rerunLevel(void* arg);

	void update() override;

	void render() override;

	void handleInput(SDL_Event &event, GameState *&nextState) override;
};
#endif //GAMEOVERSTATE_H
