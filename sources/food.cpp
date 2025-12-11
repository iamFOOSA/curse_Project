#include "../headers/food.h"

Food::Food(const std::string& n, double cal, double p, double f, double c)
        : name(n), calories(cal), proteins(p), fats(f), carbs(c) {}

void Food::display() const {
    std::cout << name << " - " << calories << " ккал" << std::endl;
}

Product::Product(const std::string& n, double cal, double p, double f, double c, double w)
        : Food(n, cal, p, f, c), weight(w) {}

void Product::display() const {
    std::cout << get_name() << " (" << weight << "г) - "
              << get_calories() * weight / 100 << " ккал" << std::endl;
}

void Product::calculate_nutrition_with_weight() const {
    double total_cal = get_calories() * weight / 100;
    double total_prot = get_proteins() * weight / 100;
    double total_fats = get_fats() * weight / 100;
    double total_carbs = get_carbs() * weight / 100;

    std::cout << "Питательная ценность " << get_name() << " (" << weight << "г):\n";
    std::cout << "Калории: " << total_cal << " ккал\n";
    std::cout << "Белки: " << total_prot << "г\n";
    std::cout << "Жиры: " << total_fats << "г\n";
    std::cout << "Углеводы: " << total_carbs << "г\n";
}

std::ostream& operator<<(std::ostream& os, const Product& product) {
    os << product.get_name() << " | " << product.get_calories() << " ккал/100г | "
       << product.get_proteins() << "Б/" << product.get_fats() << "Ж/" << product.get_carbs() << "У";
    return os;
}

void calculate_nutrition(const Product& p) {
    std::cout << "Питательная ценность " << p.get_name() << ":\n";
    std::cout << "Белки: " << p.get_proteins() * p.get_weight() / 100 << "г\n";
    std::cout << "Жиры: " << p.get_fats() * p.get_weight() / 100 << "г\n";
    std::cout << "Углеводы: " << p.get_carbs() * p.get_weight() / 100 << "г\n";
}
