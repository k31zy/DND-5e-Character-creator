#include "abilities.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>

AbilityScores::AbilityScores() {
    scores_.fill(10);
}

void AbilityScores::set(Ability a, int score) {
    if (score < 3 || score > 20) throw std::out_of_range("Ability score must be 3-20");
    scores_[static_cast<int>(a)] = score;
}

int AbilityScores::get(Ability a) const {
    return scores_[static_cast<int>(a)];
}

int AbilityScores::mod(Ability a) const {
    return ability_modifier(scores_[static_cast<int>(a)]);
}

void AbilityScores::assign_standard_array(const std::array<Ability, 6>& order) {
    for (int i = 0; i < 6; i++) {
        scores_[static_cast<int>(order[i])] = STANDARD_ARRAY[i];
    }
}

int AbilityScores::point_buy_cost(int score) {
    // PHB 2014: 8=0, 9=1, 10=2, 11=3, 12=4, 13=5, 14=7, 15=9
    if (score < 8 || score > 15) return -1;
    static const int costs[] = {0, 1, 2, 3, 4, 5, 7, 9};
    return costs[score - 8];
}

std::string AbilityScores::to_string() const {
    std::ostringstream ss;
    for (int i = 0; i < 6; i++) {
        ss << ABILITY_NAMES[i] << ": " << scores_[i]
           << " (" << std::showpos << ability_modifier(scores_[i]) << std::noshowpos << ")";
        if (i < 5) ss << "  ";
    }
    return ss.str();
}

Ability ability_from_string(const std::string& s) {
    std::string upper = s;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    for (int i = 0; i < 6; i++) {
        if (upper == ABILITY_NAMES[i]) return static_cast<Ability>(i);
    }
    throw std::invalid_argument("Unknown ability: " + s);
}
