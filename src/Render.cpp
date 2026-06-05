#include "Render.h"
#include "config.h"
#include <iostream>
#include <cstdlib>
#include <sstream>

// ===== ANSI 颜色转义序列 =====
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"

static void drawHPBar(const std::string& label, int hp, int maxHp, const std::string& color);
static void drawATBBar(const std::string& label, float gauge, const std::string& color);
static void drawLine(char left, char fill, char right, int width);
static void drawCenteredText(const std::string& text, int width);

#ifdef _WIN32
#include <windows.h>
// Windows 10+ 的终端支持 ANSI 转义
static void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#else
static void enableANSI() {}
#endif

Render::Render() {
    enableANSI();
}

void Render::clear() {
    std::cout << "\033[H\033[J";
}

static void drawLine(char left, char fill, char right, int width) {
    std::cout << left;
    for (int i = 0; i < width; ++i) {
        std::cout << fill;
    }
    std::cout << right << std::endl;
}

static void drawCenteredText(const std::string& text, int width) {
    int padding = width - static_cast<int>(text.size());
    if (padding < 0) padding = 0;
    int left = padding / 2;
    int right = padding - left;
    std::cout << "|";
    for (int i = 0; i < left; ++i) std::cout << " ";
    std::cout << text;
    for (int i = 0; i < right; ++i) std::cout << " ";
    std::cout << "|" << std::endl;
}

void Render::drawHUD(const Player& player, int currentFloor) {
    std::cout << COLOR_CYAN;
    drawLine('+', '=', '+', 63);
    std::cout << COLOR_RESET;
    std::ostringstream oss;
    oss << "地牢地下  第 " << currentFloor << " 层 / " << MAX_FLOORS
        << "    生命 " << player.getHp() << "/" << player.getMaxHp()
        << "    攻击 " << player.getAttack()
        << "    防御 " << player.getDefense()
        << "    金币 " << player.getGold();
    drawCenteredText(oss.str(), 63);
    std::cout << COLOR_CYAN;
    drawLine('+', '=', '+', 63);
    std::cout << COLOR_RESET;
}

void Render::drawMessage(const std::string& msg) {
    std::cout << COLOR_YELLOW;
    drawLine('+', '-', '+', 63);
    std::cout << COLOR_RESET;
    std::cout << " " << msg << std::endl;
    std::cout << COLOR_YELLOW;
    drawLine('+', '-', '+', 63);
    std::cout << COLOR_RESET;
}

void Render::drawAll(const Map& map, const Player& player,
                     const std::vector<Monster*>& monsters,
                     const std::vector<Item*>& items,
                     const std::string& message) {
    clear();
    drawHUD(player, player.getFloor());

    // 逐行绘制地图 + 覆盖实体
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            bool drawn = false;

            // 检查玩家 ⭐ 红色高亮
            if (x == player.getPos().first && y == player.getPos().second) {
                std::cout << COLOR_BOLD << COLOR_RED << '@' << COLOR_RESET;
                drawn = true;
            }
            // 检查道具
            if (!drawn) {
                for (Item* item : items) {
                    if (item && x == item->getPos().first && y == item->getPos().second) {
                        if (item->getType() == ItemType::HEAL_POTION)
                            std::cout << COLOR_RED << item->getSymbol() << COLOR_RESET;
                        else if (item->getType() == ItemType::GOLD_COIN)
                            std::cout << COLOR_YELLOW << item->getSymbol() << COLOR_RESET;
                        else
                            std::cout << COLOR_CYAN << item->getSymbol() << COLOR_RESET;
                        drawn = true;
                        break;
                    }
                }
            }
            // 检查怪物 ⭐ 红色显示
            if (!drawn) {
                for (Monster* monster : monsters) {
                    if (monster && monster->isAlive() &&
                        x == monster->getPos().first && y == monster->getPos().second) {
                        // Boss 用紫色加粗，小怪用红色
                        if (monster->isBoss())
                            std::cout << COLOR_BOLD << COLOR_MAGENTA << monster->getSymbol() << COLOR_RESET;
                        else
                            std::cout << COLOR_RED << monster->getSymbol() << COLOR_RESET;
                        drawn = true;
                        break;
                    }
                }
            }
            // 画地形
            if (!drawn) {
                switch (map.getTile(x, y)) {
                    case Tile::WALL:       std::cout << COLOR_WHITE << '#' << COLOR_RESET; break;
                    case Tile::FLOOR:      std::cout << '.'; break;
                    case Tile::STAIRS_DOWN: std::cout << COLOR_GREEN << '>' << COLOR_RESET; break;
                }
            }
        }
        std::cout << std::endl;
    }

    // 底部图例
    std::cout << "  说明: ";
    std::cout << COLOR_RED << "@=你" << COLOR_RESET << "  ";
    std::cout << "#=墙  .=路  ";
    std::cout << COLOR_RED << "M=怪物" << COLOR_RESET << "  ";
    std::cout << COLOR_MAGENTA << "B=Boss" << COLOR_RESET << "  ";
    std::cout << COLOR_RED << "H=血瓶" << COLOR_RESET << "  ";
    std::cout << COLOR_YELLOW << "$=金币" << COLOR_RESET << "  ";
    std::cout << COLOR_GREEN << ">=楼梯" << COLOR_RESET << "  ";
    std::cout << "A/D=卷轴" << std::endl;
    drawMessage(message);
    std::cout << "  操作: [WASD移动] [空格待机] [I背包] [Q退出]" << std::endl;
}

