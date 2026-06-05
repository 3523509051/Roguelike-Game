#include "Save.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace {

char tileToChar(Tile tile) {
    switch (tile) {
        case Tile::WALL: return '#';
        case Tile::FLOOR: return '.';
        case Tile::STAIRS_DOWN: return '>';
    }
    return '#';
}

Tile charToTile(char ch) {
    switch (ch) {
        case '.': return Tile::FLOOR;
        case '>': return Tile::STAIRS_DOWN;
        case '#':
        default: return Tile::WALL;
    }
}

std::string itemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::HEAL_POTION: return "HEAL_POTION";
        case ItemType::ATTACK_BOOST: return "ATTACK_BOOST";
        case ItemType::DEFENSE_BOOST: return "DEFENSE_BOOST";
        case ItemType::GOLD_COIN: return "GOLD_COIN";
        case ItemType::KEY: return "KEY";
        case ItemType::REVIVE: return "REVIVE";
    }
    return "HEAL_POTION";
}

ItemType stringToItemType(const std::string& text) {
    if (text == "ATTACK_BOOST") return ItemType::ATTACK_BOOST;
    if (text == "DEFENSE_BOOST") return ItemType::DEFENSE_BOOST;
    if (text == "GOLD_COIN") return ItemType::GOLD_COIN;
    if (text == "KEY") return ItemType::KEY;
    if (text == "REVIVE") return ItemType::REVIVE;
    return ItemType::HEAL_POTION;
}

char itemSymbol(ItemType type) {
    switch (type) {
        case ItemType::HEAL_POTION: return 'H';
        case ItemType::ATTACK_BOOST: return 'A';
        case ItemType::DEFENSE_BOOST: return 'D';
        case ItemType::GOLD_COIN: return '$';
        case ItemType::KEY: return 'K';
        case ItemType::REVIVE: return 'R';
    }
    return '?';
}

std::string itemName(ItemType type) {
    switch (type) {
        case ItemType::HEAL_POTION: return "血瓶";
        case ItemType::ATTACK_BOOST: return "攻击卷轴";
        case ItemType::DEFENSE_BOOST: return "防御卷轴";
        case ItemType::GOLD_COIN: return "金币";
        case ItemType::KEY: return "钥匙";
        case ItemType::REVIVE: return "复活石";
    }
    return "道具";
}

std::string monsterTypeName(const Monster& monster) {
    const std::string name = monster.getName();
    if (name == "史莱姆") return "SLIME";
    if (name == "蝙蝠") return "BAT";
    if (name == "骷髅") return "SKELETON";
    if (name == "哥布林") return "GOBLIN";
    if (name == "狼") return "WOLF";
    if (name == "地牢领主") return "BOSS";
    if (name == "死亡骑士") return "DEATH_KNIGHT";
    return name;
}

Monster* createMonster(const std::string& type, int x, int y, int hp) {
    Monster* monster = nullptr;
    if (type == "SLIME" || type == "史莱姆") monster = new Slime(x, y);
    else if (type == "BAT" || type == "蝙蝠") monster = new Bat(x, y);
    else if (type == "SKELETON" || type == "骷髅") monster = new Skeleton(x, y);
    else if (type == "GOBLIN" || type == "哥布林") monster = new Goblin(x, y);
    else if (type == "WOLF" || type == "狼") monster = new Wolf(x, y);
    else if (type == "BOSS" || type == "地牢领主") monster = new Boss(x, y);
    else if (type == "DEATH_KNIGHT" || type == "死亡骑士") monster = new DeathKnight(x, y);
    else monster = new Slime(x, y);

    if (hp < monster->getHp()) {
        monster->takeDamage(monster->getHp() - hp);
    }
    return monster;
}

void clearMonsters(std::vector<Monster*>& monsters) {
    for (auto* monster : monsters) {
        delete monster;
    }
    monsters.clear();
}

void clearItems(std::vector<Item*>& items) {
    for (auto* item : items) {
        delete item;
    }
    items.clear();
}

} // namespace

bool Save::fileExists(const std::string& filename) {
    std::ifstream f(filename.c_str());
    return f.good();
}

bool Save::saveGame(const std::string& filename,
                    const Player& player,
                    const Map& map,
                    const std::vector<Monster*>& monsters,
                    const std::vector<Item*>& items) {
    AchievementManager achMgr;
    return saveGame(filename, player, map, monsters, items, achMgr);
}

