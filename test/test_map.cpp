/**
 * 地图系统自测
 * 编译：g++ -o test_map.exe test_map.cpp ../src/Map.cpp
 * 运行：test_map.exe
 */
#include "../src/Map.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "===== 地图生成测试 =====" << std::endl;

    // 测试1：生成地图
    std::cout << "测试1：生成60x30地图" << std::endl;
    Map map(60, 30);
    map.generate();

    // 打印地图
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            Tile t = map.getTile(x, y);
            switch (t) {
                case Tile::WALL:       std::cout << '#'; break;
                case Tile::FLOOR:      std::cout << '.'; break;
                case Tile::STAIRS_DOWN: std::cout << '>'; break;
            }
        }
        std::cout << std::endl;
    }

    // 测试2：验证玩家出生点
    auto start = map.getPlayerStart();
    std::cout << "\n测试2：玩家出生点 = (" << start.first << ", " << start.second << ")" << std::endl;
    if (map.isWalkable(start.first, start.second)) {
        std::cout << "  ✅ 出生点可行走" << std::endl;
    } else {
        std::cout << "  ❌ 出生点不可行走！" << std::endl;
    }

    // 测试3：验证楼梯
    bool hasStairs = false;
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            if (map.getTile(x, y) == Tile::STAIRS_DOWN) {
                hasStairs = true;
                std::cout << "  楼梯在 (" << x << ", " << y << ")" << std::endl;
                break;
            }
        }
        if (hasStairs) break;
    }
    std::cout << (hasStairs ? "  ✅ 楼梯存在" : "  ❌ 楼梯不存在！") << std::endl;

    // 测试4：边界碰撞
    std::cout << "测试4：边界碰撞检测" << std::endl;
    std::cout << "  (-1,0) 是否可行走: " << (map.isWalkable(-1, 0) ? "是" : "否（正确）") << std::endl;
    std::cout << "  (0,-1) 是否可行走: " << (map.isWalkable(0, -1) ? "是" : "否（正确）") << std::endl;

    // 测试5：多次生成
    std::cout << "测试5：多次生成是否不同" << std::endl;
    for (int i = 0; i < 3; i++) {
        map.generateNewLevel();
        start = map.getPlayerStart();
        hasStairs = false;
        for (int y = 0; y < map.getHeight(); y++)
            for (int x = 0; x < map.getWidth(); x++)
                if (map.getTile(x, y) == Tile::STAIRS_DOWN) { hasStairs = true; break; }
        std::cout << "  第" << (i+2) << "层：出生点(" << start.first << "," << start.second
                  << ") 楼梯" << (hasStairs ? "✓" : "✗") << std::endl;
    }

    std::cout << "\n✅ 地图测试完成！" << std::endl;
    return 0;
}
