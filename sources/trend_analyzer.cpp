#include "../headers/trend_analyzer.h"
#include <QDebug>
#include <QDate>
#include <algorithm>
#include <cmath>
#include <QPair>

TrendAnalyzer::TrendAnalyzer(HistoryManager* historyMgr, User* usr)
    : historyManager(historyMgr), user(usr)
{
}

TrendAnalyzer::~TrendAnalyzer()
{
}

double TrendAnalyzer::calculateAverage(const QList<double>& values) const
{
    if (values.isEmpty()) return 0;
    double sum = 0;
    for (double value : values) {
        sum += value;
    }
    return sum / values.size();
}

double TrendAnalyzer::calculateTrend(const QList<double>& values) const
{
    if (values.size() < 2) return 0;
    
    // Разделяем на две половины и сравниваем средние
    int mid = values.size() / 2;
    QList<double> firstHalf = values.mid(0, mid);
    QList<double> secondHalf = values.mid(mid);
    
    double firstAvg = calculateAverage(firstHalf);
    double secondAvg = calculateAverage(secondHalf);
    
    if (firstAvg == 0) return 0;
    
    return ((secondAvg - firstAvg) / firstAvg) * 100.0;
}

double TrendAnalyzer::calculateCoefficientOfVariation(const QList<double>& values) const
{
    if (values.isEmpty()) return 0;
    
    double avg = calculateAverage(values);
    if (avg == 0) return 0;
    
    double variance = 0;
    for (double value : values) {
        variance += (value - avg) * (value - avg);
    }
    variance /= values.size();
    
    double stdDev = std::sqrt(variance);
    return (stdDev / avg) * 100.0;
}

QString TrendAnalyzer::determineTrendType(double trendPercent) const
{
    if (trendPercent > 5) return "рост";
    if (trendPercent < -5) return "снижение";
    return "стабильно";
}

QList<QPair<QString, int>> TrendAnalyzer::getTopProducts(const QDate& startDate, const QDate& endDate, int count) const
{
    QMap<QString, int> productFrequency;
    
    QDate currentDate = startDate;
    while (currentDate <= endDate) {
        QList<DayMealEntry> meals = historyManager->getDayMeals(currentDate);
        for (const DayMealEntry& meal : meals) {
            productFrequency[meal.productName]++;
        }
        currentDate = currentDate.addDays(1);
    }
    
    QList<QPair<int, QString>> sortedProducts;
    for (auto it = productFrequency.constBegin(); it != productFrequency.constEnd(); ++it) {
        sortedProducts.append(qMakePair(it.value(), it.key()));
    }
    
    std::sort(sortedProducts.begin(), sortedProducts.end(),
              [](const QPair<int, QString>& a, const QPair<int, QString>& b) {
                  return a.first > b.first;
              });
    
    QList<QPair<QString, int>> result;
    for (int i = 0; i < std::min(count, static_cast<int>(sortedProducts.size())); ++i) {
        result.append(qMakePair(sortedProducts[i].second, sortedProducts[i].first));
    }
    
    return result;
}

QMap<QString, double> TrendAnalyzer::calculateMealDistribution(const QDate& startDate, const QDate& endDate) const
{
    QMap<QString, QList<double>> mealCalories;
    QMap<QString, int> mealCounts;
    
    QDate currentDate = startDate;
    while (currentDate <= endDate) {
        QList<DayMealEntry> meals = historyManager->getDayMeals(currentDate);
        for (const DayMealEntry& meal : meals) {
            mealCalories[meal.mealType].append(meal.calories);
            mealCounts[meal.mealType]++;
        }
        currentDate = currentDate.addDays(1);
    }
    
    QMap<QString, double> result;
    for (auto it = mealCalories.constBegin(); it != mealCalories.constEnd(); ++it) {
        result[it.key()] = calculateAverage(it.value());
    }
    
    return result;
}

