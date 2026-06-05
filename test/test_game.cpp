#include "../src/Achievement.h"
#include "../src/Game.h"
#include "../src/Monster.h"
#include "../src/Player.h"
#include "../src/Save.h"
#include "../src/Shop.h"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "===== 游戏系统模块自测 =====" << std::endl;

    std::cout << "\n[测试1] 成就系统基本触发" << std::endl;
    AchievementManager achMgr;
    Slime slime(1, 1);
    achMgr.onMonsterKilled(slime);
    assert(achMgr.isUnlocked(AchievementID::FIRST_KILL));
    std::cout << "  FIRST_KILL 解锁成功" << std::endl;

    achMgr.onGoldGained(500);
    assert(achMgr.isUnlocked(AchievementID::RICH_MAN));
    std::cout << "  RICH_MAN 解锁成功" << std::endl;

    std::cout << "\n[测试2] 商店购买模拟" << std::endl;
    Player player(0, 0);
    player.addGold(100);
    Shop shop;
    bool bought = shop.buyItem(player, 0);
    assert(bought);
    assert(player.getGold() == 85);
    assert(player.hasItem("小血瓶"));
    std::cout << "  购买小血瓶成功，金币剩余: " << player.getGold() << std::endl;

    std::cout << "\n[测试3] 存档存在性检测" << std::endl;
    bool exists = Save::fileExists("save.dat");
    std::cout << "  save.dat " << (exists ? "存在" : "不存在") << std::endl;

    std::cout << "\n[测试4] 菜单显示（手动观察）" << std::endl;
    std::cout << "  接下来会显示主菜单，可输入 5 退出测试。" << std::endl;
    Game game;
    game.showMainMenu();

    std::cout << "\n所有游戏系统自测完成。" << std::endl;
    return 0;
}
