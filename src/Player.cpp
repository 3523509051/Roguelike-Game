#include "Player.h"
#include <algorithm>
#include <iostream>

Player::Player(int startX, int startY)
    : Entity("玩家", '@', startX, startY, 20, 20, 6, 3, 1),
      exp_(0), expToNext_(30), floor_(1), gold_(0),
      skillAtkBuff_(0), skillAtkTurns_(0) {
    initSkills();
}

void Player::gainExp(int amount) {
    exp_ += amount;
    while (exp_ >= expToNext_) {
        level_++;
        exp_ -= expToNext_;
        expToNext_ = static_cast<int>(expToNext_ * 1.5f);

        int oldMaxHp = maxHp_;
        maxHp_ += 4;
        attack_ += 2;
        defense_ += 1;

        // HP按比例继承 + 2
        float ratio = static_cast<float>(hp_) / oldMaxHp;
        hp_ = static_cast<int>(ratio * maxHp_) + 2;
        if (hp_ > maxHp_) hp_ = maxHp_;

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

// ===== ⭐ 技能系统 =====

// 每个技能的各级效果定义
namespace {

// 重击：倍率  连斩：攻击次数  破甲：倍率(无视防御)  战吼：ATK buff / 持续回合
struct SkillDef { const char* name; char key; int cd[3]; float p1[3]; int p2[3]; int baseCost; };

const SkillDef SKILL_DEFS[] = {
    // name    key  cd[Lv1,Lv2,Lv3]   p1[Lv1,Lv2,Lv3]     p2[Lv1,Lv2,Lv3]    baseCost
    { "重击",  '1', {3, 3, 2},        {1.5f, 1.8f, 2.0f}, {0, 0, 0},          80 },
    { "连斩",  '2', {5, 4, 4},        {2.0f, 2.0f, 3.0f},  {0, 0, 0},         120 },
    { "破甲",  '3', {4, 3, 3},        {1.0f, 1.2f, 1.5f},  {0, 0, 0},         100 },
    { "战吼",  '4', {6, 5, 4},        {5.0f, 7.0f, 10.0f}, {3, 3, 4},         150 },
};

std::string makeSkillDesc(int index, int level) {
    if (level == 0) return "未解锁";
    auto& d = SKILL_DEFS[index];
    int lv = level - 1;
    switch (index) {
        case 0: return std::to_string(d.p1[lv]) + "x伤害 CD" + std::to_string(d.cd[lv]);   // 重击
        case 1: return std::to_string((int)d.p1[lv]) + "连击 CD" + std::to_string(d.cd[lv]); // 连斩
        case 2: return "无视防" + std::to_string(d.p1[lv]) + "x CD" + std::to_string(d.cd[lv]); // 破甲
        case 3: return "ATK+" + std::to_string((int)d.p1[lv]) + " " + std::to_string(d.p2[lv]) + "回合 CD" + std::to_string(d.cd[lv]); // 战吼
    }
    return "";
}
} // namespace

void Player::initSkills() {
    skills_.clear();
    for (int i = 0; i < 4; i++) {
        auto& d = SKILL_DEFS[i];
        skills_.push_back({d.name, makeSkillDesc(i, 0), d.key, 0,
                           d.cd[0], 0, d.p1[0], d.p2[0]});
    }
    // ⭐ 开局赠送 重击 Lv.1
    upgradeSkill(0);
    skillAtkBuff_ = 0;
    skillAtkTurns_ = 0;
}

std::vector<Skill>& Player::getSkills() {
    return skills_;
}

bool Player::isSkillUnlocked(int index) const {
    if (index < 0 || index >= static_cast<int>(skills_.size())) return false;
    return skills_[index].level > 0;
}

bool Player::useSkill(int index) {
    if (index < 0 || index >= static_cast<int>(skills_.size())) return false;
    if (skills_[index].level == 0) return false;    // 未解锁
    if (skills_[index].cooldownRemaining > 0) return false;
    skills_[index].cooldownRemaining = skills_[index].cooldownMax;
    return true;
}

bool Player::upgradeSkill(int index) {
    if (index < 0 || index >= static_cast<int>(skills_.size())) return false;
    int& lv = skills_[index].level;
    if (lv >= 3) return false;  // 已满级

    lv++;
    auto& d = SKILL_DEFS[index];
    int li = lv - 1;  // 数组索引
    skills_[index].cooldownMax = d.cd[li];
    skills_[index].cooldownRemaining = 0;
    skills_[index].param1 = d.p1[li];
    skills_[index].param2 = d.p2[li];
    skills_[index].desc = makeSkillDesc(index, lv);
    return true;
}

int Player::getSkillUpgradeCost(int index) const {
    if (index < 0 || index >= 4) return -1;
    int lv = skills_[index].level;
    if (lv >= 3) return -1;  // 已满级
    return SKILL_DEFS[index].baseCost * (lv + 1);  // Lv0→1: base, Lv1→2: 2x, Lv2→3: 3x
}

void Player::tickCooldowns() {
    for (auto& skill : skills_) {
        if (skill.cooldownRemaining > 0) {
            skill.cooldownRemaining--;
        }
    }
}

void Player::tickBuffs() {
    if (skillAtkTurns_ > 0) {
        skillAtkTurns_--;
        if (skillAtkTurns_ == 0) {
            skillAtkBuff_ = 0;
        }
    }
}

int Player::getEffectiveAtk() const {
    return attack_ + skillAtkBuff_;
}

bool Player::hasAtkBuff() const {
    return skillAtkTurns_ > 0;
}

int Player::getAtkBuffTurns() const {
    return skillAtkTurns_;
}

void Player::setAtkBuff(int value, int turns) {
    skillAtkBuff_ = value;
    skillAtkTurns_ = turns;
}

void Player::reset(int startX, int startY) {
    x_ = startX; y_ = startY;
    hp_ = maxHp_ = 20;
    attack_ = 6;
    defense_ = 3;
    level_ = 1;
    exp_ = 0;
    expToNext_ = 30;
    floor_ = 1;
    gold_ = 0;
    inventory_.clear();
    initSkills();
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
