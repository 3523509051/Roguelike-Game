#include "Game.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    Game game;
    game.showMainMenu();   // 显示菜单，用户选择新游戏或继续
    return 0;
}
