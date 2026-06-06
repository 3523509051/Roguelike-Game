# 最新修改记录：存档交互补充（2026-06-06）

本次修改围绕“玩家可主动保存、商店内可保存、退出前提醒未保存进度”完成。

## 修改文件

### src/Input.cpp

- 新增 `P` 按键映射：`P` 转为内部按键 `p`，用于手动保存。
- 新增 `Y/N/C` 按键映射，用于退出前保存确认。

### src/Game.h

新增成员变量：

```cpp
bool hasUnsavedChanges_;
```

新增私有方法：

```cpp
bool saveCurrentGame();
bool confirmSaveBeforeExit();
```

### src/Game.cpp

新增统一保存函数：

```cpp
bool Game::saveCurrentGame();
```

主要逻辑：

```cpp
Save::saveGame(SAVE_FILE, *player_, *map_, monsters_, items_, *achievementMgr_);
```

交互变化：

- 主地图按 `P`：手动保存当前游戏。
- 保存成功后提示“保存成功。”。
- 保存失败后提示“保存失败。”。
- 主地图按 `Q`：如果当前进度未保存，会先提醒玩家选择是否保存。
- 退出提醒选项：
  - `Y`：保存并退出
  - `N`：不保存退出
  - `C`：取消退出

未保存状态规则：

- 新游戏开始后标记为未保存。
- 读档成功后标记为已保存。
- 玩家进行移动、等待等有效行动后标记为未保存。
- 保存成功后标记为已保存。
- 商店返回后保守标记为未保存，避免购买行为未被保存。

`Game::openShop()` 改为向商店传入保存回调：

```cpp
shop_->open(*player_, *render_, *input_, [this]() {
    return saveCurrentGame();
});
```

### src/Shop.h

新增头文件：

```cpp
#include <functional>
```

保留原商店入口：

```cpp
void open(Player& player, Render& render, Input& input);
```

新增带保存回调的商店入口：

```cpp
void open(Player& player, Render& render, Input& input,
          const std::function<bool()>& saveCallback);
```

### src/Shop.cpp

商店页面底部提示新增保存按钮：

```text
[1-6] 道具  [7-0] 技能  [p] 保存  [q] 离开
```

商店内按 `P` 时：

- 调用 `saveCallback()` 保存当前游戏。
- 保存成功显示“保存成功。”。
- 保存失败显示“保存失败。”。

## 当前存档操作方式

- 主地图按 `P`：保存当前游戏。
- 商店页面按 `P`：保存当前游戏。
- 主地图按 `Q`：如果有未保存进度，会询问是否保存后退出。
- 主菜单按 `2`：读取 `save.dat` 继续游戏。

## 验证结果

已使用 Visual Studio x64 开发者环境重新编译通过：

```text
[7/8] Linking CXX executable test_game.exe
[8/8] Linking CXX executable dungeon.exe
```

---
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

## 接口与文件修改记录（D 游戏系统）

本节记录 D 成员相关功能在控制台版本中的新增接口、调整接口和主要文件修改，便于后续维护和答辩说明。

### 1. 公共枚举与类型

#### `src/types.h`

新增 `AchievementID` 枚举，统一管理 10 个成就 ID：

```cpp
enum class AchievementID {
    FIRST_KILL,
    RICH_MAN,
    COMBO_MASTER,
    ESCAPE_ARTIST,
    DRAGON_SLAYER,
    COLLECTOR,
    IMMORTAL,
    PERFECT_DEFENSE,
    LOCKPICKER,
    MAX_LEVEL
};
```

#### `src/Item.h`

扩展 `ItemType`，为商店系统补充新商品类型：

```cpp
KEY,
REVIVE
```

### 2. 游戏主循环接口

#### `src/Game.h`

新增成员变量：

```cpp
AchievementManager* achievementMgr_;
Shop* shop_;
bool returnToMenu_;
```

新增公共方法：

```cpp
void showMainMenu();
void newGame();
void continueGame();
void showAchievements();
void showHelp();
void openShop();
```

主要用途：

- `showMainMenu()`：显示主菜单。
- `newGame()`：重置游戏数据并开始新游戏。
- `continueGame()`：读取存档并继续游戏。
- `showAchievements()`：显示成就列表。
- `showHelp()`：显示操作帮助。
- `openShop()`：打开商店。

#### `src/Game.cpp`

主要修改：

- 构造函数中初始化 `AchievementManager` 和 `Shop`。
- 实现主菜单、继续游戏、成就页、帮助页、商店入口。
- 游戏结束或胜利后返回主菜单。
- 进入新楼层前调用商店。
- 在战斗、拾取、升级、逃跑、死亡、胜利等事件中接入成就系统。

### 3. 成就系统接口

#### `src/Achievement.h`

成就 ID 改为引用 `types.h` 中的 `AchievementID`：

```cpp
#include "types.h"
```

`AchievementManager` 新增/完善接口：

```cpp
void onMonsterKilled(const Monster& monster);
void onGoldGained(int amount);
void onPlayerHit();
void onPlayerDeath();
void onEscape();
void onDefenseBlock(int dmg);
void onChestOpened();
void onItemPickup();
void onItemCollected();
void onLevelUp(int level);
void onVictory();

void displayAll() const;
bool isUnlocked(AchievementID id) const;
std::string getUnlockMask() const;
void loadUnlockMask(const std::string& mask);

void save(const std::string& filename) const;
void load(const std::string& filename);
```

触发逻辑：

