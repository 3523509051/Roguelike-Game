#ifndef MAP_H
#define MAP_H

#include <vector>
#include <utility>
#include "types.h"

class Map {
private:
    int width_, height_;
    std::vector<std::vector<Tile>> tiles_;
    std::pair<int, int> playerStart_;
    std::vector<std::pair<int, int>> rooms_;  // 房间中心坐标列表

public:
    Map(int w, int h);

    // 核心：随机生成地牢
    void generate();
    void generateNewLevel();

    // 查询
    Tile getTile(int x, int y) const;
    bool isWalkable(int x, int y) const;
    int getWidth() const;
    int getHeight() const;

    // 位置
    std::pair<int, int> getPlayerStart() const;
    std::pair<int, int> getRandomRoomCenter() const;
    std::vector<std::pair<int, int>> getRooms() const;

    // 修改
    bool setTile(int x, int y, Tile t);

private:
    // BSP 算法内部方法（由 generate 调用）
    void fillWithWalls();
    void connectRooms(int x1, int y1, int x2, int y2);
};

#endif // MAP_H
