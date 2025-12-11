#include "../headers/user.h"
#include <algorithm>
#include <cmath>

namespace {
    constexpr double BMR_WEIGHT_COEFFICIENT = 10.0;
    constexpr double BMR_HEIGHT_COEFFICIENT = 6.25;
    constexpr double BMR_AGE_COEFFICIENT = 5.0;
    constexpr double DEFAULT_WEIGHT = 70.0;
    constexpr double DEFAULT_HEIGHT = 170.0;
    constexpr int DEFAULT_AGE = 30;
    constexpr double MIN_WEIGHT_FOR_BMR = 50.0;
    constexpr double MIN_HEIGHT_FOR_BMR = 150.0;
    constexpr int MIN_AGE_FOR_BMR = 18;
    constexpr double CALORIE_ADJUSTMENT = 500.0;
    constexpr double MIN_SAFE_CALORIES = 1200.0;
    constexpr double PROTEIN_PER_KG = 1.8;
    constexpr double PROTEIN_PER_KG_MIN = 1.5;
    constexpr double FAT_PER_KG = 1.0;
    constexpr double FAT_PER_KG_MIN = 0.8;
    constexpr double CALORIES_PER_GRAM_PROTEIN = 4.0;
    constexpr double CALORIES_PER_GRAM_FAT = 9.0;
    constexpr double CALORIES_PER_GRAM_CARB = 4.0;
    constexpr double CALORIE_TOLERANCE = 10.0;
    constexpr double ZERO = 0.0;
}

template<typename T>
void UserData<T>::set_data(const std::string& key, const T& value) {
    data[key] = value;
}

template<typename T>
T UserData<T>::get_data(const std::string& key) const {
    auto it = data.find(key);
    if (it != data.end()) {
        return it->second;
    }
    return T();
}

template<typename T>
void UserData<T>::display_all() const {
    for (const auto& pair : data) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}

template class UserData<double>;
template class UserData<std::string>;

User::User() : name(""), age(0), height(0), weight(0), goal(""),
               daily_calories(0), daily_proteins(0), daily_fats(0), daily_carbs(0) {}

User::User(const std::string& n, int a, double h, double w, const std::string& g)
        : name(n), age(a), height(h), weight(w), goal(g),
          daily_calories(0), daily_proteins(0), daily_fats(0), daily_carbs(0) {
    initialize_data();
    calculate_daily_nutrition();
}

void User::initialize_data() {
    physical_data.set_data("weight", weight);
    physical_data.set_data("height", height);
    personal_data.set_data("goal", goal);
    personal_data.set_data("name", name);
}

void User::display_info() const {
    std::cout << "\n=== ВАША АНКЕТА ===" << std::endl;
    std::cout << "Имя: " << name << std::endl;
    std::cout << "Возраст: " << age << " лет" << std::endl;
    std::cout << "Рост: " << height << " см" << std::endl;
    std::cout << "Вес: " << weight << " кг" << std::endl;
    std::cout << "Цель: " << goal << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << "Рекомендуемые дневные нормы:" << std::endl;
    std::cout << "Калории: " << daily_calories << " ккал" << std::endl;
    std::cout << "Белки: " << daily_proteins << " г" << std::endl;
    std::cout << "Жиры: " << daily_fats << " г" << std::endl;
    std::cout << "Углеводы: " << daily_carbs << " г" << std::endl;
    std::cout << "==================" << std::endl;
}

void User::calculate_daily_nutrition() {
    daily_calories = ZERO;
    daily_proteins = ZERO;
    daily_fats = ZERO;
    daily_carbs = ZERO;

    if (weight <= ZERO || height <= ZERO || age <= 0) {
        if (weight <= ZERO) weight = DEFAULT_WEIGHT;
        if (height <= ZERO) height = DEFAULT_HEIGHT;
        if (age <= 0) age = DEFAULT_AGE;
    }

    std::string currentGoal = goal;
    
    if (currentGoal.empty()) {
        currentGoal = "поддержание";
        goal = currentGoal;
    }

    double bmr = BMR_WEIGHT_COEFFICIENT * weight + BMR_HEIGHT_COEFFICIENT * height - BMR_AGE_COEFFICIENT * age;
    
    if (bmr <= ZERO) {
        bmr = BMR_WEIGHT_COEFFICIENT * MIN_WEIGHT_FOR_BMR + BMR_HEIGHT_COEFFICIENT * MIN_HEIGHT_FOR_BMR - BMR_AGE_COEFFICIENT * MIN_AGE_FOR_BMR;
    }

    daily_calories = bmr;

    if (currentGoal == "похудение") {
        daily_calories -= CALORIE_ADJUSTMENT;
    } else if (currentGoal == "набор") {
        daily_calories += CALORIE_ADJUSTMENT;
    }

    if (daily_calories < MIN_SAFE_CALORIES) {
        daily_calories = MIN_SAFE_CALORIES;
    }

    daily_proteins = weight * PROTEIN_PER_KG;
    
    if (daily_proteins <= ZERO) {
        daily_proteins = weight * PROTEIN_PER_KG_MIN;
    }

    daily_fats = weight * FAT_PER_KG;
    
    if (daily_fats <= ZERO) {
        daily_fats = weight * FAT_PER_KG_MIN;
    }

    double caloriesFromProteins = daily_proteins * CALORIES_PER_GRAM_PROTEIN;
    double caloriesFromFats = daily_fats * CALORIES_PER_GRAM_FAT;
    double remainingCalories = daily_calories - caloriesFromProteins - caloriesFromFats;
    daily_carbs = remainingCalories / CALORIES_PER_GRAM_CARB;
    
    if (daily_carbs < ZERO) {
        double totalCaloriesNeeded = caloriesFromProteins + caloriesFromFats;
        if (totalCaloriesNeeded > daily_calories) {
            double ratio = daily_calories / totalCaloriesNeeded;
            daily_proteins *= ratio;
            daily_fats *= ratio;
            daily_carbs = ZERO;
        } else {
            daily_carbs = ZERO;
        }
    }

    double calculatedCalories = daily_proteins * CALORIES_PER_GRAM_PROTEIN + daily_fats * CALORIES_PER_GRAM_FAT + daily_carbs * CALORIES_PER_GRAM_CARB;
    double difference = std::abs(calculatedCalories - daily_calories);
    
    if (difference > CALORIE_TOLERANCE) {
        double caloriesFromProteinsFats = daily_proteins * CALORIES_PER_GRAM_PROTEIN + daily_fats * CALORIES_PER_GRAM_FAT;
        double carbsCalories = daily_calories - caloriesFromProteinsFats;
        if (carbsCalories >= ZERO) {
            daily_carbs = carbsCalories / CALORIES_PER_GRAM_CARB;
        } else {
            daily_carbs = ZERO;
        }
    }
}

void User::register_user() {
    // Метод зарезервирован для будущей реализации регистрации пользователя
}

void User::choose_goal() {
    // Метод зарезервирован для будущей реализации выбора цели пользователя
}