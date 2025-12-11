#include "../headers/manager.h"
#include "../headers/food.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cctype>

std::string to_lower_case(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
    return result;
}

// Реализации методов Manager
Manager::Manager() : day_count(0) {
    for (int i = 0; i < max_days; i++) {
        weekly_menu[i] = nullptr;
    }
}

Manager::~Manager() {
    for (int i = 0; i < day_count; i++) {
        delete weekly_menu[i];
    }
    for (auto& pair : product_database) {
        delete pair.second;
    }
}

bool Manager::load_products_from_file() {
    std::ifstream file("products.txt");
    if (!file.is_open()) {
        std::cout << "Ошибка: файл products.txt не найден!" << std::endl;
        return false;
    }

    std::string name;
    double calories, proteins, fats, carbs;

    while (file >> name >> calories >> proteins >> fats >> carbs) {
        std::replace(name.begin(), name.end(), '_', ' ');
        product_database[name] = new Product(name, calories, proteins, fats, carbs);

        std::string search_name = to_lower_case(name);
        product_database[search_name] = product_database[name];
    }

    file.close();
    std::cout << "Загружено продуктов: " << product_database.size() / 2 << std::endl;
    return true;
}

void Manager::create_detailed_menu() {
    for (int i = 0; i < day_count; i++) {
        delete weekly_menu[i];
    }
    day_count = 0;

    DailyMenu* day1 = new DailyMenu("День 1");
    day1->add_meal(new Product("Овсяная каша с фруктами", 350, 12, 6, 57));
    day1->add_meal(new Product("Йогурт натуральный", 60, 4, 3.5, 5));
    day1->add_meal(new Product("Куриная грудка с гречкой", 165, 31, 3.6, 15));
    day1->add_meal(new Product("Овощной салат", 65, 2, 0.5, 13));
    day1->add_meal(new Product("Творог", 120, 17, 5, 3));
    weekly_menu[day_count++] = day1;

    DailyMenu* day2 = new DailyMenu("День 2");
    day2->add_meal(new Product("Омлет с овощами", 180, 12, 14, 5));
    day2->add_meal(new Product("Яблоко", 52, 0.3, 0.2, 14));
    day2->add_meal(new Product("Рыба на пару с рисом", 120, 20, 4, 25));
    day2->add_meal(new Product("Салат из свежих овощей", 45, 1.5, 0.3, 9));
    day2->add_meal(new Product("Кефир", 45, 3, 2, 4));
    weekly_menu[day_count++] = day2;

    DailyMenu* day3 = new DailyMenu("День 3");
    day3->add_meal(new Product("Творожная запеканка", 200, 15, 8, 20));
    day3->add_meal(new Product("Банановый смузи", 90, 2, 0.5, 20));
    day3->add_meal(new Product("Индейка с овощами", 140, 25, 3, 10));
    day3->add_meal(new Product("Греческий салат", 130, 5, 10, 6));
    day3->add_meal(new Product("Орехи грецкие", 650, 15, 65, 14));
    weekly_menu[day_count++] = day3;
}

void Manager::display_weekly_menu() const {
    std::cout << "\n    ПРИМЕРНОЕ МЕНЮ НА 3 ДНЯ    " << std::endl;
    for (int i = 0; i < day_count; i++) {
        weekly_menu[i]->display_menu();
    }
}

void Manager::save_all_menus() const {
    for (int i = 0; i < day_count; i++) {
        weekly_menu[i]->save_to_file("weekly_menu.txt");
    }
}

void Manager::suggest_menu() const {
    std::cout << "\n    ПРЕДЛАГАЕМОЕ МЕНУ    " << std::endl;
    display_weekly_menu();
    std::cout << "Вы можете следовать ему или составить свой рацион." << std::endl;
}

Product* Manager::find_product(const std::string& name) const {
    auto it = product_database.find(name);
    if (it != product_database.end()) {
        return it->second;
    }

    std::string search_name = to_lower_case(name);
    it = product_database.find(search_name);
    if (it != product_database.end()) {
        return it->second;
    }

    return nullptr;
}

void Manager::display_product_info(const std::string& name, double grams) const {
    Product* product = find_product(name);
    if (product) {
        std::cout << "Информация о продукте '" << name << "' (" << grams << "г):" << std::endl;
        std::cout << "Калории: " << product->get_calories() * grams / 100 << " ккал" << std::endl;
        std::cout << "Белки: " << product->get_proteins() * grams / 100 << " г" << std::endl;
        std::cout << "Жиры: " << product->get_fats() * grams / 100 << " г" << std::endl;
        std::cout << "Углеводы: " << product->get_carbs() * grams / 100 << " г" << std::endl;
    } else {
        std::cout << "Продукт '" << name << "' не найден в базе." << std::endl;
    }
}

bool Manager::add_product_to_file(const std::string& name, double calories, double proteins, double fats, double carbs)
{
    std::ofstream file("products.txt", std::ios::app);
    if (!file.is_open()) {
        std::cout << "Ошибка: не удалось открыть файл products.txt для записи!" << std::endl;
        return false;
    }

    std::string file_name = name;
    std::replace(file_name.begin(), file_name.end(), ' ', '_');

    file << file_name << " " << calories << " " << proteins << " " << fats << " " << carbs << "\n";
    file.close();

    std::string search_name = to_lower_case(name);
    product_database[name] = new Product(name, calories, proteins, fats, carbs);
    product_database[search_name] = product_database[name];

    std::cout << "Продукт '" << name << "' успешно добавлен в базу!" << std::endl;
    return true;
}
