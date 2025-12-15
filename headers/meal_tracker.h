#ifndef MEAL_TRACKER_H
#define MEAL_TRACKER_H

#include "food.h"
#include "user.h"
#include "nutrition_manager.h"
#include "nutrition_advisor.h"
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <QString>
#include <QMap>
#include <QVector>


struct MealEntryParams {
    std::string date = "";
    std::string meal_type = "";
    std::string product_name = "";
    double grams = 0;
    double calories = 0;
    double proteins = 0;
    double fats = 0;
    double carbs = 0;

    // Конструкторы для удобства
    MealEntryParams() = default;

    MealEntryParams(const std::string& d, const std::string& mt,
                    const std::string& pn, double g)
        : date(d), meal_type(mt), product_name(pn), grams(g) {}

    MealEntryParams(const std::string& d, const std::string& mt,
                    const std::string& pn, double g,
                    double cal, double prot, double f, double c)
        : date(d), meal_type(mt), product_name(pn), grams(g),
        calories(cal), proteins(prot), fats(f), carbs(c) {}
};


struct MealEntry {
    std::string date;
    std::string meal_type;
    std::string product_name;
    double grams;
    double calories;
    double proteins;
    double fats;
    double carbs;

    explicit MealEntry(const MealEntryParams& params = {})
        : date(params.date), meal_type(params.meal_type),
        product_name(params.product_name), grams(params.grams),
        calories(params.calories), proteins(params.proteins),
        fats(params.fats), carbs(params.carbs) {}
};

class MealTracker {
private:
    User* current_user;
    NutritionManager* manager;
    NutritionAdvisor* advisor;

    std::vector<MealEntry> daily_meals;
    std::string current_date;

    double total_calories = 0.0;
    double total_proteins = 0.0;
    double total_fats = 0.0;
    double total_carbs = 0.0;

public:
    explicit MealTracker(User* user = nullptr, NutritionManager* mgr = nullptr, NutritionAdvisor* adv = nullptr);
    ~MealTracker() = default;

    // Управление приемами пищи
    bool add_meal(const std::string& meal_type, const std::string& product_name, double grams) const;
    bool remove_meal(int index);
    void clear_meals();

    // Расчеты
    void calculate_totals();
    double get_calories_percentage() const;
    double get_proteins_percentage() const;
    double get_fats_percentage() const;
    double get_carbs_percentage() const;

    // Сохранение и загрузка
    bool save_to_file(const std::string& filename = "daily_meals.txt") const;
    bool load_from_file(const std::string& filename = "daily_meals.txt");
    bool save_summary_to_file(const std::string& filename = "daily_summary.txt") const;

    // Геттеры
    double get_total_calories() const { return total_calories; }
    double get_total_proteins() const { return total_proteins; }
    double get_total_fats() const { return total_fats; }
    double get_total_carbs() const { return total_carbs; }
    std::string get_current_date() const { return current_date; }
    size_t get_meal_count() const { return daily_meals.size(); }
    MealEntry get_meal(int index) const {
        return (index >= 0 && index < daily_meals.size()) ? daily_meals[index] : MealEntry();
    }
    std::vector<MealEntry> get_all_meals() const { return daily_meals; }

    // Вспомогательные методы
    static std::string get_current_date_string();
    void display_progress_bar(double current, double max) const;
    void display_detailed_summary() const;

    // Qt-совместимые методы
    QVector<QStringList> get_meals_qt() const;
    QMap<QString, double> get_totals_qt() const;
    QMap<QString, double> get_percentages_qt() const;

    // Обновление данных
    void set_user(User* user) { current_user = user; }
    void set_manager(NutritionManager* mgr) { manager = mgr; }
    void set_advisor(NutritionAdvisor* adv) { advisor = adv; }

    // Сброс данных
    void reset();
};

#endif