void Render::drawGameOver(int floor, int gold) {
    clear();
    std::cout << std::endl;
    std::cout << "  +----------------------+" << std::endl;
    std::cout << "  |       游戏结束       |" << std::endl;
    std::cout << "  +----------------------+" << std::endl;
    std::cout << std::endl;
    std::cout << "  到达层数: " << floor << std::endl;
    std::cout << "  获得金币: " << gold << std::endl;
    std::cout << std::endl;
}

void Render::drawBattleScreen(const Player& player, const Monster& monster,
                              const std::vector<std::string>& battleLog,
                              const std::string& prompt) {
    clear();

    // 顶栏标题
    std::cout << COLOR_BOLD << COLOR_RED;
    drawLine('+', '=', '+', 62);
    drawCenteredText("战斗", 62);
    drawLine('+', '=', '+', 62);
    std::cout << COLOR_RESET;

    // 玩家信息
    std::cout << COLOR_BOLD << COLOR_GREEN << "  玩家  " << COLOR_RESET;
    std::cout << COLOR_BOLD << player.getName() << COLOR_RESET;
    std::cout << "  Lv." << player.getLevel() << std::endl;

    drawHPBar("  生命", player.getHp(), player.getMaxHp(), COLOR_GREEN);

    // ⭐ 玩家ATB速度条
    drawATBBar("  行动条", player.getAtbGauge(), COLOR_CYAN);
    std::cout << "  攻击:" << player.getAttack() << "  防御:" << player.getDefense() << "  速度:" << player.getSpeed() << std::endl;

    std::cout << std::endl;
    drawCenteredText("VS", 62);
    std::cout << std::endl;

    // 怪物信息
    std::string mColor = monster.isBoss() ? COLOR_MAGENTA : COLOR_RED;
    std::cout << COLOR_BOLD << mColor << "  怪物  " << COLOR_RESET;
    std::cout << mColor << COLOR_BOLD << monster.getName() << COLOR_RESET;
    if (monster.isBoss()) std::cout << COLOR_MAGENTA << " [BOSS]" << COLOR_RESET;
    std::cout << std::endl;

    drawHPBar("  生命", monster.getHp(), monster.getMaxHp(), mColor);

    // ⭐ 怪物ATB速度条
    drawATBBar("  行动条", monster.getAtbGauge(), COLOR_YELLOW);
    std::cout << "  攻击:" << monster.getAttack() << "  防御:" << monster.getDefense() << "  速度:" << monster.getSpeed() << std::endl;

    // 分割线 + 战斗日志
    std::cout << COLOR_YELLOW;
    drawLine('+', '-', '+', 62);
    std::cout << COLOR_RESET;
    int logLines = battleLog.size();
    int startIdx = (logLines > 4) ? logLines - 4 : 0;
    for (int i = startIdx; i < logLines; i++) {
        std::cout << "  " << battleLog[i] << std::endl;
    }
    if (logLines == 0) std::cout << "  战斗开始！" << std::endl;

    // 操作提示
    std::cout << COLOR_YELLOW;
    drawLine('+', '-', '+', 62);
    std::cout << COLOR_RESET;
    std::cout << COLOR_BOLD << prompt << COLOR_RESET << std::endl;
    std::cout << "  [a]攻击  [d]防御(-50%伤害)  [i]使用道具  [f]逃跑(" << (50 + player.getLevel() * 10) << "%)" << std::endl;
    std::cout << COLOR_RED << COLOR_BOLD;
    drawLine('+', '=', '+', 62);
    std::cout << COLOR_RESET;
}

// 绘制HP条（辅助函数）
static void drawHPBar(const std::string& label, int hp, int maxHp, const std::string& color) {
    int barWidth = 20;
    int filled = (maxHp > 0) ? (hp * barWidth / maxHp) : 0;
    if (filled < 0) filled = 0;
    if (filled > barWidth) filled = barWidth;

    std::cout << label << ": " << color << "[";
    for (int i = 0; i < barWidth; i++) {
        if (i < filled) std::cout << '#';
        else std::cout << '-';
    }
    std::cout << "]" << COLOR_RESET << "  " << hp << "/" << maxHp << std::endl;
}

static void drawATBBar(const std::string& label, float gauge, const std::string& color) {
    int barWidth = 20;
    int filled = static_cast<int>(gauge * barWidth / 100.0f);
    if (filled < 0) filled = 0;
    if (filled > barWidth) filled = barWidth;

    std::cout << label << ": " << color << "[";
    for (int i = 0; i < barWidth; i++) {
        if (i < filled) std::cout << '#';
        else std::cout << '-';
    }
    std::cout << "]" << COLOR_RESET << "  " << static_cast<int>(gauge) << "/100" << std::endl;
}

void Render::drawVictory() {
    clear();
    std::cout << std::endl;
    std::cout << "  +----------------------+" << std::endl;
    std::cout << "  |       恭喜通关       |" << std::endl;
    std::cout << "  +----------------------+" << std::endl;
    std::cout << std::endl;
    std::cout << "  你成功击败了地牢领主，逃出了地牢！" << std::endl;
    std::cout << std::endl;
}
