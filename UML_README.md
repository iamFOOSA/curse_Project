# UML Диаграмма классов проекта

## Файл диаграммы

Файл `project_class_diagram.puml` содержит полную диаграмму классов проекта в формате PlantUML.

## Как использовать

### Онлайн редакторы:
1. **PlantUML Online**: https://www.plantuml.com/plantuml/uml/
   - Скопируйте содержимое файла `project_class_diagram.puml`
   - Вставьте в редактор
   - Диаграмма будет автоматически сгенерирована

2. **PlantText**: https://www.planttext.com/
   - Аналогично, вставьте код из файла

### Локальные инструменты:
1. **VS Code**: Установите расширение "PlantUML"
2. **IntelliJ IDEA / CLion**: Встроенная поддержка PlantUML
3. **Command line**: Установите PlantUML и Java, затем:
   ```bash
   java -jar plantuml.jar project_class_diagram.puml
   ```

### Экспорт в другие форматы:
PlantUML поддерживает экспорт в:
- PNG
- SVG
- PDF
- EPS
- LaTeX

## Структура диаграммы

Диаграмма включает:

1. **Иерархия исключений** - все классы исключений с наследованием
2. **Базовые классы данных** - Food, Product, UserData
3. **Структуры данных** - DayMealEntry, DaySummary, MealEntry, ProductData, TrendData, TrendAnalysis
4. **Менеджеры данных** - DailyMenu, NutritionManager, Manager, HistoryManager, FoodFileManager, MealTracker
5. **Советники и аналитики** - Recommendations, NutritionAdvisor, TrendAnalyzer
6. **Qt окна** - MainWindow, RegistrationWindow, MenuWindow, TrackingWindow, SummaryWindow

## Обозначения

- `--` - ассоциация
- `<|--` - наследование
- `*--` - композиция (сильная связь)
- `-->` - использование/зависимость
- `..>` - создание/использование временных объектов

## Обновление диаграммы

При добавлении новых классов или изменении существующих:
1. Откройте `project_class_diagram.puml`
2. Добавьте новый класс с его методами и полями
3. Добавьте связи с другими классами
4. Сохраните и перегенерируйте диаграмму

