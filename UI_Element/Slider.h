//
// Created by Wiktor on 22.03.2025.
//

#ifndef SLIDER_H
#define SLIDER_H
#include "UiElement.h"


class Slider : public UiElement{
public:
	enum orientations {
		HORIZONTAL,
		VERTICAL
	};
private:
	orientations _orientation;
	int* _value;
	int _min, _max;
	bool _selected, _held;

public:

	Slider();
	explicit Slider(SDL_Rect rect);

	Slider(SDL_Rect rect, int min, int max);

	~Slider();

	void setOrientation(orientations orientation);

	void setValue(int value);

	int getValue();

	void setMinMax(int min, int max);

	void attachToValue(int* value);

	void render(SDL_Renderer* renderer) override;
	void update() override;
	void handleInput(SDL_Event& event) override;
};



#endif //SLIDER_H
