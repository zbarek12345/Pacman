//
// Created by Wiktor on 19.03.2025.
//

#include <GameState.h>
#include <vector>

#include <UiElement.h>

#include "ScrollableList.h"
#include "Slider.h"

class TestState:public GameState {
	std::vector<UiElement*> _children;
public:
	explicit TestState(SDL_Renderer* renderer):GameState(renderer) {
		SDL_Rect rect = {100,100,30,200};
		Slider* slider = new Slider(rect, 0 ,50);

		slider->setOrientation(Slider::VERTICAL);
		slider->setValue(3);

		_children.push_back(slider);
	}

	~TestState() override {
		for (auto& child : _children) {
			delete child;
		}
	}

	// Pure virtual methods for each state to implement
	void update() override{}               // Update logic (e.g., animations)
	void render() override {
		for (auto&e : _children)
			e->render(_renderer);
	}// Draw the state to the screen
	void handleInput(SDL_Event& event, GameState*& nextState) override {
		for (auto&e : _children)
			e->handleInput(event);
	}
};
