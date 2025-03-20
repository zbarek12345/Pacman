//
// Created by Wiktor on 19.03.2025.
//

#ifndef SCROLLABLELIST_H
#define SCROLLABLELIST_H



#include <SDL.h>
#include <vector>
#include <string>
#include "UiElement.h"

class ScrollableList : public UiElement {
public:
    ScrollableList(SDL_Rect rect, int itemHeight = 30)
        : UiElement(rect), itemHeight(itemHeight), scrollOffset(0), maxVisibleItems(rect.h / itemHeight) {
        backgroundColor = {50, 50, 50, 255};  // Dark gray background
        itemColor = {100, 100, 100, 255};    // Light gray items
        selectedColor = {0, 200, 0, 255};    // Green for selected item
        selectedIndex = -1; // No selection by default
    }

    void addItem(const std::string& item) {
        items.push_back(item);
    }

    void render(SDL_Renderer* renderer) override {
        // Draw background
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
        SDL_RenderFillRect(renderer, &_coordinates);

        // Draw visible items
        int startIdx = scrollOffset / itemHeight;
        int endIdx = std::min(static_cast<int>(items.size()), startIdx + maxVisibleItems + 1);

        for (int i = startIdx; i < endIdx; ++i) {
            SDL_Rect itemRect = {_coordinates.x, _coordinates.y + (i - startIdx) * itemHeight,
                                 _coordinates.w, itemHeight};
            if (i == selectedIndex) {
                SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, itemColor.r, itemColor.g, itemColor.b, 255);
            }
            SDL_RenderFillRect(renderer, &itemRect);

            // Placeholder for text rendering (requires SDL_ttf externally)
            // For now, just draw the rect; add text later if needed
        }

        // Draw scrollbar if needed
        if (items.size() > maxVisibleItems) {
            int scrollBarHeight = (_coordinates.h * maxVisibleItems) / items.size();
            int scrollBarY = _coordinates.y + (scrollOffset * _coordinates.h) / (items.size() * itemHeight);
            SDL_Rect scrollBar = {_coordinates.x + _coordinates.w - 10, scrollBarY, 10, scrollBarHeight};
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Gray scrollbar
            SDL_RenderFillRect(renderer, &scrollBar);
        }
    }

    void update() override {
        // Could add smooth scrolling animation here
    }

    void handleInput(SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Point point = {event.button.x, event.button.y};
            if (isInside(point)) {
                int relativeY = point.y - _coordinates.y;
                int clickedIndex = (relativeY / itemHeight) + (scrollOffset / itemHeight);
                if (clickedIndex >= 0 && clickedIndex < static_cast<int>(items.size())) {
                    selectedIndex = clickedIndex; // Select item
                }
            }
        } else if (event.type == SDL_MOUSEWHEEL) {
            int newOffset = scrollOffset - event.wheel.y * itemHeight; // Scroll up/down
            scrollOffset = std::max(0, std::min(newOffset, std::max(0, static_cast<int>(items.size() * itemHeight - _coordinates.h))));
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_UP && selectedIndex > 0) {
                selectedIndex--;
                adjustScroll();
            } else if (event.key.keysym.sym == SDLK_DOWN && selectedIndex < static_cast<int>(items.size()) - 1) {
                selectedIndex++;
                adjustScroll();
            }
        }
    }

    [[nodiscard]] int getSelectedIndex() const { return selectedIndex; }
    [[nodiscard]] std::string getSelectedItem() const {
        return (selectedIndex >= 0 && selectedIndex < static_cast<int>(items.size())) ? items[selectedIndex] : "";
    }

private:
    std::vector<std::string> items; // Observable list
    int itemHeight;                 // Height of each item
    int scrollOffset;               // Pixel offset for scrolling
    int maxVisibleItems;            // Number of items that fit in the list’s height
    SDL_Color backgroundColor, itemColor, selectedColor;
    int selectedIndex;              // Index of currently selected item

    void adjustScroll() {
        if (selectedIndex * itemHeight < scrollOffset) {
            scrollOffset = selectedIndex * itemHeight;
        } else if ((selectedIndex + 1) * itemHeight > scrollOffset + _coordinates.h) {
            scrollOffset = (selectedIndex + 1) * itemHeight - _coordinates.h;
        }
    }
};


#endif //SCROLLABLELIST_H
