#include "character.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <limits>

static void clear_screen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

static int read_int(int min, int max) {
    int val;
    while (true) {
        std::cin >> val;
        if (std::cin.fail() || val < min || val > max) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Введите число от " << min << " до " << max << ": ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
    }
}

static std::string read_line() {
    std::string s;
    std::getline(std::cin, s);
    return s;
}

static void print_header(const std::string& title) {
    std::cout << "\n  " << BOX_TL << hr(50) << BOX_TR << "\n";
    std::cout << "  " << BOX_V << " " << pad_right(title, 48) << BOX_V << "\n";
    std::cout << "  " << BOX_BL << hr(50) << BOX_BR << "\n\n";
}

static void print_step(int n, const std::string& name) {
    std::cout << "  ◆ Шаг " << n << ": " << name << "\n\n";
}

int main() {
    Character character;

    clear_screen();
    print_header("СОЗДАНИЕ ПЕРСОНАЖА D&D 5E (PHB 2014)");
    std::cout << "  Добро пожаловать! Создадим вашего персонажа.\n";

    // Step 1: Name
    print_step(1, "Имя персонажа");
    std::cout << "  Введите имя: ";
    std::string name = read_line();
    character.set_name(name);

    // Step 2: Race
    print_step(2, "Выбор расы");
    RaceData::list_all();
    std::cout << "  Ваш выбор (1-" << RaceData::all().size() << "): ";
    int race_idx = read_int(1, RaceData::all().size()) - 1;
    character.set_race(&RaceData::all()[race_idx]);

    const Race* r = &RaceData::all()[race_idx];
    std::cout << "\n  ◆ Выбрано: " << r->name_ru << "\n";
    std::cout << "  ├ Скорость: " << r->speed << " фт\n";
    std::cout << "  ├ Размер: " << r->size << "\n";
    std::cout << "  └ Бонусы: ";
    bool first = true;
    for (auto& [a, v] : r->ability_bonuses) {
        if (!first) std::cout << ", ";
        std::cout << "+" << v << " " << ABILITY_NAMES[static_cast<int>(a)];
        first = false;
    }
    std::cout << "\n";

    // Step 3: Ability scores
    AbilityScores base_scores;

    print_step(3, "Распределение характеристик");
    std::cout << "  Стандартный набор: 15, 14, 13, 12, 10, 8\n\n";
    std::cout << "  Назначайте значения по порядку:\n";

    for (int i = 0; i < 6; i++) {
        std::cout << "\n  ▸ Значение " << STANDARD_ARRAY[i] << " → в какую характеристику?\n";
        for (int j = 0; j < 6; j++) {
            auto a = static_cast<Ability>(j);
            std::cout << "      " << (j+1) << ". " << pad_right(ABILITY_FULL[j], 16);
            if (base_scores.get(a) > 0)
                std::cout << "[занято: " << base_scores.get(a) << "]";
            std::cout << "\n";
        }
        std::cout << "    Выбор: ";
        int choice = read_int(1, 6) - 1;
        base_scores.set(static_cast<Ability>(choice), STANDARD_ARRAY[i]);
    }

    std::cout << "\n  ◆ Итог: " << base_scores.to_string() << "\n";
    character.set_abilities(base_scores);

    // Step 4: Class
    print_step(4, "Выбор класса");
    ClassData::list_all();
    std::cout << "  Ваш выбор (1-" << ClassData::all().size() << "): ";
    int class_idx = read_int(1, ClassData::all().size()) - 1;
    character.set_class(&ClassData::all()[class_idx]);

    const DnDClass* cls = &ClassData::all()[class_idx];
    std::cout << "\n  ◆ Выбрано: " << cls->name_ru << "\n";
    std::cout << "  ├ Кость хитов: к" << cls->hit_die << "\n";
    std::cout << "  └ Главная характеристика: "
              << ABILITY_FULL[static_cast<int>(cls->primary_ability)] << "\n";

    // Step 5: Background
    print_step(5, "Выбор предыстории");
    BackgroundData::list_all();
    std::cout << "  Ваш выбор (1-" << BackgroundData::all().size() << "): ";
    int bg_idx = read_int(1, BackgroundData::all().size()) - 1;
    character.set_background(&BackgroundData::all()[bg_idx]);

    // Finalize
    character.finalize();

    // Show character sheet
    clear_screen();
    character.print_character_sheet();

    // Action menu
    std::cout << "\n";
    std::cout << "  " << BOX_TL << hr(30) << BOX_TR << "\n";
    std::cout << "  " << BOX_V << " Действия:" << std::string(19, ' ') << BOX_V << "\n";
    std::cout << "  " << BOX_V << " 1. Повысить уровень" << std::string(9, ' ') << BOX_V << "\n";
    std::cout << "  " << BOX_V << " 2. Показать лист персонажа" << BOX_V << "\n";
    std::cout << "  " << BOX_V << " 3. Краткая сводка" << std::string(13, ' ') << BOX_V << "\n";
    std::cout << "  " << BOX_V << " 4. Выход" << std::string(22, ' ') << BOX_V << "\n";
    std::cout << "  " << BOX_BL << hr(30) << BOX_BR << "\n";

    while (true) {
        std::cout << "\n  ▸ Выбор: ";
        int action = read_int(1, 4);

        switch (action) {
        case 1:
            character.level_up();
            break;
        case 2:
            clear_screen();
            character.print_character_sheet();
            break;
        case 3:
            std::cout << "\n  " << character.short_summary() << "\n";
            break;
        case 4:
            std::cout << "\n  ◆ До встречи, " << character.name() << "!\n\n";
            return 0;
        }
    }

    return 0;
}
