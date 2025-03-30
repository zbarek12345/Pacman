//
// Created by Wiktor on 29.03.2025.
//

#ifndef PLAYSTATE_H
#define PLAYSTATE_H
#include <cstdint>

#include "UiElement.h"


class PlayState {

	class Game : public UiElement {
	protected:
		Game();
		Game(SDL_Rect rect);
		~Game();
		void render(SDL_Renderer* renderer) override;
		void update() override;
		void handleInput(SDL_Event& event) override;

		struct Coordinates {
			uint8_t x;
			uint8_t y;
		};

		class Entity;
		class Map;
	};
};

class PlayState::Game::Entity {
	enum direction {
		Up,
		Down,
		Left,
		Right
	};
	direction _direction;
	Coordinates _position;

	Entity();
	Entity(Coordinates position);
	void move(direction direction);
	void render(SDL_Renderer* renderer);
	void update();
	void handleInput(SDL_Event& event);
	void setPosition(Coordinates position);
	Coordinates getPosition();
};

class PlayState::Game::Map {

	struct Cell {
		uint8_t wall :1;
		enum content {
			None,
			Collectible,
			PowerPellet,
		} content: 2;

		Cell() {
			wall = 0;
			content = None;
		}
	};
	Cell map[32][32];
};



#endif //PLAYSTATE_H
