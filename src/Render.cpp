#include "Render.h"
#include "config.h"
#include <iostream>
#include <cstdlib>

Render::Render() {}

void Render::clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Render::drawHUD(const Player& player, int currentFloor) {
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
    std::cout << "  地牢地下";
    for (int i = 0; i < currentFloor; i++) std::cout << "·";
    std::cout << "  HP: " << player.getHp() << "/" << player.getMaxHp();
    std::cout << "  ATK: " << player.getAttack() << "  DEF: " << player.getDefense();
    std::cout << "  金币: " << player.getGold();
    std::cout << "  层数: " << currentFloor << "/" << MAX_FLOORS;
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════" << std::endl;
}

void Render::drawMap(const Map& map) {
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            Tile t = map.getTile(x, y);
            switch (t) {
                case Tile::WALL:       std::cout << '#'; break;
                case Tile::FLOOR:      std::cout << '.'; break;
                case Tile::STAIRS_DOWN: std::cout << '>'; break;
            }
        }
        std::cout << std::endl;
    }
}

void Render::drawEntities(const Player& player,
                          const std::vector<Monster*>& monsters,
                          const std::vector<Item*>& items) {
    // 先画地图
    // 注：实际会由drawAll调用drawMap再覆盖实体
    // 这里仅用于逻辑分离
}

void Render::drawMessage(const std::string& msg) {
    std::cout << "───────────────────────────────────────────────────" << std::endl;
    std::cout << msg << std::endl;
    std::cout << "───────────────────────────────────────────────────" << std::endl;
}

void Render::drawAll(const Map& map, const Player& player,
                     const std::vector<Monster*>& monsters,
                     const std::vector<Item*>& items,
                     const std::string& message) {
    clear();
    drawHUD(player, player.getFloor());

    // 逐行绘制地图 + 覆盖实体
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            bool drawn = false;

            // 检查玩家
            if (x == player.getPos().first && y == player.getPos().second) {
                std::cout << '@';
                drawn = true;
            }
            // 检查道具
            if (!drawn) {
                for (Item* item : items) {
                    if (item && x == item->getPos().first && y == item->getPos().second) {
                        std::cout << item->getSymbol();
                        drawn = true;
                        break;
                    }
                }
            }
            // 检查怪物
            if (!drawn) {
                for (Monster* monster : monsters) {
                    if (monster && monster->isAlive() &&
                        x == monster->getPos().first && y == monster->getPos().second) {
                        std::cout << monster->getSymbol();
                        drawn = true;
                        break;
                    }
                }
            }
            // 画地形
            if (!drawn) {
                switch (map.getTile(x, y)) {
                    case Tile::WALL:       std::cout << '#'; break;
                    case Tile::FLOOR:      std::cout << '.'; break;
                    case Tile::STAIRS_DOWN: std::cout << '>'; break;
                }
            }
        }
        std::cout << std::endl;
    }

    // 底部图例
    std::cout << "  @=你  #=壁  .=路  M=史莱姆  B=蝙蝠  S=骷髅  D=Boss  !=道具  >=楼梯" << std::endl;
    drawMessage(message);
    std::cout << "  [wasd移动] [空格待机] [i背包] [q退出]" << std::endl;
}

void Render::drawGameOver(int floor, int gold) {
    clear();
    std::cout << std::endl;
    std::cout << "  ╔══════════════════════╗" << std::endl;
    std::cout << "  ║     💀 游戏结束 💀    ║" << std::endl;
    std::cout << "  ╚══════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "  到达层数: " << floor << std::endl;
    std::cout << "  获得金币: " << gold << std::endl;
    std::cout << std::endl;
}

void Render::drawVictory() {
    clear();
    std::cout << std::endl;
    std::cout << "  ╔══════════════════════╗" << std::endl;
    std::cout << "  ║  🏆 恭喜通关！ 🏆   ║" << std::endl;
    std::cout << "  ╚══════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "  你成功击败了地牢领主，逃出了地牢！" << std::endl;
    std::cout << std::endl;
}
