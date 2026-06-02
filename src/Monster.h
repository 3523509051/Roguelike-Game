#ifndef MONSTER_H
#define MONSTER_H

#include "Entity.h"
#include "Map.h"

class Monster : public Entity {
protected:
    int expReward_;
    int goldReward_;

public:
    Monster(const std::string& name, char sym, int x, int y,
            int hp, int atk, int def, int expR, int goldR);

    int getExpReward() const;
    int getGoldReward() const;

    // 怪物AI：每个回合的行动
    virtual void takeTurn(Player& player, const Map& map) = 0;
};

// 具体怪物类型
class Slime : public Monster {
public:
    Slime(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
};

class Bat : public Monster {
public:
    Bat(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
};

class Skeleton : public Monster {
public:
    Skeleton(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
};

class Boss : public Monster {
private:
    int regenCounter_;
public:
    Boss(int x, int y);
    void takeTurn(Player& player, const Map& map) override;
};

#endif
