#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <string>
#include <vector>
#include "Player.h"
#include "Item.h"

// ===== 陷阱系统 =====
enum class TrapType {
    SPIKE,      // 地刺 -5HP
    POISON,     // 毒雾，中毒3回合
    TELEPORT    // 传送到随机房间
};

class Trap {
private:
    TrapType type_;
    int x_, y_;
    bool triggered_;

public:
    Trap(TrapType type, int x, int y);

    TrapType getType() const;
    std::pair<int, int> getPos() const;
    bool isTriggered() const;

    void trigger(Player& player);  // TODO: A实现 - 玩家踩到陷阱的效果
};

// ===== 门系统 =====
class Door {
private:
    bool locked_;
    int x_, y_;

public:
    Door(int x, int y, bool locked);

    std::pair<int, int> getPos() const;
    bool isLocked() const;
    char getSymbol() const;  // 锁门红色'+'，普通门白色'+'

    bool tryOpen(Player& player);  // TODO: A实现 - 尝试开门（需要钥匙）
};

// ===== 宝箱系统 =====
class Chest {
private:
    std::vector<Item*> contents_;
    bool isTrap_;   // 20%概率是陷阱宝箱
    bool opened_;
    int x_, y_;

public:
    Chest(int x, int y);
    ~Chest();

    std::pair<int, int> getPos() const;
    bool isOpened() const;
    bool isTrap() const;
    char getSymbol() const;  // '□'

    void open(Player& player);  // TODO: A实现 - 打开宝箱，给玩家道具或触发陷阱

private:
    void generateContents();    // TODO: A实现 - 随机生成宝箱内容物
};

#endif // MAPOBJECT_H
