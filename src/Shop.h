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

    void initItems();
    void open(Player& player, Render& render, Input& input);
    bool buyItem(Player& player, int index);
};

#endif // SHOP_H
