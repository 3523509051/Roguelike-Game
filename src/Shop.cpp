#include "Shop.h"
#include <iostream>

Shop::Shop() {
    initItems();
}

void Shop::initItems() {
    items_.clear();
    items_.push_back({"小血瓶", 15, ItemType::HEAL_POTION, 8});
    items_.push_back({"大血瓶", 40, ItemType::HEAL_POTION, 20});
    items_.push_back({"攻击卷轴", 30, ItemType::ATTACK_BOOST, 3});
    items_.push_back({"防御卷轴", 30, ItemType::DEFENSE_BOOST, 3});
    items_.push_back({"钥匙", 20, ItemType::KEY, 1});
    items_.push_back({"复活石", 100, ItemType::REVIVE, 1});

    // ⭐ 4本技能书（value=技能索引 0-3，价格动态更新）
    items_.push_back({"重击技能书", 80, ItemType::SKILL_TOME, 0});
    items_.push_back({"连斩技能书", 120, ItemType::SKILL_TOME, 1});
    items_.push_back({"破甲技能书", 100, ItemType::SKILL_TOME, 2});
    items_.push_back({"战吼技能书", 150, ItemType::SKILL_TOME, 3});
}

void Shop::open(Player& player, Render& render, Input& input) {
    (void)render;

    bool shopping = true;
    while (shopping) {
        std::cout << "\n";
        std::cout << "==============================" << std::endl;
        std::cout << "          神秘商店" << std::endl;
        std::cout << "==============================" << std::endl;
        std::cout << "你的金币: " << player.getGold() << std::endl;
        std::cout << "------------------------------" << std::endl;

        // 道具 1-6
        for (std::size_t i = 0; i < 6; ++i) {
            const ShopItem& item = items_[i];
            std::cout << (i + 1) << ". "
                      << item.name << "  "
                      << item.price << "金" << std::endl;
        }

        std::cout << "------------------------------" << std::endl;
        // ⭐ 技能书 7-0（动态价格和状态）
        for (int i = 0; i < 4; i++) {
            int idx = 6 + i;  // items_ 中的索引
            int lv = player.getSkills()[i].level;
            int cost = player.getSkillUpgradeCost(i);

            std::cout << (i + 7) << ". " << items_[idx].name << " ";
            if (lv >= 3) {
                std::cout << "[满级] ---" << std::endl;
            } else if (lv > 0) {
                std::cout << "Lv." << lv << "→" << (lv+1) << " " << cost << "金" << std::endl;
            } else {
                std::cout << "[未解锁] " << cost << "金" << std::endl;
            }
        }

        std::cout << "------------------------------" << std::endl;
        std::cout << "[1-6] 道具  [7-0] 技能  [q] 离开" << std::endl;
        std::cout << "请选择: ";

        char key = input.getKey();
        std::cout << key << std::endl;

        if (key == 'q' || key == 'Q') {
            shopping = false;
        } else if (key >= '1' && key <= '6') {
            buyItem(player, key - '1');
        } else if (key == '7' || key == '8' || key == '9' || key == '0') {
            int skillIdx = (key == '0') ? 9 : (key - '1');  // '7'→6, '8'→7, '9'→8, '0'→9
            buyItem(player, skillIdx);
        } else if (key >= '7' && key <= '9') {
            buyItem(player, key - '1');
        } else {
            std::cout << "无效输入，请重新选择。" << std::endl;
        }
    }
}

bool Shop::buyItem(Player& player, int index) {
    if (index < 0 || index >= static_cast<int>(items_.size())) {
        std::cout << "无效商品编号。" << std::endl;
        return false;
    }

    const ShopItem& item = items_[index];

    // ⭐ 技能书特殊处理
    if (item.type == ItemType::SKILL_TOME) {
        int skillIdx = item.value;  // 0-3
        if (!player.isSkillUnlocked(skillIdx) && player.getSkills()[skillIdx].level == 0) {
            // 首次购买 → 使用固定价格
        }
        int cost = player.getSkillUpgradeCost(skillIdx);
        if (cost < 0) {
            std::cout << "该技能已满级！" << std::endl;
            return false;
        }
        if (player.getGold() < cost) {
            std::cout << "金币不足！需要 " << cost << " 金。" << std::endl;
            return false;
        }
        if (!player.upgradeSkill(skillIdx)) {
            std::cout << "升级失败。" << std::endl;
            return false;
        }
        player.addGold(-cost);
        int newLv = player.getSkills()[skillIdx].level;
        std::cout << "购买成功！" << item.name << " → "
                  << player.getSkills()[skillIdx].name << " Lv." << newLv << std::endl;
        return true;
    }

    // 普通道具
    if (player.getGold() < item.price) {
        std::cout << "金币不足！" << std::endl;
        return false;
    }

    player.addGold(-item.price);
    player.addItem(item.name);

    std::cout << "购买成功：" << item.name << std::endl;
    return true;
}
