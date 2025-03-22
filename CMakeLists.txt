cmake_minimum_required(VERSION 3.16) # Adjust version to your system
project(pacman CXX)

# Find SDL2 and SDL2_image
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories("${CMAKE_SOURCE_DIR}/States")
include_directories("${CMAKE_SOURCE_DIR}/UI_Element")
set(SDL2_IMAGE_INCLUDE_DIRS "C:/msys64/mingw64/include/SDL2")
set(SDL2_IMAGE_LIBRARIES "C:/msys64/mingw64/lib/libSDL2_image.dll.a")
set(SDL2_TTF_LIBRARIES "C:/msys64/mingw64/lib/libSDL2_ttf.dll.a")
# Add the executable
add_executable(pacman
        main.cpp
        TileRender.cpp
        TileRender.h
        States/Game.cpp
        States/Game.h
        States/MenuState.cpp
        UI_Element/Button.cpp
        UI_Element/Button.h
        UI_Element/UiElement.h
        UI_Element/Image.cpp
        UI_Element/Image.h
        UI_Element/ScrollableList.cpp
        UI_Element/ScrollableList.h
        States/testState.cpp
        States/LevelSelectState.cpp
        States/LevelSelectState.h
        UI_Element/Slider.cpp
        UI_Element/Slider.h
)

# Include SDL2 and SDL2_image directories for this target
target_include_directories(pacman PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
)

# Link SDL2 and SDL2_image libraries to the target
target_link_libraries(pacman PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
)