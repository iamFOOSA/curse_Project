#ifndef RECOMMENDATIONS_H
#define RECOMMENDATIONS_H

#include <string>
#include <vector>

class Recommendations {
private:
    std::vector<std::string> advice_list;

public:
    Recommendations();
    void show_welcome_advice() const;
    void show_tracking_advice() const;
    void show_daily_advice() const;
};

#endif
