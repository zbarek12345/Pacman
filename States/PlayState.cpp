//
// Created by Wiktor on 29.03.2025.
//

#include "PlayState.h"

#include <Button.h>

#include "../TileRender.h"
#include "Game.h"
#include <SDL2/SDL_timer.h>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <MenuState.h>

#include "GameOverState.h"
#include "GameWonState.h"
#include "Label.h"

PlayState::GameElement::GameElement(int level) : UiElement() {

	_mapTexture = SDL_CreateTexture(Game::_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 768, 768);
	SDL_SetRenderTarget(Game::_renderer, _mapTexture);
	SDL_SetRenderDrawColor(Game::_renderer, 0, 0, 0, 255); // Set clear color to black (or other desired color)
	SDL_RenderClear(Game::_renderer);
	auto* tl = new TileRender();
	tl->LoadTexture(Game::_renderer, "../Textures/wallTiles.png");
	std::string levelmap = Game::_databaseController->getLevel(level).map;
	tl->readMapString(levelmap);
	tl->renderEmptyMap(Game::_renderer);

	SDL_SetRenderTarget(Game::_renderer, nullptr);
	delete tl;

	_points = IMG_LoadTexture(Game::_renderer, "../Textures/tileset.png");

	_map = new Map();

	_map->readMapString(levelmap);

	_player = new Player(_map->getPlayerRespawn(), this);
	previousTime = -1;

	_ghosts = new Entity*[4];
	Coordinates ghostResp = _map->getGhostRespawn();
	ghostResp.y-=1;
	_ghosts[0] = new Entity(this, ghostResp, Entity::Blinky);
	ghostResp.y +=2;
	_ghosts[1] = new Entity(this, ghostResp, Entity::Inky );
	ghostResp.x ++;
	_ghosts[2] = new Entity(this, ghostResp, Entity::Pinky);
	ghostResp.y ++;
	_ghosts[3] = new Entity(this, ghostResp, Entity::Clyde);

	_level = level;
	_globalEntityState = 0;
	_globalEntityStateTime = SDL_GetTicks() + 1000;
	_score = 0;
	_startTime = SDL_GetTicks();
	_gameState = Running;
	_lives = 3;
}

PlayState::GameElement::GameElement(SDL_Rect rect, int level):UiElement(rect){
	_mapTexture = SDL_CreateTexture(Game::_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 768, 768);
	SDL_SetRenderTarget(Game::_renderer, _mapTexture);
	SDL_SetRenderDrawColor(Game::_renderer, 0, 0, 0, 255); // Set clear color to black (or other desired color)
	SDL_RenderClear(Game::_renderer);
	auto* tl = new TileRender();
	tl->LoadTexture(Game::_renderer, "../Textures/wallTiles_white.png");
	std::string levelmap = Game::_databaseController->getLevel(level).map;
	tl->readMapString(levelmap);
	tl->renderEmptyMap(Game::_renderer);

	SDL_SetRenderTarget(Game::_renderer, nullptr);
	delete tl;

	_points = IMG_LoadTexture(Game::_renderer, "../Textures/tileset.png");

	_map = new Map();
	_map->readMapString(levelmap);
	_player = new Player(_map->getPlayerRespawn(), this);
	previousTime = -1;
	_startTime = SDL_GetTicks();
	_globalEntityState = 0;
	_level = level;
	_globalEntityState = 0;
	_globalEntityStateTime = SDL_GetTicks() + 1e4;
	_score = 0;
	_startTime = SDL_GetTicks();
	_gameState = Running;
	_lives = 3;
}

PlayState::GameElement::~GameElement() {
	for(int i =0;i<4;i++)
	   delete _ghosts[i];
	delete[] _ghosts;
	delete _player;
	delete _map;
	SDL_DestroyTexture(_mapTexture);
	SDL_DestroyTexture(_points);
}

void PlayState::GameElement::render(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderCopy(renderer, _mapTexture, nullptr, &_mapRect);

	SDL_Rect pointTexture = {32,0,16,16};
	SDL_Rect superPointTexture = {48,0,16,16};

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (!_map->isWall(j, i)) {
				SDL_Rect dst = _mapRect;
				dst.x += j * _tileSize;
				dst.y += i * _tileSize;
				dst.w = _tileSize;
				dst.h = _tileSize;
				if (_map->isPellet(j, i))
					SDL_RenderCopy(renderer, _points, &superPointTexture, &dst);
				else if (_map->isPoint(j, i))
					SDL_RenderCopy(renderer, _points, &pointTexture, &dst);
			}
		}
	}

	_player->render(renderer);
	for(int i =0;i<4;i++)
	   _ghosts[i]->render(renderer);
	SDL_SetRenderDrawColor(renderer, 120,23,45,255);
	SDL_RenderDrawRect(renderer, &_coordinates);
}

