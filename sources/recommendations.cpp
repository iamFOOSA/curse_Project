#include "../headers/recommendations.h"
#include <iostream>

Recommendations::Recommendations() {
    advice_list.push_back(" Не забывайте пить воду - 2 литра в день!");
    advice_list.push_back(" Старайтесь питаться в одно и то же время");
    advice_list.push_back(" Добавляйте овощи к каждому приему пищи");
    advice_list.push_back(" Ограничьте потребление сахара и обработанных продуктов");
    advice_list.push_back(" Сочетайте правильное питание с физической активностью");
}

void Recommendations::show_welcome_advice() const {
    if (!advice_list.empty()) {
        std::cout << "\n СОВЕТ: " << advice_list[0] << std::endl;
    }
}

void Recommendations::show_tracking_advice() const {
    if (advice_list.size() > 1) {
        std::cout << "\n СОВЕТ: " << advice_list[1] << std::endl;
    }
}

void Recommendations::show_daily_advice() const {
    if (advice_list.size() > 2) {
        std::cout << "\n СОВЕТ: " << advice_list[2] << std::endl;
    }
}
