#ifndef CONFIG_H
#define CONFIG_H

// ===== 地图配置 =====
const int MAP_WIDTH = 60;
const int MAP_HEIGHT = 25;

// ===== 玩家初始属性 =====
const int PLAYER_INIT_HP = 20;
const int PLAYER_INIT_ATK = 5;
const int PLAYER_INIT_DEF = 2;

// ===== 每层生成数量 =====
const int MONSTERS_PER_FLOOR = 8;
const int ITEMS_PER_FLOOR = 5;
const int MAX_FLOORS = 5;

// ===== 控制台颜色（可选） =====
const bool ENABLE_COLOR = true;

#endif // CONFIG_H
