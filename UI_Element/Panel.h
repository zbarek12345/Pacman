//
// Created by Wiktor on 09.04.2025.
//

#ifndef PANEL_H
#define PANEL_H
#include <vector>

#include "UiElement.h"


class Panel : public UiElement {
	SDL_Color _backgroundColor, _borderColor;
	uint32_t _borderWidth = 0;
	std::vector<UiElement*> _children;

	public:
	Panel();
	Panel(SDL_Rect rect);
	~Panel();

	void addChild(UiElement* child);

	void render(SDL_Renderer* renderer)override;
	void update() override;
	void handleInput(SDL_Event& event) override;

	void setBackgroundColor(SDL_Color color);
	void setBorderColor(SDL_Color color);
	void setBorderWidth(uint32_t width);
};

#endif //PANEL_H
