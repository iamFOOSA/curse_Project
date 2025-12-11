#ifndef RECOMMENDATIONS_H
#define RECOMMENDATIONS_H

#include <string>
#include <vector>

class Recommendations {
private:
    static constexpr int max_advice = 10;
    std::vector<std::string> advice_list;
    int advice_count = 0;

public:
    Recommendations();
    void show_welcome_advice() const;
    void show_tracking_advice() const;
    void show_daily_advice() const;
};

#endif
