#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include "Monster.h"
#include "Item.h"
#include "Render.h"
#include "Input.h"
#include "Save.h"
#include "Achievement.h"
#include "Shop.h"
#include <vector>
#include <string>
#include "MapObject.h"   // ← 新增
class Game {
private:
    Map* map_;
    Player* player_;
        std::vector<Monster*> monsters_;
    std::vector<Item*> items_;
    std::vector<Trap*> traps_;        // ← 新增
    std::vector<Door*> doors_;        // ← 新增
    std::vector<Chest*> chests_;      // ← 新增
    Render* render_;
    Input* input_;
    AchievementManager* achievementMgr_;
    Shop* shop_;
    bool running_;
    bool gameOver_;
    bool victory_;
    bool returnToMenu_;
    std::string message_;

public:
    Game();
    ~Game();

    void run();      // 主循环
    void showMainMenu();
    void newGame();
    void continueGame();
    void showAchievements();
    void showHelp();
    void openShop();

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

    // ⭐ 战斗系统
    void enterBattle(Monster* monster);  // 进入全屏战斗
    std::vector<std::string> battleLog_;
    void addBattleLog(const std::string& msg);
};

#endif // GAME_H
