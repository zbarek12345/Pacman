//
// Created by Wiktor on 29.03.2025.
//

#include "PlayState.h"

#include "../TileRender.h"
#include "Game.h"
#include <SDL2/SDL_timer.h>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdio>

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
	_ghosts[0] = new Entity(this, _map->getGhostRespawn(), Entity::Blinky);
	ghostResp.y +=2;
	_ghosts[1] = new Entity(this, ghostResp, Entity::Inky );
	ghostResp.x ++;
	_ghosts[2] = new Entity(this, ghostResp, Entity::Pinky);
	ghostResp.y ++;
	_ghosts[3] = new Entity(this, ghostResp, Entity::Clyde);
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
}

PlayState::GameElement::~GameElement() {
	delete _map;
	delete _player;
	SDL_DestroyTexture(_mapTexture);
	for(int i =0;i<4;i++)
	   delete _ghosts[i];
	delete _ghosts;
}

void PlayState::GameElement::render(SDL_Renderer *renderer) {
	_coordinates.w = 512;
	_coordinates.h = 512;
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderCopy(renderer, _mapTexture, nullptr, &_coordinates);

	SDL_Rect pointTexture = {32,0,16,16};
	SDL_Rect superPointTexture = {48,0,16,16};

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (!_map->isWall(j, i)) {
				SDL_Rect dst = _coordinates;
				dst.x += j * 16;
				dst.y += i * 16;
				dst.w = 16;
				dst.h = 16;
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

	_player->updatePosition(delta);
	for(int i =0;i<4;i++)
	   _ghosts[i]->updatePosition(delta);
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

void PlayState::GameElement::verifyCollision() {
}

PlayState::PlayState(int32_t level): GameState(Game::_renderer) {
	_game = new GameElement(level);
	_game->setCoordinatesf(0.1, 0.1, 0.9, 0.9);
}

PlayState::~PlayState() {
	delete _game;
}

void PlayState::update() {
	_game->update();
}

void PlayState::render() {
	_game->render(Game::_renderer);
}

void PlayState::handleInput(SDL_Event &event, GameState *&nextState) {
	_game->handleInput(event);
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
        if(_ghost == Blinky)
            printf("Position : %d, %d \n", x, y);
        for(int i =0;i<4;i++){
            if(_ghost == Blinky)
                printf("%d : %f\n", i, costs[i]);
            if(costs[i]<bestCost){
                _direction = (direction)i;
                bestCost = costs[i];
            }
        }
        fflush(stdout);
    }
}


PlayState::GameElement::Entity::Entity(GameElement *gameElement,Coordinates spawn, ghostType ghost) {
	_gameElement = gameElement;
	_spawn = spawn;
	_position = spawn;
	_ghost = ghost;
	//printf("Start %d : %f %f\n", ghost, _position.x, _position.y);
	_previous = _spawn;
	_direction = (direction)(rand()%4);
	_state = Normal;
	_texture = nullptr;
	switch(_ghost){
	    case Blinky:
		    _texture = IMG_LoadTexture(Game::_renderer, "../Textures/blinky.png");
		break;
		case Inky:
            _texture = IMG_LoadTexture(Game::_renderer,"../Textures/inky.png");
        break;
        case Pinky:
			_texture = IMG_LoadTexture(Game::_renderer,"../Textures/pinky.png");
		break;
		case Clyde:
            _texture = IMG_LoadTexture(Game::_renderer,"../Textures/clyde.png");
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
	_directions[0].frameChangeInterval = 200;
	_directions[0].lastFrameChange = 0;

	//printf("Test: %d %d", _directions[0].frames[0].x, _directions[0].frames[1].x);
	//Down
	_directions[1] = {};
	_directions[1].frames = new SDL_Rect[2];
	_directions[1].frames[0] = {2*24,0,24,24};
	_directions[1].frames[1] = {3*24,0,24,24};
	_directions[1].currentFrame = 0;
	_directions[1].frameCount = 2;
	_directions[1].frameChangeInterval = 200;
	_directions[1].lastFrameChange = 0;

	//Left
	_directions[2] = {};
	_directions[2].frames = new SDL_Rect[2];
	_directions[2].frames[0] = {4*24,0,24,24};
	_directions[2].frames[1] = {5*24,0,24,24};
	_directions[2].currentFrame = 0;
	_directions[2].frameCount = 2;
	_directions[2].frameChangeInterval = 200;
	_directions[2].lastFrameChange = 0;

	//Right
	_directions[3] = {};
	_directions[3].frames = new SDL_Rect[2];
	_directions[3].frames[0] = {0*24,0,24,24};
	_directions[3].frames[1] = {1*24,0,24,24};
	_directions[3].currentFrame = 0;
	_directions[3].frameCount = 2;
	_directions[3].frameChangeInterval = 200;
	_directions[3].lastFrameChange = 0;

	//UP
	_deadDirections[0] = {};
	_deadDirections[0].frames = new SDL_Rect[1];
	_deadDirections[0].frames[0] = {15*24,0,24,24};
	_deadDirections[0].currentFrame = 0;
	_deadDirections[0].frameCount = 1;
	_deadDirections[0].frameChangeInterval = 200;
	_deadDirections[0].lastFrameChange = 0;

	//Down
	_deadDirections[1] = {};
	_deadDirections[1].frames = new SDL_Rect[1];
	_deadDirections[1].frames[0] = {13*24,0,24,24};
	_deadDirections[1].currentFrame = 0;
	_deadDirections[1].frameCount = 1;
	_deadDirections[1].frameChangeInterval = 200;
	_deadDirections[1].lastFrameChange = 0;

	//Left
	_deadDirections[2] = {};
	_deadDirections[2].frames = new SDL_Rect[1];
	_deadDirections[2].frames[0] = {11*24,0,24,24};
	_deadDirections[2].currentFrame = 0;
	_deadDirections[2].frameCount = 1;
	_deadDirections[2].frameChangeInterval = 200;
	_deadDirections[2].lastFrameChange = 0;

	//Right
	_deadDirections[3] = {};
	_deadDirections[3].frames = new SDL_Rect[1];
	_deadDirections[3].frames[0] = {14*24,0,24,24};
	_deadDirections[3].currentFrame = 0;
	_deadDirections[3].frameCount = 1;
	_deadDirections[3].frameChangeInterval = 200;
	_deadDirections[3].lastFrameChange = 0;

	_terrified = {};
	_terrified.frames = new SDL_Rect[4];
	_terrified.frames[0] = {8*24,0,24,24};
	_terrified.frames[1] = {9*24,0,24,24};
	_terrified.frames[2] = {10*24,0,24,24};
	_terrified.frames[3] = {11*24,0,24,24};
	_terrified.currentFrame = 0;
	_terrified.frameCount = 4;
	_terrified.frameChangeInterval = 200;
	_terrified.lastFrameChange = 0;
}

PlayState::GameElement::Entity::~Entity() {
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
	SDL_Rect dst = _gameElement->getCoordinates();
	dst.x += round(_position.x*16);
	dst.y += round(_position.y*16);
	dst.w = 16;
	dst.h = 16;
	SDL_RenderCopy(Game::_renderer, _texture, &src, &dst);
}

    void PlayState::GameElement::Entity::updatePosition(int deltaNanos) {
        updateTarget();
    	double speed = Game::_speed*deltaNanos/1e9; // Correct time calculation
    	double targetX = _position.x, targetY = _position.y;
    	if (_direction == Up) {
    		targetY -= speed;
    	}else if (_direction == Down) {
    		targetY += speed;
    	}else if (_direction == Left) {
    		targetX -= speed;
    	}else if (_direction == Right) {
    		targetX += speed;
    	}

    	if (round(targetX) != _previous.x && fabs(targetX-round(targetX))<0.05) {
    		targetX = round(targetX);
    	}if (round(targetY) != _previous.y && fabs(targetY-round(targetY))<0.05) {
    		targetY = round(targetY);
    	}

    	_position.x = targetX;
    	_position.y = targetY;

    	if (targetX == round(targetX) && targetY == round(targetY)) {
    		updateDirection(targetX, targetY);
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
		_target = {32,0};
	}else if (_state == Chasing) {
		_target = _gameElement->_player->getPosition();
	}
}

void PlayState::GameElement::Entity::PinkyTargeting(){
    if (_state == Normal) {
		_target = {0,0};
	}else if (_state == Dead) {
		_target = {0,0};
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
		_target = {0,32};
	}else if (_state == Chasing) {
	   Coordinates player = _gameElement->_player->getPosition();
	   double dist = sqrt(pow(_position.x - player.x, 2)+pow(_position.y - player.y,2));

	    if(dist<=8)
			_target = {0,32};
	    else
			_target = player;
	}
}

void PlayState::GameElement::Entity::InkyTargeting(){
    if (_state == Normal) {
		_target = {32,32};
	}else if (_state == Dead) {
		_target = {32,32};
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
			if (e == '-')
				_map[i][j].content = Cell::Collectible;
			else if (e == 'x')
				_map[i][j].content = Cell::PowerPellet;
			else if (e == '~')
				_map[i][j].content = Cell::None;
			else if(e == 'd'){
			    _map[i][j].wall = true;
				_ghostRespawn = {(double)i,(double)j};
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

bool PlayState::GameElement::Map::eat(int x, int y) {
	if (_map[y][x].content == Cell::Collectible || _map[y][x].content == Cell::PowerPellet) {
		_map[y][x].content = Cell::None;
		return true;
	}
	return false;
}

bool PlayState::GameElement::Map::isPoint(int x, int y) {
	return _map[y][x].content == Cell::Collectible;
}

bool PlayState::GameElement::Map::isPellet(int x, int y) {
	return _map[y][x].content == Cell::PowerPellet;
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
	SDL_Rect dst = _gameElement->getCoordinates();
	SDL_Rect src = _state == Normal ? _normal.frames[_normal.currentFrame] : _death.frames[_death.currentFrame];
	dst.x += round(_position.x*16);
	dst.y += round(_position.y*16);
	dst.w = 16;
	dst.h = 16;
	SDL_RenderCopyEx(renderer, _gameElement->_player->_texture, &src, &dst,
	                 _direction == Up ? 270 : _direction == Down ? 90 : 0, nullptr,
	                 _direction == Left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void PlayState::GameElement::Player::updatePosition(int deltaNanos) {
	double speed = Game::_speed*deltaNanos/1e9;// Correct time calculation
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

	// Consume items (points, power-ups, etc.) at the snapped position
	_gameElement->_map->eat(round(_position.x), round(_position.y));

	int64_t ticks = SDL_GetTicks();
	if (_state == Normal) {
		if (ticks - _normal.lastFrameChange > _normal.frameChangeInterval) {
			_normal.currentFrame = (_normal.currentFrame + 1) % _normal.frameCount;
			_normal.lastFrameChange = ticks;
		}
	}else if (_state == Dead) {
		if (ticks - _death.lastFrameChange > _death.frameChangeInterval) {
			_death.currentFrame = (_death.currentFrame + 1) % _death.frameCount;
			_death.lastFrameChange = ticks;
		}
	}
}

PlayState::GameElement::Coordinates PlayState::GameElement::Player::getPosition(){
    return _position;
}

PlayState::GameElement::Player::~Player() {
	SDL_DestroyTexture(_texture);
	delete _normal.frames;
}
