#include "Achievement.h"
#include <iostream>
#include <fstream>

// ===== Achievement 类实现 =====
Achievement::Achievement(AchievementID id, const std::string& name, const std::string& desc)
    : id_(id), name_(name), desc_(desc), unlocked_(false) {}

void Achievement::unlock() {
    if (!unlocked_) {
        unlocked_ = true;
        std::cout << "🏆 成就解锁：" << name_ << std::endl;
    }
}

bool Achievement::isUnlocked() const { return unlocked_; }
std::string Achievement::getName() const { return name_; }
std::string Achievement::getDesc() const { return desc_; }
AchievementID Achievement::getId() const { return id_; }

// ===== AchievementManager 类实现 =====
AchievementManager::AchievementManager()
    : totalKills_(0), totalGold_(0), comboCount_(0),
      escapeCount_(0), blockedDamage_(0), chestsOpened_(0), itemsCollected_(0) {

    // TODO: D实现 - 初始化10个成就
    achievements_.push_back(Achievement(AchievementID::FIRST_KILL, "初次胜利", "击败第一只怪物"));
    achievements_.push_back(Achievement(AchievementID::RICH_MAN, "富豪", "累计获得500金币"));
    achievements_.push_back(Achievement(AchievementID::COMBO_MASTER, "连杀大师", "不受伤连续击败5只怪物"));
    achievements_.push_back(Achievement(AchievementID::ESCAPE_ARTIST, "逃跑艺术家", "成功逃跑10次"));
    achievements_.push_back(Achievement(AchievementID::DRAGON_SLAYER, "屠龙者", "击败地牢领主"));
    achievements_.push_back(Achievement(AchievementID::COLLECTOR, "收藏家", "拾取50个道具"));
    achievements_.push_back(Achievement(AchievementID::IMMORTAL, "不死战神", "一条命通关"));
    achievements_.push_back(Achievement(AchievementID::PERFECT_DEFENSE, "完美防御", "用防御挡下100点伤害"));
    achievements_.push_back(Achievement(AchievementID::LOCKPICKER, "开锁高手", "打开20个宝箱"));
    achievements_.push_back(Achievement(AchievementID::MAX_LEVEL, "满级英雄", "升到Lv.10"));
}

void AchievementManager::onMonsterKilled(const Monster& monster) {
    // TODO: D实现
    (void)monster;
}

void AchievementManager::onGoldGained(int amount) {
    // TODO: D实现
    (void)amount;
}

void AchievementManager::onPlayerHit() {
    // TODO: D实现
}

void AchievementManager::onEscape() {
    // TODO: D实现
}

void AchievementManager::onDefenseBlock(int dmg) {
    // TODO: D实现
    (void)dmg;
}

void AchievementManager::onChestOpened() {
    // TODO: D实现
}

void AchievementManager::onItemCollected() {
    // TODO: D实现
}

void AchievementManager::onLevelUp(int level) {
    // TODO: D实现
    (void)level;
}

void AchievementManager::onVictory() {
    // TODO: D实现
}

void AchievementManager::displayAll() const {
    // TODO: D实现 - 显示所有成就列表
    std::cout << "===== 成就列表 =====" << std::endl;
    for (const auto& ach : achievements_) {
        std::cout << (ach.isUnlocked() ? "[✓] " : "[ ] ");
        std::cout << ach.getName() << " - " << ach.getDesc() << std::endl;
    }
}

void AchievementManager::save(const std::string& filename) const {
    // TODO: D实现 - 保存到文件
    (void)filename;
}

void AchievementManager::load(const std::string& filename) {
    // TODO: D实现 - 从文件读取
    (void)filename;
}