void PlayState::GameElement::update() {
	uint64_t delta = SDL_GetPerformanceCounter() - previousTime;
	if (previousTime == -1) {
		delta = 0;
	}

	if (_gameState == Running) {
		_player->updatePosition(delta);
		verifyCollision();
	}

	if (SDL_GetTicks() > _globalEntityStateTime) {
		_globalEntityState = (_globalEntityState+1)%2;
		_globalEntityState == 0
			? _globalEntityStateTime = SDL_GetTicks() + (uint32_t) 1e4
			: _globalEntityStateTime = SDL_GetTicks() + (uint32_t) 2e4;
        if(_globalEntityState == 1)
            Game::_audioHandler->playSound(AudioHandler::GHOST_SIREN);

		for (int i =0;i<4;i++) {
			_ghosts[i]->setState((Entity::state)_globalEntityState);
		}
	}
	previousTime = SDL_GetPerformanceCounter();
}

void PlayState::GameElement::handleInput(SDL_Event &event) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
			_player->setDirection(Up);
		}else if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
			_player->setDirection(Down);
		}else if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
			_player->setDirection(Left);
		}else if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
			_player->setDirection(Right);
		}
	}
}

void PlayState::GameElement::calculateMap() {
	int addSize = 32*4;int i = 1;
	while (i*addSize<_coordinates.w && i*addSize<_coordinates.h) {i++;}
	if (i*addSize>_coordinates.w || i*addSize>_coordinates.h){i = i-1 - (i%2 == 0);}
	printf("Val : %d\n", i);
	_tileSize = i*4;
	printf("%d\n", _tileSize);
	int mapSize = i*addSize;
	_mapRect.x = _coordinates.x + (_coordinates.w - mapSize)/2;
	_mapRect.y = _coordinates.y + (_coordinates.h - mapSize)/2;
	_mapRect.w = mapSize;
	_mapRect.h = mapSize;
}

void PlayState::GameElement::restart() {
	_map->readMapString(Game::_databaseController->getLevel(_level).map);
	restartPostions();
}

void PlayState::GameElement::restartPostions() {
	_globalEntityState = 0;
	_globalEntityStateTime = SDL_GetTicks() + (uint32_t) 1e4;
	_player->resetPosition();
	for (int i =0;i<4;i++) {
		if (_globalEntityState == 1)
			Game::_audioHandler->playSound(AudioHandler::GHOST_SIREN);
		_ghosts[i]->setState((Entity::state)_globalEntityState);
		_ghosts[i]->resetPosition();
	}
}

void PlayState::GameElement::verifyCollision() {

	auto playerCoord = _player->getPosition();

	if (_map->isPoint(round(playerCoord.x), round(playerCoord.y))) {
		_score+=10;
		_map->removeCollectible(round(playerCoord.x), round(playerCoord.y));
		Game::_audioHandler->playSound(AudioHandler::WAKA_WAKA);
	}
	else if (_map->isPellet(round(playerCoord.x), round(playerCoord.y))) {
		_score+=50;
		_map->removeCollectible(round(playerCoord.x), round(playerCoord.y));
		Game::_audioHandler->playSound(AudioHandler::POWER_UP);

		#pragma loop(unroll(4))
		for (int i =0;i<4;i++) {
			_ghosts[i]->setState(Entity::Terrified);
		}
	}

	for (int i =0;i<4;i++) {
		auto ghostCoord = _ghosts[i]->getPosition();

		if (sqrt(pow(ghostCoord.x - playerCoord.x,2) + pow(ghostCoord.y - playerCoord.y,2))<0.5) {
			auto ghoststate = _ghosts[i]->getState();
			if (ghoststate== Entity::Terrified)
				_ghosts[i]->getEaten();
			else if ((ghoststate == Entity::Chasing||ghoststate == Entity::Normal)&& !_player->isDead()) {
				_lives--;
				_player->Kill();
				Game::_audioHandler->playSound(AudioHandler::PAC_MAN_DEATH);
			}
		}
	}
}

PlayState::StopMenu::StopMenu(PlayState *element) {
	_playState = element;
	_panel = new Panel();
	_panel->setCoordinatesf(0.25,0.25,0.5, 0.5);
	_panel->setBorderColor({186, 168, 2});
	_panel->setBorderWidth(10);

	Label* pause_label = new Label();
	pause_label->setCoordinatesf(0.25,0.25,0.5, 0.20);
	pause_label->setText("Pause Menu");
	pause_label->setFont(Game::_font);
	pause_label->setTextColor({186, 168, 2});
	pause_label->setTextAlign(Label::CENTER);
	pause_label->setTextSize(50);

	Button* cont = new Button();
	cont->setCoordinatesf(0.3,0.45, 0.4, 0.1);
	cont->setColor({186, 168, 2});
	cont->setFont(Game::_font);
	cont->setText("Continue");
	cont->setArgument(_playState);
	cont->onClick([](void* arg) {
		PlayState* element = (PlayState*)arg;
		element->Start();
	});

	Button* mainMenu = new Button();
	mainMenu->setCoordinatesf(0.3,0.6, 0.4, 0.1);
	mainMenu->setColor({186, 168, 2});
	mainMenu->setFont(Game::_font);
	mainMenu->setText("Main Menu");
	mainMenu->onClick([](void* arg) {_next = new MenuState(Game::_renderer);});

	_panel->addChild(pause_label);
	_panel->addChild(cont);
	_panel->addChild(mainMenu);
}

