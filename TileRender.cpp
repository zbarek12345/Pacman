//
// Created by Wiktor on 17.03.2025.
//

#include "TileRender.h"

TileRender::TileRender() {
	map = new tile*[mapSize];
	for (int i = 0; i < mapSize; i++) {
		map[i] = new tile[mapSize];
	}
}

void TileRender::LoadTexture(SDL_Renderer* renderer,std::string path) {
	SDL_Surface* tempSurface = IMG_Load(path.c_str());
	if (!tempSurface) {
		std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
	}
	texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	SDL_FreeSurface(tempSurface);
}

void TileRender::readMap(std::string &fileName) {

	FILE* f = fopen(fileName.c_str(), "r");
	char c;

	for (int i =0;i<mapSize;i++) {
		memset(map[i], 0, sizeof(tile) * mapSize);
	}

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			fscanf(f, "%c", &c);
			if (c==','||c=='\n') {
				j--;
				continue;
			}
			if (c == '#' || c=='P')
				map[i][j].isWall = 1;
			else
				map[i][j].isWall = 0;

			printf("%c", c);
		}
		printf("\n");
	}

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (i > 0) {
				if (map[i - 1][j].isWall) {
					map[i][j].top = map[i - 1][j].isWall;
				}
			}
			if (j > 0) {
				if (map[i][j - 1].isWall) {
					map[i][j].left = map[i][j - 1].isWall;
				}
			}
			if (i < mapSize - 1) {
				if (map[i + 1][j].isWall) {
					map[i][j].bottom = map[i + 1][j].isWall;
				}
			}
			if (j < mapSize - 1) {
				if (map[i][j + 1].isWall) {
					map[i][j].right = map[i][j + 1].isWall;
				}
			}
		}
	}
	fclose(f);
}

void TileRender::DrawTile(SDL_Renderer* renderer, tile* tile, int x, int y) {

	if (tile->isWall) {
		unsigned char boundCT = tile->top+tile->bottom+tile->left+tile->right;

		//Standalone Situation
		if (boundCT==0) {
			SDL_Rect dst;
			///Draw Corners
			///TopLeft
			dst = {x,y,8,8};
			SDL_RenderCopy(renderer,texture, &convex[0], &dst);

			///TopRight
			dst = {x+16,y,8,8};
			SDL_RenderCopy(renderer,texture, &convex[3], &dst);

			///BottomRight
			dst = {x+16,y+16,8,8};
			SDL_RenderCopy(renderer,texture, &convex[2], &dst);

			///BottomLeft
			dst = {x,y+16,8,8};
			SDL_RenderCopy(renderer,texture, &convex[1], &dst);
		}

		//Endpoint Situation
		else if (boundCT==1) {
			if (tile->top) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,texture, &walls[1], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,texture, &walls[3], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,texture, &convex[2], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,texture, &convex[1], &dst);
			}
			else if (tile->bottom) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,texture, &convex[0], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,texture, &convex[3], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,texture, &walls[3], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,texture, &walls[1], &dst);
			}
			else if (tile->left) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,texture, &walls[2], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,texture, &convex[3], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,texture, &convex[2], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,texture, &walls[0], &dst);
			}
			else {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,texture, &convex[0], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,texture, &walls[2], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,texture, &walls[0], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,texture, &convex[1], &dst);
			}
		}

		else {
			SDL_Rect dst;
			///TopLeft
			dst = {x,y,8,8};
			SDL_RenderCopy(renderer,texture, &walls[0], &dst);

			///TopRight
			dst = {x+16,y,8,8};
			SDL_RenderCopy(renderer,texture, &walls[3], &dst);

			///BottomRight
			dst = {x+16,y+16,8,8};
			SDL_RenderCopy(renderer,texture, &walls[2], &dst);

			///BottomLeft
			dst = {x,y+16,8,8};
			SDL_RenderCopy(renderer,texture, &walls[1], &dst);
		}
		// //Either line - 1 thick or edge (Convex)
		// else if (boundCT==2) {
		//
		// }
		//
		// //Either line - 1 thick or edge (Convex)
		// else if (boundCT==3) {
		//
		// }

		SDL_Rect dst;
		///Bottom
		dst = {x+8,y+16,8,8};
		SDL_RenderCopy(renderer,texture, &walls[0], &dst);
		///Left
		dst = {x,y+8,8,8};
		SDL_RenderCopy(renderer,texture, &walls[1], &dst);
		///Top
		dst = {x+8,y,8,8};
		SDL_RenderCopy(renderer,texture, &walls[2], &dst);
		///Right
		dst = {x+16,y+8,8,8};
		SDL_RenderCopy(renderer,texture, &walls[3], &dst);
	}
}

void TileRender::renderMap(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 30, 144, 255);
	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (map[i][j].isWall) {
				DrawTile(renderer, &map[i][j], j * 24, i * 24);
			}
		}
	}
}

TileRender::~TileRender() {
	if (texture!=nullptr)
		SDL_DestroyTexture(texture);
	for (int i = 0; i < mapSize; i++) {
		delete[] map[i];
	}
	delete[] map;
}