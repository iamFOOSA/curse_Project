# Иерархия классов исключений

## Структура наследования

```
NutritionException (базовый класс)
├── FileException (ошибки работы с файлами)
├── DataException (ошибки данных)
├── ValidationException (ошибки валидации)
│   ├── InvalidUserDataException (невалидные данные пользователя)
│   └── InvalidProductException (невалидные данные продукта)
└── CalculationException (ошибки расчетов)
```

## Примеры использования

### 1. Обработка ошибок файлов

```cpp
#include "nutrition_exceptions.h"

try {
    FoodFileManager manager("data/products.txt");
    if (!manager.loadProductsFromFile()) {
        throw FileException("Не удалось загрузить файл продуктов", "data/products.txt", "FoodFileManager::loadProductsFromFile");
    }
} catch (const FileException& e) {
    qDebug() << "Ошибка файла:" << e.get_full_message().c_str();
    qDebug() << "Файл:" << e.get_filename().c_str();
} catch (const NutritionException& e) {
    qDebug() << "Общая ошибка:" << e.what();
}
```

### 2. Валидация данных пользователя

```cpp
#include "nutrition_exceptions.h"

void User::set_weight(double w) {
    if (w <= 0) {
        throw InvalidUserDataException("Вес должен быть положительным числом", "weight", std::to_string(w));
    }
    if (w > 500) {
        throw InvalidUserDataException("Вес слишком большой", "weight", std::to_string(w));
    }
    weight = w;
}

try {
    user.set_weight(-10);
} catch (const InvalidUserDataException& e) {
    qDebug() << "Ошибка валидации пользователя:" << e.get_full_message().c_str();
    qDebug() << "Поле:" << e.get_field_name().c_str();
    qDebug() << "Значение:" << e.get_invalid_value().c_str();
}
```

### 3. Валидация данных продукта

```cpp
#include "nutrition_exceptions.h"

bool NutritionManager::add_product(const std::string& name, double calories, double proteins, double fats, double carbs) {
    if (name.empty()) {
        throw InvalidProductException("Название продукта не может быть пустым", "name", "");
    }
    if (calories < 0 || proteins < 0 || fats < 0 || carbs < 0) {
        throw InvalidProductException("Значения питательных веществ не могут быть отрицательными", "nutrition_values", "");
    }
    // ... остальной код
}
```

### 4. Ошибки расчетов

```cpp
#include "nutrition_exceptions.h"

double User::calculate_daily_nutrition() {
    if (weight <= 0 || height <= 0) {
        throw CalculationException("Невозможно рассчитать норму: невалидные данные пользователя", "BMR", "User::calculate_daily_nutrition");
    }
    
    double bmr = BMR_WEIGHT_COEFFICIENT * weight + BMR_HEIGHT_COEFFICIENT * height - BMR_AGE_COEFFICIENT * age;
    
    if (bmr <= 0) {
        throw CalculationException("Результат расчета BMR некорректен", "BMR", "User::calculate_daily_nutrition");
    }
    
    // ... остальной код
}
```

### 5. Ошибки данных

```cpp
#include "nutrition_exceptions.h"

DaySummary HistoryManager::getDaySummary(const QString& date) const {
    if (!historyData.contains(date)) {
        throw DataException("Данные за указанную дату не найдены", "DaySummary", "HistoryManager::getDaySummary");
    }
    return historyData[date];
}
```

## Преимущества иерархии

1. **Полиморфизм**: Можно перехватывать все исключения через базовый класс `NutritionException`
2. **Специфичность**: Каждый тип исключения содержит специфичную информацию
3. **Расширяемость**: Легко добавить новые типы исключений, наследуясь от существующих
4. **Отладка**: Каждое исключение содержит контекст и детали ошибки

