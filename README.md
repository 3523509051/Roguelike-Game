# 🗡️ 终端 Roguelike 地牢探险游戏

一个纯 C++ 命令行 Roguelike 游戏，不依赖任何第三方库。

## ⭐ 核心特性

- 🗺️ **BSP算法随机生成20层地牢** — 每次游戏地图都不同
- ⚔️ **ATB速度条战斗系统** — 类似最终幻想，速度快的角色行动频率高
- 👾 **7种怪物** — 速度各不相同（蝙蝠15速，Boss6速）
- 🎨 **ANSI彩色渲染** — 玩家红色/小怪红色/Boss紫色/道具黄色
- 🔊 **音效系统** — Windows Beep API / Linux响铃
- 🏆 **10个成就系统** — 首杀/富豪/连杀/屠龙者等
- 🏪 **商店系统** — 每层前用金币购买道具
- 🎯 **地牢探索** — 陷阱/锁门/宝箱（20%是陷阱宝箱）

## 玩法

1. 在随机生成的地牢中探索（wasd 移动）
2. 遇到怪物进入ATB战斗（速度条满了才能行动）
3. 拾取道具恢复和强化
4. 找到楼梯 `>` 进入下一层
5. 共 20 层，击败第 20 层的 Boss 通关

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
├── main.cpp          — 入口
├── config.h          — 全局配置（20层地牢）
├── types.h           — 公共枚举
├── Map.h/cpp         — 地图系统（BSP随机生成）
├── MapObject.h/cpp   — ⭐陷阱/门/宝箱系统
├── Entity.h/cpp      — 实体基类（含ATB速度条）
├── Player.h/cpp      — 玩家类
├── Monster.h/cpp     — 怪物类（7种，含速度值）
├── Item.h/cpp        — 道具类
├── Combat.h/cpp      — 战斗引擎
├── Render.h/cpp      — 控制台渲染（ANSI彩色）
├── Sound.h/cpp       — ⭐音效系统
├── Input.h/cpp       — 按键输入
├── Save.h/cpp        — 存档系统
├── Achievement.h/cpp — ⭐成就系统（10个成就）
├── Shop.h/cpp        — ⭐商店系统
└── Game.h/cpp        — 游戏主循环（含ATB战斗）
```

## 团队分工（4人版）

- **成员A**：Map 地图系统 + MapObject 陷阱/门/宝箱
- **成员B**：Entity / Player / Monster / Item / Combat 战斗系统（含ATB）
- **成员C**：Render / Sound / Input / Save 渲染、音效、输入、存档
- **成员D**：Game / Achievement / Shop 主循环、成就、商店

## 技术亮点

- **ATB速度条战斗** — Entity中每个角色有speed和atbGauge，速度决定行动频率
- **BSP算法** — 二叉空间分割生成随机地牢
- **OOP三层继承** — Entity → Player/Monster → 7种具体怪物
- **ANSI转义序列** — 终端彩色渲染
- **AI辅助开发** — 全程记录prompt和人工调整

## 开发说明

详见 `docs/开发文档.md`，包含：
- 编译说明
- 代码规范
- Git工作流
- 调试技巧
- 测试指南
