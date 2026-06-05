#ifndef SAVE_H
#define SAVE_H

#include <string>
#include <vector>
#include "Player.h"
#include "Monster.h"
#include "Item.h"
#include "Map.h"
#include "Achievement.h"

class Save {
public:
    static bool saveGame(const std::string& filename,
                         const Player& player, const Map& map,
                         const std::vector<Monster*>& monsters,
                         const std::vector<Item*>& items);

    static bool saveGame(const std::string& filename,
                         const Player& player, const Map& map,
                         const std::vector<Monster*>& monsters,
                         const std::vector<Item*>& items,
                         const AchievementManager& achMgr);

    static bool loadGame(const std::string& filename,
                         Player& player, Map& map,
                         std::vector<Monster*>& monsters,
                         std::vector<Item*>& items);

    static bool loadGame(const std::string& filename,
                         Player& player, Map& map,
                         std::vector<Monster*>& monsters,
                         std::vector<Item*>& items,
                         AchievementManager& achMgr);

    static bool fileExists(const std::string& filename);
};

#endif // SAVE_H
