//
// Created by Wiktor on 18.03.2025.
//

#include "Image.h"

#include <SDL_image.h>

Image::Image():UiElement() {
	_texture = nullptr;
}

Image::Image(SDL_Rect rect):UiElement(rect) {
	_texture = nullptr;
}

Image::~Image() {
	SDL_DestroyTexture(_texture);
}

void Image::setTexture(SDL_Texture *texture) {
	SDL_DestroyTexture(_texture);
	_texture = texture;
}


void Image::render(SDL_Renderer *renderer) {
	SDL_RenderCopy(renderer, _texture, nullptr, &_coordinates);
}

void Image::update() {

}

void Image::handleInput(SDL_Event &event) {

}

