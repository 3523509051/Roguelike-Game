#ifndef RENDER_H
#define RENDER_H

#include "Map.h"
#include "Player.h"
#include "Monster.h"
#include "Item.h"
#include <string>
#include <vector>

class Render {
public:
    Render();

    void clear();                // 清屏
    void drawHUD(const Player& player, int currentFloor);  // 状态栏
    void drawMap(const Map& map);                          // 地图
    void drawEntities(const Player& player,                // 玩家+怪物+道具
                      const std::vector<Monster*>& monsters,
                      const std::vector<Item*>& items);
    void drawMessage(const std::string& msg);   // 消息
    void drawAll(const Map& map, const Player& player,
                 const std::vector<Monster*>& monsters,
                 const std::vector<Item*>& items,
                 const std::string& message);   // 一次画全部
    void drawGameOver(int floor, int gold);     // 游戏结束
    void drawVictory();                          // 胜利画面

    // ⭐ 战斗界面
    void drawBattleScreen(const Player& player, const Monster& monster,
                          const std::vector<std::string>& battleLog,
                          const std::string& prompt);
};

#endif // RENDER_H
