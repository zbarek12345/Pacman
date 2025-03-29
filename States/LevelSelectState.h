//
// Created by Wiktor on 21.03.2025.
//

#ifndef LEVELSELECTSTATE_H
#define LEVELSELECTSTATE_H
#include <GameState.h>
#include <vector>

#include "Button.h"
#include "Image.h"


class LevelSelectState : public GameState{
	Image* _levelPreview;
	Button* _button,* _leftButton,* _rightButton;
	std::vector<std::string> _levels;
public:

	LevelSelectState(SDL_Renderer* renderer);

	void renderPreview();

	~LevelSelectState() override;

	void update() override;
	void render() override;

	void handleInput(SDL_Event& event, GameState*& nextState) override;
};



#endif //LEVELSELECTSTATE_H
