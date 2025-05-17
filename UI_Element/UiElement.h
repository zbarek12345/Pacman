//
// Created by Wiktor on 18.03.2025.
//

#ifndef UIELEMENT_H
#define UIELEMENT_H
#include <SDL_events.h>
#include <SDL_rect.h>
#include <Game.h>
class UiElement {
public:
	explicit UiElement() {
		_coordinates = {};
	}

	explicit UiElement(SDL_Rect rect) {
		_coordinates = rect;
	}

	virtual ~UiElement() =default;

	SDL_Rect _coordinates;

	/// Renders an UIelement object
	/// @param renderer Pointer to the sdl2 renderer
	virtual void render(SDL_Renderer* renderer) = 0;

	///Updates UIelement object
	virtual void update() = 0;

	///Enables input handling for the UIelement
	virtual void handleInput(SDL_Event& event) = 0;

	void setPosition(int x, int y) ;
	void setSize(int w, int h) ;
	[[nodiscard]] bool isInside(SDL_Point point) ;

	void setCoordinatesf(float x, float y, float w, float h);
	void setPositionf(float x, float y);
	void setSizef(float w, float h);

	SDL_Rect getCoordinates() const;
};

inline void UiElement::setPosition(int32_t x, int32_t y) {
	_coordinates.x = x;
	_coordinates.y = y;
}

inline void UiElement::setSize(int32_t w, int32_t h) {
	_coordinates.w = w;
	_coordinates.h = h;
}

inline bool UiElement::isInside(SDL_Point point){
	return point.x > _coordinates.x && point.x < _coordinates.x + _coordinates.w && point.y > _coordinates.y && point.y < _coordinates.y + _coordinates.h;
}

inline void UiElement::setCoordinatesf(float x, float y, float w, float h) {
	int x_size, y_size;
	SDL_GetWindowSize(Game::_window,&x_size,&y_size);
	SDL_Rect coords;
	coords.x = std::min(static_cast<int>(x * x_size),x_size);
	coords.y = std::min(static_cast<int>(y * y_size),y_size);
	coords.w = std::min(static_cast<int>(w * x_size),x_size-_coordinates.x);
	coords.h = std::min(static_cast<int>(h * y_size),y_size-_coordinates.y);
	_coordinates = coords;
}

inline void UiElement::setPositionf(float x, float y){
	int x_size, y_size;
	SDL_GetWindowSize(Game::_window,&x_size,&y_size);
	_coordinates.x = std::min(static_cast<int>(x * x_size),x_size);
	_coordinates.y = std::min(static_cast<int>(y * y_size),y_size);
	_coordinates.w = std::min((int)_coordinates.w,x_size-_coordinates.x);
	_coordinates.h = std::min((int)_coordinates.w,y_size-_coordinates.y);
}

inline void  UiElement::setSizef(float w, float h){
	int x_size, y_size;
	SDL_GetWindowSize(Game::_window,&x_size,&y_size);
	_coordinates.w = std::min(static_cast<int>(w * x_size),x_size-_coordinates.x);
	_coordinates.h = std::min(static_cast<int>(h * y_size),y_size-_coordinates.y);
}

inline SDL_Rect UiElement::getCoordinates() const {
	return _coordinates;
}
#endif //UIELEMENT_H
