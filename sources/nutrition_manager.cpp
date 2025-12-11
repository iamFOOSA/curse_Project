#include "../headers/nutrition_manager.h"
#include "../headers/food.h"
#include <iostream>
#include <fstream>
#include <algorithm>

DailyMenu::DailyMenu(const std::string& d) : day(d), meal_count(0) {
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
        meals[meal_count++] = meal;
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

NutritionManager::NutritionManager(NutritionAdvisor* adv) : advisor(adv), day_count(0) {
    for (int i = 0; i < max_days; i++) {
        weekly_menu[i] = nullptr;
    }
}

NutritionManager::~NutritionManager() {
    for (int i = 0; i < day_count; i++) {
        delete weekly_menu[i];
    }
    for (auto& pair : product_database) {
        delete pair.second;
    }
}

bool NutritionManager::load_products_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string name;
    double calories, proteins, fats, carbs;

    while (file >> name >> calories >> proteins >> fats >> carbs) {
        std::replace(name.begin(), name.end(), '_', ' ');
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
}

QStringList NutritionManager::get_product_names_qt() const {
    QStringList list;
    for (const auto& pair : product_database) {
        list.append(QString::fromStdString(pair.first));
    }
    return list;
}

Product* NutritionManager::find_product_qt(const QString& name) const {
    return find_product(name.toStdString());
}

bool NutritionManager::add_product_qt(const QString& name, double calories, double proteins, double fats, double carbs) {
    return add_product(name.toStdString(), calories, proteins, fats, carbs);
}

bool NutritionManager::save_products_to_file(const std::string& filename) const { return true; }
bool NutritionManager::remove_product(const std::string& name) { return true; }
std::vector<std::string> NutritionManager::get_all_product_names() const { return {}; }
void NutritionManager::display_product_info(const std::string& name, double grams) const {}
