#include "Render.h"
#include "config.h"
#include <iostream>
#include <cstdlib>
#include <sstream>

// ===== ANSI color escape sequences =====
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"

// ===== helpers: write to ostream =====

static void drawLine(std::ostream& out, char left, char fill, char right, int width) {
    out << left;
    for (int i = 0; i < width; ++i) out << fill;
    out << right << '\n';
}

static void drawCenteredText(std::ostream& out, const std::string& text, int width) {
    int padding = width - static_cast<int>(text.size());
    if (padding < 0) padding = 0;
    int leftPad = padding / 2;
    int rightPad = padding - leftPad;
    out << '|';
    for (int i = 0; i < leftPad; ++i) out << ' ';
    out << text;
    for (int i = 0; i < rightPad; ++i) out << ' ';
    out << "|\n";
}

static void drawHPBar(std::ostream& out, const std::string& label,
                       int hp, int maxHp, const std::string& color) {
    const int barWidth = 20;
    int filled = (maxHp > 0) ? (hp * barWidth / maxHp) : 0;
    if (filled < 0) filled = 0;
    if (filled > barWidth) filled = barWidth;
    out << label << ": " << color << '[';
    for (int i = 0; i < barWidth; ++i) out << (i < filled ? '#' : '-');
    out << ']' << COLOR_RESET << "  " << hp << '/' << maxHp << '\n';
}

static void drawATBBar(std::ostream& out, const std::string& label,
                        float gauge, const std::string& color) {
    const int barWidth = 20;
    int filled = static_cast<int>(gauge * barWidth / 100.0f);
    if (filled < 0) filled = 0;
    if (filled > barWidth) filled = barWidth;
    out << label << ": " << color << '[';
    for (int i = 0; i < barWidth; ++i) out << (i < filled ? '#' : '-');
    out << ']' << COLOR_RESET << "  " << static_cast<int>(gauge) << "/100\n";
}

// ===== platform: atomic frame flush =====

#ifdef _WIN32
#include <windows.h>

static HANDLE g_hConsole = NULL;

