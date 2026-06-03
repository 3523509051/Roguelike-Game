#include "Monster.h"
#include <cstdlib>
#include <cmath>

Monster::Monster(const std::string& name, char sym, int x, int y,
                 int hp, int atk, int def, int expR, int goldR, int spd)
    : Entity(name, sym, x, y, hp, hp, atk, def, 1, spd),
      expReward_(expR), goldReward_(goldR) {}

int Monster::getExpReward() const { return expReward_; }
int Monster::getGoldReward() const { return goldReward_; }

static int dist(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

static void moveToward(Monster& m, const Player& p, const Map& map) {
    int dx = p.getPos().first - m.getPos().first;
    int dy = p.getPos().second - m.getPos().second;
    int nx = m.getPos().first;
    int ny = m.getPos().second;
    if (std::abs(dx) >= std::abs(dy))
        nx += (dx > 0) ? 1 : -1;
    else
        ny += (dy > 0) ? 1 : -1;
    if (map.isWalkable(nx, ny) && (nx != p.getPos().first || ny != p.getPos().second))
        m.setPos(nx, ny);
}

static void moveRandom(Monster& m, const Map& map) {
    int dir = rand() % 4;
    int nx = m.getPos().first, ny = m.getPos().second;
    switch (dir) { case 0: ny--; break; case 1: ny++; break; case 2: nx--; break; case 3: nx++; break; }
    if (map.isWalkable(nx, ny)) m.setPos(nx, ny);
}

Slime::Slime(int x, int y) : Monster("史莱姆", 'M', x, y, 8, 3, 1, 8, 2, 8) {}
void Slime::takeTurn(Player& player, const Map& map) {
    if (dist(x_, y_, player.getPos().first, player.getPos().second) <= 1) return;
    moveRandom(*this, map);
}

Bat::Bat(int x, int y) : Monster("蝙蝠", 'M', x, y, 5, 5, 0, 10, 3, 15) {}
void Bat::takeTurn(Player& player, const Map& map) {
    if (dist(x_, y_, player.getPos().first, player.getPos().second) <= 1) return;
    if (rand() % 100 < 40) moveToward(*this, player, map);
    else moveRandom(*this, map);
}

Skeleton::Skeleton(int x, int y) : Monster("骷髅", 'M', x, y, 15, 7, 3, 20, 8, 10) {}
void Skeleton::takeTurn(Player& player, const Map& map) {
    if (dist(x_, y_, player.getPos().first, player.getPos().second) <= 1) return;
    moveToward(*this, player, map);
}

Boss::Boss(int x, int y) : Monster("地牢领主", 'B', x, y, 50, 12, 5, 100, 50, 6), regenCounter_(0) {}  // ⭐速度6（Boss很慢但高攻高防）
void Boss::takeTurn(Player& player, const Map& map) {
    (void)map; // Boss 不会移动
    if (dist(x_, y_, player.getPos().first, player.getPos().second) <= 1) {
        regenCounter_++;
        if (regenCounter_ % 3 == 0) heal(5);
        return; // 玩家在相邻格 → 攻击会在Combat中处理
    }
    // 玩家不在旁边 → Boss原地不动，等待玩家过来
}

// ===== Goblin =====
Goblin::Goblin(int x, int y) : Monster("哥布林", 'M', x, y, 12, 5, 2, 15, 5, 11) {}  // ⭐速度11
void Goblin::takeTurn(Player& player, const Map& map) {
    if (dist(x_, y_, player.getPos().first, player.getPos().second) <= 1) return;
    // 50%概率追玩家
    if (rand() % 100 < 50) moveToward(*this, player, map);
    else moveRandom(*this, map);
}

// ===== Wolf =====
Wolf::Wolf(int x, int y) : Monster("狼", 'M', x, y, 10, 7, 1, 18, 6, 14) {}  // ⭐速度14（快）
void Wolf::takeTurn(Player& player, const Map& map) {
    if (dist(x_, y_, player.getPos().first, player.getPos().second) <= 1) return;
    // 70%追玩家，速度快
    if (rand() % 100 < 70) moveToward(*this, player, map);
    else moveRandom(*this, map);
    // 狼有机会走两步（二次移动）
    if (rand() % 100 < 30) {
        if (dist(x_, y_, player.getPos().first, player.getPos().second) <= 1) return;
        if (rand() % 100 < 70) moveToward(*this, player, map);
    }
}

// ===== DeathKnight =====
DeathKnight::DeathKnight(int x, int y) : Monster("死亡骑士", 'B', x, y, 35, 10, 6, 60, 30, 7) {}  // ⭐速度7（精英慢）
void DeathKnight::takeTurn(Player& player, const Map& map) {
    (void)map; // 死亡骑士不会移动
    if (dist(x_, y_, player.getPos().first, player.getPos().second) <= 1) return;
    // 玩家不在旁边 → 原地不动
}
