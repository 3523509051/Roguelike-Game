#include "Map.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

Map::Map(int w, int h) : width_(w), height_(h) {
    tiles_.resize(h, std::vector<Tile>(w, Tile::WALL));
}

void Map::fillWithWalls() {
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            tiles_[y][x] = Tile::WALL;
        }
    }
}

void Map::connectRooms(int x1, int y1, int x2, int y2) {
    // L 形走廊：先水平再垂直
    int curX = x1, curY = y1;
    while (curX != x2) {
        if (curY >= 0 && curY < height_ && curX >= 0 && curX < width_) {
            tiles_[curY][curX] = Tile::FLOOR;
            if (curX + 1 < width_) tiles_[curY][curX + 1] = Tile::FLOOR;
        }
        curX += (x2 > x1) ? 1 : -1;
    }
    while (curY != y2) {
        if (curY >= 0 && curY < height_ && curX >= 0 && curX < width_) {
            tiles_[curY][curX] = Tile::FLOOR;
            if (curY + 1 < height_) tiles_[curY + 1][curX] = Tile::FLOOR;
        }
        curY += (y2 > y1) ? 1 : -1;
    }
}

struct BSPNode {
    int x, y, w, h;
    BSPNode* left;
    BSPNode* right;
    bool isLeaf;
    std::pair<int, int> roomCenter;

    BSPNode(int x_, int y_, int w_, int h_)
        : x(x_), y(y_), w(w_), h(h_), left(nullptr), right(nullptr),
          isLeaf(false), roomCenter(-1, -1) {}
};

static BSPNode* splitNode(BSPNode* node, int minSize) {
    if (node->w < minSize * 2 && node->h < minSize * 2) {
        node->isLeaf = true;
        return node;
    }

    bool splitHorizontal = (rand() % 2 == 0);
    // 如果某一维度不够分，强制切另一方向
    if (node->w < minSize * 2) splitHorizontal = false;
    if (node->h < minSize * 2) splitHorizontal = true;

    int maxSplit = splitHorizontal ? (node->h - minSize) : (node->w - minSize);
    if (maxSplit <= minSize) {
        node->isLeaf = true;
        return node;
    }
    int split = (rand() % (maxSplit - minSize)) + minSize;

    if (splitHorizontal) {
        node->left  = new BSPNode(node->x, node->y, node->w, split);
        node->right = new BSPNode(node->x, node->y + split, node->w, node->h - split);
    } else {
        node->left  = new BSPNode(node->x, node->y, split, node->h);
        node->right = new BSPNode(node->x + split, node->y, node->w - split, node->h);
    }

    splitNode(node->left, minSize);
    splitNode(node->right, minSize);
    return node;
}

static void createRoom(BSPNode* node, std::vector<std::vector<Tile>>& tiles,
                       std::vector<std::pair<int,int>>& rooms) {
    if (!node) return;
    if (node->isLeaf) {
        // 在叶子节点内随机挖一个房间
        int roomW = (rand() % 5) + 3;  // 3~7
        int roomH = (rand() % 5) + 3;
        int roomX = node->x + (rand() % (node->w - roomW - 1)) + 1;
        int roomY = node->y + (rand() % (node->h - roomH - 1)) + 1;

        // 确保不超出地图范围
        if (roomX + roomW >= (int)tiles[0].size()) roomW = tiles[0].size() - roomX - 1;
        if (roomY + roomH >= (int)tiles.size())    roomH = tiles.size() - roomY - 1;

        for (int y = roomY; y < roomY + roomH; y++) {
            for (int x = roomX; x < roomX + roomW; x++) {
                tiles[y][x] = Tile::FLOOR;
            }
        }
        node->roomCenter = std::make_pair(roomX + roomW / 2, roomY + roomH / 2);
        rooms.push_back(node->roomCenter);
    } else {
        createRoom(node->left, tiles, rooms);
        createRoom(node->right, tiles, rooms);
    }
}

static void connectBSP(BSPNode* node, std::vector<std::vector<Tile>>& tiles,
                       void (*connectFn)(int,int,int,int,std::vector<std::vector<Tile>>&)) {
    if (!node || node->isLeaf) return;
    connectBSP(node->left, tiles, connectFn);
    connectBSP(node->right, tiles, connectFn);

    if (node->left && node->right &&
        !node->left->isLeaf && !node->right->isLeaf) {
        // 连接左右子树中的某一对房间
        int x1 = node->left->roomCenter.first;
        int y1 = node->left->roomCenter.second;
        int x2 = node->right->roomCenter.first;
        int y2 = node->right->roomCenter.second;
        connectFn(x1, y1, x2, y2, tiles);
    }
}

// 修正的走廊连接（适配静态函数签名）
static void connectTiles(int x1, int y1, int x2, int y2,
                         std::vector<std::vector<Tile>>& tiles) {
    int cx = x1, cy = y1;
    int w = tiles[0].size(), h = tiles.size();
    while (cx != x2) {
        if (cy >= 0 && cy < h && cx >= 0 && cx < w) {
            tiles[cy][cx] = Tile::FLOOR;
            if (cx + 1 < w) tiles[cy][cx + 1] = Tile::FLOOR;
        }
        cx += (x2 > x1) ? 1 : -1;
    }
    while (cy != y2) {
        if (cy >= 0 && cy < h && cx >= 0 && cx < w) {
            tiles[cy][cx] = Tile::FLOOR;
            if (cy + 1 < h) tiles[cy + 1][cx] = Tile::FLOOR;
        }
        cy += (y2 > y1) ? 1 : -1;
    }
}

void Map::generate() {
    fillWithWalls();
    rooms_.clear();

    int minRoomSize = 5;
    BSPNode* root = new BSPNode(1, 1, width_ - 2, height_ - 2);
    splitNode(root, minRoomSize);
    createRoom(root, tiles_, rooms_);
    connectBSP(root, tiles_, connectTiles);

    delete root;

    // 确保有房间，否则重试
    if (rooms_.size() < 3) {
        generate();
        return;
    }

    // 在第一个房间放楼梯，最后一个房间放玩家出生点
    auto stairRoom = rooms_.front();
    auto startRoom = rooms_.back();
    tiles_[stairRoom.second][stairRoom.first] = Tile::STAIRS_DOWN;
    playerStart_ = startRoom;
}

void Map::generateNewLevel() {
    generate();
}

Tile Map::getTile(int x, int y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
        return Tile::WALL;
    return tiles_[y][x];
}

bool Map::isWalkable(int x, int y) const {
    Tile t = getTile(x, y);
    return (t == Tile::FLOOR || t == Tile::STAIRS_DOWN);
}

int Map::getWidth() const { return width_; }
int Map::getHeight() const { return height_; }

std::pair<int, int> Map::getPlayerStart() const {
    return playerStart_;
}

std::pair<int, int> Map::getRandomRoomCenter() const {
    if (rooms_.empty()) return std::make_pair(width_ / 2, height_ / 2);
    int idx = rand() % rooms_.size();
    return rooms_[idx];
}

std::vector<std::pair<int, int>> Map::getRooms() const {
    return rooms_;
}

bool Map::setTile(int x, int y, Tile t) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
        return false;
    tiles_[y][x] = t;
    return true;
}
