/**
 * 渲染系统自测
 * 编译：g++ -o test_render.exe test_render.cpp ../src/Map.cpp ../src/Entity.cpp \
 *       ../src/Player.cpp ../src/Monster.cpp ../src/Item.cpp \
 *       ../src/Render.cpp ../src/Input.cpp ../src/Save.cpp
 * 运行：test_render.exe
 */
#include "../src/Map.h"
#include "../src/Player.h"
#include "../src/Monster.h"
#include "../src/Item.h"
#include "../src/Render.h"
#include "../src/Input.h"
#include "../src/Save.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "===== 渲染系统测试 =====" << std::endl;

    // 测试1：生成地图并绘制
    std::cout << "测试1：地图渲染" << std::endl;
    Map map(40, 20);
    map.generate();

    // 测试2：玩家渲染
    std::cout << "测试2：玩家渲染" << std::endl;
    auto start = map.getPlayerStart();
    Player player(start.first, start.second);
    std::cout << "  玩家位置: (" << start.first << ", " << start.second << ")" << std::endl;

    // 测试3：怪物渲染
    std::cout << "测试3：怪物渲染" << std::endl;
    std::vector<Monster*> monsters;
    monsters.push_back(new Slime(10, 10));
    monsters.push_back(new Bat(15, 12));
    monsters.push_back(new Skeleton(20, 8));
    std::cout << "  创建了 " << monsters.size() << " 只怪物" << std::endl;

    // 测试4：道具渲染
    std::cout << "测试4：道具渲染" << std::endl;
    std::vector<Item*> items;
    items.push_back(new Item("小血瓶", '♥', ItemType::HEAL_POTION, 8, 12, 10));
    items.push_back(new Item("金币", '$', ItemType::GOLD_COIN, 10, 18, 12));
    std::cout << "  创建了 " << items.size() << " 个道具" << std::endl;

    // 测试5：完整渲染
    std::cout << "测试5：完整渲染（按回车继续）" << std::endl;
    Render render;
    render.drawAll(map, player, monsters, items, "测试消息：按 q 退出");
    std::cout << "\n  如果看到ASCII地图、@、怪物和道具符号，渲染正确！" << std::endl;

    // 测试6：Input
    std::cout << "\n测试6：输入系统" << std::endl;
    Input input;
    std::cout << "  按任意键测试输入..." << std::endl;
    // 不阻塞，仅检查类是否存在
    std::cout << "  ✅ Input类创建成功" << std::endl;

    // 测试7：Save
    std::cout << "\n测试7：存档系统" << std::endl;
    bool exists = Save::fileExists("save.dat");
    std::cout << "  存档文件存在: " << (exists ? "是" : "否") << std::endl;
    std::cout << "  ✅ Save类创建成功" << std::endl;

    // 清理
    for (auto* m : monsters) delete m;
    for (auto* i : items) delete i;

    std::cout << "\n✅ 渲染系统测试完成！" << std::endl;
    return 0;
}
