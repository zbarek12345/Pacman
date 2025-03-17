cmake_minimum_required(VERSION 3.16) # Adjust version to your system
project(pacman CXX)

# Find SDL2 and SDL2_image
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
set(SDL2_IMAGE_INCLUDE_DIRS "C:/msys64/mingw64/include/SDL2")
set(SDL2_IMAGE_LIBRARIES "C:/msys64/mingw64/lib/libSDL2_image.dll.a")
# Add the executable
add_executable(pacman
        main.cpp
        TileRender.cpp
        TileRender.h
)

# Include SDL2 and SDL2_image directories for this target
target_include_directories(pacman PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
)

# Link SDL2 and SDL2_image libraries to the target
target_link_libraries(pacman PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
)