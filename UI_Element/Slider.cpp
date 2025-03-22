//
// Created by Wiktor on 22.03.2025.
//

#include "Slider.h"

Slider::Slider():UiElement() {
	_orientation = HORIZONTAL;
	_value = 0;
	_min = 0;
	_max = 0;
	_selected = false;
	_held = false;
}

Slider::Slider(SDL_Rect rect):UiElement(rect) {
	_orientation = HORIZONTAL;
	_value = 0;
	_min = 0;
	_max = 0;
	_selected = false;
	_held = false;
}

Slider::Slider(SDL_Rect rect, int min, int max) : UiElement(rect), _min(min), _max(max) {
	_orientation = HORIZONTAL;
	_value = 0;
	_selected = false;
	_held = false;
}

Slider::~Slider() {
}

void Slider::setOrientation(orientations orientation) {
	_orientation = orientation;
}

void Slider::setValue(int value) {
	_value = value;
}

void Slider::setMinMax(int min, int max) {
	_min = min;
	_max = max;
}

void Slider::render(SDL_Renderer *renderer) {

	int sliderSize = std::min( 10, (_max-_min)*(_orientation == VERTICAL ? _coordinates.h : _coordinates.w) / (20));

	SDL_SetRenderDrawColor(renderer, 120, 50, 0, 255);

	SDL_Rect barCoordinates = _coordinates;
	if (_orientation == VERTICAL) {
		int size = _coordinates.w;
		barCoordinates.w =(int) size*0.6;
		barCoordinates.x = _coordinates.x + (size - barCoordinates.w)/2;
	}

	else {
		int size = _coordinates.w;
		barCoordinates.h =(int) size*0.6;
		barCoordinates.y = _coordinates.y + (size - barCoordinates.h)/2;
	}

	SDL_RenderFillRect(renderer, &barCoordinates);

	SDL_SetRenderDrawColor(renderer, 120, 200, 0, 255);

	if (_orientation == VERTICAL) {
		barCoordinates.x = _coordinates.x;
		barCoordinates.w = _coordinates.w;
		barCoordinates.y = std::max(_coordinates.y + (_coordinates.h/(_max-_min)*(_value-_min)), _coordinates.y);
		barCoordinates.h = sliderSize;
	}

	else {
		barCoordinates.x = std::max(_coordinates.x + (_coordinates.w/(_max-_min)*(_value-_min))/2, _coordinates.x);
		barCoordinates.w = sliderSize;
		barCoordinates.y = _coordinates.y;
		barCoordinates.h = _coordinates.h;
	}
	SDL_RenderFillRect(renderer, &barCoordinates);
}

void Slider::update() {
}

void Slider::handleInput(SDL_Event &event) {

	if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (event.motion.x >= _coordinates.x && event.motion.x <= _coordinates.x + _coordinates.w && event.motion.y >= _coordinates.y && event.motion.y <= _coordinates.y + _coordinates.h) {
			_selected = true;
			if (_orientation == VERTICAL) {
				_value = (int) (((event.motion.y - _coordinates.y) / (float) _coordinates.h) * (_max - _min) + _min);
			}
			else {
				_value = (int) (((event.motion.x - _coordinates.x) / (float) _coordinates.w) * (_max - _min) + _min);
			}
			_held = true;
			printf("%d\n", _value);
		}
		else {
			_selected = false;
		}
	}
	else if (event.type == SDL_MOUSEMOTION && _selected && _held) {
		if (_orientation == VERTICAL) {
			_value = (int) (((event.motion.y - _coordinates.y) / (float) _coordinates.h) * (_max - _min) + _min);
		}
		else {
			_value = (int) (((event.motion.x - _coordinates.x) / (float) _coordinates.w) * (_max - _min) + _min);
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP) {
		_held = false;
	}
	else if (event.type ==SDL_KEYDOWN && _selected) {
		if (event.key.keysym.sym == SDLK_LEFT) {
			_value = std::max(_value-1, _min);
		}
		else if (event.key.keysym.sym == SDLK_RIGHT) {
			_value = std::min(_value+1, _max);
		}

		printf("%d\n", _value);
	}

}
