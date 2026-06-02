# 🗡️ 终端 Roguelike 地牢探险游戏

一个纯 C++ 命令行 Roguelike 游戏，不依赖任何第三方库。

## 玩法

1. 在随机生成的地牢中探索（wasd 移动）
2. 遇到怪物自动战斗（回合制）
3. 拾取道具恢复和强化
4. 找到楼梯 `>` 进入下一层
5. 共 5 层，击败第 5 层的 Boss 通关

## 符号说明

| 符号 | 含义 |
|------|------|
| `@` 🔴 | 你（玩家）—— 红色 |
| `#` | 墙壁 |
| `.` | 地板 |
| `>` 🟢 | 楼梯（下一层）|
| `M` 🔴 | 小怪（史莱姆/蝙蝠/骷髅/哥布林/狼）|
| `B` 🟣 | Boss（地牢领主/死亡骑士）—— 紫色加粗 |
| `♥` 🔴 | 血瓶 |
| `⚔` | 攻击卷轴 |
| `🛡` | 防御卷轴 |
| `$` 🟡 | 金币 |

> 小怪均为红色 `M` 显示（遭遇前无法区分类型），Boss 为紫色加粗 `B` 显示。

## 编译与运行

### 环境要求
- C++11 及以上编译器
- Make（可选）

### 编译
```bash
# 使用 Make
make

# 或手动编译
g++ -std=c++11 -o dungeon.exe src/*.cpp

# 运行
./dungeon.exe
```

### 测试
```bash
make test_map
make test_combat
make test_render
```

## 项目结构

```
src/
├── main.cpp       — 入口
├── config.h       — 全局配置
├── types.h        — 公共枚举
├── Map.h/cpp      — 地图系统（BSP随机生成）
├── Entity.h/cpp   — 实体基类
├── Player.h/cpp   — 玩家类
├── Monster.h/cpp  — 怪物类（4种）
├── Item.h/cpp     — 道具类
├── Combat.h/cpp   — 战斗引擎
├── Render.h/cpp   — 控制台渲染
├── Input.h/cpp    — 按键输入
├── Save.h/cpp     — 存档系统
└── Game.h/cpp     — 游戏主循环
```

## 团队分工

- **成员A**：Map 地图系统（BSP 算法）
- **成员B**：Entity / Player / Monster / Item / Combat 战斗系统
- **成员C**：Render / Input / Save / Game 渲染、输入、存档、主循环
