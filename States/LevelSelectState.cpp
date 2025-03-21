//
// Created by Wiktor on 21.03.2025.
//

#include "LevelSelectState.h"

#include <SDL_image.h>

#include "Image.h"

void dec_iterator();
void inc_iterator();

std::string buttons[2] = {"<-", "->"};

int _level, _elemSize;
LevelSelectState::LevelSelectState(SDL_Renderer* renderer):GameState(renderer) {
	_button = new Button();
	_leftButton = new Button();
	_rightButton = new Button();
	_leftButton->setCoordinatesf(0.2,0.4,0.1,0.1);
	_rightButton->setCoordinatesf(0.7,0.4,0.1,0.1);

	_button->setCoordinatesf(0.3,0.4,0.4,0.1);
	_button->setColor({0,0,0});
	_button->setBorderColor(255,255,255);
	_button->setBorderWidth(2);
	_button->setFontSize(25);

	_leftButton->setColor({0,0,0});
	_leftButton->setBorderColor(255,255,255);
	_leftButton->setBorderWidth(2);
	_leftButton->setFontSize(25);

	_rightButton->setColor({0,0,0});
	_rightButton->setBorderColor(255,255,255);
	_rightButton->setBorderWidth(2);
	_rightButton->setFontSize(25);
	_level = 0;
	_levels = {"level 1", "level 2", "level 3"};
	_button->setText(_levels[_level]);


	// SDL_Surface* temp = IMG_Load("../Textures/right_arrow.png");
	// _leftButton = new Button();
	// _leftButton->setTexture(SDL_CreateTextureFromSurface(_renderer, temp));
	// SDL_FreeSurface(temp);
	//
	// SDL_Surface* temp2 = IMG_Load("../Images/left_arrow.png");
	// _rightButton = new Button();
	// _rightButton->setTexture(SDL_CreateTextureFromSurface(_renderer, temp2));
	// SDL_FreeSurface(temp2);
	_rightButton->setFont(Game::_font);
	_leftButton->setFont(Game::_font);
	_rightButton->setFontSize(50);
	_leftButton->setFontSize(50);
	_rightButton->setText(buttons[1]);
	_leftButton->setText(buttons[0]);

	_button->setFont(Game::_font);
	_elemSize = _levels.size();
	_leftButton->onClick(inc_iterator);
	_rightButton->onClick(dec_iterator);
}
LevelSelectState::~LevelSelectState() {
	if (_button != nullptr)
		delete _button;
	SDL_DestroyTexture(_leftButton->getTexture());
	SDL_DestroyTexture(_rightButton->getTexture());
	delete _leftButton;
	delete _rightButton;
}

void LevelSelectState::update() {

}


void LevelSelectState::render() {
	_button->setText(_levels[_level]);
	_button->render(_renderer);
	_leftButton->render(_renderer);
	_rightButton->render(_renderer);
}


void inc_iterator() {
	_level = (_level+1)%_elemSize;
}

void dec_iterator() {
	_level = (_level+_elemSize-1)%_elemSize;
}

bool clicked = false;
void LevelSelectState::handleInput(SDL_Event& event, GameState*& nextState) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) {
			dec_iterator();
		}
		else if (event.key.keysym.sym == SDLK_RIGHT) {
			inc_iterator();
		}
	}
	_button->handleInput(event);
	_leftButton->handleInput(event);
	_rightButton->handleInput(event);
}