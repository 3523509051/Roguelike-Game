#include "Game.h"
#include "Combat.h"
#include "config.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#ifdef _WIN32
#include <conio.h>
#endif

namespace {
const char* SAVE_FILE = "save.dat";

void waitForEnter() {
    std::cout << "按任意键返回...";
#ifdef _WIN32
    _getch();
#else
    std::cin.ignore(10000, '\n');
    std::cin.get();
#endif
}
}

Game::Game()
    : map_(nullptr),
      player_(nullptr),
      render_(nullptr),
      input_(nullptr),
      achievementMgr_(nullptr),
      shop_(nullptr),
      running_(false),
      gameOver_(false),
      victory_(false),
      returnToMenu_(false),
      hasUnsavedChanges_(false) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    render_ = new Render();
    input_ = new Input();
    achievementMgr_ = new AchievementManager();
    shop_ = new Shop();
}

Game::~Game() {
    delete map_;
    delete player_;
    for (auto* monster : monsters_) delete monster;
    for (auto* item : items_) delete item;
    delete render_;
    delete input_;
    delete achievementMgr_;
    delete shop_;
}

void Game::setMessage(const std::string& msg) {
    message_ = msg;
}

void Game::showMainMenu() {
    bool inMenu = true;
    while (inMenu) {
        render_->clear();
        std::cout << "\033[1;36m";
        std::cout << "+================================+" << std::endl;
        std::cout << "|           Dungeon RL           |" << std::endl;
        std::cout << "+================================+" << std::endl;
        std::cout << "\033[0m";
        std::cout << "|  1. 新游戏                     |" << std::endl;
        std::cout << "|  2. 继续游戏                   |" << std::endl;
        std::cout << "|  3. 成就列表                   |" << std::endl;
        std::cout << "|  4. 操作帮助                   |" << std::endl;
        std::cout << "|  5. 退出游戏                   |" << std::endl;
        std::cout << "+--------------------------------+" << std::endl;
        std::cout << "请选择 1-5: ";

        char key = input_->getKey();
        switch (key) {
            case '1': newGame(); break;
            case '2': continueGame(); break;
            case '3': showAchievements(); break;
            case '4': showHelp(); break;
            case '5': inMenu = false; break;
            default:
                std::cout << "无效选择。" << std::endl;
                waitForEnter();
                break;
        }
    }
}

void Game::newGame() {
    delete map_;
    delete player_;
    for (auto* monster : monsters_) delete monster;
    for (auto* item : items_) delete item;
    monsters_.clear();
    items_.clear();

    map_ = nullptr;
    player_ = nullptr;
    delete achievementMgr_;
    achievementMgr_ = new AchievementManager();

    returnToMenu_ = false;
    hasUnsavedChanges_ = true;
    init();
    run();
}

void Game::continueGame() {
    if (!Save::fileExists(SAVE_FILE)) {
        render_->clear();
        std::cout << "+--------------------------------+" << std::endl;
        std::cout << "|            继续游戏            |" << std::endl;
        std::cout << "+--------------------------------+" << std::endl;
        std::cout << " 没有找到存档。" << std::endl;
        waitForEnter();
        return;
    }

    if (map_ == nullptr) map_ = new Map(MAP_WIDTH, MAP_HEIGHT);
    if (player_ == nullptr) player_ = new Player(0, 0);

    if (!Save::loadGame(SAVE_FILE, *player_, *map_, monsters_, items_, *achievementMgr_)) {
        render_->clear();
        std::cout << "+--------------------------------+" << std::endl;
        std::cout << "|            继续游戏            |" << std::endl;
        std::cout << "+--------------------------------+" << std::endl;
        std::cout << " 读取存档失败。" << std::endl;
        waitForEnter();
        return;
    }

    running_ = true;
    gameOver_ = false;
    victory_ = false;
    returnToMenu_ = false;
    hasUnsavedChanges_ = false;
    setMessage("读取存档成功。");
    run();
}

