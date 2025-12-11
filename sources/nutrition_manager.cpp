#include "../headers/nutrition_manager.h"
#include "../headers/food.h"
#include <iostream>
#include <fstream>
#include <algorithm>

DailyMenu::DailyMenu(const std::string& d) : day(d) {
    for (int i = 0; i < max_meals; i++) {
        meals[i] = nullptr;
    }
}

DailyMenu::~DailyMenu() {
    for (int i = 0; i < meal_count; i++) {
        delete meals[i];
    }
}

void DailyMenu::add_meal(Food* meal) {
    if (meal_count < max_meals) {
        meals[meal_count] = meal;
        ++meal_count;
    }
}

void DailyMenu::display_menu() const {
    std::cout << "Меню на " << day << ":\n";
    for (int i = 0; i < meal_count; i++) {
        if (meals[i]) {
            meals[i]->display();
        }
    }
}

void DailyMenu::save_to_file(const std::string& filename) const {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << "Меню на " << day << ":\n";
        for (int i = 0; i < meal_count; i++) {
            if (meals[i]) {
                file << meals[i]->get_name() << " - " << meals[i]->get_calories() << " ккал\n";
            }
        }
        file.close();
    }
}

NutritionManager::NutritionManager(NutritionAdvisor* adv) : advisor(adv) {
    for (int i = 0; i < max_days; i++) {
        weekly_menu[i] = nullptr;
    }
}

NutritionManager::~NutritionManager() {
    for (int i = 0; i < day_count; i++) {
        delete weekly_menu[i];
    }
    for (const auto& [key, value] : product_database) {
        (void)key;
        delete value;
    }
}

bool NutritionManager::load_products_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string name;
    double calories = 0.0;
    double proteins = 0.0;
    double fats = 0.0;
    double carbs = 0.0;

    while (file >> name >> calories >> proteins >> fats >> carbs) {
        std::ranges::replace(name, '_', ' ');
        product_database[name] = new Product(name, calories, proteins, fats, carbs);
    }

    file.close();
    return true;
}

Product* NutritionManager::find_product(const std::string& name) const {
    auto it = product_database.find(name);
    if (it != product_database.end()) {
        return it->second;
    }
    return nullptr;
}

bool NutritionManager::add_product(const std::string& name, double calories, double proteins, double fats, double carbs) {
    if (find_product(name)) {
        return false;
    }

    product_database[name] = new Product(name, calories, proteins, fats, carbs);
    return true;
}

void NutritionManager::create_detailed_menu() {
    // Метод зарезервирован для будущей реализации создания детального меню
}

void NutritionManager::display_weekly_menu() const {
    for (int i = 0; i < day_count; i++) {
        if (weekly_menu[i]) {
            weekly_menu[i]->display_menu();
        }
    }
}

void NutritionManager::save_all_menus(const std::string& filename) const {
    for (int i = 0; i < day_count; i++) {
        if (weekly_menu[i]) {
            weekly_menu[i]->save_to_file(filename);
        }
    }
}

void NutritionManager::suggest_menu() const {
    // Метод зарезервирован для будущей реализации предложения меню
}

QStringList NutritionManager::get_product_names_qt() const {
    QStringList list;
    for (const auto& [key, value] : product_database) {
        (void)value;
        list.append(QString::fromStdString(key));
    }
    return list;
}

Product* NutritionManager::find_product_qt(const QString& name) const {
    return find_product(name.toStdString());
}

bool NutritionManager::add_product_qt(const QString& name, double calories, double proteins, double fats, double carbs) {
    return add_product(name.toStdString(), calories, proteins, fats, carbs);
}

bool NutritionManager::save_products_to_file(const std::string& /*filename*/) const { 
    // Метод зарезервирован для будущей реализации сохранения продуктов
    return true; 
}

bool NutritionManager::remove_product(const std::string& /*name*/) { 
    // Метод зарезервирован для будущей реализации удаления продукта
    return true; 
}

std::vector<std::string> NutritionManager::get_all_product_names() const { 
    // Метод зарезервирован для будущей реализации получения всех имен продуктов
    return {}; 
}

void NutritionManager::display_product_info(const std::string& /*name*/, double /*grams*/) const {
    // Метод зарезервирован для будущей реализации отображения информации о продукте
}
