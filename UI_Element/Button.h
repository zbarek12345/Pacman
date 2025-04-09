//
// Created by Wiktor on 18.03.2025.
//

#ifndef BUTTON_H
#define BUTTON_H
#include <GameState.h>
#include <SDL_events.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>

#include "UiElement.h"

class Button : public UiElement {
	uint8_t _borderWidth = 1;
	SDL_Texture* _texture;
	SDL_Color _color{}, _baseColor{}, _borderColor{}, _hoverColor{}, _clickedColor{};
	void(*_onClick)(void* arg) = nullptr;
	void* _arg = nullptr;
	std::string _text;
	int16_t _fontSize = 16;
	TTF_Font *_font;


public:
	Button();

	Button(SDL_Rect rect);

	~Button() = default;

	void render(SDL_Renderer* renderer) override;

	void onClick(void (*callback)(void * arg));

	void setText(std::string text);

	void handleInput(SDL_Event &event) override;

	void setBorderWidth(int32_t width);

	void update() override;

	void setFont(TTF_Font *font);

	void setCoordinates(int x, int y, int w, int h);
	void setColor(SDL_Color color);

	void setColor(Uint8 r, Uint8 g, Uint8 b);

	void setBorderColor(Uint8 r, Uint8 g, Uint8 b);

	void setBorderColor(SDL_Color color);

	void setHoverColor(Uint8 r, Uint8 g, Uint8 b);

	void setHoverColor(SDL_Color color);

	void setOnClickColor(Uint8 r, Uint8 g, Uint8 b);

	void setOnClickColor(SDL_Color color);

	void setArgument(void *arg);

	void setTexture(SDL_Texture* texture);
	SDL_Rect getCoordinates() const;
	SDL_Color getColor() const;
	SDL_Texture* getTexture() const;
	void setFontSize(int i);
};

inline void Button::setArgument(void* arg) {
	_arg = arg;
}
#endif //BUTTON_H