PlayState::StopMenu::~StopMenu() {
	delete _panel;
}

void PlayState::StopMenu::render(SDL_Renderer *renderer) {
	_panel->render(renderer);
}

void PlayState::StopMenu::update() {
	_panel->update();
}

void PlayState::StopMenu::handleInput(SDL_Event &event) {
	_panel->handleInput(event);
}

PlayState::PlayState(int32_t level): GameState(Game::_renderer) {
	_game = new GameElement(level);
	_game->setCoordinatesf(0.1, 0.1, 0.8, 0.8);
	_game->calculateMap();
	_next = nullptr;
	this->_game->_gameState = GameElement::Running;
	_stopMenu = new StopMenu(this);

	_timeLabel = new Label();
	_timeLabel->setCoordinatesf(0.05, 0.05, 0.2, 0.05);
	_timeLabel->setText("Time: ");
	_timeLabel->setFont(Game::_font);
	_timeLabel->setTextColor({186, 168, 2});
	_timeLabel->setTextAlign(Label::LEFT);
	_timeLabel->setTextSize(20);

	_scoreLabel = new Label();
	_scoreLabel->setCoordinatesf(0.05, 0.1, 0.2, 0.05);
	_scoreLabel->setText("Score: ");
	_scoreLabel->setFont(Game::_font);
	_scoreLabel->setTextColor({186, 168, 2});
	_scoreLabel->setTextAlign(Label::LEFT);
	_scoreLabel->setTextSize(20);

	_livesLabel = new Label();
	_livesLabel->setCoordinatesf(0.05, 0.15, 0.2, 0.05);
	_livesLabel->setText("Lives: ");
	_livesLabel->setFont(Game::_font);
	_livesLabel->setTextColor({186, 168, 2});
	_livesLabel->setTextAlign(Label::LEFT);
	_livesLabel->setTextSize(20);
	Game::_audioHandler->stopAllSounds();
}

PlayState::~PlayState() {
	delete _game;
	delete _stopMenu;
}

void PlayState::update() {
	_scoreLabel->setText("Score : " + std::to_string(_game->_score));
	int32_t ts = (SDL_GetTicks() - _game->_startTime)/1e3;
	_timeLabel->setText("Time : " + std::to_string(int32_t(ts/60)) + ":" + std::to_string((ts%60)));
	_livesLabel->setText("Lives : " + std::to_string(_game->_lives));
	_game->update();

	if (_game->_map->getCollectibleCount()==0) {
		uint32_t seconds = floor((SDL_GetTicks() - _game->_startTime)/1e3);
		uint8_t minutes = seconds/60;
		seconds = seconds%60;
		_next = new GameWonState(Game::_renderer, _game->_level,minutes + seconds*0.01, _game->_score);
		SDL_Event event;
		event.type = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_ESCAPE;
		SDL_PushEvent(&event);
	}
	else if (_game->_lives == 0 && !_game->_player->isDead()) {
		uint32_t seconds = floor((SDL_GetTicks() - _game->_startTime)/1e3);
		uint8_t minutes = seconds/60;
		seconds = seconds%60;
		_next = new GameOverState(Game::_renderer, _game->_level,minutes + seconds*0.01, _game->_score);
		SDL_Event event;
		event.type = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_ESCAPE;
		SDL_PushEvent(&event);
	}

}

void PlayState::render() {
	if (_next != nullptr) {
		_next->render();
		return;
	}
	SDL_RenderClear(Game::_renderer);

	_game->render(Game::_renderer);
	_scoreLabel->render(Game::_renderer);
	_timeLabel->render(Game::_renderer);
	_livesLabel->render(Game::_renderer);

	if (_game->_gameState == GameElement::Paused) {
		_stopMenu->render(Game::_renderer);
	}
}

void PlayState::handleInput(SDL_Event &event, GameState *&nextState) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			Stop();
		}
	}

	if (_game->_gameState == GameElement::Paused) {
		_stopMenu->handleInput(event);
	}
	else {
		_game->handleInput(event);
	}

	if (_next != nullptr) {
		auto cstate = nextState;
		nextState = _next;
		delete cstate;
		_next = nullptr;
	}
}

void PlayState::Stop() {
	_game->_gameState = GameElement::Paused;
}

void PlayState::Start() {
	_game->_gameState = GameElement::Running;
	_game->previousTime = SDL_GetPerformanceCounter();
}

