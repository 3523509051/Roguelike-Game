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

        // ⭐ 20层难度曲线设计
        // 第1-5层：新手区，简单怪
        // 第6-10层：进阶区，加入哥布林和狼
        // 第11-15层：困难区，怪物密度增加
        // 第16-19层：精英区，每层有1个精英（死亡骑士）
        // 第20层：最终Boss

        if (level >= MAX_FLOORS && placed == 0) {
            // 第20层 → 最终Boss
            monsters_.push_back(new Boss(pos.first, pos.second));
        } else if (level >= 16 && placed == 0) {
            // 第16-19层 → 每层1个精英
            monsters_.push_back(new DeathKnight(pos.first, pos.second));
        } else {
            // 小怪分布：按层数逐渐变强
            int r = rand() % 100;

            if (level <= 5) {
                // 第1-5层：新手区
                if (r < 50) monsters_.push_back(new Slime(pos.first, pos.second));
                else if (r < 80) monsters_.push_back(new Bat(pos.first, pos.second));
                else monsters_.push_back(new Skeleton(pos.first, pos.second));
            } else if (level <= 10) {
                // 第6-10层：进阶区
                if (r < 30) monsters_.push_back(new Slime(pos.first, pos.second));
                else if (r < 50) monsters_.push_back(new Bat(pos.first, pos.second));
                else if (r < 70) monsters_.push_back(new Skeleton(pos.first, pos.second));
                else if (r < 85) monsters_.push_back(new Goblin(pos.first, pos.second));
                else monsters_.push_back(new Wolf(pos.first, pos.second));
            } else {
                // 第11-15层：困难区（强力怪物为主）
                if (r < 20) monsters_.push_back(new Skeleton(pos.first, pos.second));
                else if (r < 50) monsters_.push_back(new Goblin(pos.first, pos.second));
                else monsters_.push_back(new Wolf(pos.first, pos.second));
            }
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

void Game::addBattleLog(const std::string& msg) {
    battleLog_.push_back(msg);
}

// ⭐ 全屏ATB战斗系统（速度条战斗）
void Game::enterBattle(Monster* monster) {
    battleLog_.clear();
    addBattleLog("⚔ " + monster->getName() + " 出现了！");

    // 重置双方ATB槽
    player_->resetAtb();
    monster->resetAtb();

    bool fleeing = false;
    bool playerDefeated = false;

    // ⭐ ATB主循环：双方速度条同时增长
    while (monster->isAlive() && player_->isAlive() && !fleeing) {
        // 速度条增长（每帧增长）
        const float deltaTime = 1.0f;
        player_->updateAtb(deltaTime);
        monster->updateAtb(deltaTime);

        // 渲染战斗画面（包含速度条）
        std::string prompt = "等待行动中...";

        // 检查谁的速度条满了
        bool playerReady = player_->isAtbFull();
        bool monsterReady = monster->isAtbFull();

        if (playerReady) {
            prompt = "你的回合！选择行动：";
        }

        render_->drawBattleScreen(*player_, *monster, battleLog_, prompt);

        // ⭐ 玩家回合
        if (playerReady) {
            char key = input_->getKey();
            bool playerActed = false;

            switch (key) {
                case 'a': { // 攻击
                    int dmg = Combat::calcDamage(player_->getAttack(), monster->getDefense());
                    int actual = monster->takeDamage(dmg);
                    addBattleLog("→ 你挥剑斩向" + monster->getName() + "！");
                    addBattleLog("💥 造成 " + std::to_string(actual) + " 点伤害！");
                    if (!monster->isAlive()) {
                        addBattleLog("🏆 " + monster->getName() + " 被击败了！");
                        addBattleLog("✨ 获得 " + std::to_string(monster->getExpReward()) + " 经验");
                        addBattleLog("💰 获得 " + std::to_string(monster->getGoldReward()) + " 金币");
                        render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                        input_->getKey();
                        player_->gainExp(monster->getExpReward());
                        player_->addGold(monster->getGoldReward());
                        return; // 战斗胜利，退出
                    }
                    playerActed = true;
                    break;
                }
                case 'd': { // 防御
                    addBattleLog("🛡 你举起盾牌防御！");
                    playerActed = true;
                    break;
                }
                case 'i': { // 使用道具
                    auto inv = player_->getInventory();
                    bool foundHeal = false;
                    for (const auto& itemName : inv) {
                        if (itemName.find("血瓶") != std::string::npos) {
                            player_->useItem(itemName);
                            int healAmt = (itemName.find("大") != std::string::npos) ? 20 : 8;
                            player_->heal(healAmt);
                            addBattleLog("♥ 使用了" + itemName + "，回复 " + std::to_string(healAmt) + " HP！");
                            foundHeal = true;
                            break;
                        }
                    }
                    if (!foundHeal) {
                        addBattleLog("❌ 背包里没有血瓶！");
                        continue; // 不消耗回合
                    }
                    playerActed = true;
                    break;
                }
                case 'f': { // 逃跑
                    int chance = 50 + player_->getLevel() * 10;
                    if (rand() % 100 < chance) {
                        addBattleLog("🏃 你成功逃跑了！");
                        render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                        input_->getKey();
                        fleeing = true;
                    } else {
                        addBattleLog("❌ 逃跑失败！");
                        playerActed = true; // 逃跑失败也会消耗一回合
                    }
                    break;
                }
                default:
                    continue; // 无效按键，不消耗回合
            }

            // ⭐ 行动后重置ATB
            if (playerActed) {
                player_->resetAtb();
            }
        }

        // ⭐ 怪物回合
        if (monsterReady && monster->isAlive() && !fleeing) {
            int dmgReduction = 1;
            int rawDmg = Combat::calcDamage(monster->getAttack(), player_->getDefense());
            int actualDmg = rawDmg / dmgReduction;
            player_->takeDamage(actualDmg);
            addBattleLog("💥 " + monster->getName() + " 攻击！受到 " + std::to_string(actualDmg) + " 点伤害！");

            if (!player_->isAlive()) {
                addBattleLog("💀 你被 " + monster->getName() + " 击败了...");
                render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                input_->getKey();
                playerDefeated = true;
            }

            // ⭐ 怪物行动后重置ATB
            monster->resetAtb();
        }
    }

    if (playerDefeated) {
        gameOver();
    }
}
            case 'a': { // 攻击
                int dmg = Combat::calcDamage(player_->getAttack(), monster->getDefense());
                int actual = monster->takeDamage(dmg);
                addBattleLog("→ 你挥剑斩向" + monster->getName() + "！");
                addBattleLog("💥 造成 " + std::to_string(actual) + " 点伤害！");
                if (!monster->isAlive()) {
                    addBattleLog("🏆 " + monster->getName() + " 被击败了！");
                    addBattleLog("✨ 获得 " + std::to_string(monster->getExpReward()) + " 经验");
                    addBattleLog("💰 获得 " + std::to_string(monster->getGoldReward()) + " 金币");
                    render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                    input_->getKey();
                    player_->gainExp(monster->getExpReward());
                    player_->addGold(monster->getGoldReward());
                    return; // 战斗胜利，退出
                }
                playerActed = true;
                break;
            }
            case 'd': { // 防御
                addBattleLog("🛡 你举起盾牌防御！");
                playerActed = true;
                break;
            }
            case 'i': { // 使用道具
                auto inv = player_->getInventory();
                bool foundHeal = false;
                for (const auto& itemName : inv) {
                    if (itemName.find("血瓶") != std::string::npos) {
                        player_->useItem(itemName);
                        int healAmt = (itemName.find("大") != std::string::npos) ? 20 : 8;
                        player_->heal(healAmt);
                        addBattleLog("♥ 使用了" + itemName + "，回复 " + std::to_string(healAmt) + " HP！");
                        foundHeal = true;
                        break;
                    }
                }
                if (!foundHeal) {
                    addBattleLog("❌ 背包里没有血瓶！");
                    continue; // 不消耗回合
                }
                playerActed = true;
                break;
            }
            case 'f': { // 逃跑
                int chance = 50 + player_->getLevel() * 10;
                if (rand() % 100 < chance) {
                    addBattleLog("🏃 你成功逃跑了！");
                    render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                    input_->getKey();
                    fleeing = true;
                } else {
                    addBattleLog("❌ 逃跑失败！");
                    playerActed = true; // 逃跑失败也会消耗一回合
                }
                break;
            }
            default:
                continue; // 无效按键，不消耗回合
        }

        if (!fleeing && playerActed) {
            // 怪物反击
            if (monster->isAlive()) {
                int dmgReduction = (key == 'd') ? 2 : 1; // 防御时伤害减半
                int rawDmg = Combat::calcDamage(monster->getAttack(), player_->getDefense());
                int actualDmg = rawDmg / dmgReduction;
                player_->takeDamage(actualDmg);
                if (key == 'd') {
                    addBattleLog("🛡 防御成功！伤害从 " + std::to_string(rawDmg) +
                                 " 减为 " + std::to_string(actualDmg));
                } else {
                    addBattleLog("💥 " + monster->getName() + " 反击！受到 " + std::to_string(actualDmg) + " 点伤害！");
                }

                if (!player_->isAlive()) {
                    addBattleLog("💀 你被 " + monster->getName() + " 击败了...");
                    render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                    input_->getKey();
                    playerDefeated = true;
                }
            }
        }
    }

    if (playerDefeated) {
        gameOver();
    }
}
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

    // 检查是否与怪物在同一个格子（进入全屏战斗）
    for (size_t i = 0; i < monsters_.size(); i++) {
        auto* m = monsters_[i];
        if (m && m->isAlive() && m->getPos().first == px && m->getPos().second == py) {
            enterBattle(m);
            // 战斗结束后，如果怪物死了就移除
            if (!m->isAlive()) {
                delete m;
                monsters_[i] = nullptr;
            }
            // 如果玩家死了，gameOver已在enterBattle内处理
            if (!player_->isAlive()) return;
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