bool Save::saveGame(const std::string& filename,
                    const Player& player,
                    const Map& map,
                    const std::vector<Monster*>& monsters,
                    const std::vector<Item*>& items,
                    const AchievementManager& achMgr) {
    std::ofstream out(filename);
    if (!out) {
        return false;
    }

    const auto playerPos = player.getPos();
    out << "FLOOR " << player.getFloor() << std::endl;
    out << "PLAYER "
        << player.getHp() << " "
        << player.getMaxHp() << " "
        << player.getAttack() << " "
        << player.getDefense() << " "
        << player.getLevel() << " "
        << player.getExp() << " "
        << player.getExpToNext() << " "
        << player.getGold() << " "
        << playerPos.first << " "
        << playerPos.second << std::endl;

    out << "INVENTORY";
    for (const auto& item : player.getInventory()) {
        out << " " << item;
    }
    out << std::endl;

    out << "ACHIEVEMENTS " << achMgr.getUnlockMask() << std::endl;

    out << "MAP" << std::endl;
    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            out << tileToChar(map.getTile(x, y));
        }
        out << std::endl;
    }
    out << "ENDMAP" << std::endl;

    int monsterCount = 0;
    for (const auto* monster : monsters) {
        if (monster != nullptr) {
            ++monsterCount;
        }
    }

    out << "MONSTERS " << monsterCount << std::endl;
    for (const auto* monster : monsters) {
        if (monster == nullptr) {
            continue;
        }
        const auto pos = monster->getPos();
        out << monsterTypeName(*monster) << " "
            << pos.first << " "
            << pos.second << " "
            << monster->getHp() << std::endl;
    }

    int itemCount = 0;
    for (const auto* item : items) {
        if (item != nullptr) {
            ++itemCount;
        }
    }

    out << "ITEMS " << itemCount << std::endl;
    for (const auto* item : items) {
        if (item == nullptr) {
            continue;
        }
        const auto pos = item->getPos();
        out << itemTypeToString(item->getType()) << " "
            << pos.first << " "
            << pos.second << " "
            << item->getValue() << std::endl;
    }

    return true;
}

bool Save::loadGame(const std::string& filename,
                    Player& player,
                    Map& map,
                    std::vector<Monster*>& monsters,
                    std::vector<Item*>& items) {
    AchievementManager achMgr;
    return loadGame(filename, player, map, monsters, items, achMgr);
}

bool Save::loadGame(const std::string& filename,
                    Player& player,
                    Map& map,
                    std::vector<Monster*>& monsters,
                    std::vector<Item*>& items,
                    AchievementManager& achMgr) {
    std::ifstream in(filename);
    if (!in) {
        return false;
    }

    std::string tag;
    int floor = 1;
    if (!(in >> tag >> floor) || tag != "FLOOR") {
        return false;
    }

    int hp = 0;
    int maxHp = 0;
    int attack = 0;
    int defense = 0;
    int level = 0;
    int exp = 0;
    int expToNext = 0;
    int gold = 0;
    int x = 0;
    int y = 0;
    if (!(in >> tag >> hp >> maxHp >> attack >> defense >> level
          >> exp >> expToNext >> gold >> x >> y) || tag != "PLAYER") {
        return false;
    }

    std::vector<std::string> inventory;
    std::string line;
    std::getline(in, line);
    if (!std::getline(in, line)) {
        return false;
    }
    std::istringstream inventoryStream(line);
    inventoryStream >> tag;
    if (tag != "INVENTORY") {
        return false;
    }
    std::string inventoryItem;
    while (inventoryStream >> inventoryItem) {
        inventory.push_back(inventoryItem);
    }

    std::string achievementMask;
    if (!(in >> tag >> achievementMask) || tag != "ACHIEVEMENTS") {
        return false;
    }
    achMgr.loadUnlockMask(achievementMask);

    if (!(in >> tag) || tag != "MAP") {
        return false;
    }

    std::getline(in, line);
    for (int row = 0; row < map.getHeight(); ++row) {
        if (!std::getline(in, line)) {
            return false;
        }
        for (int col = 0; col < map.getWidth() && col < static_cast<int>(line.size()); ++col) {
            map.setTile(col, row, charToTile(line[col]));
        }
    }

    if (!(in >> tag) || tag != "ENDMAP") {
        return false;
    }

    clearMonsters(monsters);
    int monsterCount = 0;
    if (!(in >> tag >> monsterCount) || tag != "MONSTERS") {
        return false;
    }
    for (int i = 0; i < monsterCount; ++i) {
        std::string type;
        int mx = 0;
        int my = 0;
        int mhp = 0;
        if (!(in >> type >> mx >> my >> mhp)) {
            clearMonsters(monsters);
            return false;
        }
        monsters.push_back(createMonster(type, mx, my, mhp));
    }

    clearItems(items);
    int itemCount = 0;
    if (!(in >> tag >> itemCount) || tag != "ITEMS") {
        clearMonsters(monsters);
        return false;
    }
    for (int i = 0; i < itemCount; ++i) {
        std::string typeName;
        int ix = 0;
        int iy = 0;
        int value = 0;
        if (!(in >> typeName >> ix >> iy >> value)) {
            clearMonsters(monsters);
            clearItems(items);
            return false;
        }
        const ItemType type = stringToItemType(typeName);
        items.push_back(new Item(itemName(type), itemSymbol(type), type, value, ix, iy));
    }

    player.loadState(hp, maxHp, attack, defense, level,
                     exp, expToNext, gold, floor, x, y, inventory);
    return true;
}
