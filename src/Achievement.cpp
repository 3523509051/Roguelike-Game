#include "Achievement.h"
#include "Player.h"
#include "Monster.h"
#include <fstream>
#include <iostream>

Achievement::Achievement(AchievementID id, const std::string& name, const std::string& desc)
    : id_(id), name_(name), description_(desc), unlocked_(false) {}

void Achievement::unlock() {
    if (!unlocked_) {
        unlocked_ = true;
        std::cout << "成就解锁：" << name_ << std::endl;
    }
}

bool Achievement::isUnlocked() const {
    return unlocked_;
}

std::string Achievement::getName() const {
    return name_;
}

std::string Achievement::getDesc() const {
    return description_;
}

AchievementID Achievement::getId() const {
    return id_;
}

AchievementManager::AchievementManager()
    : totalKills_(0),
      totalGold_(0),
      comboCount_(0),
      escapeCount_(0),
      blockedDamage_(0),
      chestsOpened_(0),
      itemsCollected_(0),
      hasDied_(false) {
    achievements_.push_back(Achievement(AchievementID::FIRST_KILL, "初次胜利", "击败第一只怪物"));
    achievements_.push_back(Achievement(AchievementID::RICH_MAN, "富豪", "累计获得500金币"));
    achievements_.push_back(Achievement(AchievementID::COMBO_MASTER, "连杀大师", "不受伤连续击败5只怪物"));
    achievements_.push_back(Achievement(AchievementID::ESCAPE_ARTIST, "逃跑艺术家", "成功逃跑10次"));
    achievements_.push_back(Achievement(AchievementID::DRAGON_SLAYER, "屠龙者", "击败地牢领主或Boss"));
    achievements_.push_back(Achievement(AchievementID::COLLECTOR, "收藏家", "拾取50个道具"));
    achievements_.push_back(Achievement(AchievementID::IMMORTAL, "不死战神", "一条命通关"));
    achievements_.push_back(Achievement(AchievementID::PERFECT_DEFENSE, "完美防御", "用防御挡下100点伤害"));
    achievements_.push_back(Achievement(AchievementID::LOCKPICKER, "开锁高手", "打开20个宝箱"));
    achievements_.push_back(Achievement(AchievementID::MAX_LEVEL, "满级英雄", "升到Lv.10"));
}

Achievement* AchievementManager::findAchievement(AchievementID id) {
    for (auto& achievement : achievements_) {
        if (achievement.getId() == id) {
            return &achievement;
        }
    }
    return nullptr;
}

const Achievement* AchievementManager::findAchievement(AchievementID id) const {
    for (const auto& achievement : achievements_) {
        if (achievement.getId() == id) {
            return &achievement;
        }
    }
    return nullptr;
}

void AchievementManager::unlockAchievement(AchievementID id) {
    Achievement* achievement = findAchievement(id);
    if (achievement != nullptr && !achievement->isUnlocked()) {
        achievement->unlock();
    }
}

void AchievementManager::onMonsterKilled(const Monster& monster) {
    ++totalKills_;
    ++comboCount_;

    if (totalKills_ == 1) {
        unlockAchievement(AchievementID::FIRST_KILL);
    }

    if (comboCount_ >= 5) {
        unlockAchievement(AchievementID::COMBO_MASTER);
    }

    const std::string monsterName = monster.getName();
    if (monsterName.find("地牢领主") != std::string::npos ||
        monsterName.find("Boss") != std::string::npos) {
        unlockAchievement(AchievementID::DRAGON_SLAYER);
    }
}

void AchievementManager::onGoldGained(int amount) {
    if (amount <= 0) {
        return;
    }

    totalGold_ += amount;
    if (totalGold_ >= 500) {
        unlockAchievement(AchievementID::RICH_MAN);
    }
}

void AchievementManager::onPlayerHit() {
    comboCount_ = 0;
}

void AchievementManager::onPlayerDeath() {
    hasDied_ = true;
    comboCount_ = 0;
}

void AchievementManager::onEscape() {
    ++escapeCount_;
    if (escapeCount_ >= 10) {
        unlockAchievement(AchievementID::ESCAPE_ARTIST);
    }
}

void AchievementManager::onDefenseBlock(int dmg) {
    if (dmg <= 0) {
        return;
    }

    blockedDamage_ += dmg;
    if (blockedDamage_ >= 100) {
        unlockAchievement(AchievementID::PERFECT_DEFENSE);
    }
}

void AchievementManager::onChestOpened() {
    ++chestsOpened_;
    if (chestsOpened_ >= 20) {
        unlockAchievement(AchievementID::LOCKPICKER);
    }
}

void AchievementManager::onItemPickup() {
    ++itemsCollected_;
    if (itemsCollected_ >= 50) {
        unlockAchievement(AchievementID::COLLECTOR);
    }
}

void AchievementManager::onItemCollected() {
    onItemPickup();
}

void AchievementManager::onLevelUp(int level) {
    if (level >= 10) {
        unlockAchievement(AchievementID::MAX_LEVEL);
    }
}

void AchievementManager::onVictory() {
    if (!hasDied_) {
        unlockAchievement(AchievementID::IMMORTAL);
    }
}

void AchievementManager::displayAll() const {
    std::cout << "===== 成就列表 =====" << std::endl;
    for (const auto& achievement : achievements_) {
        std::cout << (achievement.isUnlocked() ? "[已解锁] " : "[未解锁] ")
                  << achievement.getName() << " - "
                  << achievement.getDesc() << std::endl;
    }
}

bool AchievementManager::isUnlocked(AchievementID id) const {
    const Achievement* achievement = findAchievement(id);
    return achievement != nullptr && achievement->isUnlocked();
}

std::string AchievementManager::getUnlockMask() const {
    std::string mask;
    for (const auto& achievement : achievements_) {
        mask.push_back(achievement.isUnlocked() ? '1' : '0');
    }
    return mask;
}

void AchievementManager::loadUnlockMask(const std::string& mask) {
    const std::size_t count = (mask.size() < achievements_.size()) ? mask.size() : achievements_.size();
    for (std::size_t i = 0; i < count; ++i) {
        if (mask[i] == '1') {
            achievements_[i].unlock();
        }
    }
}

void AchievementManager::save(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        return;
    }

    const std::string mask = getUnlockMask();
    out.write(mask.c_str(), static_cast<std::streamsize>(mask.size()));
}

void AchievementManager::load(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        return;
    }

    std::string mask;
    char ch;
    while (in.read(&ch, sizeof(ch))) {
        mask.push_back(ch);
    }
    loadUnlockMask(mask);
}
