#ifndef RECOMMENDATIONS_H
#define RECOMMENDATIONS_H

#include <string>

class Recommendations {
private:
    static const int max_advice = 10;
    std::string advice_list[max_advice];
    int advice_count;

public:
    Recommendations();
    void show_welcome_advice() const;
    void show_tracking_advice() const;
    void show_daily_advice() const;
};

#endif
