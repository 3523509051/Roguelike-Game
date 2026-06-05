cmake_minimum_required(VERSION 3.16)

project(RoguelikeGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dungeon
    src/Achievement.cpp
    src/Combat.cpp
    src/Entity.cpp
    src/Game.cpp
    src/Input.cpp
    src/Item.cpp
    src/main.cpp
    src/Map.cpp
    src/MapObject.cpp
    src/Monster.cpp
    src/Player.cpp
    src/Render.cpp
    src/Save.cpp
    src/Shop.cpp
    src/Sound.cpp
)

target_include_directories(dungeon PRIVATE src)

if (MSVC)
    target_compile_options(dungeon PRIVATE /utf-8)
endif()

add_executable(test_game
    test/test_game.cpp
    src/Achievement.cpp
    src/Combat.cpp
    src/Entity.cpp
    src/Game.cpp
    src/Input.cpp
    src/Item.cpp
    src/Map.cpp
    src/MapObject.cpp
    src/Monster.cpp
    src/Player.cpp
    src/Render.cpp
    src/Save.cpp
    src/Shop.cpp
    src/Sound.cpp
)

target_include_directories(test_game PRIVATE src)

if (MSVC)
    target_compile_options(test_game PRIVATE /utf-8)
endif()
