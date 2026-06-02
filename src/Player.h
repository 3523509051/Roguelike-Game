#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <vector>
#include <string>

class Player : public Entity {
private:
    int exp_;
    int expToNext_;
    std::vector<std::string> inventory_;
    int floor_;
    int gold_;

public:
    Player(int startX, int startY);

    // 经验与升级
    void gainExp(int amount);
    int getExp() const;
    int getExpToNext() const;

    // 背包
    bool addItem(const std::string& itemName);
    bool hasItem(const std::string& itemName) const;
    bool useItem(const std::string& itemName);
    std::vector<std::string> getInventory() const;

    // 金币
    void addGold(int amount);
    int getGold() const;

    // 楼层
    int getFloor() const;
    void nextFloor();

    // 重置
    void reset(int startX, int startY);
};

#endif // PLAYER_H
