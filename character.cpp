#include "character.h"
#include "utils.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

Character::Character() {
    std::srand(std::time(nullptr));
}

void Character::set_name(const std::string& name) { name_ = name; }
void Character::set_race(const Race* race) { race_ = race; }
void Character::set_class(const DnDClass* dnd_class) { dnd_class_ = dnd_class; }
void Character::set_abilities(const AbilityScores& scores) { abilities_ = scores; }
void Character::set_background(const Background* bg) { background_ = bg; }

void Character::finalize() {
    if (race_) {
        for (auto& [a, v] : race_->ability_bonuses) {
            int new_score = abilities_.get(a) + v;
            if (new_score > 20) new_score = 20;
            abilities_.set(a, new_score);
        }
    }
    if (dnd_class_) {
        hp_.max = dnd_class_->hit_die + abilities_.mod(Ability::CON);
        hp_.current = hp_.max;
    }
}

void Character::level_up() {
    if (level_ >= 20) {
        std::cout << "\n  ⚠ Уже максимальный уровень (20)!\n";
        return;
    }
    level_++;
    int hp_gain = roll_hit_die() + abilities_.mod(Ability::CON);
    if (hp_gain < 1) hp_gain = 1;
    hp_.max += hp_gain;
    hp_.current = hp_.max;

    std::cout << "\n  ◆ Новый уровень! Теперь уровень " << level_ << "\n";
    std::cout << "  ├ Получено " << hp_gain << " ХП (всего: " << hp_.max << ")\n";
    std::cout << "  ├ Бонус мастерства: +" << proficiency_bonus() << "\n";

    auto feats = dnd_class_->features_at_level(level_);
    if (!feats.empty()) {
        std::cout << "  └ Новые умения:\n";
        for (auto& f : feats) {
            std::cout << "     • " << f.name << "\n";
        }
    } else {
        std::cout << "  └ Новых умений на этом уровне нет\n";
    }
}

int Character::roll_hit_die() const {
    return (dnd_class_->hit_die / 2) + 1;
}

int Character::armor_class() const {
    int base = 10 + abilities_.mod(Ability::DEX);
    if (dnd_class_->id == "monk")
        base = 10 + abilities_.mod(Ability::DEX) + abilities_.mod(Ability::WIS);
    else if (dnd_class_->id == "barbarian")
        base = 10 + abilities_.mod(Ability::DEX) + abilities_.mod(Ability::CON);
    return base;
}

int Character::spell_save_dc() const {
    if (dnd_class_->caster_type == CasterType::NONE) return 0;
    return 8 + proficiency_bonus() + abilities_.mod(dnd_class_->spellcasting_ability);
}

int Character::spell_attack_bonus() const {
    if (dnd_class_->caster_type == CasterType::NONE) return 0;
    return proficiency_bonus() + abilities_.mod(dnd_class_->spellcasting_ability);
}

