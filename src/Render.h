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

    void clear();
    void drawHUD(const Player& player, int currentFloor);
    void drawMap(const Map& map);
    void drawEntities(const Player& player,
                      const std::vector<Monster*>& monsters,
                      const std::vector<Item*>& items);
    void drawMessage(const std::string& msg);
    void drawAll(const Map& map, const Player& player,
                 const std::vector<Monster*>& monsters,
                 const std::vector<Item*>& items,
                 const std::string& message);
    void drawGameOver(int floor, int gold);
    void drawVictory();

    void drawBattleScreen(const Player& player, const Monster& monster,
                          const std::vector<std::string>& battleLog,
                          const std::string& prompt);

private:
    // 内部版本 —— 写到 ostream，供 drawAll / drawBattleScreen 双缓冲复用
    void drawHUD(std::ostream& out, const Player& player, int currentFloor);
    void drawMessage(std::ostream& out, const std::string& msg);
};

#endif // RENDER_H
