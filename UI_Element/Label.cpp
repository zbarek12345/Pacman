//
// Created by Wiktor on 09.04.2025.
//

#include "Label.h"

#include <algorithm>

Label::Label() : UiElement(){
	_font = nullptr;
	_text = "";
	_color = {};
	_pos = LEFT;
	_fontSize = 12;
}

Label::Label(SDL_Rect rect) :UiElement(rect){
	_font = nullptr;
	_text = "";
	_color = {};
	_pos = LEFT;
	_fontSize = 12;
}

Label::~Label() {
}

void Label::render(SDL_Renderer *renderer) {
	TTF_SetFontSize(_font, _fontSize);
	SDL_Surface* surface = TTF_RenderText_Solid(_font, _text.c_str(), _color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	int textureWidth, textureHeight;

	// Query the texture for its width and height
	if (SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight) != 0) {}
	SDL_Rect dst = _coordinates;
	if (textureWidth < dst.w || textureHeight < dst.h) {
		dst.y += (_coordinates.h - textureHeight)/2;
		dst.h = textureHeight;
		dst.w = textureWidth;
		switch (_pos) {
			case LEFT:
				dst.x += std::min(_coordinates.w - textureWidth, 5);
				break;
			case RIGHT:
				dst.x += std::max(_coordinates.w - textureWidth, 5);
				break;
			case CENTER:
				dst.x += (_coordinates.w - textureWidth)/2.0;
				break;
		}
	}

	SDL_RenderCopy(renderer, texture, nullptr, &dst);
	SDL_DestroyTexture(texture);
}

void Label::update() {
}

void Label::handleInput(SDL_Event &event) {
}

void Label::setText(std::string text) {
	_text = text;
}

void Label::setTextColor(SDL_Color color) {
	_color = color;
}

void Label::setTextSize(int size) {
	_fontSize = size;
}

void Label::setTextAlign(textPositioning textPostion) {
	_pos = textPostion;
}

void Label::setFont(TTF_Font *font) {
	_font = font;
}