std::string Character::character_sheet() const {
    std::ostringstream ss;
    const int W = 64; // sheet width

    auto box_top = [&]() { ss << "  " << BOX_TL << hr(W) << BOX_TR << "\n"; };
    auto box_bot = [&]() { ss << "  " << BOX_BL << hr(W) << BOX_BR << "\n"; };
    auto box_sep = [&]() { ss << "  " << BOX_V << hr(W) << BOX_V << "\n"; };
    auto box_row = [&](const std::string& label, const std::string& value) {
        ss << "  " << BOX_V << " " << pad_right(label, 20) << pad_right(value, W - 22) << BOX_V << "\n";
    };

    ss << "\n";
    box_top();
    ss << "  " << BOX_V << " " << pad_right("ЛИСТ ПЕРСОНАЖА D&D 5E", W - 2) << BOX_V << "\n";
    box_sep();

    // Basic info
    box_row("Имя", name_);
    if (race_) box_row("Раса", race_->name_ru);
    if (dnd_class_) {
        std::string class_info = dnd_class_->name_ru + "  •  Уровень " + std::to_string(level_);
        box_row("Класс", class_info);
    }
    if (background_) box_row("Предыстория", background_->name_ru);

    // Combat stats
    box_sep();
    {
        std::ostringstream combat;
        combat << "ХП " << hp_.max << " (к" << (dnd_class_ ? dnd_class_->hit_die : 0) << "+ТЕЛ)"
               << "  │  КД " << armor_class()
               << "  │  Инициатива " << std::showpos << initiative_bonus() << std::noshowpos;
        box_row("Бой", combat.str());
    }
    {
        std::ostringstream prof;
        prof << "БМ +" << proficiency_bonus()
             << "  │  Скорость " << (race_ ? race_->speed : 30) << " фт"
             << "  │  Пасс. Восприятие " << passive_perception();
        box_row("", prof.str());
    }

    // Ability scores
    box_sep();
    ss << "  " << BOX_V << " " << pad_right("ХАРАКТЕРИСТИКИ", W - 2) << BOX_V << "\n";
    box_sep();
    for (int i = 0; i < 6; i += 2) {
        std::ostringstream line;
        for (int j = 0; j < 2 && i + j < 6; j++) {
            Ability a = static_cast<Ability>(i + j);
            int score = abilities_.get(a);
            int mod = abilities_.mod(a);
            line << std::left << std::setw(14) << ABILITY_FULL[i + j]
                 << ": " << std::setw(2) << score
                 << " (" << std::showpos << mod << std::noshowpos << ")";
            if (j == 0) line << "    ";
        }
        ss << "  " << BOX_V << " " << pad_right(line.str(), W - 2) << BOX_V << "\n";
    }

    // Saving throws
    if (dnd_class_) {
        std::ostringstream saves;
        saves << ABILITY_FULL[static_cast<int>(dnd_class_->saving_throw1)]
              << " +" << (proficiency_bonus() + abilities_.mod(dnd_class_->saving_throw1))
              << "    "
              << ABILITY_FULL[static_cast<int>(dnd_class_->saving_throw2)]
              << " +" << (proficiency_bonus() + abilities_.mod(dnd_class_->saving_throw2));
        box_row("Спасброски", saves.str());
    }

    // Spellcasting
    if (dnd_class_ && dnd_class_->caster_type != CasterType::NONE) {
        box_sep();
        ss << "  " << BOX_V << " " << pad_right("ЗАКЛИНАНИЯ", W - 2) << BOX_V << "\n";
        box_sep();

        std::ostringstream sc;
        sc << "СЛ " << spell_save_dc() << "  │  Атака +" << spell_attack_bonus()
           << "  │  " << ABILITY_FULL[static_cast<int>(dnd_class_->spellcasting_ability)];
        box_row("", sc.str());

        auto slots = DnDClass::spell_slots(dnd_class_->caster_type, level_);
        if (slots.cantrips || slots.level1) {
            std::ostringstream sl;
            if (slots.cantrips) sl << "Заговоры: " << slots.cantrips << "  ";
            if (slots.level1) sl << "1-й:" << slots.level1 << " ";
            if (slots.level2) sl << "2-й:" << slots.level2 << " ";
            if (slots.level3) sl << "3-й:" << slots.level3 << " ";
            if (slots.level4) sl << "4-й:" << slots.level4 << " ";
            if (slots.level5) sl << "5-й:" << slots.level5 << " ";
            if (slots.level6) sl << "6-й:" << slots.level6 << " ";
            box_row("Ячейки", sl.str());
        }
    }

    // Race traits
    if (race_ && !race_->traits.empty()) {
        box_sep();
        ss << "  " << BOX_V << " " << pad_right("РАСОВЫЕ ЧЕРТЫ", W - 2) << BOX_V << "\n";
        box_sep();
        for (auto& t : race_->traits) {
            ss << "  " << BOX_V << " " << pad_right(t.name + ": " + t.description, W - 2) << BOX_V << "\n";
        }
    }

    // Class features
    if (dnd_class_) {
        box_sep();
        ss << "  " << BOX_V << " " << pad_right("УМЕНИЯ КЛАССА — Уровень " + std::to_string(level_), W - 2) << BOX_V << "\n";
        box_sep();
        for (auto& f : dnd_class_->features) {
            if (f.level <= level_) {
                std::string text = "[Ур." + std::to_string(f.level) + "] " + f.name + ": " + f.description;
                ss << "  " << BOX_V << " " << pad_right(text, W - 2) << BOX_V << "\n";
            }
        }
    }

    box_bot();
    return ss.str();
}

void Character::print_character_sheet() const {
    std::cout << character_sheet();
}

std::string Character::short_summary() const {
    std::ostringstream ss;
    ss << name_ << "  •  Уровень " << level_ << "  •  "
       << (dnd_class_ ? dnd_class_->name_ru : "?") << "  •  "
       << (race_ ? race_->name_ru : "?") << "  •  "
       << "ХП:" << hp_.max << "  КД:" << armor_class();
    return ss.str();
}
