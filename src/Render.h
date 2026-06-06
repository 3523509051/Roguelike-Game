#ifndef RENDER_H
#define RENDER_H

#include "Map.h"
#include "Player.h"
#include "Monster.h"
#include "Item.h"
#include <string>
#include <vector>
#include <ostream>

class Render {
public:
    Render();

    void clear();                // 清屏（只移动光标到左上角，不擦除）
    void drawAll(const Map& map, const Player& player,
                 const std::vector<Monster*>& monsters,
                 const std::vector<Item*>& items,
                 const std::string& message);   // ⭐ 双缓冲绘制全部
    void drawGameOver(int floor, int gold);     // 游戏结束
    void drawVictory();                          // 胜利画面

    // ⭐ 战斗界面（双缓冲）
    void drawBattleScreen(const Player& player, const Monster& monster,
                          const std::vector<std::string>& battleLog,
                          const std::string& prompt);

private:
    // 内部版本 —— 都写到 ostream，供 drawAll / drawBattleScreen 复用
    void drawHUD(std::ostream& out, const Player& player, int currentFloor);
    void drawMessage(std::ostream& out, const std::string& msg);
};

#endif // RENDER_H
