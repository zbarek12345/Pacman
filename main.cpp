#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <MenuState.h>

#include "States/Game.h"
#include "TileRender.h"
#include "UI_Element/Button.h"

// Tile dimensions
const int TILE_SIZE = 8;
const int TEXTURE_SIZE = 32;

bool initSDL(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    *window = SDL_CreateWindow("SDL2 Texture Demo",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                             1080, 720,
                               SDL_WINDOW_SHOWN);
    if (!*window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_SOFTWARE);
    if (!*renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load(path.c_str());
    if (!tempSurface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return texture;
}

void renderSubTextures(SDL_Renderer* renderer, SDL_Texture* texture) {
    // Loop through each of the 8x8 tiles (2D grid of 4x4 tiles in a 32x32 image)
    for (int y = 0; y < TEXTURE_SIZE / TILE_SIZE; ++y) {
        for (int x = 0; x < TEXTURE_SIZE / TILE_SIZE; ++x) {
            // Define the source rectangle (part of the big texture)
            SDL_Rect srcRect = {
                x * TILE_SIZE,    // x-offset
                y * TILE_SIZE,    // y-offset
                TILE_SIZE,        // width
                TILE_SIZE         // height
            };

            // Define the destination rectangle (where it will be drawn on the screen)
            SDL_Rect destRect = {
                x * TILE_SIZE * 2,  // x-offset (scaled for demonstration)
                y * TILE_SIZE * 2,  // y-offset (scaled for demonstration)
                TILE_SIZE * 2,      // width (scaled size for clarity)
                TILE_SIZE * 2       // height (scaled size for clarity)
            };

            // Render the sub-texture
            SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!initSDL(&window, &renderer)) {
        return -1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return -1;
    }

    if (!TTF_Init()) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
    }

    // Load the 32x32 tileset texture
    SDL_Texture* texture = loadTexture("../Textures/wallTiles.png", renderer);
    if (!texture) {
        SDL_Quit();
        return -1;
    }

    // auto tileRender = new TileRender();
    // std::string fileName = "../Levels/Lvl3";
    // tileRender->readMap(fileName);
    // tileRender->LoadTexture(renderer, "../Textures/wallTiles_white.png");

    Game game = Game(window, renderer);
    Game::run();
    // bool running = true;
    // SDL_Event event;
    // while (running) {
    //     while (SDL_PollEvent(&event)) {
    //         state->handleInput(event, state);
    //         if (event.type == SDL_QUIT) {
    //             running = false;
    //         }
    //     }
    //
    //     // Clear the screen
    //     SDL_SetRenderDrawColor(renderer, 255, 255, 255,255);
    //     SDL_RenderClear(renderer);
    //
    //     //state->render();
    //     // Present rendered content
    //     SDL_RenderPresent(renderer);
    // }

    //delete tileRender;
    // Cleanup
    //TTF_CloseFont(font);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}