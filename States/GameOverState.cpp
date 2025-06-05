//
// Created by Wiktor on 18.05.2025.
//

#include "GameOverState.h"

#include <Button.h>
#include <LevelSelectState.h>
#include <MenuState.h>

#include "Label.h"
#include "Panel.h"
#include "PlayState.h"

GameOverState::HighScore::HighScore(): UiElement(){
	_updatetime = SDL_GetTicks();
	_text = new Label();
	_text->setCoordinatesf(0.3,0.3,0.2,0.2);
	_text->setFont(Game::_font);
	_text->setTextSize(50);
	_text->setText("New High Score");
	_text->setTextColor({232, 0, 0});
	_text->setTextAlign(Label::CENTER);
	//printf("High Score Initialized");
}

GameOverState::HighScore::~HighScore() {
	delete _text;
}


void GameOverState::HighScore::render(SDL_Renderer* renderer) {
	_text->render(renderer);
}

void GameOverState::HighScore::handleInput(SDL_Event &event) {}

void GameOverState::HighScore::update() {}

GameOverState::GameOverState(SDL_Renderer *renderer, int level, float time, int score) : GameState(renderer){
	_renderer = renderer;
	_level = level;

	Panel* panel = new Panel;
	Label* name = new Label();
	Button* rerunLevelButton = new Button(), *levelSelectButton = new Button(), *mainMenuButton = new Button();
	SDL_Color normal = {137, 209, 54}, hover = {111, 191, 19}, pressed = {74, 138, 0};

	auto stats = Game::_databaseController->getLevel(level);
	if (stats.best_score < score) {
		hs = new HighScore();
		stats.best_score = score;
		Game::_databaseController->updateLevel(stats);
	}
	else
		hs = nullptr;

	#pragma region Button Predefinition
		rerunLevelButton->setFont(Game::_font);
		rerunLevelButton->setColor(normal);
		rerunLevelButton->setHoverColor(hover);
		rerunLevelButton->setOnClickColor(pressed);
		rerunLevelButton->setBorderWidth(2);
		rerunLevelButton->setFontSize(50);
		rerunLevelButton->setBorderColor({0,0,0,255});

		levelSelectButton->setFont(Game::_font);
		levelSelectButton->setColor(normal);
		levelSelectButton->setHoverColor(hover);
		levelSelectButton->setOnClickColor(pressed);
		levelSelectButton->setBorderWidth(2);
		levelSelectButton->setFontSize(50);
		levelSelectButton->setBorderColor({0,0,0,255});

		mainMenuButton->setFont(Game::_font);
		mainMenuButton->setColor(normal);
		mainMenuButton->setHoverColor(hover);
		mainMenuButton->setOnClickColor(pressed);
		mainMenuButton->setBorderWidth(2);
		mainMenuButton->setFontSize(50);
		mainMenuButton->setBorderColor({0,0,0,255});
	#pragma endregion

	rerunLevelButton->setText("Try Again");
	levelSelectButton->setText("Select Another Level");
	mainMenuButton->setText("Main Menu");

	name->setText("Game Over");
	name->setTextAlign(Label::CENTER);
	name->setFont(Game::_font);
	name->setTextColor({255,255,255});
	name->setTextSize(150);

	name->setCoordinatesf(0.25,0.225,0.5,0.2);
	rerunLevelButton->setCoordinatesf(0.3,0.45,0.4,0.1);
	levelSelectButton->setCoordinatesf(0.3,0.575,0.4,0.1);
	mainMenuButton->setCoordinatesf(0.3,0.7,0.4,0.1);

	rerunLevelButton->onClick(rerunLevel);
	rerunLevelButton->setArgument(this);
	levelSelectButton->onClick([](void* arg){_next = new LevelSelectState(Game::_renderer);});
	mainMenuButton->onClick([](void* arg){_next = new MenuState(Game::_renderer);});

	panel->setCoordinatesf(0.2,0.2,0.6,0.65);
	panel->setBackgroundColor({13, 105, 72});
	panel->setBorderWidth(10);
	panel->setBorderColor({33, 219, 0,255});
	panel->addChild(name);
	panel->addChild(rerunLevelButton);
	panel->addChild(levelSelectButton);
	panel->addChild(mainMenuButton);
	_children.push_back(panel);
	Game::_audioHandler->stopAllSounds();
	Game::_audioHandler->playSound(AudioHandler::CUTSCENE);
	//printf("Game Over Constructor");
}

GameOverState::~GameOverState() {
	for (auto& e : _children) {
		delete e;
	}
}

void GameOverState::rerunLevel(void *arg) {
	auto state = (GameOverState*)arg;
	_next = new PlayState(state->_level);
}

void GameOverState::update() {
	if (hs!=nullptr) {
		if (hs->_updatetime + 1e4 < SDL_GetTicks()) {
			//printf("High Score Deleted");
			delete hs;
			hs = nullptr;
		}
	}
}

void GameOverState::render() {
	for (auto& e : _children) {
		e->render(_renderer);
	}

	if (hs!=nullptr) {
		hs->render(_renderer);
	}
}

void GameOverState::handleInput(SDL_Event &event, GameState *&nextState) {
	for (auto& e : _children) {
		e->handleInput(event);
	}

	if (_next != nullptr) {
		GameState *cstate = nextState;
		nextState = _next;
		_next = nullptr;
		delete cstate;
	}
}
