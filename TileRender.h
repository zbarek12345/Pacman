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

	SDL_Texture* texture = nullptr;
	tile**map = nullptr;

	///Bottom, Right, Top, Left
	SDL_Rect walls[4] = {
		{0,0,8,8},
		{8,0,8,8},
		{16,0,8,8},
		{24,0,8,8},
	};

	///LeftBottom, TopLeft, TopRight, BottomRight
	SDL_Rect concave[4] = {
		{0,8,8,8},
		{8,8,8,8},
		{16,8,8,8},
		{24,8,8,8}
	};

	///TopLeft, BottomLeft, BottomRight, TopRight
	SDL_Rect convex[4] {
		{0,16,8,8},
		{8,16,8,8},
		{16,16,8,8},
		{24,16,8,8}
	};

public:

	TileRender();

	void LoadTexture(SDL_Renderer *renderer, std::string path);

	void readMap(std::string &fileName);

	void DrawTile(SDL_Renderer *renderer, tile *tile, int x, int y);

	~TileRender();

	void renderMap(SDL_Renderer *renderer);
};



#endif //TILERENDER_H
