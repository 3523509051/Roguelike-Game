#include "Game.h"
#include "config.h"
#include "Combat.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game()
    : map_(nullptr), player_(nullptr),
      render_(nullptr), input_(nullptr),
      running_(false), gameOver_(false), victory_(false) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    render_ = new Render();
    input_ = new Input();
}

Game::~Game() {
    delete map_;
    delete player_;
    for (auto* m : monsters_) delete m;
    for (auto* i : items_) delete i;
    delete render_;
    delete input_;
}

void Game::setMessage(const std::string& msg) {
    message_ = msg;
}

void Game::run() {
    init();

    while (running_) {
        render_->drawAll(*map_, *player_, monsters_, items_, message_);
        char key = input_->getKey();
        update(key);

        if (!player_->isAlive()) {
            gameOver();
        }
    }
}

void Game::init() {
    map_ = new Map(MAP_WIDTH, MAP_HEIGHT);
    player_ = new Player(0, 0);
    initFloor();
    running_ = true;
    gameOver_ = false;
    victory_ = false;
    setMessage("欢迎来到地牢！找到出口 '>' 进入下一层。");
}

void Game::initFloor() {
    // 清空旧怪物和道具
    for (auto* m : monsters_) delete m;
    monsters_.clear();
    for (auto* i : items_) delete i;
    items_.clear();

    // 生成地图
    map_->generate();
    auto start = map_->getPlayerStart();
    player_->setPos(start.first, start.second);

    // 生成怪物和道具
    int monsterCount = MONSTERS_PER_FLOOR + player_->getFloor() * 2;
    if (monsterCount > 20) monsterCount = 20;
    spawnMonsters(monsterCount);

    int itemCount = ITEMS_PER_FLOOR + player_->getFloor();
    if (itemCount > 10) itemCount = 10;
    spawnItems(itemCount);

    setMessage("进入第 " + std::to_string(player_->getFloor()) + " 层！");
}

void Game::spawnMonsters(int count) {
    int placed = 0;
    int attempts = 0;
    while (placed < count && attempts < 100) {
        auto pos = map_->getRandomRoomCenter();
        // 确保不放在玩家位置
        if (pos == player_->getPos()) continue;

        int level = player_->getFloor();
        if (level >= MAX_FLOORS && placed == 0) {
            // 最后一层放Boss
            monsters_.push_back(new Boss(pos.first, pos.second));
        } else {
            int r = rand() % 100;
            if (r < 40)
                monsters_.push_back(new Slime(pos.first, pos.second));
            else if (r < 70)
                monsters_.push_back(new Bat(pos.first, pos.second));
            else
                monsters_.push_back(new Skeleton(pos.first, pos.second));
        }
        placed++;
        attempts++;
    }
}

void Game::spawnItems(int count) {
    int placed = 0;
    int attempts = 0;
    while (placed < count && attempts < 100) {
        auto pos = map_->getRandomRoomCenter();
        if (pos == player_->getPos()) continue;
        // 确保不放在怪物上
        bool occupied = false;
        for (auto* m : monsters_) {
            if (m && m->getPos() == pos) { occupied = true; break; }
        }
        if (occupied) continue;

        int r = rand() % 100;
        if (r < 40)
            items_.push_back(new Item("小血瓶", '♥', ItemType::HEAL_POTION, 8, pos.first, pos.second));
        else if (r < 60)
            items_.push_back(new Item("大血瓶", '♥', ItemType::HEAL_POTION, 20, pos.first, pos.second));
        else if (r < 75)
            items_.push_back(new Item("攻击卷轴", '⚔', ItemType::ATTACK_BOOST, 3, pos.first, pos.second));
        else if (r < 90)
            items_.push_back(new Item("防御卷轴", '🛡', ItemType::DEFENSE_BOOST, 3, pos.first, pos.second));
        else
            items_.push_back(new Item("金币", '$', ItemType::GOLD_COIN, 10 + rand() % 15, pos.first, pos.second));
        placed++;
        attempts++;
    }
}

