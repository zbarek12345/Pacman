//
// Created by Wiktor on 18.03.2025.
//

#ifndef UIELEMENT_H
#define UIELEMENT_H
#include <SDL_events.h>
#include <SDL_rect.h>

class UiElement {
public:
	explicit UiElement() {
		_coordinates = {};
	}

	explicit UiElement(SDL_Rect rect) {
		_coordinates = rect;
	}
	virtual ~UiElement() = default;

	SDL_Rect _coordinates = {};

	virtual void render(SDL_Renderer* renderer) = 0;
	virtual void update() = 0;
	virtual void handleInput(SDL_Event& event) = 0;

	void setPosition(int x, int y);
	void setSize(int w, int h);
	[[nodiscard]] bool isInside(SDL_Point point) const;
};

inline void UiElement::setPosition(int32_t x, int32_t y) {
	_coordinates.x = x;
	_coordinates.y = y;
}

inline void UiElement::setSize(int32_t w, int32_t h) {
	_coordinates.w = w;
	_coordinates.h = h;
}

inline bool UiElement::isInside(SDL_Point point) const {
	return point.x > _coordinates.x && point.x < _coordinates.x + _coordinates.w && point.y > _coordinates.y && point.y < _coordinates.y + _coordinates.h;
}
#endif //UIELEMENT_H