void Game::showAchievements() {
    render_->clear();
    std::cout << "+--------------------------------+" << std::endl;
    std::cout << "|            成就列表            |" << std::endl;
    std::cout << "+--------------------------------+" << std::endl;
    achievementMgr_->displayAll();
    waitForEnter();
}

void Game::showHelp() {
    render_->clear();
    std::cout << "+--------------------------------+" << std::endl;
    std::cout << "|            操作帮助            |" << std::endl;
    std::cout << "+--------------------------------+" << std::endl;
    std::cout << " 地图操作" << std::endl;
    std::cout << "   WASD / 方向键  移动" << std::endl;
    std::cout << "   空格           等待一回合" << std::endl;
    std::cout << "   I              查看背包" << std::endl;
    std::cout << "   P              保存游戏" << std::endl;
    std::cout << "   Q              退出当前游戏" << std::endl;
    std::cout << std::endl;
    std::cout << " 战斗操作" << std::endl;
    std::cout << "   A              攻击" << std::endl;
    std::cout << "   D              防御，减半伤害" << std::endl;
    std::cout << "   I              使用血瓶" << std::endl;
    std::cout << "   F              尝试逃跑" << std::endl;
    std::cout << std::endl;
    std::cout << " 目标" << std::endl;
    std::cout << "   找到绿色的 > 楼梯，进入下一层。" << std::endl;
    waitForEnter();
}

void Game::openShop() {
    if (player_ != nullptr && render_ != nullptr && input_ != nullptr && shop_ != nullptr) {
        shop_->open(*player_, *render_, *input_, [this]() {
            return saveCurrentGame();
        });
        hasUnsavedChanges_ = true;
    }
}

bool Game::saveCurrentGame() {
    if (map_ == nullptr || player_ == nullptr || achievementMgr_ == nullptr) {
        return false;
    }

    const bool saved = Save::saveGame(SAVE_FILE, *player_, *map_, monsters_, items_, *achievementMgr_);
    if (saved) {
        hasUnsavedChanges_ = false;
    }
    return saved;
}

bool Game::confirmSaveBeforeExit() {
    if (!hasUnsavedChanges_) {
        return true;
    }

    render_->clear();
    std::cout << "+--------------------------------+" << std::endl;
    std::cout << "|            退出游戏            |" << std::endl;
    std::cout << "+--------------------------------+" << std::endl;
    std::cout << " 当前进度尚未保存，是否保存后退出？" << std::endl;
    std::cout << " [Y] 保存并退出  [N] 不保存退出  [C] 取消" << std::endl;
    std::cout << "请选择: ";

    while (true) {
        const char key = input_->getKey();
        std::cout << key << std::endl;
        if (key == 'y' || key == 'Y') {
            if (saveCurrentGame()) {
                setMessage("保存成功，游戏已退出。");
                return true;
            }
            setMessage("保存失败，已取消退出。");
            waitForEnter();
            return false;
        }
        if (key == 'n' || key == 'N') {
            setMessage("游戏已退出，当前进度未保存。");
            return true;
        }
        if (key == 'c' || key == 'C' || key == 'q' || key == 'Q') {
            setMessage("已取消退出。");
            return false;
        }
    }
}

void Game::run() {
    if (map_ == nullptr || player_ == nullptr) {
        init();
    }

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
    delete map_;
    delete player_;
    for (auto* monster : monsters_) delete monster;
    for (auto* item : items_) delete item;
    monsters_.clear();
    items_.clear();

    map_ = new Map(MAP_WIDTH, MAP_HEIGHT);
    player_ = new Player(0, 0);
    running_ = true;
    gameOver_ = false;
    victory_ = false;
    initFloor();
    setMessage("欢迎来到地牢！找到出口 '>' 进入下一层。");
}

