#include "Shop.h"
#include <iostream>

Shop::Shop() {
    initItems();
}

void Shop::initItems() {
    // TODO: D实现 - 初始化6种商品
    items_.push_back({"小血瓶", 15, ItemType::HEAL_POTION, 8});
    items_.push_back({"大血瓶", 40, ItemType::HEAL_POTION, 20});
    items_.push_back({"攻击卷轴", 30, ItemType::ATTACK_BOOST, 3});
    items_.push_back({"防御卷轴", 30, ItemType::DEFENSE_BOOST, 3});
    // items_.push_back({"钥匙", 20, ItemType::KEY, 1});           // 需要先在 ItemType 中加 KEY
    // items_.push_back({"复活石", 100, ItemType::REVIVE, 1});     // 需要先在 ItemType 中加 REVIVE
}

void Shop::open(Player& player, Render& render, Input& input) {
    // TODO: D实现 - 显示商店界面
    // 循环：
    //   1. 清屏
    //   2. 显示商店标题："🏪 神秘商店"
    //   3. 显示玩家金币："你的金币: XXX"
    //   4. 列出所有商品（编号、名称、价格）
    //   5. 显示操作提示："[1-6]购买  [q]离开"
    //   6. 等待按键
    //   7. 如果按1-6，调用 buyItem()
    //   8. 如果按q，退出

    (void)player;
    (void)render;
    (void)input;

    std::cout << "TODO: D实现商店界面" << std::endl;
}

bool Shop::buyItem(Player& player, int index) {
    // TODO: D实现 - 购买逻辑
    // 1. 检查 index 是否有效
    // 2. 检查玩家金币是否足够
    // 3. 扣除金币
    // 4. 添加道具到背包
    // 5. 显示购买成功消息

    if (index < 0 || index >= (int)items_.size()) {
        return false;
    }

    ShopItem& item = items_[index];

    if (player.getGold() < item.price) {
        std::cout << "金币不足！" << std::endl;
        return false;
    }

    // TODO: 实际购买逻辑
    // player.addGold(-item.price);
    // player.addItem(item.name);

    (void)player;
    std::cout << "购买了 " << item.name << std::endl;
    return true;
}
