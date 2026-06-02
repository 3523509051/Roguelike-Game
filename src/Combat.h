#ifndef COMBAT_H
#define COMBAT_H

#include "Player.h"
#include "Monster.h"

class Combat {
public:
    // 玩家攻击怪物。返回 true 表示怪物死亡。
    static bool playerAttack(Player& player, Monster& monster);

    // 怪物攻击玩家。返回 true 表示玩家死亡。
    static bool monsterAttack(Monster& monster, Player& player);

    // 伤害计算
    static int calcDamage(int attack, int defense);
};

#endif // COMBAT_H
