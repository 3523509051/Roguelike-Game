#include "Entity.h"

Entity::Entity(const std::string& name, char sym, int x, int y,
               int hp, int maxHp, int atk, int def, int lv)
    : name_(name), symbol_(sym), x_(x), y_(y),
      hp_(hp), maxHp_(maxHp), attack_(atk), defense_(def), level_(lv) {}

std::pair<int, int> Entity::getPos() const { return {x_, y_}; }
void Entity::setPos(int nx, int ny) { x_ = nx; y_ = ny; }

int Entity::getHp() const { return hp_; }
int Entity::getMaxHp() const { return maxHp_; }
int Entity::getAttack() const { return attack_; }
int Entity::getDefense() const { return defense_; }
int Entity::getLevel() const { return level_; }
char Entity::getSymbol() const { return symbol_; }
std::string Entity::getName() const { return name_; }
bool Entity::isAlive() const { return hp_ > 0; }

int Entity::takeDamage(int dmg) {
    int actualDmg = (dmg < 0) ? 0 : dmg;
    actualDmg = (actualDmg > hp_) ? hp_ : actualDmg;
    hp_ -= actualDmg;
    return actualDmg;
}

void Entity::heal(int amount) {
    hp_ += amount;
    if (hp_ > maxHp_) hp_ = maxHp_;
}
