#include "../headers/recommendations.h"
#include <iostream>

Recommendations::Recommendations() : advice_count(5) {
    advice_list[0] = " Не забывайте пить воду - 2 литра в день!";
    advice_list[1] = " Старайтесь питаться в одно и то же время";
    advice_list[2] = " Добавляйте овощи к каждому приему пищи";
    advice_list[3] = " Ограничьте потребление сахара и обработанных продуктов";
    advice_list[4] = " Сочетайте правильное питание с физической активностью";
}

void Recommendations::show_welcome_advice() const {
    std::cout << "\n СОВЕТ: " << advice_list[0] << std::endl;
}

void Recommendations::show_tracking_advice() const {
    std::cout << "\n СОВЕТ: " << advice_list[1] << std::endl;
}

void Recommendations::show_daily_advice() const {
    std::cout << "\n СОВЕТ: " << advice_list[2] << std::endl;
}
