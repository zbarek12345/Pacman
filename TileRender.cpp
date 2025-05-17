//
// Created by Wiktor on 17.03.2025.
//

#include "TileRender.h"

TileRender::TileRender() {
	_map = new tile*[mapSize];
	for (int i = 0; i < mapSize; i++) {
		_map[i] = new tile[mapSize];
	}
}

void TileRender::LoadTexture(SDL_Renderer* renderer,std::string path) {
	SDL_Surface* tempSurface = IMG_Load(path.c_str());
	if (!tempSurface) {
		std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
	}
	_texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	SDL_FreeSurface(tempSurface);

	tempSurface = IMG_Load("../Textures/tileset.png");
	if (!tempSurface) {
		std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
	}
	_texture2 = SDL_CreateTextureFromSurface(renderer, tempSurface);
	SDL_FreeSurface(tempSurface);
}

void TileRender::readMapString(std::string map) {

	for (int i =0;i<mapSize;i++) {
		memset(_map[i], 0, sizeof(tile) * mapSize);
	}

	int i = 0, j = 0;
	for (auto &e: map) {
		if (e == ',' || e == '\n' || e=='\r') {
			continue;
		}
		if (e == '#' || e == 'p')
			_map[i][j].isWall = 1;
		else {
			_map[i][j].isWall = 0;
			if (e == '-')
				_points.push_back({j * 24, i * 24, 24, 24});
			else if (e == 'x')
				_superPoints.push_back({j * 24, i * 24, 24, 24});
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
			if (i > 0) {
				if (_map[i - 1][j].isWall) {
					_map[i][j].top = _map[i - 1][j].isWall;
				}
			}
			if (j > 0) {
				if (_map[i][j - 1].isWall) {
					_map[i][j].left = _map[i][j - 1].isWall;
				}
			}
			if (i < mapSize - 1) {
				if (_map[i + 1][j].isWall) {
					_map[i][j].bottom = _map[i + 1][j].isWall;
				}
			}
			if (j < mapSize - 1) {
				if (_map[i][j + 1].isWall) {
					_map[i][j].right = _map[i][j + 1].isWall;
				}
			}
		}
	}
}

