//
// Created by Wiktor on 19.03.2025.
//

#include <GameState.h>
#include <vector>

#include <UiElement.h>

#include "ScrollableList.h"

class TestState:public GameState {
	std::vector<UiElement*> _children;
public:
	explicit TestState(SDL_Renderer* renderer):GameState(renderer) {
		ScrollableList* scrollable = new ScrollableList({0,0,0,0}, 20);
		scrollable->setCoordinatesf(0.1,0.1,0.8,0.8);
		std::vector<std::string> list = {"aaa", "bbb", "ccc", "ddd"};

		for (auto& item : list) {
			scrollable->addItem(item);
		}
		_children.push_back(scrollable);
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
