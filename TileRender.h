//
// Created by Wiktor on 17.03.2025.
//

#ifndef TILERENDER_H
#define TILERENDER_H
#include <SDL_render.h>
#include <string>
#include <iostream>
#include <SDL_image.h>
#include <SDL_render.h>
#include <vector>

static int mapSize = 32;

class TileRender {
	struct tile {
		unsigned char isWall: 1;
		unsigned char top:1;
		unsigned char left:1;
		unsigned char bottom:1;
		unsigned char right:1;
		unsigned char door:1;
	};

	SDL_Texture* _texture = nullptr;
	SDL_Texture* _texture2 = nullptr;
	int16_t _x_offset, _y_offset;
	tile**_map = nullptr;

	///Bottom, Right, Top, Left
	SDL_Rect _walls[4] = {
		{0,0,8,8},
		{8,0,8,8},
		{16,0,8,8},
		{24,0,8,8},
	};

	///TopLeft, BottomLeft, BottomRight, TopRight
	SDL_Rect _concave[4] = {
		{0,8,8,8},
		{8,8,8,8},
		{16,8,8,8},
		{24,8,8,8}
	};

	///TopLeft, BottomLeft, BottomRight, TopRight
	SDL_Rect _convex[4] {
		{0,16,8,8},
		{8,16,8,8},
		{16,16,8,8},
		{24,16,8,8}
	};
	SDL_Rect _pointTexture = {32,0,16,16};
	SDL_Rect _superPointTexture = {48,0,16,16};
	std::vector<SDL_Rect> _points, _superPoints;

public:

		TileRender();

	void LoadTexture(SDL_Renderer *renderer, std::string path);

	void readMap(std::string &fileName);

	void DrawTile(SDL_Renderer *renderer, tile *tile, int x, int y);

	~TileRender();

	void renderMap(SDL_Renderer *renderer);
};



#endif //TILERENDER_H