static void initConsole() {
    g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(g_hConsole, &mode);
    SetConsoleMode(g_hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    std::ios::sync_with_stdio(false);
}

// Atomic write: hide cursor, clear screen, home, frame, show cursor
// ALL in one WriteConsoleA call => no flicker, no leftover
static void flushFrame(const std::string& frame) {
    std::ostringstream final;
    final << "\033[?25l"     // hide cursor
          << "\033[2J"       // CLEAR ENTIRE SCREEN (fixes short-line leftovers)
          << "\033[H"        // cursor home
          << frame
          << "\033[?25h";    // show cursor
    std::string out = final.str();
    DWORD written;
    WriteConsoleA(g_hConsole, out.c_str(), static_cast<DWORD>(out.size()), &written, NULL);
}
#else
static void initConsole() {}
static void flushFrame(const std::string& frame) {
    std::ostringstream final;
    final << "\033[?25l\033[2J\033[H" << frame << "\033[?25h";
    std::string out = final.str();
    std::cout << out << std::flush;
}
#endif

// ===== Render class =====

Render::Render() {
    initConsole();
}

void Render::clear() {
#ifdef _WIN32
    COORD pos = {0, 0};
    SetConsoleCursorPosition(g_hConsole, pos);
#else
    std::cout << "\033[H" << std::flush;
#endif
}

// ===== public wrappers (backward-compatible, for old callers) =====

void Render::drawHUD(const Player& player, int currentFloor) {
    drawHUD(std::cout, player, currentFloor);
}

void Render::drawMap(const Map& map) {
    (void)map;
    // deprecated — use drawAll()
}

void Render::drawEntities(const Player& player,
                          const std::vector<Monster*>& monsters,
                          const std::vector<Item*>& items) {
    (void)player; (void)monsters; (void)items;
    // deprecated — use drawAll()
}

void Render::drawMessage(const std::string& msg) {
    drawMessage(std::cout, msg);
}

// ===== internal draw helpers (write to ostream) =====

void Render::drawHUD(std::ostream& out, const Player& player, int currentFloor) {
    out << COLOR_CYAN;
    drawLine(out, '+', '=', '+', 63);
    out << COLOR_RESET;

    std::ostringstream oss;
    oss << "Dungeon Floor " << currentFloor << "/" << MAX_FLOORS
        << "    HP " << player.getHp() << "/" << player.getMaxHp()
        << "    ATK " << player.getAttack()
        << "    DEF " << player.getDefense()
        << "    Gold " << player.getGold();
    drawCenteredText(out, oss.str(), 63);

    out << COLOR_CYAN;
    drawLine(out, '+', '=', '+', 63);
    out << COLOR_RESET;
}

void Render::drawMessage(std::ostream& out, const std::string& msg) {
    out << COLOR_YELLOW;
    drawLine(out, '+', '-', '+', 63);
    out << COLOR_RESET;
    out << ' ' << msg << '\n';
    out << COLOR_YELLOW;
    drawLine(out, '+', '-', '+', 63);
    out << COLOR_RESET;
}

// ===== drawAll: double-buffered main view =====

void Render::drawAll(const Map& map, const Player& player,
                     const std::vector<Monster*>& monsters,
                     const std::vector<Item*>& items,
                     const std::string& message) {
    std::ostringstream b;

    // HUD
    drawHUD(b, player, player.getFloor());

    // Map tiles + entities
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            bool drawn = false;

            // Player
            if (x == player.getPos().first && y == player.getPos().second) {
                b << COLOR_BOLD << COLOR_GREEN << '@' << COLOR_RESET;
                drawn = true;
            }
            // Items
            if (!drawn) {
                for (Item* item : items) {
                    if (item && x == item->getPos().first && y == item->getPos().second) {
                        if (item->getType() == ItemType::HEAL_POTION)
                            b << COLOR_RED << item->getSymbol() << COLOR_RESET;
                        else if (item->getType() == ItemType::GOLD_COIN)
                            b << COLOR_YELLOW << item->getSymbol() << COLOR_RESET;
                        else
                            b << COLOR_CYAN << item->getSymbol() << COLOR_RESET;
                        drawn = true;
                        break;
                    }
                }
            }
            // Monsters
            if (!drawn) {
                for (Monster* monster : monsters) {
                    if (monster && monster->isAlive() &&
                        x == monster->getPos().first && y == monster->getPos().second) {
                        if (monster->isBoss())
                            b << COLOR_BOLD << COLOR_MAGENTA << monster->getSymbol() << COLOR_RESET;
                        else
                            b << COLOR_RED << monster->getSymbol() << COLOR_RESET;
                        drawn = true;
                        break;
                    }
                }
            }
            // Terrain
            if (!drawn) {
                switch (map.getTile(x, y)) {
                    case Tile::WALL:        b << COLOR_WHITE << '#' << COLOR_RESET; break;
                    case Tile::FLOOR:       b << '.'; break;
                    case Tile::STAIRS_DOWN: b << COLOR_GREEN << '>' << COLOR_RESET; break;
                }
            }
        }
        b << '\n';
    }

    // Legend
    b << "  Legend: ";
    b << COLOR_GREEN << "@=你" << COLOR_RESET << "  #=Wall  .=Floor  ";
    b << COLOR_RED << "M=怪物" << COLOR_RESET << "  ";
    b << COLOR_MAGENTA << "B=Boss" << COLOR_RESET << "  ";
    b << COLOR_RED << "H=Potion" << COLOR_RESET << "  ";
    b << COLOR_YELLOW << "$=马内" << COLOR_RESET << "  ";
    b << COLOR_GREEN << ">=楼梯" << COLOR_RESET << "  A/D=Scroll\n";

    drawMessage(b, message);
    b << "  [WASD move] [Space wait] [I inventory] [Q quit]\n";

    flushFrame(b.str());
}

