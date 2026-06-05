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

        for (std::size_t i = 0; i < items_.size(); ++i) {
            const ShopItem& item = items_[i];
            std::cout << (i + 1) << ". "
                      << item.name << "  "
                      << item.price << "金" << std::endl;
        }

        std::cout << "------------------------------" << std::endl;
        std::cout << "[1-6] 购买    [q] 离开" << std::endl;
        std::cout << "请选择: ";

        char key = input.getKey();
        std::cout << key << std::endl;

        if (key == 'q' || key == 'Q') {
            shopping = false;
        } else if (key >= '1' && key <= '6') {
            const int index = key - '1';
            buyItem(player, index);
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
    if (player.getGold() < item.price) {
        std::cout << "金币不足！" << std::endl;
        return false;
    }

    player.addGold(-item.price);
    player.addItem(item.name);

    std::cout << "购买成功：" << item.name << std::endl;
    return true;
}
