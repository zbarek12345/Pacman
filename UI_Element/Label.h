//
// Created by Wiktor on 09.04.2025.
//

#ifndef LABEL_H
#define LABEL_H
#include "UiElement.h"


class Label : public UiElement{
public:
	enum textPositioning {
		CENTER,
		LEFT,
		RIGHT,
	} ;

private:
	TTF_Font *_font;
	SDL_Color _color;
	std::string _text;
	int _fontSize;
	textPositioning _pos;

public:
	Label();
	Label(SDL_Rect rect);
	~Label();

	void render(SDL_Renderer* renderer) override;
	void update() override;
	void handleInput(SDL_Event &event) override;

	void setText(std::string text);
	void setTextColor(SDL_Color color);
	void setTextSize(int size);
	void setTextAlign(textPositioning textPostion);
	void setFont(TTF_Font* font);

};



#endif //LABEL_H
