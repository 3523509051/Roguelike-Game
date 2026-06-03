#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <utility>

class Entity {
protected:
    std::string name_;
    char symbol_;
    int x_, y_;
    int hp_, maxHp_;
    int attack_, defense_;
    int level_;
    int speed_;           // ⭐速度值（决定ATB增长快慢）
    float atbGauge_;      // ⭐ATB时间条（0.0-100.0，满了就能行动）

public:
    Entity(const std::string& name, char sym, int x, int y,
           int hp, int maxHp, int atk, int def, int lv = 1, int spd = 10);
    virtual ~Entity() = default;

    // 位置
    std::pair<int, int> getPos() const;
    void setPos(int nx, int ny);

    // 属性读取
    int getHp() const;
    int getMaxHp() const;
    int getAttack() const;
    int getDefense() const;
    int getLevel() const;
    char getSymbol() const;
    std::string getName() const;
    bool isAlive() const;

    // ⭐ ATB系统
    int getSpeed() const;
    float getAtbGauge() const;
    void updateAtb(float deltaTime);  // 增加ATB槽
    void resetAtb();                  // 行动后重置ATB
    bool isAtbFull() const;           // 是否满槽可行动

    // 战斗相关
    virtual int takeDamage(int dmg);
    virtual void heal(int amount);
    virtual bool isBoss() const { return false; }
};

#endif // ENTITY_H