QMap<QString, double> TrendAnalyzer::calculateWeekdayDistribution(const QDate& startDate, const QDate& endDate) const
{
    QMap<QString, QList<double>> weekdayCalories;
    
    QDate currentDate = startDate;
    while (currentDate <= endDate) {
        QString dayName;
        int dayOfWeek = currentDate.dayOfWeek();
        switch (dayOfWeek) {
            case 1: dayName = "Понедельник"; break;
            case 2: dayName = "Вторник"; break;
            case 3: dayName = "Среда"; break;
            case 4: dayName = "Четверг"; break;
            case 5: dayName = "Пятница"; break;
            case 6: dayName = "Суббота"; break;
            case 7: dayName = "Воскресенье"; break;
        }
        
        DaySummary summary = historyManager->getDaySummary(currentDate);
        if (summary.totalCalories > 0) {
            weekdayCalories[dayName].append(summary.totalCalories);
        }
        
        currentDate = currentDate.addDays(1);
    }
    
    QMap<QString, double> result;
    for (auto it = weekdayCalories.constBegin(); it != weekdayCalories.constEnd(); ++it) {
        result[it.key()] = calculateAverage(it.value());
    }
    
    return result;
}

QPair<QString, QString> TrendAnalyzer::findExtremeDays(const QMap<QString, double>& weekdayData) const
{
    if (weekdayData.isEmpty()) return qMakePair(QString(), QString());
    
    QString mostCaloric, leastCaloric;
    double maxCal = 0, minCal = 1000000;
    
    for (auto it = weekdayData.constBegin(); it != weekdayData.constEnd(); ++it) {
        if (it.value() > maxCal) {
            maxCal = it.value();
            mostCaloric = it.key();
        }
        if (it.value() < minCal && it.value() > 0) {
            minCal = it.value();
            leastCaloric = it.key();
        }
    }
    
    return qMakePair(mostCaloric, leastCaloric);
}

TrendAnalysis TrendAnalyzer::analyzeTrends(int days)
{
    QDate endDate = QDate::currentDate();
    QDate startDate = endDate.addDays(-(days - 1));
    return analyzeTrends(startDate, endDate);
}

TrendAnalysis TrendAnalyzer::analyzeTrends(const QDate& startDate, const QDate& endDate)
{
    TrendAnalysis analysis;
    
    // Шаг 1: Получить начальную и конечную даты периода
    analysis.startDate = startDate;
    analysis.endDate = endDate;
    analysis.totalDays = startDate.daysTo(endDate) + 1;
    
    // Шаг 2: Загрузить историю питания за период
    if (!historyManager || !user) {
        analysis.analysisText = "Ошибка: не инициализированы необходимые компоненты";
        return analysis;
    }
    
    // Шаг 3: Проверить наличие данных и собрать данные за период
    QList<double> dailyCalories, dailyProteins, dailyFats, dailyCarbs;
    int daysWithDataCount = 0;
    int daysOver = 0, daysUnder = 0;
    double targetCalories = user->get_daily_calories();
    
    QDate currentDate = startDate;
    while (currentDate <= endDate) {
        DaySummary summary = historyManager->getDaySummary(currentDate);
        
        if (summary.totalCalories > 0) {
            dailyCalories.append(summary.totalCalories);
            dailyProteins.append(summary.totalProteins);
            dailyFats.append(summary.totalFats);
            dailyCarbs.append(summary.totalCarbs);
            daysWithDataCount++;
            
            // Шаг 12: Найти периоды превышения/недостатка норм
            if (summary.totalCalories > targetCalories * 1.1) {
                daysOver++;
            } else if (summary.totalCalories < targetCalories * 0.9) {
                daysUnder++;
            }
        }
        
        currentDate = currentDate.addDays(1);
    }
    
    analysis.daysWithData = daysWithDataCount;
    analysis.daysOverTarget = daysOver;
    analysis.daysUnderTarget = daysUnder;
    
    if (daysWithDataCount == 0) {
        analysis.analysisText = "Нет данных за выбранный период";
        return analysis;
    }
    
    // Шаг 4: Вычислить средние значения калорий, белков, жиров, углеводов за период
    analysis.avgCalories = calculateAverage(dailyCalories);
    analysis.avgProteins = calculateAverage(dailyProteins);
    analysis.avgFats = calculateAverage(dailyFats);
    analysis.avgCarbs = calculateAverage(dailyCarbs);
    
    // Шаг 5-6: Разбить период на части и вычислить тренды
    // Шаг 7: Определить тренд (рост/снижение/стабильность) для каждого макронутриента
    double calTrend = calculateTrend(dailyCalories);
    double protTrend = calculateTrend(dailyProteins);
    double fatTrend = calculateTrend(dailyFats);
    double carbTrend = calculateTrend(dailyCarbs);
    
    analysis.caloriesTrend = determineTrendType(calTrend);
    analysis.proteinsTrend = determineTrendType(protTrend);
    analysis.fatsTrend = determineTrendType(fatTrend);
    analysis.carbsTrend = determineTrendType(carbTrend);
    
    // Шаг 8: Найти топ-10 наиболее часто употребляемых продуктов
    analysis.topProducts = getTopProducts(startDate, endDate, 10);
    
    // Шаг 9: Проанализировать распределение калорий по приемам пищи
    analysis.mealDistribution = calculateMealDistribution(startDate, endDate);
    
    // Шаг 10: Определить дни недели с наибольшим/наименьшим потреблением калорий
    analysis.weekdayCalories = calculateWeekdayDistribution(startDate, endDate);
    QPair<QString, QString> extremeDays = findExtremeDays(analysis.weekdayCalories);
    analysis.mostCaloricDay = extremeDays.first;
    analysis.leastCaloricDay = extremeDays.second;
    
    // Шаг 11: Вычислить коэффициент вариации для оценки стабильности питания
    double calVariation = calculateCoefficientOfVariation(dailyCalories);
    double protVariation = calculateCoefficientOfVariation(dailyProteins);
    double avgVariation = (calVariation + protVariation) / 2.0;
    
    // Преобразуем в оценку стабильности (0-100, выше = стабильнее)
    analysis.stabilityScore = std::max(0.0, std::min(100.0, 100.0 - avgVariation));
    
    // Шаг 13: Сформировать список выявленных трендов
    // Шаг 14: Сгенерировать текстовый отчет с выводами
    analysis.analysisText = getAnalysisText(analysis);
    
    // Шаг 15: Вернуть структурированные данные
    return analysis;
}

