#ifndef MANAGER_H
#define MANAGER_H

#include "user.h"
#include "nutrition_manager.h"
#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cctype>


class Manager {
private:
    static const int max_days = 7;
    DailyMenu* weekly_menu[max_days];
    int day_count;
    std::map<std::string, class Product*> product_database;

public:
    Manager();
    ~Manager();

    bool load_products_from_file();
    void create_detailed_menu();
    void display_weekly_menu() const;
    void save_all_menus() const;
    void suggest_menu() const;

    class Product* find_product(const std::string& name) const;
    void display_product_info(const std::string& name, double grams) const;

    bool add_product_to_file(const std::string& name, double calories, double proteins, double fats, double carbs);
};

#endif
