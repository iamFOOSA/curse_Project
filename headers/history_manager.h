#ifndef HISTORY_MANAGER_H
#define HISTORY_MANAGER_H

#include <QString>
#include <QDate>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>
#include <QStringList>

struct DayMealEntry {
    QString date;
    QString mealType;      // Завтрак, Обед, Ужин, Перекус
    QString productName;
    double grams;
    double calories;
    double proteins;
    double fats;
    double carbs;
    QString timestamp;     // Время добавления

    DayMealEntry()
        : grams(0), calories(0), proteins(0), fats(0), carbs(0) {}

    DayMealEntry(const QString& d, const QString& mt, const QString& pn,
                 double g, double cal, double prot, double fat, double carb, const QString& ts)
        : date(d), mealType(mt), productName(pn), grams(g),
          calories(cal), proteins(prot), fats(fat), carbs(carb), timestamp(ts) {}
};

struct DaySummary {
    QString date;
    double totalCalories = 0.0;
    double totalProteins = 0.0;
    double totalFats = 0.0;
    double totalCarbs = 0.0;
    QList<DayMealEntry> meals;

    DaySummary() = default;

    explicit DaySummary(const QString& d) : date(d) {}
};

class HistoryManager {
private:
    QString historyFilePath;
    QMap<QString, DaySummary> historyData;  // Ключ - дата в формате "yyyy-MM-dd"

    // Вспомогательные методы
    QString formatDate(const QDate& date) const;
    QDate parseDate(const QString& dateString) const;
    QJsonObject daySummaryToJson(const DaySummary& summary) const;
    DaySummary jsonToDaySummary(const QJsonObject& json) const;
    QJsonObject mealEntryToJson(const DayMealEntry& entry) const;
    DayMealEntry jsonToMealEntry(const QJsonObject& json) const;

public:
    HistoryManager(const QString& filepath = "data/nutrition_history.json");
    ~HistoryManager();

    // Загрузка и сохранение истории
    bool loadHistoryFromFile();
    bool loadHistoryFromFile(const QString& filepath);
    bool saveHistoryToFile() const;
    bool saveHistoryToFile(const QString& filepath) const;

    struct MealEntryParams {
        QString date;
        QString mealType;
        QString productName;
        double grams;
        double calories;
        double proteins;
        double fats;
        double carbs;
        QString timestamp;
        
        MealEntryParams() : grams(0), calories(0), proteins(0), fats(0), carbs(0) {}
    };

    // Добавление записей
    bool addMealEntry(const MealEntryParams& params);
    bool addMealEntry(const QString& date, const QString& mealType, const QString& productName,
                      double grams, double calories, double proteins, double fats, double carbs,
                      const QString& timestamp = "");
    bool addMealEntry(const DayMealEntry& entry);

    // Получение данных
    DaySummary getDaySummary(const QString& date) const;
    DaySummary getDaySummary(const QDate& date) const;
    DaySummary getTodaySummary() const;
    QList<DayMealEntry> getDayMeals(const QString& date) const;
    QList<DayMealEntry> getDayMeals(const QDate& date) const;


    QStringList getAvailableDates() const;
    QList<QDate> getAvailableDatesAsQDate() const;

    // Статистика
    QMap<QString, double> getWeeklyTotals(const QDate& startDate) const;
    QMap<QString, double> getMonthlyTotals(const QDate& date) const;
    double getAverageDailyCalories(const QDate& startDate, const QDate& endDate) const;

    // Удаление записей
    bool removeMealEntry(const QString& date, int index);
    bool removeDayData(const QString& date);
    bool removeDayData(const QDate& date);

    // Обновление данных дня
    void updateDaySummary(const QString& date, const DaySummary& summary);

    // Геттеры
    size_t getHistorySize() const { return historyData.size(); }
    bool hasDataForDate(const QString& date) const;
    bool hasDataForDate(const QDate& date) const;
};

#endif // HISTORY_MANAGER_H