void Game::initFloor() {
    for (auto* monster : monsters_) delete monster;
    monsters_.clear();
    for (auto* item : items_) delete item;
    items_.clear();

    if (player_->getFloor() > 1) {
        openShop();
    }

    map_->generate();
    auto start = map_->getPlayerStart();
    player_->setPos(start.first, start.second);

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
        ++attempts;
        auto pos = map_->getRandomRoomCenter();
        if (pos == player_->getPos()) continue;

        int level = player_->getFloor();
        if (level >= MAX_FLOORS && placed == 0) {
            monsters_.push_back(new Boss(pos.first, pos.second));
        } else if (level >= 16 && placed == 0) {
            monsters_.push_back(new DeathKnight(pos.first, pos.second));
        } else {
            int r = rand() % 100;
            if (level <= 5) {
                if (r < 50) monsters_.push_back(new Slime(pos.first, pos.second));
                else if (r < 80) monsters_.push_back(new Bat(pos.first, pos.second));
                else monsters_.push_back(new Skeleton(pos.first, pos.second));
            } else if (level <= 10) {
                if (r < 30) monsters_.push_back(new Slime(pos.first, pos.second));
                else if (r < 50) monsters_.push_back(new Bat(pos.first, pos.second));
                else if (r < 70) monsters_.push_back(new Skeleton(pos.first, pos.second));
                else if (r < 85) monsters_.push_back(new Goblin(pos.first, pos.second));
                else monsters_.push_back(new Wolf(pos.first, pos.second));
            } else {
                if (r < 20) monsters_.push_back(new Skeleton(pos.first, pos.second));
                else if (r < 50) monsters_.push_back(new Goblin(pos.first, pos.second));
                else monsters_.push_back(new Wolf(pos.first, pos.second));
            }
        }
        monsters_.back()->applyFloorScaling(level);  // ⭐ 按楼层缩放怪物
        ++placed;
    }
}

void Game::spawnItems(int count) {
    int placed = 0;
    int attempts = 0;
    while (placed < count && attempts < 100) {
        ++attempts;
        auto pos = map_->getRandomRoomCenter();
        if (pos == player_->getPos()) continue;

        bool occupied = false;
        for (auto* monster : monsters_) {
            if (monster && monster->getPos() == pos) {
                occupied = true;
                break;
            }
        }
        if (occupied) continue;

        int r = rand() % 100;
        if (r < 40)
            items_.push_back(new Item("小血瓶", 'H', ItemType::HEAL_POTION, 8, pos.first, pos.second));
        else if (r < 60)
            items_.push_back(new Item("大血瓶", 'H', ItemType::HEAL_POTION, 20, pos.first, pos.second));
        else if (r < 75)
            items_.push_back(new Item("攻击卷轴", 'A', ItemType::ATTACK_BOOST, 3, pos.first, pos.second));
        else if (r < 90)
            items_.push_back(new Item("防御卷轴", 'D', ItemType::DEFENSE_BOOST, 3, pos.first, pos.second));
        else
            items_.push_back(new Item("金币", '$', ItemType::GOLD_COIN, 10 + rand() % 15, pos.first, pos.second));
        ++placed;
    }
}

void Game::addBattleLog(const std::string& msg) {
    battleLog_.push_back(msg);
}

