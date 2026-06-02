#include "Item.h"

Item::Item(const std::string& name, char sym, ItemType type, int value, int x, int y)
    : name_(name), symbol_(sym), type_(type), value_(value), x_(x), y_(y) {}

std::pair<int, int> Item::getPos() const { return {x_, y_}; }
std::string Item::getName() const { return name_; }
char Item::getSymbol() const { return symbol_; }
ItemType Item::getType() const { return type_; }
int Item::getValue() const { return value_; }

void Item::setPosition(int nx, int ny) { x_ = nx; y_ = ny; }
