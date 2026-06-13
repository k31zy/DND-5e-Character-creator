#ifndef ABILITIES_H
#define ABILITIES_H

#include <string>
#include <array>
#include <map>

enum class Ability {
    STR = 0, DEX, CON, INT, WIS, CHA, COUNT
};

constexpr const char* ABILITY_NAMES[] = {"STR", "DEX", "CON", "INT", "WIS", "CHA"};
constexpr const char* ABILITY_FULL[] = {
    "Сила", "Ловкость", "Телосложение",
    "Интеллект", "Мудрость", "Харизма"
};

// Standard array for ability scores (PHB 2014)
constexpr int STANDARD_ARRAY[] = {15, 14, 13, 12, 10, 8};

// Ability modifier: (score - 10) / 2, rounded DOWN (toward negative infinity for negatives)
inline int ability_modifier(int score) {
    int diff = score - 10;
    if (diff >= 0) return diff / 2;
    return (diff - 1) / 2;  // floor for negative: -1/2 = 0, want -1; -3/2 = -1, want -2
}

class AbilityScores {
public:
    AbilityScores();

    // Set score for an ability
    void set(Ability a, int score);
    int get(Ability a) const;

    // Get modifier
    int mod(Ability a) const;

    // Assign from standard array
    void assign_standard_array(const std::array<Ability, 6>& order);

    // Point buy: 27 points, costs increase at 13+
    static int point_buy_cost(int score);

    // Display scores
    std::string to_string() const;

    // All scores
    const std::array<int, 6>& scores() const { return scores_; }

private:
    std::array<int, 6> scores_;
};

Ability ability_from_string(const std::string& s);

#endif