void Game::enterBattle(Monster* monster) {
    battleLog_.clear();
    addBattleLog(monster->getName() + " 出现了！");
    player_->resetAtb();
    monster->resetAtb();

    bool fleeing = false;
    bool playerDefeated = false;

    while (monster->isAlive() && player_->isAlive() && !fleeing) {
        const float deltaTime = 1.0f;
        player_->updateAtb(deltaTime);
        monster->updateAtb(deltaTime);

        bool playerReady = player_->isAtbFull();
        bool monsterReady = monster->isAtbFull();
        render_->drawBattleScreen(*player_, *monster, battleLog_,
                                  playerReady ? "你的回合！选择行动：" : "等待行动中...");

        if (playerReady) {
            char key = input_->getKey();
            bool playerActed = false;

            switch (key) {
                case 'a': {
                    int dmg = Combat::calcDamage(player_->getEffectiveAtk(), monster->getDefense());
                    int actual = monster->takeDamage(dmg);
                    addBattleLog("你攻击了" + monster->getName() + "，造成 " + std::to_string(actual) + " 点伤害。");
                    playerActed = true;

                    if (!monster->isAlive()) {
                        int oldLevel = player_->getLevel();
                        bool bossKilled = monster->isBoss();
                        achievementMgr_->onMonsterKilled(*monster);
                        player_->gainExp(monster->getExpReward());
                        player_->addGold(monster->getGoldReward());
                        achievementMgr_->onGoldGained(monster->getGoldReward());
                        if (player_->getLevel() > oldLevel) {
                            achievementMgr_->onLevelUp(player_->getLevel());
                        }
                        if (bossKilled && player_->getFloor() >= MAX_FLOORS) {
                            achievementMgr_->onVictory();
                        }

                        addBattleLog(monster->getName() + " 被击败了！");
                        render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                        input_->getKey();
                        return;
                    }
                    break;
                }
                case 'd': {
                    int rawDmg = Combat::calcDamage(monster->getAttack(), player_->getDefense());
                    int actualDmg = rawDmg / 2;
                    player_->takeDamage(actualDmg);
                    achievementMgr_->onDefenseBlock(rawDmg - actualDmg);
                    if (actualDmg > 0) achievementMgr_->onPlayerHit();
                    addBattleLog("你进行防御，受到 " + std::to_string(actualDmg) + " 点伤害。");
                    if (!player_->isAlive()) playerDefeated = true;
                    playerActed = true;
                    monster->resetAtb();
                    break;
                }
                case 'i': {
                    auto inv = player_->getInventory();
                    bool foundHeal = false;
                    for (const auto& itemName : inv) {
                        if (itemName.find("血瓶") != std::string::npos) {
                            player_->useItem(itemName);
                            int healAmt = (itemName.find("大") != std::string::npos) ? 20 : 8;
                            player_->heal(healAmt);
                            addBattleLog("使用了" + itemName + "，回复 " + std::to_string(healAmt) + " HP。");
                            foundHeal = true;
                            break;
                        }
                    }
                    if (!foundHeal) {
                        addBattleLog("背包里没有血瓶！");
                        continue;
                    }
                    playerActed = true;
                    break;
                }
                case 'f': {
                    int chance = 50 + player_->getLevel() * 10;
                    if (rand() % 100 < chance) {
                        achievementMgr_->onEscape();
                        addBattleLog("你成功逃跑了！");
                        render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                        input_->getKey();
                        fleeing = true;
                    } else {
                        addBattleLog("逃跑失败！");
                        playerActed = true;
                    }
                    break;
                }
                // ⭐ 技能：重击
                case '1': {
                    if (!player_->isSkillUnlocked(0)) {
                        addBattleLog("重击未解锁！请在商店购买。");
                        continue;
                    }
                    if (!player_->useSkill(0)) {
                        addBattleLog("重击冷却中！还需 " + std::to_string(player_->getSkills()[0].cooldownRemaining) + " 回合");
                        continue;
                    }
                    float mult = player_->getSkills()[0].param1;
                    int dmg = Combat::calcSkillDamage(player_->getEffectiveAtk(), monster->getDefense(), mult);
                    int actual = monster->takeDamage(dmg);
                    addBattleLog("重击(" + std::to_string(mult) + "x)！造成 " + std::to_string(actual) + " 点伤害。");
                    playerActed = true;

                    if (!monster->isAlive()) {
                        int oldLevel = player_->getLevel();
                        bool bossKilled = monster->isBoss();
                        achievementMgr_->onMonsterKilled(*monster);
                        player_->gainExp(monster->getExpReward());
                        player_->addGold(monster->getGoldReward());
                        achievementMgr_->onGoldGained(monster->getGoldReward());
                        if (player_->getLevel() > oldLevel) {
                            achievementMgr_->onLevelUp(player_->getLevel());
                        }
                        if (bossKilled && player_->getFloor() >= MAX_FLOORS) {
                            achievementMgr_->onVictory();
                        }
                        addBattleLog(monster->getName() + " 被击败了！");
                        render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                        input_->getKey();
                        return;
                    }
                    break;
                }
                // ⭐ 技能：连斩（2-3连击）
                case '2': {
                    if (!player_->isSkillUnlocked(1)) {
                        addBattleLog("连斩未解锁！请在商店购买。");
                        continue;
                    }
                    if (!player_->useSkill(1)) {
                        addBattleLog("连斩冷却中！还需 " + std::to_string(player_->getSkills()[1].cooldownRemaining) + " 回合");
                        continue;
                    }
                    int hits = static_cast<int>(player_->getSkills()[1].param1);  // Lv.1=2, Lv.3=3
                    bool dead = false;
                    int totalDmg = 0;
                    for (int h = 0; h < hits && !dead; h++) {
                        int dmg = Combat::calcDamage(player_->getEffectiveAtk(), monster->getDefense());
                        int actual = monster->takeDamage(dmg);
                        totalDmg += actual;
                        addBattleLog("连斩第" + std::to_string(h+1) + "击！造成 " + std::to_string(actual) + " 点伤害。");
                        if (!monster->isAlive()) {
                            dead = true;
                        }
                    }
                    playerActed = true;

                    if (!monster->isAlive()) {
                        int oldLevel = player_->getLevel();
                        bool bossKilled = monster->isBoss();
                        achievementMgr_->onMonsterKilled(*monster);
                        player_->gainExp(monster->getExpReward());
                        player_->addGold(monster->getGoldReward());
                        achievementMgr_->onGoldGained(monster->getGoldReward());
                        if (player_->getLevel() > oldLevel) achievementMgr_->onLevelUp(player_->getLevel());
                        if (bossKilled && player_->getFloor() >= MAX_FLOORS) achievementMgr_->onVictory();
                        addBattleLog(monster->getName() + " 被击败了！");
                        render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                        input_->getKey();
                        return;
                    }
                    break;
                }
                // ⭐ 技能：破甲（无视防御）
                case '3': {
                    if (!player_->isSkillUnlocked(2)) {
                        addBattleLog("破甲未解锁！请在商店购买。");
                        continue;
                    }
                    if (!player_->useSkill(2)) {
                        addBattleLog("破甲冷却中！还需 " + std::to_string(player_->getSkills()[2].cooldownRemaining) + " 回合");
                        continue;
                    }
                    float mult = player_->getSkills()[2].param1;
                    int dmg = Combat::calcSkillDamage(player_->getEffectiveAtk(), 0, mult);  // 无视防御
                    int actual = monster->takeDamage(dmg);
                    addBattleLog("破甲(" + std::to_string(mult) + "x无视防)！造成 " + std::to_string(actual) + " 点伤害。");
                    playerActed = true;

                    if (!monster->isAlive()) {
                        int oldLevel = player_->getLevel();
                        bool bossKilled = monster->isBoss();
                        achievementMgr_->onMonsterKilled(*monster);
                        player_->gainExp(monster->getExpReward());
                        player_->addGold(monster->getGoldReward());
                        achievementMgr_->onGoldGained(monster->getGoldReward());
                        if (player_->getLevel() > oldLevel) achievementMgr_->onLevelUp(player_->getLevel());
                        if (bossKilled && player_->getFloor() >= MAX_FLOORS) achievementMgr_->onVictory();
                        addBattleLog(monster->getName() + " 被击败了！");
                        render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                        input_->getKey();
                        return;
                    }
                    break;
                }
                // ⭐ 技能：战吼（ATK+5~10 持续3~4回合）
                case '4': {
                    if (!player_->isSkillUnlocked(3)) {
                        addBattleLog("战吼未解锁！请在商店购买。");
                        continue;
                    }
                    if (!player_->useSkill(3)) {
                        addBattleLog("战吼冷却中！还需 " + std::to_string(player_->getSkills()[3].cooldownRemaining) + " 回合");
                        continue;
                    }
                    int buffAtk = static_cast<int>(player_->getSkills()[3].param1);
                    int buffTurns = player_->getSkills()[3].param2;
                    player_->setAtkBuff(buffAtk, buffTurns);
                    addBattleLog("战吼！攻击力+" + std::to_string(buffAtk) + "，持续" + std::to_string(buffTurns) + "回合（当前ATK: " + std::to_string(player_->getEffectiveAtk()) + "）。");
                    playerActed = true;
                    break;
                }
                default:
                    continue;
            }

            if (playerActed) {
                player_->tickCooldowns();  // ⭐ 技能冷却-1
                player_->tickBuffs();      // ⭐ 增益回合-1
                player_->resetAtb();
            }
        }

        if (monsterReady && monster->isAlive() && !fleeing) {
            int rawDmg = Combat::calcDamage(monster->getAttack(), player_->getDefense());
            int actualDmg = player_->takeDamage(rawDmg);
            if (actualDmg > 0) achievementMgr_->onPlayerHit();
            addBattleLog(monster->getName() + " 攻击，造成 " + std::to_string(actualDmg) + " 点伤害。");

            if (!player_->isAlive()) {
                playerDefeated = true;
                render_->drawBattleScreen(*player_, *monster, battleLog_, "按任意键继续...");
                input_->getKey();
            }

            monster->resetAtb();
        }
    }

    if (playerDefeated) {
        gameOver();
    }
}

