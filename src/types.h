#ifndef TYPES_H
#define TYPES_H

// 地图格子类型
enum class Tile {
    WALL,        // '#'
    FLOOR,       // '.'
    STAIRS_DOWN  // '>'
};

// 成就ID
enum class AchievementID {
    FIRST_KILL,       // 击败第一只怪物
    RICH_MAN,         // 累计500金币
    COMBO_MASTER,     // 连杀5只不受伤
    ESCAPE_ARTIST,    // 逃跑10次
    DRAGON_SLAYER,    // 击败地牢领主
    COLLECTOR,        // 拾取50个道具
    IMMORTAL,         // 一条命通关
    PERFECT_DEFENSE,  // 防御挡下100点伤害
    LOCKPICKER,       // 打开20个宝箱
    MAX_LEVEL         // 升到Lv.10
};

#endif // TYPES_H
