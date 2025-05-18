//
// Created by Wiktor on 29.03.2025.
//

#ifndef PLAYSTATE_H
#define PLAYSTATE_H
#include <Button.h>
#include <cstdint>
#include <string>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include "GameState.h"
#include "Label.h"
#include "Panel.h"
#include "UiElement.h"


class PlayState :public GameState{

public:
	class GameElement : public UiElement {
	public:

		enum inGameState {
			Running,
			Paused,
		} _gameState;

		GameElement(int level);
		GameElement(SDL_Rect rect, int level);
		~GameElement();
		void render(SDL_Renderer* renderer) override;
		void update() override;
		void handleInput(SDL_Event& event) override;
		void calculateMap();
		void restart();
		void restartPostions();

		struct Coordinates {
			double x;
			double y;
		};

		enum direction {
			Up,
			Down,
			Left,
			Right,
			None
		};

		struct Animation {
			Animation() = default;
			~Animation() = default;
			SDL_Rect* frames;
			uint8_t frameCount;
			uint8_t currentFrame;
			uint64_t lastFrameChange;
			uint64_t frameChangeInterval;
		};

		class Entity;
		class Map;
		class Player;
		int _score;
		int _lives;
		int _level;
		int _globalEntityState;
		uint32_t _globalEntityStateTime;

		SDL_Texture* _mapTexture;

		Map* _map;
		Player* _player;
		Entity** _ghosts;
		uint64_t previousTime;
		uint64_t _startTime;
		SDL_Texture* _points;
		SDL_Rect _mapRect;
		int _tileSize;
		//std::vector<Entity> _entities;

		void verifyCollision();
	};

	class StopMenu : public UiElement {
		PlayState* _playState;
		Panel* _panel;
	public:
		explicit StopMenu(PlayState* element);
		~StopMenu();
		void render(SDL_Renderer* renderer) override;
		void update() override;
		void handleInput(SDL_Event& event) override;
	};

	StopMenu* _stopMenu;
	GameElement* _game;
	Label* _scoreLabel;
	Label* _timeLabel;
	Label* _livesLabel;
public:
	explicit PlayState(int32_t level);

	~PlayState() override;

	void update() override;
	void render() override;
	void handleInput(SDL_Event& event, GameState*& nextState) override;

	void Stop();
	void Start();
};

class PlayState::GameElement::Entity {
public:
	enum state{
		Normal,
		Chasing,
		Terrified,
		Dead
	};
private:
	state _state;
	Animation _directions[4],_deadDirections[4], _terrified;
	direction _direction;
	Coordinates _position, _target, _spawn, _previous;
	GameElement* _gameElement;
	SDL_Texture* _texture;
	bool _released = false;
	uint32_t _releaseTime; uint32_t _terrifiedTime;
	pthread_t _ghostThread;
	bool _thread;

	void updateTarget();
	void updateDirection(int x, int y);
	bool isWall(int x, int y);
	bool isCrossroad(int x, int y);
	void BlinkyTargeting();
	void InkyTargeting();
	void PinkyTargeting();
	void ClydeTargeting();
	static void* ghostThread(void* arg);

public:
    enum ghostType{
	   Blinky,
	   Inky,
	   Pinky,
	   Clyde
	} _ghost;

	explicit Entity(GameElement* gameElement, Coordinates spawn, ghostType ghost);
	~Entity();
	void render(SDL_Renderer* renderer);

	void resetPosition();

	void updatePosition(uint64_t deltaNanos);
	Coordinates getPosition();
	void setTexture(SDL_Texture* texture);
	state getState();
	void getEaten();
	void setState(state state);
};

class PlayState::GameElement::Map {
	struct Cell {
		bool wall;
		enum content {
			None,
			Collectible,
			PowerPellet,
		} content: 2;

		Cell() {
			wall = false;
			content = None;
		}
	};
	Cell** _map;
	Coordinates _playerRespawn{};
	Coordinates _ghostRespawn{};
	uint16_t _collectibleCount;
public:
	Map();
	~Map();

	void readMapString(std::string map);

	bool isWall(int x, int y);
	bool isPoint(int x, int y);
	bool isPellet(int x, int y);

	uint16_t getCollectibleCount();

	void removeCollectible(int x, int y);

	Coordinates getPlayerRespawn();
	Coordinates getGhostRespawn();
};

class PlayState::GameElement::Player{
	enum state{
		Normal,
		Dead
	} _state;

	direction _direction;
	Coordinates _position{};
	SDL_Texture* _texture;
	GameElement* _gameElement;

	Animation _normal{}, _death{};
public:
	Player(Coordinates position, GameElement* gameElement);
	void setDirection(direction direction);
	direction getDirection();
	void render(SDL_Renderer* renderer);
	void updatePosition(uint64_t deltaNanos);
	void resetPosition();
	bool isDead();
	~Player();

	Coordinates getPosition();
	void Kill();
};

#endif //PLAYSTATE_H