void Game::update(char key) {
    switch (key) {
        case 'w': movePlayer(0, -1); break;
        case 's': movePlayer(0, 1); break;
        case 'a': movePlayer(-1, 0); break;
        case 'd': movePlayer(1, 0); break;
        case ' ': setMessage("你停了一回合。"); break;
        case 'i': showInventory(); break;
        case 'p':
            if (saveCurrentGame()) {
                setMessage("保存成功。");
            } else {
                setMessage("保存失败。");
            }
            return;
        case 'q':
            if (confirmSaveBeforeExit()) {
                running_ = false;
            }
            return;
        default: return;
    }

    hasUnsavedChanges_ = true;
    processMonsters();
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
            int beforeHp = player_->getHp();
            monster->takeTurn(*player_, *map_);
            if (player_->getHp() < beforeHp) {
                achievementMgr_->onPlayerHit();
            }
        }
    }
}

void Game::checkEvents() {
    int px = player_->getPos().first;
    int py = player_->getPos().second;

    for (std::size_t i = 0; i < monsters_.size(); ++i) {
        Monster* monster = monsters_[i];
        if (monster && monster->isAlive() && monster->getPos().first == px && monster->getPos().second == py) {
            enterBattle(monster);
            if (!monster->isAlive()) {
                delete monster;
                monsters_[i] = nullptr;
            }
            if (!player_->isAlive()) return;
        }
    }

    for (std::size_t i = 0; i < items_.size(); ++i) {
        Item* item = items_[i];
        if (item && item->getPos().first == px && item->getPos().second == py) {
            achievementMgr_->onItemPickup();
            if (item->getType() == ItemType::GOLD_COIN) {
                player_->addGold(item->getValue());
                achievementMgr_->onGoldGained(item->getValue());
                setMessage("捡到 " + item->getName() + " +" + std::to_string(item->getValue()));
            } else if (item->getType() == ItemType::HEAL_POTION) {
                player_->heal(item->getValue());
                setMessage("使用 " + item->getName() + "，回复 " + std::to_string(item->getValue()) + " HP！");
            } else {
                player_->addItem(item->getName());
                setMessage("获得 " + item->getName());
            }
            delete item;
            items_[i] = nullptr;
        }
    }

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
    std::string msg = "背包: ";
    auto inv = player_->getInventory();
    if (inv.empty()) {
        msg += "空";
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
    achievementMgr_->onPlayerDeath();
    render_->drawGameOver(player_->getFloor(), player_->getGold());
}

void Game::victory() {
    running_ = false;
    victory_ = true;
    achievementMgr_->onVictory();
    render_->drawVictory();
}
