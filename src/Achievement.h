#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include "types.h"
#include <string>
#include <vector>

class Monster;

class Achievement {
private:
    AchievementID id_;
    std::string name_;
    std::string description_;
    bool unlocked_;

public:
    Achievement(AchievementID id, const std::string& name, const std::string& desc);

    void unlock();
    void setUnlocked(bool unlocked);
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
    bool hasDied_;

    Achievement* findAchievement(AchievementID id);
    const Achievement* findAchievement(AchievementID id) const;
    void unlockAchievement(AchievementID id);

public:
    AchievementManager();

    void onMonsterKilled(const Monster& monster);
    void onGoldGained(int amount);
    void onPlayerHit();
    void onPlayerDeath();
    void onEscape();
    void onDefenseBlock(int dmg);
    void onChestOpened();
    void onItemPickup();
    void onItemCollected();
    void onLevelUp(int level);
    void onVictory();

    void displayAll() const;
    bool isUnlocked(AchievementID id) const;
    std::string getUnlockMask() const;
    void loadUnlockMask(const std::string& mask);

    void save(const std::string& filename) const;
    void load(const std::string& filename);
};

#endif // ACHIEVEMENT_H