void Game::update(char key) {
    switch (key) {
        case 'w': movePlayer(0, -1); break;
        case 's': movePlayer(0, 1);  break;
        case 'a': movePlayer(-1, 0); break;
        case 'd': movePlayer(1, 0);  break;
        case ' ': setMessage("你停了一回合。"); break;
        case 'i': showInventory(); break;
        case 'q': running_ = false; setMessage("游戏已退出。"); break;
        default: return; // 无效按键，不消耗回合
    }
    // 玩家行动后，怪物行动
    processMonsters();
    // 检查事件
    checkEvents();
}

void Game::movePlayer(int dx, int dy) {
    int nx = player_->getPos().first + dx;
    int ny = player_->getPos().second + dy;

    if (!map_->isWalkable(nx, ny)) {
        setMessage("前面是墙壁，过不去。");
        return;
    }

    player_->setPos(nx, ny);
}

void Game::processMonsters() {
    for (auto* monster : monsters_) {
        if (monster && monster->isAlive()) {
            monster->takeTurn(*player_, *map_);
        }
    }
}

void Game::checkEvents() {
    int px = player_->getPos().first;
    int py = player_->getPos().second;

    // 检查是否与怪物在同一个格子（战斗）
    for (size_t i = 0; i < monsters_.size(); i++) {
        auto* m = monsters_[i];
        if (m && m->isAlive() && m->getPos().first == px && m->getPos().second == py) {
            setMessage("遇到了 " + m->getName() + "！");
            bool monsterDead = Combat::playerAttack(*player_, *m);
            if (monsterDead) {
                setMessage("击败了 " + m->getName() + "！获得 " +
                          std::to_string(m->getExpReward()) + " 经验。");
                delete m;
                monsters_[i] = nullptr;
            } else {
                bool playerDead = Combat::monsterAttack(*m, *player_);
                if (playerDead) return;
            }
        }
    }

    // 检查道具
    for (size_t i = 0; i < items_.size(); i++) {
        auto* item = items_[i];
        if (item && item->getPos().first == px && item->getPos().second == py) {
            if (item->getType() == ItemType::GOLD_COIN) {
                player_->addGold(item->getValue());
                setMessage("捡到 " + item->getName() + " +" + std::to_string(item->getValue()));
            } else if (item->getType() == ItemType::HEAL_POTION) {
                player_->heal(item->getValue());
                setMessage("使用 " + item->getName() + "，回复 " + std::to_string(item->getValue()) + " HP！");
            } else if (item->getType() == ItemType::ATTACK_BOOST) {
                // 简化处理：直接加攻击力
                setMessage("获得 " + item->getName() + "，攻击力+" + std::to_string(item->getValue()));
            } else if (item->getType() == ItemType::DEFENSE_BOOST) {
                setMessage("获得 " + item->getName() + "，防御力+" + std::to_string(item->getValue()));
            }
            delete item;
            items_[i] = nullptr;
        }
    }

    // 检查是否到楼梯
    if (map_->getTile(px, py) == Tile::STAIRS_DOWN) {
        nextFloor();
    }
}

void Game::nextFloor() {
    if (player_->getFloor() >= MAX_FLOORS) {
        victory();
        return;
    }
    player_->nextFloor();
    initFloor();
}

void Game::showInventory() {
    std::string msg = "💼 背包: ";
    auto inv = player_->getInventory();
    if (inv.empty()) {
        msg += "（空）";
    } else {
        for (const auto& item : inv) {
            msg += item + "  ";
        }
    }
    setMessage(msg);
}

void Game::gameOver() {
    running_ = false;
    gameOver_ = true;
    render_->drawGameOver(player_->getFloor(), player_->getGold());
}

void Game::victory() {
    running_ = false;
    victory_ = true;
    render_->drawVictory();
}
