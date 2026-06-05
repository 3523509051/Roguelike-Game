#include "Input.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <iostream>
#endif

Input::Input() {}

char Input::getKey() {
#ifdef _WIN32
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    DWORD oldMode = 0;
    GetConsoleMode(input, &oldMode);
    SetConsoleMode(input, oldMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));

    char result = '\0';
    INPUT_RECORD record;
    DWORD read = 0;
    while (ReadConsoleInput(input, &record, 1, &read)) {
        if (record.EventType != KEY_EVENT) {
            continue;
        }

        const KEY_EVENT_RECORD& key = record.Event.KeyEvent;
        if (!key.bKeyDown) {
            continue;
        }

        switch (key.wVirtualKeyCode) {
            case VK_UP: result = 'w'; break;
            case VK_DOWN: result = 's'; break;
            case VK_LEFT: result = 'a'; break;
            case VK_RIGHT: result = 'd'; break;
            case VK_SPACE: result = ' '; break;
            case VK_RETURN: result = '\n'; break;
            case VK_ESCAPE: result = 'q'; break;
            case 'W': result = 'w'; break;
            case 'A': result = 'a'; break;
            case 'S': result = 's'; break;
            case 'D': result = 'd'; break;
            case 'I': result = 'i'; break;
            case 'Q': result = 'q'; break;
            case 'F': result = 'f'; break;
            case '1': result = '1'; break;
            case '2': result = '2'; break;
            case '3': result = '3'; break;
            case '4': result = '4'; break;
            case '5': result = '5'; break;
            case '6': result = '6'; break;
            default:
                if (key.uChar.AsciiChar != '\0') {
                    result = key.uChar.AsciiChar;
                }
                break;
        }

        if (result != '\0') {
            break;
        }
    }

    SetConsoleMode(input, oldMode);
    return result;
#else
    char c;
    std::cin >> c;
    return c;
#endif
}
