#ifndef RACE_H
#define RACE_H

#include "abilities.h"
#include <string>
#include <vector>
#include <map>

struct RaceTrait {
    std::string name;
    std::string description;
};

struct Race {
    std::string id;
    std::string name_en;
    std::string name_ru;
    int speed = 30;                    // feet per round
    std::string size = "Medium";
    std::vector<std::pair<Ability, int>> ability_bonuses; // +2 STR, +1 CHA etc.
    std::vector<RaceTrait> traits;
    std::vector<std::string> languages;
};

class RaceData {
public:
    static const std::vector<Race>& all();
    static const Race* find(const std::string& id);
    static void list_all();

private:
    static std::vector<Race> load_races();
};

#endif