void PlayState::GameElement::Entity::updateTarget(){
    switch (_ghost) {
        case Blinky:
            BlinkyTargeting();
        break;
        case Inky:
            InkyTargeting();
        break;
        case Pinky:
            PinkyTargeting();
        break;
        case Clyde:
            ClydeTargeting();
        break;
    }
}

bool PlayState::GameElement::Entity::isWall(int x, int y){
    return _gameElement->_map->isWall(x,y) || x== 0 || x==31 || y== 0 || y==31 ;
}

bool PlayState::GameElement::Entity::isCrossroad(int x, int y){
    return (isWall(x-1, y) + isWall(x+1, y)+isWall(x, y+1) + isWall(x, y-1))<2;
}

void PlayState::GameElement::Entity::updateDirection(int x, int y) {
	if (_state == Terrified) {
		std::vector<direction> directions;
		if (!isWall(x - 1, y)) directions.push_back(Left);
		if (!isWall(x + 1, y)) directions.push_back(Right);
		if (!isWall(x, y - 1)) directions.push_back(Up);
		if (!isWall(x, y + 1)) directions.push_back(Down);
		_direction = directions[rand()%directions.size()];
		return;
	}
    Coordinates nextPos = _position;
    if(_direction == Up)
        nextPos.y -=1;
    else if(_direction== Down)
        nextPos.y +=1;
    else if(_direction == Right)
        nextPos.x+=1;
    else
        nextPos.x-=1;
    if(isCrossroad(x, y) || isWall(nextPos.x, nextPos.y)){
        double costs[4];
        costs[Up] = isWall(_position.x, _position.y-1) ? 1e8:sqrt((pow(_position.x - _target.x,2) +
            pow(_position.y-1 - _target.y,2)));
        costs[Down] = isWall(_position.x, _position.y+1) ? 1e8:sqrt((pow(_position.x - _target.x,2) +
            pow(_position.y+1 - _target.y,2)));
        costs[Left] = isWall(_position.x-1, _position.y) ? 1e8:sqrt((pow(_position.x - 1 - _target.x,2) +
            pow(_position.y - _target.y,2)));
        costs[Right] = isWall(_position.x+1, _position.y) ? 1e8:sqrt((pow(_position.x+1 - _target.x,2) +
            pow(_position.y - _target.y,2)));

        if(_direction%2==0)
            costs[_direction+1] = 1e8;
        else
            costs[_direction-1] = 1e8;
        double bestCost = 1e8;

        for(int i =0;i<4;i++){
            //if(_ghost == Blinky)
                //printf("%d : %f\n", i, costs[i]);
            if(costs[i]<bestCost){
                _direction = (direction)i;
                bestCost = costs[i];
            }
        }
        fflush(stdout);
    }
}

void* PlayState::GameElement::Entity::ghostThread(void* arg){
	auto* entity = (Entity*)arg;
	auto previous = SDL_GetPerformanceCounter();
	while(entity->_thread){
		auto delta = SDL_GetPerformanceCounter() - previous;

		if(entity->_gameElement->_gameState == Running)
			entity->updatePosition(delta);
		previous = SDL_GetPerformanceCounter();
		SDL_Delay(20);
	}
	return nullptr;
}

