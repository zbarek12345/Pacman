cmake_minimum_required(VERSION 3.16) # Adjust version to your system
project(pacman CXX)
# Find SDL2 and SDL2_image
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(SDL2_image REQUIRED CONFIG COMPONENTS SDL2_image)
find_package(SDL2_ttf REQUIRED CONFIG COMPONENTS SDL2_ttf)
set(SQLite3_INCLUDE_DIR "C:/msys64/mingw64/include")
set(SQLite3_LIBRARY "C:/msys64/mingw64/lib/libsqlite3.dll.a")
find_package(SQLite3 REQUIRED SQLite3)
include_directories("${CMAKE_SOURCE_DIR}/States")
include_directories("${CMAKE_SOURCE_DIR}/UI_Element")
include_directories("${CMAKE_SOURCE_DIR}/Database")
# Add the executable
add_executable(pacman
        main.cpp
        TileRender.cpp
        States/Game.cpp
        States/MenuState.cpp
        UI_Element/Button.cpp
        UI_Element/UiElement.h
        UI_Element/Image.cpp
        UI_Element/ScrollableList.cpp
        States/testState.cpp
        States/LevelSelectState.cpp
        UI_Element/Slider.cpp
        Database/DatabaseController.cpp
        Database/DatabaseController.h
        States/PlayState.cpp
        States/PlayState.h
        States/GameState.cpp
        UI_Element/Label.cpp
        UI_Element/Label.h
        UI_Element/Panel.cpp
        UI_Element/Panel.h
)

add_compile_options(--static -03 -march=native)

# Include SDL2 and SDL2_image directories for this target
target_include_directories(pacman PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SQLite3_INCLUDE_DIRS}
)

# Link SDL2 and SDL2_image libraries to the target
target_link_libraries(pacman PRIVATE SDL2::SDL2main)
target_link_libraries(pacman PRIVATE
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
        SDL2_image::SDL2_image
        SQLite::SQLite3
)