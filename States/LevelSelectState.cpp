//
// Created by Wiktor on 21.03.2025.
//

#include "LevelSelectState.h"

#include <SDL_image.h>

#include "Image.h"
#include "PlayState.h"
#include "../TileRender.h"

void dec_iterator(void* arg);
void inc_iterator(void* arg);
void load_level(void* arg);


std::string buttons[2] = {"<-", "->"};

int _level, _previousLevel, _elemSize;
LevelSelectState::LevelSelectState(SDL_Renderer* renderer):GameState(renderer) {
	_levelPreview = new Image();
	_button = new Button();
	_leftButton = new Button();
	_rightButton = new Button();
	_leftButton->setCoordinatesf(0.2,0.8,0.1,0.1);
	_rightButton->setCoordinatesf(0.7,0.8,0.1,0.1);
	_button->setCoordinatesf(0.3,0.8,0.4,0.1);
	_levelPreview->setCoordinatesf(0.32222222,0.273333,0.35555555555,0.42666666666);

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
	_previousLevel = 0;

	_levels = Game::_databaseController->getLevels();
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

	_leftButton->onClick(dec_iterator);
	_rightButton->onClick(inc_iterator);
	_button->onClick(load_level);
	renderPreview();
}

void LevelSelectState::renderPreview() {
	SDL_Texture *texture = SDL_CreateTexture(Game::_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 768, 768);
	SDL_SetRenderTarget(Game::_renderer, texture);
	SDL_SetRenderDrawColor(Game::_renderer, 0, 0, 0, 255); // Set clear color to black (or other desired color)
	SDL_RenderClear(Game::_renderer);
	TileRender* tl = new TileRender();
	tl->LoadTexture(Game::_renderer, "../Textures/wallTiles_white.png");
	tl->readMapString(Game::_databaseController->getLevel(_level+1).map);
	tl->renderMap(Game::_renderer);

	_levelPreview->setTexture(texture);

	SDL_SetRenderTarget(Game::_renderer, nullptr);
	delete tl;
}

LevelSelectState::~LevelSelectState() {
	if (_button != nullptr)
		delete _button;
	SDL_DestroyTexture(_leftButton->getTexture());
	SDL_DestroyTexture(_rightButton->getTexture());
	delete _leftButton;
	delete _rightButton;
	delete _levelPreview;
}

void LevelSelectState::update() {

}


void LevelSelectState::render() {
	_button->setText(_levels[_level]);
	_button->render(_renderer);
	_leftButton->render(_renderer);
	_rightButton->render(_renderer);
	_levelPreview->render(_renderer);
	if (_level != _previousLevel) {
		renderPreview();
		_previousLevel = _level;
	}
}


void inc_iterator(void* arg) {
	_level = (_level+1)%_elemSize;
}

void dec_iterator(void* arg) {
	_level = (_level+_elemSize-1)%_elemSize;
}

void load_level(void* arg) {
	GameState::_next = new PlayState(_level+1);
}

bool clicked = false;
void LevelSelectState::handleInput(SDL_Event& event, GameState*& nextState) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) {
			dec_iterator(nullptr);
		}
		else if (event.key.keysym.sym == SDLK_RIGHT) {
			inc_iterator(nullptr);
		}
	}
	_button->handleInput(event);
	_leftButton->handleInput(event);
	_rightButton->handleInput(event);

	if (_next != nullptr){
		GameState *cstate = nextState;
		nextState = _next;
		_next = nullptr;
		delete cstate;
	}
}