PlayState::GameElement::Entity::Entity(GameElement *gameElement,Coordinates spawn, ghostType ghost) {
	_gameElement = gameElement;
	_spawn = spawn;
	_position = spawn;
	_ghost = ghost;
	//printf("Start %d : %f %f\n", ghost, _position.x, _position.y);
	_previous = _spawn;
	_released = false;
	_direction = Up;
	updateDirection(_spawn.x, _spawn.y);
	_state = Chasing;
	_texture = nullptr;
	_thread = true;
	switch(_ghost){
	    case Blinky:
		    _texture = IMG_LoadTexture(Game::_renderer, "../Textures/blinky.png");
			_releaseTime = SDL_GetTicks();
		break;
		case Inky:
            _texture = IMG_LoadTexture(Game::_renderer,"../Textures/inky.png");
			_releaseTime = SDL_GetTicks() + 2000;
        break;
        case Pinky:
			_texture = IMG_LoadTexture(Game::_renderer,"../Textures/pinky.png");
			_releaseTime = SDL_GetTicks() + 4000;
		break;
		case Clyde:
            _texture = IMG_LoadTexture(Game::_renderer,"../Textures/clyde.png");
			_releaseTime = SDL_GetTicks() + 6000;
        break;
	}
	if(!_texture)
	   printf("Texture did not load properly");
	//printf("%s", IMG_GetError());
	//UP
	_directions[0] = {};
	_directions[0].frames = new SDL_Rect[2];
	_directions[0].frames[0] = {6*24,0,24,24};
	_directions[0].frames[1] = {7*24,0,24,24};
	_directions[0].currentFrame = 0;
	_directions[0].frameCount = 2;
	_directions[0].frameChangeInterval = 500;
	_directions[0].lastFrameChange = 0;

	//printf("Test: %d %d", _directions[0].frames[0].x, _directions[0].frames[1].x);
	//Down
	_directions[1] = {};
	_directions[1].frames = new SDL_Rect[2];
	_directions[1].frames[0] = {2*24,0,24,24};
	_directions[1].frames[1] = {3*24,0,24,24};
	_directions[1].currentFrame = 0;
	_directions[1].frameCount = 2;
	_directions[1].frameChangeInterval = 500;
	_directions[1].lastFrameChange = 0;

	//Left
	_directions[2] = {};
	_directions[2].frames = new SDL_Rect[2];
	_directions[2].frames[0] = {4*24,0,24,24};
	_directions[2].frames[1] = {5*24,0,24,24};
	_directions[2].currentFrame = 0;
	_directions[2].frameCount = 2;
	_directions[2].frameChangeInterval = 500;
	_directions[2].lastFrameChange = 0;

	//Right
	_directions[3] = {};
	_directions[3].frames = new SDL_Rect[2];
	_directions[3].frames[0] = {0*24,0,24,24};
	_directions[3].frames[1] = {1*24,0,24,24};
	_directions[3].currentFrame = 0;
	_directions[3].frameCount = 2;
	_directions[3].frameChangeInterval = 500;
	_directions[3].lastFrameChange = 0;

	//UP
	_deadDirections[0] = {};
	_deadDirections[0].frames = new SDL_Rect[1];
	_deadDirections[0].frames[0] = {15*24,0,24,24};
	_deadDirections[0].currentFrame = 0;
	_deadDirections[0].frameCount = 1;
	_deadDirections[0].frameChangeInterval = 500;
	_deadDirections[0].lastFrameChange = 0;

	//Down
	_deadDirections[1] = {};
	_deadDirections[1].frames = new SDL_Rect[1];
	_deadDirections[1].frames[0] = {13*24,0,24,24};
	_deadDirections[1].currentFrame = 0;
	_deadDirections[1].frameCount = 1;
	_deadDirections[1].frameChangeInterval = 500;
	_deadDirections[1].lastFrameChange = 0;

	//Left
	_deadDirections[2] = {};
	_deadDirections[2].frames = new SDL_Rect[1];
	_deadDirections[2].frames[0] = {14*24,0,24,24};
	_deadDirections[2].currentFrame = 0;
	_deadDirections[2].frameCount = 1;
	_deadDirections[2].frameChangeInterval = 500;
	_deadDirections[2].lastFrameChange = 0;

	//Right
	_deadDirections[3] = {};
	_deadDirections[3].frames = new SDL_Rect[1];
	_deadDirections[3].frames[0] = {12*24,0,24,24};
	_deadDirections[3].currentFrame = 0;
	_deadDirections[3].frameCount = 1;
	_deadDirections[3].frameChangeInterval = 500;
	_deadDirections[3].lastFrameChange = 0;

	_terrified = {};
	_terrified.frames = new SDL_Rect[4];
	_terrified.frames[0] = {8*24,0,24,24};
	_terrified.frames[1] = {9*24,0,24,24};
	_terrified.frames[2] = {10*24,0,24,24};
	_terrified.frames[3] = {11*24,0,24,24};
	_terrified.currentFrame = 0;
	_terrified.frameCount = 4;
	_terrified.frameChangeInterval = 500;
	_terrified.lastFrameChange = 0;

	pthread_create(&_ghostThread, nullptr, ghostThread, this);
}

PlayState::GameElement::Entity::~Entity() {
	_thread = false;
	pthread_join(_ghostThread, nullptr);
	for (int i = 0; i < 4; i++) {
		delete[] _directions[i].frames;
	}
	for (int i = 0; i < 4; i++) {
		delete[] _deadDirections[i].frames;
	}
	delete[] _terrified.frames;
	SDL_DestroyTexture(_texture);
}

void PlayState::GameElement::Entity::render(SDL_Renderer *renderer) {
	SDL_Rect src = {0,0, 24,24};
	if (_state == Normal || _state == Chasing) {
	   if(SDL_GetTicks() - _directions[_direction].lastFrameChange > _directions[_direction].frameChangeInterval){
			_directions[_direction].currentFrame =
			(_directions[_direction].currentFrame+1) % _directions[_direction].frameCount;
		}
		src.x = _directions[_direction].frames[_directions[_direction].currentFrame].x;
	}else if (_state == Dead) {
		src.x = _deadDirections[_direction].frames[_deadDirections[_direction].currentFrame].x;
	}else if (_state == Terrified) {
		src = _terrified.frames[_terrified.currentFrame];
	}
	SDL_Rect dst = _gameElement->_mapRect;
	dst.x += round(_position.x*_gameElement->_tileSize);
	dst.y += round(_position.y*_gameElement->_tileSize);
	dst.w = _gameElement->_tileSize;
	dst.h = _gameElement->_tileSize;
	SDL_RenderCopy(Game::_renderer, _texture, &src, &dst);
}