// ===== drawBattleScreen: double-buffered battle view =====

void Render::drawBattleScreen(const Player& player, const Monster& monster,
                              const std::vector<std::string>& battleLog,
                              const std::string& prompt) {
    std::ostringstream b;

    // Top bar
    b << COLOR_BOLD << COLOR_RED;
    drawLine(b, '+', '=', '+', 62);
    drawCenteredText(b, "BATTLE", 62);
    drawLine(b, '+', '=', '+', 62);
    b << COLOR_RESET;

    // Player
    b << COLOR_BOLD << COLOR_GREEN << "  You     " << COLOR_RESET;
    b << COLOR_BOLD << player.getName() << COLOR_RESET;
    b << "  Lv." << player.getLevel() << '\n';
    drawHPBar(b, "  HP ", player.getHp(), player.getMaxHp(), COLOR_GREEN);
    drawATBBar(b, "  ATB", player.getAtbGauge(), COLOR_CYAN);
    b << "  ATK:" << player.getAttack()
      << "  DEF:" << player.getDefense()
      << "  SPD:" << player.getSpeed() << '\n';
    b << '\n';
    drawCenteredText(b, "VS", 62);
    b << '\n';

    // Monster
    std::string mColor = monster.isBoss() ? COLOR_MAGENTA : COLOR_RED;
    b << COLOR_BOLD << mColor << "  Enemy   " << COLOR_RESET;
    b << mColor << COLOR_BOLD << monster.getName() << COLOR_RESET;
    if (monster.isBoss()) b << COLOR_MAGENTA << " [BOSS]" << COLOR_RESET;
    b << '\n';
    drawHPBar(b, "  HP ", monster.getHp(), monster.getMaxHp(), mColor);
    drawATBBar(b, "  ATB", monster.getAtbGauge(), COLOR_YELLOW);
    b << "  ATK:" << monster.getAttack()
      << "  DEF:" << monster.getDefense()
      << "  SPD:" << monster.getSpeed() << '\n';

    // Battle log
    b << COLOR_YELLOW;
    drawLine(b, '+', '-', '+', 62);
    b << COLOR_RESET;
    int logLines = static_cast<int>(battleLog.size());
    int startIdx = (logLines > 4) ? logLines - 4 : 0;
    for (int i = startIdx; i < logLines; i++) {
        b << "  " << battleLog[i] << '\n';
    }
    if (logLines == 0) b << "  Battle start!\n";

    // Controls
    b << COLOR_YELLOW;
    drawLine(b, '+', '-', '+', 62);
    b << COLOR_RESET;
    b << COLOR_BOLD << prompt << COLOR_RESET << '\n';
    b << "  [a]攻击  [d]防御(-50%)  [i]物品  [f]当逃兵("
      << (50 + player.getLevel() * 10) << "%)\n";
    b << COLOR_RED << COLOR_BOLD;
    drawLine(b, '+', '=', '+', 62);
    b << COLOR_RESET;

    flushFrame(b.str());
}

// ===== game over / victory =====

void Render::drawGameOver(int floor, int gold) {
    std::ostringstream b;
    b << '\n';
    b << "  +----------------------+\n";
    b << "  |      GAME OVER       |\n";
    b << "  +----------------------+\n";
    b << '\n';
    b << "  Floor reached: " << floor << '\n';
    b << "  Gold collected: " << gold << '\n';
    b << '\n';
    flushFrame(b.str());
}

void Render::drawVictory() {
    std::ostringstream b;
    b << '\n';
    b << "  +----------------------+\n";
    b << "  |      YOU WIN!        |\n";
    b << "  +----------------------+\n";
    b << '\n';
    b << "  You defeated the Dungeon Lord and escaped!\n";
    b << '\n';
    flushFrame(b.str());
}