QString TrendAnalyzer::getAnalysisText(const TrendAnalysis& analysis) const
{
    QString text;
    
    text += QString("Анализ трендов питания\n");
    text += QString("Период: %1 - %2 (%3 дней, данных: %4)\n\n")
        .arg(analysis.startDate.toString("dd.MM.yyyy"))
        .arg(analysis.endDate.toString("dd.MM.yyyy"))
        .arg(analysis.totalDays)
        .arg(analysis.daysWithData);
    
    text += "Средние значения за период:\n";
    text += QString("  Калории: %1 ккал\n")
        .arg(static_cast<int>(analysis.avgCalories));
    text += QString("  Белки: %1 г\n")
        .arg(static_cast<int>(analysis.avgProteins));
    text += QString("  Жиры: %1 г\n")
        .arg(static_cast<int>(analysis.avgFats));
    text += QString("  Углеводы: %1 г\n\n")
        .arg(static_cast<int>(analysis.avgCarbs));
    
    text += "Тренды:\n";
    text += QString("  Калории: %1\n").arg(analysis.caloriesTrend);
    text += QString("  Белки: %1\n").arg(analysis.proteinsTrend);
    text += QString("  Жиры: %1\n").arg(analysis.fatsTrend);
    text += QString("  Углеводы: %1\n\n").arg(analysis.carbsTrend);
    
    if (!analysis.topProducts.isEmpty()) {
        text += "Топ-10 наиболее часто употребляемых продуктов:\n";
        for (int i = 0; i < analysis.topProducts.size(); ++i) {
            text += QString("  %1. %2 (%3 раз)\n")
                .arg(i + 1)
                .arg(analysis.topProducts[i].first)
                .arg(analysis.topProducts[i].second);
        }
        text += "\n";
    }
    
    if (!analysis.mealDistribution.isEmpty()) {
        text += "Распределение калорий по приемам пищи:\n";
        for (auto it = analysis.mealDistribution.constBegin(); it != analysis.mealDistribution.constEnd(); ++it) {
            text += QString("  %1: %2 ккал\n")
                .arg(it.key())
                .arg(static_cast<int>(it.value()));
        }
        text += "\n";
    }
    
    if (!analysis.mostCaloricDay.isEmpty()) {
        text += QString("Самый калорийный день недели: %1\n").arg(analysis.mostCaloricDay);
        text += QString("Самый низкокалорийный день: %1\n\n").arg(analysis.leastCaloricDay);
    }
    
    text += QString("Стабильность питания: %1/100\n").arg(static_cast<int>(analysis.stabilityScore));
    text += QString("Дней превышения нормы: %1\n").arg(analysis.daysOverTarget);
    text += QString("Дней ниже нормы: %1\n").arg(analysis.daysUnderTarget);
    
    return text;
}