void PlayState::GameElement::Entity::resetPosition() {
	_position = _spawn;
	_previous = _spawn;
	_released = false;
	_direction = Up;
	updateDirection(_spawn.x, _spawn.y);

	switch (_ghost) {
		case Blinky:
			_releaseTime = SDL_GetTicks();
		break;
		case Inky:
			_releaseTime = SDL_GetTicks() + 2000;
		break;
		case Pinky:
			_releaseTime = SDL_GetTicks() + 4000;
		break;
		case Clyde:
			_releaseTime = SDL_GetTicks() + 6000;
		break;
	}
}

void PlayState::GameElement::Entity::updatePosition(uint64_t deltaNanos) {
	updateTarget();
	double speed = _gameElement->_player->isDead() ? 0: Game::_speed*deltaNanos/1e9; // Correct time calculation
	double targetX = _position.x, targetY = _position.y;

	if (_state == Terrified && SDL_GetTicks()>_terrifiedTime) {
		_state = (state) _gameElement->_globalEntityState;
		_direction = (direction)( _direction %2 == 0 ? _direction+1: _direction-1);
	}else if (_state == Terrified) {
		speed/=5;
	}else if (isWall(floor(_position.x), floor(_position.y))) {
		speed/=2;
	}

	switch (_direction) {
	case Up:
		targetY -= speed;
		if (targetY<0) {
			targetY+=31;
			_previous.y = 31;
		}
		if (ceil(targetY) != _previous.y) {
			targetY = _previous.y-1;
		}
		break;
	case Down:
		targetY += speed;
		if (targetY>31) {
			targetY-=31;
			_previous.y = 0;
		}
		if (floor(targetY) != _previous.y) {
			targetY = _previous.y+1;
		}
		break;
	case Left:
		targetX -= speed;
		if (targetX<0) {
			targetX+=31;
			_previous.x = 31;
		}
		if (ceil(targetX) != _previous.x) {
			targetX = _previous.x-1;
		}
		break;
	case Right:
		targetX += speed;
		if (targetX>31) {
			targetX-=31;
			_previous.x = 0;
		}
		else if (floor(targetX) != _previous.x) {
			targetX = _previous.x+1;
		}
		break;
	}

	_position.x = targetX;
	_position.y = targetY;

	if (_state == Dead && fabs(_position.x - _gameElement->_map->getGhostRespawn().x)<4 && fabs(_position.y - (_gameElement->_map->getGhostRespawn().y+1.5))<3.5) {
		_state =(state) _gameElement->_globalEntityState;
		_position = _gameElement->_map->getGhostRespawn();
		_position.y++;
	}

	if (!_released && SDL_GetTicks()>_releaseTime) {
		_state = (state) _gameElement->_globalEntityState;
		_released = true;
		_position = _gameElement->_map->getGhostRespawn();
		_position.y--;
	}


	if (_position.x == round(_position.x) && _position.y == round(_position.y)) {
		updateDirection(_position.x, _position.y);
		_previous = _position;
	}
}

PlayState::GameElement::Coordinates PlayState::GameElement::Entity::getPosition() {
	return _position;
}

void PlayState::GameElement::Entity::BlinkyTargeting() {
	if (_state == Normal) {
		_target = {32,0};
	}else if (_state == Dead) {
		_target = _spawn;
		_target.y ++;
	}else if (_state == Chasing) {
		_target = _gameElement->_player->getPosition();
	}
}

void PlayState::GameElement::Entity::PinkyTargeting(){
    if (_state == Normal) {
		_target = {0,0};
	}else if (_state == Dead) {
		_target = _spawn;
		_target.y ++;
	}else if (_state == Chasing) {
		_target = _gameElement->_player->getPosition();
		direction playerDir = _gameElement->_player->getDirection();
		switch (playerDir) {
		    case Up:
				_target.y-=2; _target.x-=2;
			break;
			case Down:
			    _target.y +=2;
			break;
			case Left:
			    _target.x -=2;
			break;
			case Right:
			    _target.x +=2;
			break;
		}
	}
}

void PlayState::GameElement::Entity::ClydeTargeting(){
    if (_state == Normal) {
		_target = {0,32};
	}else if (_state == Dead) {
		_target = _spawn;
		_target.y ++;
	}else if (_state == Chasing) {
	   Coordinates player = _gameElement->_player->getPosition();
	   double dist = sqrt(pow(_position.x - player.x, 2)+pow(_position.y - player.y,2));

	    if(dist<=8)
			_target = {0,32};
	    else
			_target = player;
	}
}

PlayState::GameElement::Entity::state PlayState::GameElement::Entity::getState() {
	return _state;
}

