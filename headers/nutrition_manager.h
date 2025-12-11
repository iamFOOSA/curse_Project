#ifndef NUTRITION_MANAGER_H
#define NUTRITION_MANAGER_H

#include "food.h"
#include "user.h"
#include "nutrition_advisor.h"
#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <vector>
#include <array>
#include <QString>
#include <QStringList>
#include <functional>

class DailyMenu {
private:
    std::string day;
    static constexpr int max_meals = 5;
    std::array<Food*, max_meals> meals{};
    int meal_count = 0;

public:
    explicit DailyMenu(const std::string& d);
    ~DailyMenu();
    DailyMenu(const DailyMenu&) = delete;
    DailyMenu& operator=(const DailyMenu&) = delete;

    void add_meal(Food* meal);
    void display_menu() const;
    void save_to_file(const std::string& filename) const;
    std::string get_day() const { return day; }
    int get_meal_count() const { return meal_count; }
    Food* get_meal(int index) const {
        return (index >= 0 && index < meal_count) ? meals[index] : nullptr;
    }
};

class NutritionManager {
private:
    static constexpr int max_days = 7;
    std::array<DailyMenu*, max_days> weekly_menu{};
    int day_count = 0;
    std::map<std::string, Product*, std::less<>> product_database;
    NutritionAdvisor* advisor;

public:
    explicit NutritionManager(NutritionAdvisor* adv = nullptr);
    ~NutritionManager();
    NutritionManager(const NutritionManager&) = delete;
    NutritionManager& operator=(const NutritionManager&) = delete;

    // Управление продуктами
    bool load_products_from_file(const std::string& filename = "products.txt");
    bool save_products_to_file(const std::string& filename = "products.txt") const;
    Product* find_product(const std::string& name) const;
    bool add_product(const std::string& name, double calories, double proteins, double fats, double carbs);
    bool remove_product(const std::string& name);
    std::vector<std::string> get_all_product_names() const;

    // Управление меню
    void create_detailed_menu();
    void display_weekly_menu() const;
    void save_all_menus(const std::string& filename = "weekly_menu.txt") const;
    void suggest_menu() const;

    // Информация о продуктах
    void display_product_info(const std::string& name, double grams) const;

    // Геттеры
    int get_day_count() const { return day_count; }
    DailyMenu* get_daily_menu(int index) const {
        return (index >= 0 && index < day_count) ? weekly_menu[index] : nullptr;
    }
    size_t get_product_count() const { return product_database.size(); }

    // Qt-совместимые методы
    QStringList get_product_names_qt() const;
    Product* find_product_qt(const QString& name) const;
    bool add_product_qt(const QString& name, double calories, double proteins, double fats, double carbs);

    // Совместимость со старым кодом
    bool load_products_from_file() { return load_products_from_file("products.txt"); }
    bool add_product_to_file(const std::string& name, double calories, double proteins, double fats, double carbs) {
        return add_product(name, calories, proteins, fats, carbs);
    }
};

#endif