void TileRender::readMap(std::string &fileName) {

	FILE* f = fopen(fileName.c_str(), "r");
	char c;

	for (int i =0;i<mapSize;i++) {
		memset(_map[i], 0, sizeof(tile) * mapSize);
	}

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			fscanf(f, "%c", &c);
			if (c==','||c=='\n') {
				j--;
				continue;
			}
			if (c == '#' || c=='p')
				_map[i][j].isWall = 1;
			else {
				_map[i][j].isWall = 0;
				if (c=='-')
					_points.push_back({j*24,i*24,24,24});
				else if (c=='x')
					_superPoints.push_back({j*24,i*24,24,24});
			}

			printf("%c", c);
		}
		printf("\n");
	}

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (i > 0) {
				if (_map[i - 1][j].isWall) {
					_map[i][j].top = _map[i - 1][j].isWall;
				}
			}
			if (j > 0) {
				if (_map[i][j - 1].isWall) {
					_map[i][j].left = _map[i][j - 1].isWall;
				}
			}
			if (i < mapSize - 1) {
				if (_map[i + 1][j].isWall) {
					_map[i][j].bottom = _map[i + 1][j].isWall;
				}
			}
			if (j < mapSize - 1) {
				if (_map[i][j + 1].isWall) {
					_map[i][j].right = _map[i][j + 1].isWall;
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
			SDL_RenderCopy(renderer,_texture, &_convex[0], &dst);

			///TopRight
			dst = {x+16,y,8,8};
			SDL_RenderCopy(renderer,_texture, &_convex[3], &dst);

			///BottomRight
			dst = {x+16,y+16,8,8};
			SDL_RenderCopy(renderer,_texture, &_convex[2], &dst);

			///BottomLeft
			dst = {x,y+16,8,8};
			SDL_RenderCopy(renderer,_texture, &_convex[1], &dst);
		}

		//Endpoint Situation
		else if (boundCT==1) {
			if (tile->top) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[3], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[1], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[2], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[1], &dst);
			}
			else if (tile->bottom) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[0], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[3], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[1], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[3], &dst);
			}
			else if (tile->left) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[2], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[3], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[2], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[0], &dst);
			}
			else {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[0], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[2], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[0], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[1], &dst);
			}
		}


		// //Either line - 1 thick or edge (Convex)
		else if (boundCT==2) {
			///Verrtical Line
			if (tile->top && tile->bottom) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[3], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[1], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[1], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[3], &dst);
			}
			///Horizontal Line
			else if (tile->left && tile->right) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[2], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[2], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[0], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[0], &dst);
			}

			///Top Left
			else if (tile->top && tile->left) {
				SDL_Rect dst;
				///Skipping Top Left Corner

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[1], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[2], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[0], &dst);
			}

			///Top Right
			else if (tile->top && tile->right) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[3], &dst);

				///Skipping TopRight

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[0], &dst);

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[1], &dst);
			}

			///Bottom Left
			else if (tile->bottom && tile->left) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[2], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[3], &dst);

				///BottomRight
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[1], &dst);

				/// Skipping BottomLeft
			}

			///Bottom Right
			else if (tile->bottom && tile->right) {
				SDL_Rect dst;
				///TopLeft
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_convex[0], &dst);

				///TopRight
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[2], &dst);

				///Skipping BottomRight

				///BottomLeft
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[3], &dst);
			}
		}

		else if (boundCT==3){
			SDL_Rect dst;
			if (!tile->top) {
				///Top Left
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[2], &dst);
				///Top Left
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[2], &dst);
			}
			if (!tile->bottom) {
				///Top Left
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[0], &dst);
				///Top Left
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[0], &dst);
			}

			if (!tile->left) {
				///Top Left
				dst = {x,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[3], &dst);
				///Top Left
				dst = {x,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[3], &dst);
			}

			if (!tile->right) {
				///Top Left
				dst = {x+16,y,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[1], &dst);
				///Top Left
				dst = {x+16,y+16,8,8};
				SDL_RenderCopy(renderer,_texture, &_walls[1], &dst);
			}
		}

		SDL_Rect dst;

		///Bottom
		if (!tile->bottom) {
			dst = {x + 8, y + 16, 8, 8};
			SDL_RenderCopy(renderer, _texture, &_walls[0], &dst);
		}
		///Left
		if (!tile->left) {
			dst = {x, y + 8, 8, 8};
			SDL_RenderCopy(renderer, _texture, &_walls[3], &dst);
		}

		///Top
		if (!tile->top) {
			dst = {x + 8, y, 8, 8};
			SDL_RenderCopy(renderer,_texture, &_walls[2], &dst);
		}

		///Right
		if (!tile->right) {
			dst = {x+16,y+8,8,8};
			SDL_RenderCopy(renderer,_texture, &_walls[1], &dst);
		}
	}
	else {
		if (tile->top && tile->left) {
			SDL_Rect dst;
			dst = {x-8, y-8, 8, 8};
			SDL_RenderCopy(renderer, _texture, &_concave[0], &dst);
		}
		if (tile->top&&tile->right) {
			SDL_Rect dst;
			dst = {x+24, y-8, 8, 8};
			SDL_RenderCopy(renderer, _texture, &_concave[3], &dst);
		}
		if (tile->bottom&&tile->left) {
			SDL_Rect dst;
			dst = {x-8, y+24, 8, 8};
			SDL_RenderCopy(renderer, _texture, &_concave[1], &dst);
		}
		if (tile->bottom&&tile->right) {
			SDL_Rect dst;
			dst = {x+24, y+24, 8, 8};
			SDL_RenderCopy(renderer, _texture, &_concave[2], &dst);
		}
	}
}

void TileRender::renderMap(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 30, 144, 255);
	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			DrawTile(renderer, &_map[i][j], j * 24, i * 24);
		}
	}

	for (auto& e: _points)
		SDL_RenderCopy(renderer, _texture2, &_pointTexture, &e);
	for (auto& e: _superPoints)
		SDL_RenderCopy(renderer, _texture2, &_superPointTexture, &e);
}

void TileRender::renderEmptyMap(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 30, 144, 255);
	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			DrawTile(renderer, &_map[i][j], j * 24, i * 24);
		}
	}
}

TileRender::~TileRender() {
	if (_texture!=nullptr)
		SDL_DestroyTexture(_texture);
	for (int i = 0; i < mapSize; i++) {
		delete[] _map[i];
	}
	delete[] _map;
}
