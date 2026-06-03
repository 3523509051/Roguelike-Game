#ifndef SHOP_H
#define SHOP_H

#include <string>
#include <vector>
#include "Player.h"
#include "Item.h"
#include "Render.h"
#include "Input.h"

struct ShopItem {
    std::string name;
    int price;
    ItemType type;
    int value;
};

class Shop {
private:
    std::vector<ShopItem> items_;

public:
    Shop();

    // TODO: D实现 - 显示商店界面并处理购买
    void open(Player& player, Render& render, Input& input);

    // TODO: D实现 - 购买道具
    bool buyItem(Player& player, int index);

private:
    // TODO: D实现 - 初始化商品列表
    void initItems();
};

#endif // SHOP_H
