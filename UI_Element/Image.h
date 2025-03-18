//
// Created by Wiktor on 18.03.2025.
//

#ifndef IMAGE_H
#define IMAGE_H
#include <SDL_render.h>
#include "UiElement.h"

class Image: public UiElement {

	SDL_Texture* _texture;
public:
	Image();
	explicit Image(SDL_Rect rect);

	~Image();

	void setTexture(SDL_Texture* texture);

	void render(SDL_Renderer* renderer) override;

	void update() override;

	virtual void handleInput(SDL_Event& event) override;
};
#endif //IMAGE_H