void PlayState::GameElement::Entity::getEaten() {
	_state = Dead;
	_releaseTime = SDL_GetTicks() + 5e3;
	_released = false;
	Game::_audioHandler->playSound(AudioHandler::GHOST_RETURN);
}

void PlayState::GameElement::Entity::setState(state state) {
	if (_state == Dead)
		return;

	if (state == Terrified) {
		if (_state == Terrified) {
			_terrifiedTime+= 1e4;
		}
		else {
			_state = state;
			_terrifiedTime = SDL_GetTicks() + (uint32_t)1e4;
			switch (_direction) {
				case Up:
					_previous = {round(_position.x),round(_position.y)-1};
				break;
				case Down:
					_previous = {round(_position.x),round(_position.y)+1};
				break;
				case Left:
					_previous = {round(_position.x)-1,round(_position.y)};
				break;
				case Right:
					_previous = {round(_position.x)+1,round(_position.y)};
				break;
			}
			_direction = (direction)( _direction %2 == 0 ? _direction+1: _direction-1);
		}
	}
	else if (_state == Terrified && SDL_GetTicks() < _terrifiedTime) {
		return;
	}
	else {
		_state = state;
		switch (_direction) {
			case Up:
				_previous = {round(_position.x),round(_position.y)-1};
			break;
			case Down:
				_previous = {round(_position.x),round(_position.y)+1};
			break;
			case Left:
				_previous = {round(_position.x)-1,round(_position.y)};
			break;
			case Right:
				_previous = {round(_position.x)+1,round(_position.y)};
			break;
		}
		_direction = (direction)( _direction %2 == 0 ? _direction+1: _direction-1);
	}
}

void PlayState::GameElement::Entity::InkyTargeting(){
    if (_state == Normal) {
		_target = {32,32};
	}else if (_state == Dead) {
		_target = _spawn;
		_target.y ++;
	}else if (_state == Chasing) {
		_target = _gameElement->_player->getPosition();
		direction playerDir = _gameElement->_player->getDirection();
		switch (playerDir) {
		    case Up:
				_target.y-=2; _target.x-=2;
			break;
			case Down:
			    _target.y +=2;
			break;
			case Left:
			    _target.x -=2;
			break;
			case Right:
			    _target.x +=2;
			break;
		}
	}
}

PlayState::GameElement::Map::Map() {

	_map = new Cell*[mapSize];
	for (int i = 0; i < mapSize; i++) {
		_map[i] = new Cell[mapSize];
	}
	_playerRespawn = {0,0};
	_ghostRespawn = {0,0};
	_collectibleCount=0;
}

PlayState::GameElement::Map::~Map() {
	for (int i = 0; i < mapSize; i++) {
		delete[] _map[i];
	}
	delete[] _map;
}

void PlayState::GameElement::Map::readMapString(std::string map) {
	int i = 0, j = 0;
	for (auto &e: map) {
		if (e == ',' || e == '\n' || e=='\r') {
			continue;
		}
		if (e == '#' || e == 'p')
			_map[i][j].wall = true;
		else {
			_map[i][j].wall = false;
			if (e == '-') {
				_map[i][j].content = Cell::Collectible;
				_collectibleCount++;
			}
			else if (e == 'x') {
				_collectibleCount++;
				_map[i][j].content = Cell::PowerPellet;
			}
			else if (e == '~')
				_map[i][j].content = Cell::None;
			else if(e == 'd'){
			    _map[i][j].wall = true;
				_ghostRespawn = {(double)j,(double)i};
			}
			else if (e == 's') {
				_playerRespawn.x = j;
				_playerRespawn.y = i;
			}
		}
		printf("%c", e);
		j++;
		if (j == mapSize) {
			j = 0;
			i++;
		}
	}

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			printf("%c", _map[i][j].wall?'#':' ');
		}
		printf("\n");
	}
}

bool PlayState::GameElement::Map::isWall(int x, int y) {
	if (x<0 || x>=mapSize || y<0 || y>=mapSize)
		return false;
	return _map[y][x].wall;
}

bool PlayState::GameElement::Map::isPoint(int x, int y) {
	return _map[y][x].content == Cell::Collectible;
}

bool PlayState::GameElement::Map::isPellet(int x, int y) {
	return _map[y][x].content == Cell::PowerPellet;
}

uint16_t PlayState::GameElement::Map::getCollectibleCount() {
	return _collectibleCount;
}

void PlayState::GameElement::Map::removeCollectible(int x, int y) {
	_map[y][x].content = Cell::None;
	_collectibleCount--;
}

PlayState::GameElement::Coordinates PlayState::GameElement::Map::getPlayerRespawn() {
	return _playerRespawn;
}

PlayState::GameElement::Coordinates PlayState::GameElement::Map::getGhostRespawn() {
	return _ghostRespawn;
}

