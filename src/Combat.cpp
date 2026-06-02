#include "Combat.h"
#include <cstdlib>
#include <iostream>

int Combat::calcDamage(int atk, int def) {
    int base = atk - def;
    int variance = rand() % 5 - 2;  // -2 ~ +2
    int dmg = base + variance;
    return (dmg < 1) ? 1 : dmg;
}

bool Combat::playerAttack(Player& player, Monster& monster) {
    int dmg = calcDamage(player.getAttack(), monster.getDefense());
    int actual = monster.takeDamage(dmg);
    std::cout << player.getName() << " 攻击了 " << monster.getName()
              << "，造成 " << actual << " 点伤害！" << std::endl;
    if (!monster.isAlive()) {
        std::cout << monster.getName() << " 被击败了！" << std::endl;
        player.gainExp(monster.getExpReward());
        player.addGold(monster.getGoldReward());
        return true;
    }
    return false;
}

bool Combat::monsterAttack(Monster& monster, Player& player) {
    int dmg = calcDamage(monster.getAttack(), player.getDefense());
    int actual = player.takeDamage(dmg);
    std::cout << monster.getName() << " 攻击了 " << player.getName()
              << "，造成 " << actual << " 点伤害！" << std::endl;
    if (!player.isAlive()) {
        std::cout << "你被 " << monster.getName() << " 击败了..." << std::endl;
        return true;
    }
    return false;
}
