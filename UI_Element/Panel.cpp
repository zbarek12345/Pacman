//
// Created by Wiktor on 09.04.2025.
//

#include "Panel.h"

Panel::Panel() {
	_children = {};
	_backgroundColor = {0,0,0};
	_borderColor = {0,0,0};
	_borderWidth = 0;
}

Panel::Panel(SDL_Rect rect) : UiElement(rect) {
	_children = {};
	_backgroundColor = {0,0,0};
	_borderColor = {0,0,0};
	_borderWidth = 0;
}

Panel::~Panel() {
	for (auto& e : _children) {
		delete e;
	}
}

void Panel::addChild(UiElement *child) {
	_children.push_back(child);
}

void Panel::render(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, _backgroundColor.r, _backgroundColor.g, _backgroundColor.b, _backgroundColor.a);
	SDL_RenderFillRect(renderer, &_coordinates);

	SDL_SetRenderDrawColor(renderer, _borderColor.r, _borderColor.g, _borderColor.b, _borderColor.a);
	///Horizontal
	SDL_Rect borderRect = {_coordinates.x, _coordinates.y, _coordinates.w, (int)_borderWidth};
	SDL_RenderFillRect(renderer, &borderRect);
	borderRect.y = _coordinates.y +_coordinates.h -_borderWidth;
	SDL_RenderFillRect(renderer, &borderRect);

	///Vertical
	borderRect = {_coordinates.x, _coordinates.y, (int)_borderWidth, _coordinates.h};
	SDL_RenderFillRect(renderer, &borderRect);
	borderRect.x = _coordinates.x + _coordinates.w -_borderWidth;
	SDL_RenderFillRect(renderer, &borderRect);



	for (auto& e : _children) {
		e->render(renderer);
	}


}

void Panel::update() {
	for (auto& e : _children) {
		e->update();
	}
}

void Panel::handleInput(SDL_Event &event) {
	for (auto& e : _children) {
		e->handleInput(event);
	}
}

void Panel::setBackgroundColor(SDL_Color color) {
	_backgroundColor = color;
}

void Panel::setBorderColor(SDL_Color color) {
	_borderColor = color;
}

void Panel::setBorderWidth(uint32_t width) {
	_borderWidth = width;
}