//
// Created by Wiktor on 18.03.2025.
//

#include "MenuState.h"

#include <SDL_image.h>


MenuState::MenuState(SDL_Renderer *renderer) : GameState(renderer) {
	auto img = new Image({0, 0, 670, 670});
	auto Play = new Button({600, 250, 100, 100});
	SDL_Color normal = {137, 209, 54}, hover = {111, 191, 19}, pressed = {74, 138, 0};
	auto Exit = new Button({600, 400, 100, 100});

	Play->setColor(normal);Exit->setColor(normal);
	Play->setHoverColor(hover);Exit->setHoverColor(hover);
	Play->setOnClickColor(pressed);Exit->setOnClickColor(pressed);
	Play->setBorderWidth(2);Exit->setBorderWidth(2);
	Play->setBorderColor({0,0,0,255});Exit->setBorderColor({0,0,0,255});
	SDL_Surface* surface = IMG_Load("../Textures/PAC-MAN-0.webp");
	img->setTexture(SDL_CreateTextureFromSurface(_renderer, surface));
	SDL_FreeSurface(surface);

	_children.push_back(img);
	_children.push_back(Play);
	_children.push_back(Exit);
}

MenuState::~MenuState() {
	for (auto& e: _children) {
		delete e;
	}
}

void MenuState::update(){}            // Update logic (e.g., animations)
void MenuState::render() {
	for (auto& e: _children) {
		e->render(_renderer);
	}
}              // Draw the state to the screen
void MenuState::handleInput(SDL_Event& event, GameState*& nextState) {
	for (auto& e: _children) {
		e->handleInput(event);
	}
}
