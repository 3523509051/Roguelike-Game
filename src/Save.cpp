#include "Save.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool Save::fileExists(const std::string& filename) {
    std::ifstream f(filename.c_str());
    return f.good();
}

bool Save::saveGame(const std::string& filename,
                    const Player& player, const Map& map,
                    const std::vector<Monster*>& monsters,
                    const std::vector<Item*>& items) {
    std::ofstream out(filename);
    if (!out) return false;

    // 玩家数据
    out << "FLOOR " << player.getFloor() << std::endl;
    out << "PLAYER " << player.getPos().first << " " << player.getPos().second
        << " " << player.getHp() << " " << player.getMaxHp()
        << " " << player.getAttack() << " " << player.getDefense()
        << " " << player.getLevel()
        << " " << player.getExp() << " " << player.getGold() << std::endl;

    // 背包
    out << "INVENTORY";
    for (const auto& item : player.getInventory()) {
        out << " " << item;
    }
    out << std::endl;

    // 地图（简化为尺寸+玩家起始点）
    out << "MAP " << map.getWidth() << " " << map.getHeight() << std::endl;
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            Tile t = map.getTile(x, y);
            switch (t) {
                case Tile::WALL:       out << '#'; break;
                case Tile::FLOOR:      out << '.'; break;
                case Tile::STAIRS_DOWN: out << '>'; break;
            }
        }
        out << std::endl;
    }
    out << "ENDMAP" << std::endl;

    // 怪物
    out << "MONSTERS " << monsters.size() << std::endl;
    for (auto* m : monsters) {
        if (m) {
            out << m->getName() << " " << m->getPos().first << " " << m->getPos().second
                << " " << m->getHp() << " " << m->getMaxHp()
                << " " << m->getAttack() << " " << m->getDefense()
                << " " << m->getExpReward() << " " << m->getGoldReward() << std::endl;
        }
    }

    // 道具
    out << "ITEMS " << items.size() << std::endl;
    for (auto* item : items) {
        if (item) {
            out << item->getName() << " " << (int)item->getType()
                << " " << item->getValue() << " " << item->getPos().first
                << " " << item->getPos().second << std::endl;
        }
    }

    out.close();
    return true;
}

bool Save::loadGame(const std::string& filename,
                    Player& player, Map& map,
                    std::vector<Monster*>& monsters,
                    std::vector<Item*>& items) {
    // 简化实现：仅占位
    // 实际使用时需完整解析 save.dat 格式
    return false;
}
