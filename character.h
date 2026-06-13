#ifndef CHARACTER_H
#define CHARACTER_H

#include "abilities.h"
#include "race.h"
#include "class.h"
#include "background.h"
#include <string>
#include <vector>

struct HitPoints {
    int max = 0;
    int current = 0;
    int temp = 0;
};

class Character {
public:
    Character();

    // Character creation steps
    void set_name(const std::string& name);
    void set_race(const Race* race);
    void set_class(const DnDClass* dnd_class);
    void set_abilities(const AbilityScores& scores);
    void set_background(const Background* bg);
    void finalize();  // apply race bonuses, calculate HP, etc.

    // Level up
    void level_up();

    // Getters
    const std::string& name() const { return name_; }
    const Race* race() const { return race_; }
    const DnDClass* dnd_class() const { return dnd_class_; }
    const Background* background() const { return background_; }
    const AbilityScores& abilities() const { return abilities_; }
    int level() const { return level_; }
    int proficiency_bonus() const { return ProficiencyBonus::at_level(level_); }
    const HitPoints& hp() const { return hp_; }

    // Derived stats
    int armor_class() const;
    int initiative_bonus() const { return abilities_.mod(Ability::DEX); }
    int passive_perception() const { return 10 + abilities_.mod(Ability::WIS); }
    int spell_save_dc() const;
    int spell_attack_bonus() const;

    // Display
    std::string character_sheet() const;
    void print_character_sheet() const;

    // For serialization / save
    std::string short_summary() const;

private:
    int roll_hit_die() const;

    std::string name_;
    const Race* race_ = nullptr;
    const DnDClass* dnd_class_ = nullptr;
    const Background* background_ = nullptr;
    AbilityScores abilities_;
    int level_ = 1;
    HitPoints hp_;
};

#endif
