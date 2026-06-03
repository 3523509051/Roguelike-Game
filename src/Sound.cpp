#include "Sound.h"

#ifdef _WIN32
#include <windows.h>
#endif

void Sound::playAttack() {
    // TODO: C实现
    // Windows: Beep(800, 100);
    // Linux: std::cout << "\a";
}

void Sound::playHit() {
    // TODO: C实现
    // Windows: Beep(400, 150);
}

void Sound::playLevelUp() {
    // TODO: C实现
    // 连续三声：
    // Beep(600, 100); Beep(800, 100); Beep(1000, 150);
}

void Sound::playVictory() {
    // TODO: C实现
    // 胜利旋律（多个音符组成）
    // Beep(523, 200); Beep(659, 200); Beep(784, 400);
}

void Sound::playGameOver() {
    // TODO: C实现
    // 失败音效（降调）
    // Beep(400, 300); Beep(300, 300); Beep(200, 500);
}

void Sound::playPickup() {
    // TODO: C实现
    // Beep(1000, 80);
}
