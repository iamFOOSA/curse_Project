#ifndef NUTRITION_ADVISOR_H
#define NUTRITION_ADVISOR_H

#include "user.h"
#include <string>
#include <vector>
#include <QString>

class NutritionAdvisor {
private:
    std::vector<std::string> advice_list;

public:
    NutritionAdvisor();

    // Базовые советы
    void show_welcome_advice() const;
    void show_tracking_advice() const;
    void show_daily_advice() const;

    // Персонализированные советы
    void show_goal_based_advice(const User& user) const;
    void show_nutrition_analysis_advice(const User& user, double calories,
                                        double proteins, double fats, double carbs) const;
    void show_progress_advice(const User& user, double calories_percentage,
                              double proteins_percentage, double fats_percentage,
                              double carbs_percentage) const;

    // Qt-совместимые методы
    QString get_welcome_advice_qt() const;
    QString get_goal_based_advice_qt(const User& user) const;
    QString get_nutrition_analysis_qt(const User& user, double calories,
                                      double proteins, double fats, double carbs) const;
    QString get_progress_advice_qt(const User& user, double calories_percentage,
                                   double proteins_percentage, double fats_percentage,
                                   double carbs_percentage) const;
};

#endif
