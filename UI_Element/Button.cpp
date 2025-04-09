//
// Created by Wiktor on 18.03.2025.
//

#include "Button.h"

Button::Button() : UiElement() {
	_color = {255, 255, 255};
	_baseColor = {255, 255, 255};
	_borderColor = {0, 0, 0};
	_hoverColor = {255, 255, 255};
	_clickedColor = {255, 255, 255};
	_borderWidth = 0;
	_texture = nullptr;
	_onClick = nullptr;
}

Button::Button(SDL_Rect rect): UiElement(rect) {
	_coordinates = rect;
	_color = {255, 255, 255};
	_baseColor = {255, 255, 255};
	_borderColor = {0, 0, 0};
	_hoverColor = {255, 255, 255};
	_clickedColor = {255, 255, 255};
	_borderWidth = 0;
	_texture = nullptr;
	_onClick = nullptr;
}

void Button::render(SDL_Renderer* renderer) {

	SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, 255);
	SDL_RenderFillRect(renderer, &_coordinates);

	///Draw the Border
	if (_borderWidth !=0) {
		SDL_SetRenderDrawColor(renderer, _borderColor.r, _borderColor.g, _borderColor.b, 255);
		SDL_Rect border = _coordinates;

		///Vertical Lines
		border.w = _borderWidth;
		SDL_RenderFillRect(renderer, &border);
		border.x = _coordinates.x + _coordinates.w - _borderWidth;
		SDL_RenderFillRect(renderer, &border);
		border = _coordinates;

		///Horizontal Lines
		border.h = _borderWidth;
		SDL_RenderFillRect(renderer, &border);
		border.y = _coordinates.y + _coordinates.h - _borderWidth;
		SDL_RenderFillRect(renderer, &border);
	}

	if (_font!= nullptr) {
		SDL_Color color = {255, 255, 255, 255}; // White text
		TTF_SetFontSize(_font,_fontSize);
		SDL_Surface* surface = TTF_RenderText_Solid(_font, _text.c_str(), color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		int textureWidth, textureHeight;

		// Query the texture for its width and height
		if (SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight) != 0) {
			//printf("Unable to query texture! SDL Error: %s\n", SDL_GetError());
		}

		int32_t textX = _coordinates.x + (_coordinates.w - textureWidth) / 2, textY = _coordinates.y + (_coordinates.h - textureHeight) / 2;
		SDL_Rect destination = {textX,textY, textureWidth, textureHeight};
		SDL_RenderCopy(renderer, texture, nullptr, &destination);
		SDL_DestroyTexture(texture);
	}

	///Draw the Texture
	if (_texture != nullptr) {
		SDL_RenderCopy(renderer, _texture, nullptr, &_coordinates);
	}
}

void Button::setFontSize(int i) {
	_fontSize = i;
}

void Button::onClick(void (*callback)(void* arg)) {
	_onClick = callback;
}

void Button::setText(std::string text) {
	_text = text;
}

bool operator!=(const SDL_Color & lhs, const SDL_Color & rhs) {
	return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b;
}

void Button::handleInput(SDL_Event& event) {
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && isInside({event.button.x, event.button.y})) {
		_color = _clickedColor;
		//_text = "clicked";
		if (_onClick != nullptr) _onClick(_arg);
	}
	else if (isInside({event.button.x, event.button.y})) {
		//_text = "hovered";
		_color = _hoverColor;
	}
	else {
		//_text = "normal";
		_color = _baseColor;
	}
}

void Button::setBorderWidth(int32_t width) {
	_borderWidth = width;
}

void Button::update() {

}

void Button::setFont(TTF_Font* font) {
	_font = font;
}

void Button::setColor(SDL_Color color) {
	_baseColor = color;
}

void Button::setColor(uint8_t r, uint8_t g, uint8_t b) {
	_baseColor = {r, g, b};
}

void Button::setBorderColor(uint8_t r, uint8_t g, uint8_t b) {
	_borderColor = {r, g, b};
}

void Button::setBorderColor(SDL_Color color) {
	_borderColor = color;
}

void Button::setHoverColor(uint8_t r, uint8_t g, uint8_t b) {
	_hoverColor = {r, g, b};
}

void Button::setHoverColor(SDL_Color color) {
	_hoverColor = color;
}

void Button::setOnClickColor(uint8_t r, uint8_t g, uint8_t b) {
	_clickedColor = {r, g, b};
}

void Button::setOnClickColor(SDL_Color color) {
	_clickedColor = color;
}

void Button::setTexture(SDL_Texture* texture) {
	_texture = texture;
}

SDL_Color Button::getColor() const {
	return _color;
}

SDL_Texture* Button::getTexture() const {
	return _texture;
}
