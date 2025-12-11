#include "../headers/nutrition_advisor.h"
#include <iostream>

NutritionAdvisor::NutritionAdvisor() {
    advice_list.push_back("Пейте достаточно воды - 2 литра в день!");
    advice_list.push_back("Сбалансируйте приемы пищи");
    advice_list.push_back("Не пропускайте завтрак");
}

void NutritionAdvisor::show_welcome_advice() const {
    std::cout << "Совет: " << advice_list[0] << std::endl;
}

void NutritionAdvisor::show_tracking_advice() const {
    std::cout << "Совет: " << advice_list[1] << std::endl;
}

void NutritionAdvisor::show_daily_advice() const {
    std::cout << "Совет: " << advice_list[2] << std::endl;
}

void NutritionAdvisor::show_goal_based_advice(const User& user) const {
    std::string goal = user.get_goal();
    if (goal == "похудение") {
        std::cout << "Для похудения создавайте дефицит калорий." << std::endl;
    } else if (goal == "набор") {
        std::cout << "Для набора массы увеличьте потребление белков." << std::endl;
    } else {
        std::cout << "Для поддержания веса следите за балансом калорий." << std::endl;
    }
}

void NutritionAdvisor::show_nutrition_analysis_advice(const User& user, double calories, double proteins, double fats, double carbs) const {

}

void NutritionAdvisor::show_progress_advice(const User& user, double calories_percentage, double proteins_percentage, double fats_percentage, double carbs_percentage) const {

}

QString NutritionAdvisor::get_welcome_advice_qt() const {
    return QString::fromStdString("Совет: " + advice_list[0]);
}

QString NutritionAdvisor::get_goal_based_advice_qt(const User& user) const {
    return "Совет по вашей цели";
}

QString NutritionAdvisor::get_nutrition_analysis_qt(const User& user, double calories, double proteins, double fats, double carbs) const {
    return "Анализ питания";
}

QString NutritionAdvisor::get_progress_advice_qt(const User& user, double calories_percentage, double proteins_percentage, double fats_percentage, double carbs_percentage) const {
    return "Совет по прогрессу";
}
