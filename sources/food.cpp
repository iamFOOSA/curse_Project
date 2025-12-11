#include "../headers/food.h"

Food::Food(const std::string& n, double cal, double p, double f, double c)
        : name(n), calories(cal), proteins(p), fats(f), carbs(c) {}

void Food::display() const {
    std::cout << name << " - " << calories << " ккал" << std::endl;
}

Product::Product(const std::string& n, double cal, double p, double f, double c, double w)
        : Food(n, cal, p, f, c), weight(w) {}

void Product::display() const {
    std::cout << name << " (" << weight << "г) - "
              << calories * weight / 100 << " ккал" << std::endl;
}

void Product::calculate_nutrition_with_weight() const {
    double total_cal = calories * weight / 100;
    double total_prot = proteins * weight / 100;
    double total_fats = fats * weight / 100;
    double total_carbs = carbs * weight / 100;

    std::cout << "Питательная ценность " << name << " (" << weight << "г):\n";
    std::cout << "Калории: " << total_cal << " ккал\n";
    std::cout << "Белки: " << total_prot << "г\n";
    std::cout << "Жиры: " << total_fats << "г\n";
    std::cout << "Углеводы: " << total_carbs << "г\n";
}

std::ostream& operator<<(std::ostream& os, const Product& product) {
    os << product.name << " | " << product.calories << " ккал/100г | "
       << product.proteins << "Б/" << product.fats << "Ж/" << product.carbs << "У";
    return os;
}

void calculate_nutrition(const Product& p) {
    std::cout << "Питательная ценность " << p.name << ":\n";
    std::cout << "Белки: " << p.proteins * p.weight / 100 << "г\n";
    std::cout << "Жиры: " << p.fats * p.weight / 100 << "г\n";
    std::cout << "Углеводы: " << p.carbs * p.weight / 100 << "г\n";
}
