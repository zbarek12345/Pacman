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


Button::Button(const Button& other) : UiElement(other),
	_borderWidth(other._borderWidth),
	_color(other._color),
	_baseColor(other._baseColor),
	_borderColor(other._borderColor),
	_hoverColor(other._hoverColor),
	_clickedColor(other._clickedColor),
	_onClick(other._onClick),
	_arg(other._arg),
	_text(other._text),
	_fontSize(other._fontSize),
	_texture(nullptr),
	_font(nullptr)
{
	// Copy font (share or recreate)
	if (other._font) {
		// Option 1: Share the font (safe, as SDL_ttf manages reference counting)
		_font = other._font;
		// Option 2: Recreate font (if you want a deep copy, requires font path)
		// _font = TTF_OpenFont("path/to/font.ttf", _fontSize);
		// if (!_font) { throw std::runtime_error("Failed to copy font: " + std::string(TTF_GetError())); }
	}

	// Copy texture (recreate from text if possible)
	if (other._texture && _font && !_text.empty()) {
		SDL_Surface* surface = TTF_RenderText_Solid(_font, _text.c_str(), _color);
		if (surface) {
			_texture = SDL_CreateTextureFromSurface(Game::_renderer, surface); // Assumes Game::_renderer exists
			SDL_FreeSurface(surface);
			if (!_texture) {
				printf("Failed to copy texture: %s\n", SDL_GetError());
			}
		}
	}
}

Button& Button::operator=(const Button& other) {
	if (this == &other) {
		return *this; // Handle self-assignment
	}

	// Clean up existing resources
	if (_texture) {
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
	}
	if (_font && _font != other._font) {
		// Only close if not sharing the same font
		TTF_CloseFont(_font);
		_font = nullptr;
	}

	// Copy base class
	UiElement::operator=(other);

	// Copy members
	_borderWidth = other._borderWidth;
	_color = other._color;
	_baseColor = other._baseColor;
	_borderColor = other._borderColor;
	_hoverColor = other._hoverColor;
	_clickedColor = other._clickedColor;
	_onClick = other._onClick;
	_arg = other._arg;
	_text = other._text;
	_fontSize = other._fontSize;

	// Copy font
	if (other._font) {
		// Option 1: Share the font
		_font = other._font;
		// Option 2: Recreate font (requires font path)
		// _font = TTF_OpenFont("path/to/font.ttf", _fontSize);
		// if (!_font) { throw std::runtime_error("Failed to copy font: " + std::string(TTF_GetError())); }
	}

	// Copy texture
	if (other._texture && _font && !_text.empty()) {
		SDL_Surface* surface = TTF_RenderText_Solid(_font, _text.c_str(), _color);
		if (surface) {
			_texture = SDL_CreateTextureFromSurface(Game::_renderer, surface);
			SDL_FreeSurface(surface);
			if (!_texture) {
				printf("Failed to copy texture: %s\n", SDL_GetError());
			}
		}
	}

	return *this;
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
	_color = color;
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