- `FIRST_KILL`：第一次击杀怪物。
- `RICH_MAN`：累计获得金币达到 500。
- `COMBO_MASTER`：连续击杀 5 只怪物且期间未受伤。
- `ESCAPE_ARTIST`：成功逃跑 10 次。
- `DRAGON_SLAYER`：击败名字包含“地牢领主”或“Boss”的怪物。
- `COLLECTOR`：拾取 50 个道具。
- `IMMORTAL`：未死亡通关。
- `PERFECT_DEFENSE`：累计防御抵挡 100 点伤害。
- `LOCKPICKER`：打开 20 个宝箱。
- `MAX_LEVEL`：等级达到 10。

### 4. 商店系统接口

#### `src/Shop.h`

定义商品结构：

```cpp
struct ShopItem {
    std::string name;
    int price;
    ItemType type;
    int value;
};
```

商店接口：

```cpp
Shop();
void initItems();
void open(Player& player, Render& render, Input& input);
bool buyItem(Player& player, int index);
```

#### `src/Shop.cpp`

主要修改：

- 初始化 6 种商品：小血瓶、大血瓶、攻击卷轴、防御卷轴、钥匙、复活石。
- 支持按 `1-6` 购买，`q` 退出。
- 购买时检查金币是否足够。
- 购买成功后扣除金币并加入玩家背包。

### 5. 存档系统接口

#### `src/Player.h`

新增玩家状态恢复接口，用于读档：

```cpp
void loadState(int hp, int maxHp, int atk, int def, int level,
               int exp, int expToNext, int gold, int floor,
               int x, int y,
               const std::vector<std::string>& inventory);
```

#### `src/Save.h`

保留原有存档接口，同时新增支持成就系统的重载：

```cpp
static bool saveGame(const std::string& filename,
                     const Player& player,
                     const Map& map,
                     const std::vector<Monster*>& monsters,
                     const std::vector<Item*>& items,
                     const AchievementManager& achMgr);

static bool loadGame(const std::string& filename,
                     Player& player,
                     Map& map,
                     std::vector<Monster*>& monsters,
                     std::vector<Item*>& items,
                     AchievementManager& achMgr);
```

#### `src/Save.cpp`

主要修改：

- 保存楼层、玩家属性、经验、金币、坐标、背包。
- 保存成就掩码，例如 `0010110000`。
- 保存地图字符数据。
- 保存怪物类型、坐标、当前生命。
- 保存道具类型、坐标、数值。
- 读档时清理原有怪物和道具并重新创建。
- 读档时通过 `Player::loadState()` 恢复玩家私有状态。
- 读档时通过 `AchievementManager::loadUnlockMask()` 恢复成就状态。

### 6. 输入与渲染修改

#### `src/Input.cpp`

`Input::getKey()` 接口签名保持不变：

```cpp
char getKey();
```

实现修改：

- Windows 下改为读取控制台按键事件。
- 支持即时按键，不需要回车。
- 支持 WASD、方向键、空格、I/Q/F、数字键等。

#### `src/Render.cpp`

接口基本保持不变，主要修改内部显示：

- 将英文标签改为中文：
  - `HP` → `生命`
  - `ATK` → `攻击`
  - `DEF` → `防御`
  - `ATB` → `行动条`
  - `SPD` → `速度`
- 血条和行动条改为 ASCII 风格，避免特殊字符乱码。
- 去除部分容易乱码的 emoji 和特殊边框字符。
- `clear()` 改为 ANSI 光标重绘方式，减少整屏刷新闪烁。

### 7. 地图与怪物相关修复

#### `src/Map.cpp`

主要修改：

- 修复 BSP 房间生成中 `rand() % 0` 导致的除零崩溃。
- 房间过小时跳过生成，避免非法随机范围。
- 增加保底房间生成逻辑。
- 修复 BSP 子树连接逻辑，保证房间之间有走廊连通。

#### `src/Monster.h`

补充：

```cpp
#include "Player.h"
```

用于解决 `Monster::takeTurn(Player& player, ...)` 中 `Player` 类型未声明的问题。

### 8. 测试程序

#### `test/test_game.cpp`

新增/重写游戏系统自测，覆盖：

- 成就系统基本触发。
- `FIRST_KILL` 解锁检查。
- `RICH_MAN` 解锁检查。
- 商店购买小血瓶。
- 玩家金币和背包变化检查。
- `Save::fileExists("save.dat")` 存档存在性检查。
- 主菜单显示手动观察。

### 9. 入口文件

#### `src/main.cpp`

入口改为先显示主菜单：

```cpp
int main() {
    Game game;
    game.showMainMenu();
    return 0;
}
```

Windows 下新增控制台 UTF-8 设置：

```cpp
SetConsoleOutputCP(CP_UTF8);
SetConsoleCP(CP_UTF8);
```

### 10. CMake 配置

#### `CMakeLists.txt`

新增 CMake 构建配置，支持 Visual Studio 打开文件夹后识别目标：

- `dungeon`：控制台游戏主程序。
- `test_game`：游戏系统自测程序。
- `dungeon_win32`：后续新增的 Win32 窗口版目标。

其中控制台版本主要使用 `dungeon` 和 `test_game`。

### 11. 已验证内容

使用 Visual Studio 工具链完成过多次编译验证：

- 控制台主程序可完整编译。
- `test_game` 自测程序可编译运行。
- `test_map` 地图测试可编译运行。
- 地图生成不再出现除零崩溃。
- 主菜单、即时按键、中文显示、成就、商店、存档相关链路已做基础验证。
