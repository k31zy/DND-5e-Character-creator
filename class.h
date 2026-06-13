#ifndef CLASS_H
#define CLASS_H

#include "abilities.h"
#include <string>
#include <vector>

struct ClassFeature {
    int level;
    std::string name;
    std::string description;
};

struct ProficiencyBonus {
    static int at_level(int level) {
        return 2 + (level - 1) / 4;
    }
};

// Spell slot progression for full casters
struct SpellSlots {
    int cantrips = 0;
    int level1 = 0, level2 = 0, level3 = 0, level4 = 0;
    int level5 = 0, level6 = 0, level7 = 0, level8 = 0, level9 = 0;
};

enum class CasterType { NONE, FULL, HALF, THIRD };

struct DnDClass {
    std::string id;
    std::string name_en;
    std::string name_ru;
    int hit_die = 8;                    // d8, d10, d12 etc.
    Ability primary_ability;
    Ability saving_throw1, saving_throw2;
    int skill_choices = 2;              // number of class skills to pick
    std::vector<std::string> class_skills;
    CasterType caster_type = CasterType::NONE;
    Ability spellcasting_ability = Ability::CHA;
    std::vector<ClassFeature> features;

    // Get features gained at a specific level
    std::vector<ClassFeature> features_at_level(int level) const;

    // Spell slots for full/half/third casters
    static SpellSlots spell_slots(CasterType type, int level);
};

class ClassData {
public:
    static const std::vector<DnDClass>& all();
    static const DnDClass* find(const std::string& id);
    static void list_all();
private:
    static std::vector<DnDClass> load_classes();
};

#endif
