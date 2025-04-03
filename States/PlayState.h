//
// Created by Wiktor on 29.03.2025.
//

#ifndef PLAYSTATE_H
#define PLAYSTATE_H
#include <cstdint>
#include <string>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include "GameState.h"
#include "UiElement.h"


class PlayState :public GameState{

	class GameElement : public UiElement {
	public:
		GameElement(int level);
		GameElement(SDL_Rect rect, int level);
		~GameElement();
		void render(SDL_Renderer* renderer) override;
		void update() override;
		void handleInput(SDL_Event& event) override;

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
		SDL_Texture* _mapTexture;

		Map* _map;
		Player* _player;
		Entity** _ghosts;
		uint64_t previousTime;
		uint64_t _startTime;
		SDL_Texture* _points;
		//std::vector<Entity> _entities;

		void verifyCollision();
	};

public:
	GameElement* _game;
	explicit PlayState(int32_t level);

	~PlayState() override;

	void update() override;
	void render() override;
	void handleInput(SDL_Event& event, GameState*& nextState) override;

};

class PlayState::GameElement::Entity {

	enum state{
		Normal,
		Chasing,
		Terrified,
		Dead
	} _state;

	Animation _directions[4],_deadDirections[4], _terrified;
	direction _direction;
	Coordinates _position, _target, _spawn, _previous;
	GameElement* _gameElement;
	SDL_Texture* _texture;

	void updateTarget();
	void updateDirection(int x, int y);
	bool isWall(int x, int y);
	bool isCrossroad(int x, int y);
	void BlinkyTargeting();
	void InkyTargeting();
	void PinkyTargeting();
	void ClydeTargeting();

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
	void updatePosition(int deltaNanos);
	Coordinates getPosition();
	void setTexture(SDL_Texture* texture);
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
public:
	Map();
	~Map();

	void readMapString(std::string map);

	bool isWall(int x, int y);

	bool eat(int x, int y);

	bool isPoint(int x, int y);
	bool isPellet(int x, int y);
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
	void updatePosition(int deltaNanos);
	~Player();

	Coordinates getPosition();
};

#endif //PLAYSTATE_H