PlayState::GameElement::Player::Player(Coordinates position, GameElement* gameElement) {
	_direction = None;
	_position = position;
	_gameElement = gameElement;
	_texture = IMG_LoadTexture(Game::_renderer,"../Textures/pacman.png");

	_normal = {};
	_normal.frames = new SDL_Rect[3];
	_normal.frames[0] = {0,0,28,28};
	_normal.frames[1] = {28,0,28,28};
	_normal.frames[2] = {56,0,28,28};
	_normal.currentFrame = 0;
	_normal.frameCount = 3;
	_normal.frameChangeInterval = 200;
	_normal.lastFrameChange = 0;

	_death = {};
	_death.frames = new SDL_Rect[12];

	#pragma loop(unroll(12))
	for (int i = 0; i < 12; i++) {
		_death.frames[i] = {(i+3)*28,0,28,28};
	}
	_death.currentFrame = 0;
	_death.frameCount = 12;
	_death.frameChangeInterval = 200;
	_state = Normal;
}

PlayState::GameElement::direction PlayState::GameElement::Player::getDirection(){
    return _direction;
}

void PlayState::GameElement::Player::setDirection(direction direction) {
	_direction = direction;
	//printf("Direction: %d\n", _direction);
}

void PlayState::GameElement::Player::render(SDL_Renderer *renderer) {
	SDL_Rect dst = _gameElement->_mapRect;
	SDL_Rect src = _state == Normal ? _normal.frames[_normal.currentFrame] : _death.frames[_death.currentFrame];
	dst.x += round(_position.x*_gameElement->_tileSize);
	dst.y += round(_position.y*_gameElement->_tileSize);
	dst.w = _gameElement->_tileSize;
	dst.h = _gameElement->_tileSize;
	SDL_RenderCopyEx(renderer, _gameElement->_player->_texture, &src, &dst,
	                 _direction == Up ? 270 : _direction == Down ? 90 : 0, nullptr,
	                 _direction == Left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void PlayState::GameElement::Player::updatePosition(uint64_t deltaNanos) {
	double speed = isDead()?0: Game::_speed*deltaNanos/1e9;// Correct time calculation
	double targetX = _position.x, targetY = _position.y;

	if (_direction == Up) {
		targetY -= speed;
	} else if (_direction == Down) {
		targetY += speed;
	} else if (_direction == Left) {
		targetX -= speed;
	} else if (_direction == Right) {
		targetX += speed;
	} else {
		return; // No movement if direction is None
	}

	// Check for walls using target positions
	int nextTileX = round(targetX);
	int nextTileY = round(targetY);

	if (_direction == Up && !_gameElement->_map->isWall(nextTileX, floor(targetY))) {
		_position.y = targetY;
		_position.y < 0 ? _position.y +=31 : _position.y; // Snap to tile center if close enough
	} else if (_direction == Down && !_gameElement->_map->isWall(nextTileX, ceil(targetY))) {
		_position.y = targetY;
		_position.y > 31 ? _position.y -=31 : _position.y;
	} else if (_direction == Left && !_gameElement->_map->isWall(floor(targetX), nextTileY)) {
		//printf("Left\n, Position %lf %lf , mapCoord : %d %d\n", targetX, targetY, floor(targetX), nextTileY);
		_position.x = targetX;
		_position.x < 0 ? _position.x +=31 : _position.x;
	} else if (_direction == Right && !_gameElement->_map->isWall(ceil(targetX), nextTileY)) {
		_position.x = targetX;
		_position.x >31 ? _position.x -=31 : _position.x;
	}

	// Snap to tile center if close enough
	double snapThreshold = 0.002;
	if (fabs(_position.x - round(_position.x)) < snapThreshold) {
		_position.x = round(_position.x);
	}
	if (fabs(_position.y - round(_position.y)) < snapThreshold) {
		_position.y = round(_position.y);
	}

	int64_t ticks = SDL_GetTicks();
	if (_state == Normal) {
		if (ticks - _normal.lastFrameChange > _normal.frameChangeInterval) {
			_normal.currentFrame = (_normal.currentFrame + 1) % _normal.frameCount;
			_normal.lastFrameChange = ticks;
		}
	}else if (_state == Dead) {
		if (ticks - _death.lastFrameChange > _death.frameChangeInterval) {
			_death.currentFrame++;
			_death.lastFrameChange = ticks;

			if (_death.currentFrame == _death.frameCount) {
				_state = Normal;
				_gameElement->restartPostions();
				_death.currentFrame = 0;
			}
		}
	}
}

void PlayState::GameElement::Player::resetPosition() {
	_position = _gameElement->_map->getPlayerRespawn();
	_direction = None;
}

bool PlayState::GameElement::Player::isDead() {
	return _state == Dead;
}

PlayState::GameElement::Coordinates PlayState::GameElement::Player::getPosition(){
    return _position;
}

void PlayState::GameElement::Player::Kill() {
	_state = Dead;
}

PlayState::GameElement::Player::~Player() {
	SDL_DestroyTexture(_texture);
	delete[] _normal.frames;
}
