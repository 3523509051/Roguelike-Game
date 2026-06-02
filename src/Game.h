#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include "Monster.h"
#include "Item.h"
#include "Render.h"
#include "Input.h"
#include "Save.h"
#include <vector>
#include <string>

class Game {
private:
    Map* map_;
    Player* player_;
    std::vector<Monster*> monsters_;
    std::vector<Item*> items_;
    Render* render_;
    Input* input_;
    bool running_;
    bool gameOver_;
    bool victory_;
    std::string message_;

public:
    Game();
    ~Game();

    void run();      // 主循环

private:
    void init();         // 初始化第一层
    void update(char key);
    void movePlayer(int dx, int dy);
    void processMonsters();
    void checkEvents();

    void initFloor();    // 初始化一层
    void spawnMonsters(int count);
    void spawnItems(int count);
    void nextFloor();
    void gameOver();
    void victory();
    void showInventory();

    void setMessage(const std::string& msg);
};

#endif // GAME_H
