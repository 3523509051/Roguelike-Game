#include "Player.h"
#include <algorithm>
#include <iostream>

Player::Player(int startX, int startY)
    : Entity("玩家", '@', startX, startY, 20, 20, 5, 2, 1),
      exp_(0), expToNext_(20), floor_(1), gold_(0) {}

void Player::gainExp(int amount) {
    exp_ += amount;
    while (exp_ >= expToNext_) {
        level_++;
        exp_ -= expToNext_;
        expToNext_ = static_cast<int>(expToNext_ * 1.5f);
        maxHp_ += 5;
        attack_ += 2;
        defense_ += 1;
        hp_ = maxHp_;
        std::cout << "升级了！现在是 Lv." << level_ << std::endl;
    }
}

int Player::getExp() const { return exp_; }
int Player::getExpToNext() const { return expToNext_; }

bool Player::addItem(const std::string& itemName) {
    inventory_.push_back(itemName);
    return true;
}

bool Player::hasItem(const std::string& itemName) const {
    return std::find(inventory_.begin(), inventory_.end(), itemName) != inventory_.end();
}

bool Player::useItem(const std::string& itemName) {
    auto it = std::find(inventory_.begin(), inventory_.end(), itemName);
    if (it == inventory_.end()) return false;
    inventory_.erase(it);
    return true;
}

std::vector<std::string> Player::getInventory() const {
    return inventory_;
}

void Player::addGold(int amount) { gold_ += amount; }
int Player::getGold() const { return gold_; }

int Player::getFloor() const { return floor_; }
void Player::nextFloor() { floor_++; }

void Player::reset(int startX, int startY) {
    x_ = startX; y_ = startY;
    hp_ = maxHp_ = 20;
    attack_ = 5;
    defense_ = 2;
    level_ = 1;
    exp_ = 0;
    expToNext_ = 20;
    floor_ = 1;
    gold_ = 0;
    inventory_.clear();
}

void Player::loadState(int hp, int maxHp, int atk, int def, int level,
                       int exp, int expToNext, int gold, int floor,
                       int x, int y, const std::vector<std::string>& inventory) {
    hp_ = hp;
    maxHp_ = maxHp;
    attack_ = atk;
    defense_ = def;
    level_ = level;
    exp_ = exp;
    expToNext_ = expToNext;
    gold_ = gold;
    floor_ = floor;
    x_ = x;
    y_ = y;
    inventory_ = inventory;
}
