#ifndef FOOD_H
#define FOOD_H

#include <string>
#include <iostream>
#include <iomanip>

class Food {
private:
    std::string name;
    double calories;
    double proteins;
    double fats;
    double carbs;

protected:
    std::string& get_name_ref() { return name; }
    const std::string& get_name_ref() const { return name; }
    double& get_calories_ref() { return calories; }
    double& get_proteins_ref() { return proteins; }
    double& get_fats_ref() { return fats; }
    double& get_carbs_ref() { return carbs; }

public:
    explicit Food(const std::string& n = "", double cal = 0, double p = 0, double f = 0, double c = 0);
    virtual ~Food() = default;

    virtual void display() const;

    std::string get_name() const { return name; }
    double get_calories() const { return calories; }
    double get_proteins() const { return proteins; }
    double get_fats() const { return fats; }
    double get_carbs() const { return carbs; }

    void set_name(const std::string& n) { name = n; }
    void set_calories(double cal) { calories = cal; }
    void set_proteins(double p) { proteins = p; }
    void set_fats(double f) { fats = f; }
    void set_carbs(double c) { carbs = c; }
};

class Product : public Food {
private:
    double weight;

public:
    explicit Product(const std::string& n = "", double cal = 0, double p = 0, double f = 0, double c = 0, double w = 100);

    friend std::ostream& operator<<(std::ostream& os, const Product& product);
    friend void calculate_nutrition(const Product& p);

    void display() const override;
    double get_weight() const { return weight; }
    void set_weight(double w) { weight = w; }

    void calculate_nutrition_with_weight() const;

    // Для расчета питания с произвольным весом
    double calculate_calories(double grams = 100) const { return calories * grams / 100; }
    double calculate_proteins(double grams = 100) const { return proteins * grams / 100; }
    double calculate_fats(double grams = 100) const { return fats * grams / 100; }
    double calculate_carbs(double grams = 100) const { return carbs * grams / 100; }
};

#endif
