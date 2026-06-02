#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <utility>

enum class ItemType {
    HEAL_POTION,     // 回血
    ATTACK_BOOST,    // 加攻击
    DEFENSE_BOOST,   // 加防御
    GOLD_COIN        // 金币
};

class Item {
private:
    std::string name_;
    char symbol_;
    ItemType type_;
    int value_;
    int x_, y_;

public:
    Item(const std::string& name, char sym, ItemType type, int value, int x, int y);

    std::pair<int, int> getPos() const;
    std::string getName() const;
    char getSymbol() const;
    ItemType getType() const;
    int getValue() const;

    void setPosition(int nx, int ny);
};

#endif // ITEM_H
