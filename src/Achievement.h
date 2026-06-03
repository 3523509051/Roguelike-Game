#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <string>
#include <vector>
#include "Player.h"
#include "Monster.h"

enum class AchievementID {
    FIRST_KILL,       // 击败第一只怪物
    RICH_MAN,         // 累计500金币
    COMBO_MASTER,     // 连杀5只不受伤
    ESCAPE_ARTIST,    // 逃跑10次
    DRAGON_SLAYER,    // 击败地牢领主
    COLLECTOR,        // 拾取50个道具
    IMMORTAL,         // 一条命通关
    PERFECT_DEFENSE,  // 防御挡下100点伤害
    LOCKPICKER,       // 打开20个宝箱
    MAX_LEVEL         // 升到Lv.10
};

class Achievement {
private:
    AchievementID id_;
    std::string name_;
    std::string desc_;
    bool unlocked_;

public:
    Achievement(AchievementID id, const std::string& name, const std::string& desc);

    void unlock();
    bool isUnlocked() const;
    std::string getName() const;
    std::string getDesc() const;
    AchievementID getId() const;
};

class AchievementManager {
private:
    std::vector<Achievement> achievements_;
    int totalKills_;
    int totalGold_;
    int comboCount_;
    int escapeCount_;
    int blockedDamage_;
    int chestsOpened_;
    int itemsCollected_;

public:
    AchievementManager();

    // TODO: D实现 - 事件通知（Game在各种事件后调用）
    void onMonsterKilled(const Monster& monster);
    void onGoldGained(int amount);
    void onPlayerHit();         // 玩家受伤，连杀中断
    void onEscape();            // 逃跑成功
    void onDefenseBlock(int dmg); // 防御挡伤
    void onChestOpened();       // 打开宝箱
    void onItemCollected();     // 拾取道具
    void onLevelUp(int level);  // 升级
    void onVictory();           // 通关

    // TODO: D实现 - 显示
    void displayAll() const;    // 显示所有成就（用于主菜单）

    // TODO: D实现 - 存档
    void save(const std::string& filename) const;
    void load(const std::string& filename);
};

#endif // ACHIEVEMENT_H
