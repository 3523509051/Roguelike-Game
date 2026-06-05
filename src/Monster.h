#ifndef MONSTER_H
#define MONSTER_H

#include "Entity.h"
#include "Map.h"
#include "Player.h"

class Monster : public Entity {
protected:
    int expReward_;
    int goldReward_;

public:
    Monster(const std::string& name, char sym, int x, int y,
            int hp, int atk, int def, int expR, int goldR, int spd = 10);

    int getExpReward() const;
    int getGoldReward() const;

    // 怪物AI：每个回合的行动
    virtual void takeTurn(Player& player, const Map& map) = 0;
};

// 具体怪物类型
// ===== 小怪（均用红色 M 表示） =====

// 史莱姆：随机移动，相邻才攻击
class Slime : public Monster {
public:
    Slime(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
};

// 蝙蝠：快速低防，40%追玩家
class Bat : public Monster {
public:
    Bat(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
};

// 骷髅：主动追踪玩家
class Skeleton : public Monster {
public:
    Skeleton(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
};

// 哥布林：中速，成群出现
class Goblin : public Monster {
public:
    Goblin(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
};

// 狼：速度快，会绕路包夹
class Wolf : public Monster {
public:
    Wolf(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
};

// ===== Boss（用紫色 B 表示） =====

// 地牢领主：追踪+回血
class Boss : public Monster {
private:
    int regenCounter_;
public:
    Boss(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
    bool isBoss() const override { return true; }
};

// 死亡骑士（第3层精英）：高攻高防，移速慢
class DeathKnight : public Monster {
public:
    DeathKnight(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
    bool isBoss() const override { return true; }
};

#endif
