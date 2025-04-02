//
// Created by Wiktor on 29.03.2025.
//

#include "PlayState.h"

#include "../TileRender.h"

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
}

PlayState::GameElement::~GameElement() {
	delete _map;
	delete _player;
	SDL_DestroyTexture(_mapTexture);
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
	SDL_SetRenderDrawColor(renderer, 120,23,45,255);
	SDL_RenderDrawRect(renderer, &_coordinates);
}

void PlayState::GameElement::update() {
	uint64_t delta = SDL_GetPerformanceCounter() - previousTime;
	if (previousTime == -1) {
		delta = 0;
	}

	_player->updatePosition(delta);
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

PlayState::GameElement::Map::Map() {

	_map = new Cell*[mapSize];
	for (int i = 0; i < mapSize; i++) {
		_map[i] = new Cell[mapSize];
	}
	_playerRespawn = {0,0};
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
	double speed = Game::_speed * deltaNanos / 1e7; // Correct time calculation
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
	double snapThreshold = 0.01;
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

PlayState::GameElement::Player::~Player() {
	SDL_DestroyTexture(_texture);
	delete _normal.frames;
}
