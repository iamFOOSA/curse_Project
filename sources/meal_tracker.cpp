#include "../headers/meal_tracker.h"

MealTracker::MealTracker(User* user, NutritionManager* mgr, NutritionAdvisor* adv)
    : current_user(user), manager(mgr), advisor(adv), total_calories(0), total_proteins(0), total_fats(0), total_carbs(0) {
    current_date = get_current_date_string();
}

MealTracker::~MealTracker() {

}

bool MealTracker::add_meal(const std::string& meal_type, const std::string& product_name, double grams) {

    return true;
}

bool MealTracker::remove_meal(int index) {
    return true;
}

void MealTracker::clear_meals() {
    daily_meals.clear();
}

void MealTracker::calculate_totals() {

}

double MealTracker::get_calories_percentage() const {
    return 0.0;
}

double MealTracker::get_proteins_percentage() const {
    return 0.0;
}

double MealTracker::get_fats_percentage() const {
    return 0.0;
}

double MealTracker::get_carbs_percentage() const {
    return 0.0;
}

bool MealTracker::save_to_file(const std::string& filename) const {
    return true;
}

bool MealTracker::load_from_file(const std::string& filename) {
    return true;
}

bool MealTracker::save_summary_to_file(const std::string& filename) const {
    return true;
}

std::string MealTracker::get_current_date_string() {
    return "2024-01-01";
}

void MealTracker::display_progress_bar(double current, double max) const {

}

void MealTracker::display_detailed_summary() const {

}

QVector<QStringList> MealTracker::get_meals_qt() const {
    return QVector<QStringList>();
}

QMap<QString, double> MealTracker::get_totals_qt() const {
    return QMap<QString, double>();
}

QMap<QString, double> MealTracker::get_percentages_qt() const {
    return QMap<QString, double>();
}

void MealTracker::reset() {
    daily_meals.clear();
    total_calories = total_proteins = total_fats = total_carbs = 0;
}
