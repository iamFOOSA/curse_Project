#ifndef TREND_ANALYZER_H
#define TREND_ANALYZER_H

#include "history_manager.h"
#include "user.h"
#include <QString>
#include <QList>
#include <QMap>
#include <QDate>
#include <QPair>

struct TrendData {
    QString period;
    double avgCalories = 0.0;
    double avgProteins = 0.0;
    double avgFats = 0.0;
    double avgCarbs = 0.0;
    double trendCalories = 0.0;      // Положительное = рост, отрицательное = снижение
    double trendProteins = 0.0;
    double trendFats = 0.0;
    double trendCarbs = 0.0;
    double stabilityCoefficient = 0.0; // Коэффициент вариации (0-100, меньше = стабильнее)
    
    TrendData() = default;
};

struct TrendAnalysis {
    QDate startDate;
    QDate endDate;
    int totalDays = 0;
    int daysWithData = 0;
    
    // Средние значения за период
    double avgCalories = 0.0;
    double avgProteins = 0.0;
    double avgFats = 0.0;
    double avgCarbs = 0.0;
    
    // Тренды (рост/снижение/стабильность)
    QString caloriesTrend;      // "рост", "снижение", "стабильно"
    QString proteinsTrend;
    QString fatsTrend;
    QString carbsTrend;
    
    // Топ продуктов
    QList<QPair<QString, int>> topProducts; // Название продукта и частота употребления
    
    // Распределение по приемам пищи
    QMap<QString, double> mealDistribution; // Тип приема пищи -> средние калории
    
    // Дни недели
    QMap<QString, double> weekdayCalories; // День недели -> средние калории
    QString mostCaloricDay;
    QString leastCaloricDay;
    
    // Стабильность питания
    double stabilityScore = 0.0; // 0-100, выше = стабильнее
    
    // Периоды превышения/недостатка норм
    int daysOverTarget = 0;
    int daysUnderTarget = 0;
    
    QString analysisText;
    
    TrendAnalysis() = default;
};

class TrendAnalyzer {
private:
    HistoryManager* historyManager;
    User* user;
    
    // Вспомогательные методы
    double calculateAverage(const QList<double>& values) const;
    double calculateTrend(const QList<double>& values) const; // Возвращает процент изменения
    double calculateCoefficientOfVariation(const QList<double>& values) const;
    QString determineTrendType(double trendPercent) const;
    QList<QPair<QString, int>> getTopProducts(const QDate& startDate, const QDate& endDate, int count = 10) const;
    QMap<QString, double> calculateMealDistribution(const QDate& startDate, const QDate& endDate) const;
    QMap<QString, double> calculateWeekdayDistribution(const QDate& startDate, const QDate& endDate) const;
    QPair<QString, QString> findExtremeDays(const QMap<QString, double>& weekdayData) const;
    
public:
    TrendAnalyzer(HistoryManager* historyMgr, User* usr);
    ~TrendAnalyzer();
    
    // Основной метод анализа трендов
    TrendAnalysis analyzeTrends(const QDate& startDate, const QDate& endDate) const;
    TrendAnalysis analyzeTrends(int days = 30); // Анализ за последние N дней
    
    // Получение анализа в текстовом виде
    QString getAnalysisText(const TrendAnalysis& analysis) const;
    
    // Геттеры
    bool isValid() const { return historyManager && user; }
};

#endif // TREND_ANALYZER_H

