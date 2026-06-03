#ifndef SOUND_H
#define SOUND_H

// 音效系统 - 使用 Windows Beep API 或 Linux 响铃
class Sound {
public:
    // TODO: C实现
    static void playAttack();     // 攻击音效 - Beep(800, 100)
    static void playHit();        // 受伤音效 - Beep(400, 150)
    static void playLevelUp();    // 升级音效 - 连续三声 Beep
    static void playVictory();    // 胜利旋律
    static void playGameOver();   // 失败音效
    static void playPickup();     // 拾取道具音效
};

#endif // SOUND_H
