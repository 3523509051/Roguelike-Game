#include "MapObject.h"
#include <cstdlib>

// ===== Trap 实现 =====
Trap::Trap(TrapType type, int x, int y)
    : type_(type), x_(x), y_(y), triggered_(false) {}

TrapType Trap::getType() const { return type_; }
std::pair<int, int> Trap::getPos() const { return {x_, y_}; }
bool Trap::isTriggered() const { return triggered_; }

void Trap::trigger(Player& player) {
    if (triggered_) return;

    switch (type_) {
        case TrapType::SPIKE:
            player.takeDamage(5);
            break;
        case TrapType::POISON:
            player.takeDamage(3);
            break;
        case TrapType::TELEPORT:
            break;
    }

    triggered_ = true;
}


// ===== Door 实现 =====
Door::Door(int x, int y, bool locked)
    : locked_(locked), x_(x), y_(y) {}

std::pair<int, int> Door::getPos() const { return {x_, y_}; }
bool Door::isLocked() const { return locked_; }

char Door::getSymbol() const {
    return '+';  // 渲染时根据 isLocked() 决定颜色
}

bool Door::tryOpen(Player& player) {
    // TODO: A实现
    // if (locked_ && player.hasItem("钥匙")) {
    //     player.useItem("钥匙");
    //     locked_ = false;
    //     return true;
    // } else if (!locked_) {
    //     return true;
    // }
    // return false;
    (void)player;
    return false;
}

// ===== Chest 实现 =====
Chest::Chest(int x, int y)
    : isTrap_(rand() % 100 < 20), opened_(false), x_(x), y_(y) {
    generateContents();
}

Chest::~Chest() {
    for (auto* item : contents_) {
        delete item;
    }
}

std::pair<int, int> Chest::getPos() const { return {x_, y_}; }
bool Chest::isOpened() const { return opened_; }
bool Chest::isTrap() const { return isTrap_; }
char Chest::getSymbol() const { return 'C'; }  // 用'C'代表宝箱

void Chest::generateContents() {
    // TODO: A实现
    // 随机生成1-3个道具：
    // - 血瓶
    // - 攻击/防御卷轴
    // - 金币
    // - 钥匙
    // 如果 isTrap_ == true，则不生成道具（开启时扣血）
}

void Chest::open(Player& player) {
    // TODO: A实现
    // if (opened_) return;
    // opened_ = true;
    // if (isTrap_) {
    //     player.takeDamage(10);
    //     // 显示消息："这是个陷阱宝箱！受到10点伤害！"
    // } else {
    //     for (auto* item : contents_) {
    //         player.addItem(item->getName());
    //         // 显示消息："获得了 XXX"
    //     }
    // }
    (void)player;
}
