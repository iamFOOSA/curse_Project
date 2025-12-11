#ifndef USER_H
#define USER_H

#include <string>
#include <map>
#include <iostream>
#include <QString>

template<typename T>
class UserData {
private:
    std::map<std::string, T> data;

public:
    void set_data(const std::string& key, const T& value);
    T get_data(const std::string& key) const;
    void display_all() const;
    bool contains(const std::string& key) const;
    void remove_data(const std::string& key);
    void clear() { data.clear(); }
    size_t size() const { return data.size(); }
};

class User {
private:
    std::string name;
    int age;
    double height;
    double weight;
    std::string goal;
    double daily_calories;
    double daily_proteins;
    double daily_fats;
    double daily_carbs;
    UserData<double> physical_data;
    UserData<std::string> personal_data;

public:
    User();
    User(const std::string& n, int a, double h, double w, const std::string& g);

    void initialize_data();
    void display_info() const;
    void calculate_daily_nutrition();

    void register_user();
    void choose_goal();

    void set_name(const std::string& n) { name = n; }
    void set_age(int a) { age = a; }
    void set_height(double h) { height = h; }
    void set_weight(double w) { weight = w; }
    void set_goal(const std::string& g) { goal = g; }
    void set_daily_calories(double cal) { daily_calories = cal; }
    void set_daily_proteins(double prot) { daily_proteins = prot; }
    void set_daily_fats(double f) { daily_fats = f; }
    void set_daily_carbs(double c) { daily_carbs = c; }

    std::string get_name() const { return name; }
    int get_age() const { return age; }
    double get_height() const { return height; }
    double get_weight() const { return weight; }
    std::string get_goal() const { return goal; }
    double get_daily_calories() const { return daily_calories; }
    double get_daily_proteins() const { return daily_proteins; }
    double get_daily_fats() const { return daily_fats; }
    double get_daily_carbs() const { return daily_carbs; }

    QString get_name_qt() const { return QString::fromStdString(name); }
    QString get_goal_qt() const { return QString::fromStdString(goal); }

    bool is_valid() const;
};

#endif