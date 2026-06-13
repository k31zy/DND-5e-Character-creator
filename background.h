#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "abilities.h"
#include <string>
#include <vector>

struct Background {
    std::string id;
    std::string name_en;
    std::string name_ru;
    std::vector<std::string> skill_proficiencies;
    std::vector<std::string> tool_proficiencies;
    std::vector<std::string> languages;
    std::string equipment;
    std::string feature_name;
    std::string feature_desc;
};

class BackgroundData {
public:
    static const std::vector<Background>& all();
    static const Background* find(const std::string& id);
    static void list_all();
private:
    static std::vector<Background> load_backgrounds();
};

#endif
