//
// Created by Wiktor on 18.03.2025.
//

#ifndef MENUSTATE_H
#define MENUSTATE_H
#include <GameState.h>
#include <vector>
#include <UiElement.h>
#include <Image.h>
#include <Button.h>
#include <SDL_render.h>


class MenuState : public GameState{
public:
	std::vector<UiElement*> _children;
	explicit MenuState(SDL_Renderer* renderer);

	~MenuState() override;

	// Pure virtual methods for each state to implement
	void update() override;               // Update logic (e.g., animations)
	void render() override;               // Draw the state to the screen
	void handleInput(SDL_Event& event, GameState*& nextState) override;
};



#endif //MENUSTATE_H
