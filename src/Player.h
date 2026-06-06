#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <vector>
#include <string>

// ⭐ 技能结构
struct Skill {
    std::string name;
    std::string desc;       // 动态描述（含当前等级效果）
    char key;               // 快捷键 1-4
    int level;              // 0=未解锁, 1-3=等级
    int cooldownMax;        // 冷却回合数
    int cooldownRemaining;  // 剩余冷却（0=可用）
    float param1;           // 效果参数1：倍率 或 攻击次数
    int param2;             // 效果参数2：buff回合数 或 buff攻击值
};

class Player : public Entity {
private:
    int exp_;
    int expToNext_;
    std::vector<std::string> inventory_;
    int floor_;
    int gold_;

    // ⭐ 技能系统
    std::vector<Skill> skills_;
    int skillAtkBuff_;      // 战吼攻击增益
    int skillAtkTurns_;     // 增益剩余回合

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

    // ⭐ 技能系统
    void initSkills();                  // 初始化4个空技能（全level=0）
    std::vector<Skill>& getSkills();
    bool useSkill(int index);           // 使用技能，返回是否成功（需已解锁+不在CD）
    bool upgradeSkill(int index);       // 升级/解锁技能，返回是否成功（已达上限则false）
    int getSkillUpgradeCost(int index) const;  // 升级所需金币
    bool isSkillUnlocked(int index) const;     // 是否已解锁

    void tickCooldowns();               // 每回合减少冷却
    void tickBuffs();                   // 每回合减少增益时间
    int getEffectiveAtk() const;        // 攻击力（含增益）
    bool hasAtkBuff() const;            // 是否有攻击增益
    int getAtkBuffTurns() const;        // 增益剩余回合
    void setAtkBuff(int value, int turns);  // ⭐ 设置攻击增益

    // 重置
    void reset(int startX, int startY);
    void loadState(int hp, int maxHp, int atk, int def, int level,
                   int exp, int expToNext, int gold, int floor,
                   int x, int y, const std::vector<std::string>& inventory);
};

#endif // PLAYER_H
