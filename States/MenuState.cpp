//
// Created by Wiktor on 18.03.2025.
//

#include "MenuState.h"

#include <SDL_image.h>

#include "LevelSelectState.h"



MenuState::MenuState(SDL_Renderer *renderer) : GameState(renderer) {
	auto img = new Image();
	img->setCoordinatesf(0.2,0.1,0.6,0.4);
	auto Play = new Button();
	Play->setCoordinatesf(0.3,0.55,0.4,0.15);
	SDL_Color normal = {137, 209, 54}, hover = {111, 191, 19}, pressed = {74, 138, 0};
	auto Exit = new Button();
	Exit->setCoordinatesf(0.3,0.75,0.4,0.15);

	Play->setFont(Game::_font);Exit->setFont(Game::_font);

	Play->setColor(normal);Exit->setColor(normal);
	Play->setHoverColor(hover);Exit->setHoverColor(hover);
	Play->setOnClickColor(pressed);Exit->setOnClickColor(pressed);
	Play->setBorderWidth(2);Exit->setBorderWidth(2);
	Play->setText("Play");Exit->setText("Exit");
	Play->setFontSize(50);Exit->setFontSize(50);
	Play->setBorderColor({0,0,0,255});Exit->setBorderColor({0,0,0,255});
	SDL_Surface* surface = IMG_Load("../Textures/PAC-MAN-0.webp");
	img->setTexture(SDL_CreateTextureFromSurface(_renderer, surface));
	SDL_FreeSurface(surface);

	Play->onClick([](void* arg){_next = new LevelSelectState(Game::_renderer);});
	Exit->onClick([](void* arg){Game::_running = SDL_FALSE;});
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

	if (_next != nullptr){
		GameState *cstate = nextState;
		nextState = _next;
		_next = nullptr;
		delete cstate;
	}
}
