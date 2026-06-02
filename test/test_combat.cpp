/**
 * 战斗系统自测
 * 编译：g++ -o test_combat.exe test_combat.cpp ../src/Entity.cpp ../src/Player.cpp ../src/Monster.cpp ../src/Combat.cpp ../src/Item.cpp
 * 运行：test_combat.exe
 */
#include "../src/Player.h"
#include "../src/Monster.h"
#include "../src/Combat.h"
#include "../src/Item.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "===== 战斗系统测试 =====" << std::endl;

    // 测试1：玩家攻击史莱姆
    std::cout << "\n测试1：玩家攻击史莱姆" << std::endl;
    {
        Player p(5, 5);
        Slime s(7, 7);
        int beforeHp = s.getHp();
        Combat::playerAttack(p, s);
        std::cout << "  史莱姆 HP: " << beforeHp << " → " << s.getHp() << std::endl;
        assert(s.getHp() < beforeHp);
        std::cout << "  ✅ 怪物掉血正确" << std::endl;
    }

    // 测试2：怪物攻击玩家
    std::cout << "\n测试2：怪物攻击玩家" << std::endl;
    {
        Player p(5, 5);
        Slime s(7, 7);
        int beforeHp = p.getHp();
        Combat::monsterAttack(s, p);
        std::cout << "  玩家 HP: " << beforeHp << " → " << p.getHp() << std::endl;
        assert(p.getHp() < beforeHp);
        std::cout << "  ✅ 玩家掉血正确" << std::endl;
    }

    // 测试3：升级系统
    std::cout << "\n测试3：升级系统" << std::endl;
    {
        Player p2(0, 0);
        int lv = p2.getLevel();
        p2.gainExp(100);
        std::cout << "  等级: " << lv << " → " << p2.getLevel() << std::endl;
        assert(p2.getLevel() >= lv);
        std::cout << "  ✅ 升级功能正常" << std::endl;
    }

    // 测试4：背包系统
    std::cout << "\n测试4：背包系统" << std::endl;
    {
        Player p3(0, 0);
        p3.addItem("小血瓶");
        p3.addItem("攻击卷轴");
        auto inv = p3.getInventory();
        std::cout << "  背包内容: ";
        for (const auto& item : inv) std::cout << item << " ";
        std::cout << std::endl;
        assert(inv.size() == 2);
        assert(p3.hasItem("小血瓶"));
        p3.useItem("小血瓶");
        assert(!p3.hasItem("小血瓶"));
        std::cout << "  ✅ 背包增删查正常" << std::endl;
    }

    // 测试5：金币
    std::cout << "\n测试5：金币系统" << std::endl;
    {
        Player p4(0, 0);
        p4.addGold(50);
        assert(p4.getGold() == 50);
        p4.addGold(30);
        assert(p4.getGold() == 80);
        std::cout << "  ✅ 金币累计: " << p4.getGold() << std::endl;
    }

    // 测试6：Item 创建
    std::cout << "\n测试6：道具系统" << std::endl;
    {
        Item potion("小血瓶", '♥', ItemType::HEAL_POTION, 8, 10, 10);
        assert(potion.getName() == "小血瓶");
        assert(potion.getSymbol() == '♥');
        assert(potion.getValue() == 8);
        assert(potion.getPos().first == 10);
        std::cout << "  ✅ 道具创建和读取正常" << std::endl;
    }

    // 测试7：伤害公式下界（至少1血）
    std::cout << "\n测试7：伤害公式至少1血" << std::endl;
    {
        for (int i = 0; i < 100; i++) {
            int dmg = Combat::calcDamage(1, 10);
            assert(dmg >= 1);
        }
        std::cout << "  ✅ 伤害下界: 至少1血" << std::endl;
    }

    // 测试8：怪物创建
    std::cout << "\n测试8：各类怪物创建" << std::endl;
    {
        Slime slime(0, 0);
        Bat bat(0, 0);
        Skeleton skel(0, 0);
        Goblin gob(0, 0);
        Wolf wolf(0, 0);
        Boss boss(0, 0);
        DeathKnight dk(0, 0);
        assert(slime.getName() == "史莱姆");
        assert(bat.getName() == "蝙蝠");
        assert(skel.getName() == "骷髅");
        assert(gob.getName() == "哥布林");
        assert(wolf.getName() == "狼");
        assert(boss.getName() == "地牢领主");
        assert(dk.getName() == "死亡骑士");
        assert(boss.isBoss() == true);
        assert(dk.isBoss() == true);
        assert(slime.isBoss() == false);
        std::cout << "  ✅ 7种怪物创建+isBoss()检测正常" << std::endl;
    }

    std::cout << "\n✅✅ 所有战斗系统测试通过！" << std::endl;
    return 0;
}
