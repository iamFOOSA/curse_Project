#include "../headers/history_manager.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <ranges>
#include <algorithm>


HistoryManager::HistoryManager(const QString& filepath)
    : historyFilePath(filepath)
{
}

HistoryManager::~HistoryManager()
{
    saveHistoryToFile();
}

QString HistoryManager::formatDate(const QDate& date) const
{
    return date.toString("yyyy-MM-dd");
}

QDate HistoryManager::parseDate(const QString& dateString) const
{
    return QDate::fromString(dateString, "yyyy-MM-dd");
}

QJsonObject HistoryManager::mealEntryToJson(const DayMealEntry& entry) const
{
    QJsonObject obj;
    obj["date"] = entry.date;
    obj["mealType"] = entry.mealType;
    obj["productName"] = entry.productName;
    obj["grams"] = entry.grams;
    obj["calories"] = entry.calories;
    obj["proteins"] = entry.proteins;
    obj["fats"] = entry.fats;
    obj["carbs"] = entry.carbs;
    obj["timestamp"] = entry.timestamp;
    return obj;
}

DayMealEntry HistoryManager::jsonToMealEntry(const QJsonObject& json) const
{
    DayMealEntry entry;
    entry.date = json["date"].toString();
    entry.mealType = json["mealType"].toString();
    entry.productName = json["productName"].toString();
    entry.grams = json["grams"].toDouble();
    entry.calories = json["calories"].toDouble();
    entry.proteins = json["proteins"].toDouble();
    entry.fats = json["fats"].toDouble();
    entry.carbs = json["carbs"].toDouble();
    entry.timestamp = json["timestamp"].toString();
    return entry;
}

QJsonObject HistoryManager::daySummaryToJson(const DaySummary& summary) const
{
    QJsonObject obj;
    obj["date"] = summary.date;
    obj["totalCalories"] = summary.totalCalories;
    obj["totalProteins"] = summary.totalProteins;
    obj["totalFats"] = summary.totalFats;
    obj["totalCarbs"] = summary.totalCarbs;

    QJsonArray mealsArray;
    for (const DayMealEntry& meal : summary.meals) {
        mealsArray.append(mealEntryToJson(meal));
    }
    obj["meals"] = mealsArray;

    return obj;
}

DaySummary HistoryManager::jsonToDaySummary(const QJsonObject& json) const
{
    DaySummary summary;
    summary.date = json["date"].toString();
    summary.totalCalories = json["totalCalories"].toDouble();
    summary.totalProteins = json["totalProteins"].toDouble();
    summary.totalFats = json["totalFats"].toDouble();
    summary.totalCarbs = json["totalCarbs"].toDouble();

    if (json.contains("meals") && json["meals"].isArray()) {
        QJsonArray mealsArray = json["meals"].toArray();
        for (const QJsonValue& value : mealsArray) {
            if (value.isObject()) {
                summary.meals.append(jsonToMealEntry(value.toObject()));
            }
        }
    }

    return summary;
}

bool HistoryManager::loadHistoryFromFile()
{
    return loadHistoryFromFile(historyFilePath);
}

bool HistoryManager::loadHistoryFromFile(const QString& filepath)
{
    historyFilePath = filepath;
    historyData.clear();

    QFile file(filepath);
    if (!file.exists()) {
        QFileInfo fileInfo(filepath);
        if (QDir dir = fileInfo.absoluteDir(); !dir.exists()) {
            if (!dir.mkpath(".")) {
                qDebug() << "Не удалось создать директорию для файла истории";
                return false;
            }
        }
        qDebug() << "Файл истории не существует, будет создан новый";
        return true; // Файла нет, это нормально
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла истории:" << file.errorString();
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString jsonString = in.readAll();
    file.close();

    if (jsonString.isEmpty() || jsonString.trimmed().isEmpty()) {
        qDebug() << "Файл истории пустой";
        return true; // Пустой файл - это нормально
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Ошибка парсинга JSON истории:" << parseError.errorString();
        return false;
    }

    if (!doc.isObject()) {
        qDebug() << "Ошибка: JSON истории не является объектом";
        return false;
    }

    QJsonObject rootObject = doc.object();

    if (!rootObject.contains("days") || !rootObject["days"].isArray()) {
        qDebug() << "Предупреждение: формат файла истории устарел или неверен";
        return true;
    }

    QJsonArray daysArray = rootObject["days"].toArray();
    for (const QJsonValue& value : daysArray) {
        if (value.isObject()) {
            if (DaySummary summary = jsonToDaySummary(value.toObject()); !summary.date.isEmpty()) {
                historyData[summary.date] = summary;
            }
        }
    }

    qDebug() << "Загружено дней истории:" << historyData.size();
    return true;
}

bool HistoryManager::saveHistoryToFile() const
{
    return saveHistoryToFile(historyFilePath);
}

bool HistoryManager::saveHistoryToFile(const QString& filepath) const
{
    QFileInfo fileInfo(filepath);
    if (QDir dir = fileInfo.absoluteDir(); !dir.exists()) {
        dir.mkpath(".");
    }
    
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Ошибка записи файла истории:" << file.errorString();
        return false;
    }

    QJsonObject rootObject;
    QJsonArray daysArray;

    for (auto it = historyData.constBegin(); it != historyData.constEnd(); ++it) {
        daysArray.append(daySummaryToJson(it.value()));
    }

    rootObject["days"] = daysArray;
    rootObject["lastUpdate"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(rootObject);
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << doc.toJson(QJsonDocument::Indented);

    file.close();
    return true;
}

bool HistoryManager::addMealEntry(const MealEntryParams& params)
{
    QString actualTimestamp = params.timestamp.isEmpty() 
        ? QDateTime::currentDateTime().toString("hh:mm") 
        : params.timestamp;

    DayMealEntry entry(params.date, params.mealType, params.productName, 
                       params.grams, params.calories, params.proteins, 
                       params.fats, params.carbs, actualTimestamp);

    return addMealEntry(entry);
}

bool HistoryManager::addMealEntry(const DayMealEntry& entry)
{
    if (entry.date.isEmpty()) {
        return false;
    }

    QString date = entry.date;
    if (!historyData.contains(date)) {
        historyData[date] = DaySummary(date);
    }

    DaySummary& summary = historyData[date];
    summary.meals.append(entry);

    summary.totalCalories += entry.calories;
    summary.totalProteins += entry.proteins;
    summary.totalFats += entry.fats;
    summary.totalCarbs += entry.carbs;

    return true;
}

DaySummary HistoryManager::getDaySummary(const QString& date) const
{
    if (historyData.contains(date)) {
        return historyData[date];
    }
    return DaySummary(date);
}

DaySummary HistoryManager::getDaySummary(const QDate& date) const
{
    return getDaySummary(formatDate(date));
}

DaySummary HistoryManager::getTodaySummary() const
{
    return getDaySummary(QDate::currentDate());
}

QList<DayMealEntry> HistoryManager::getDayMeals(const QString& date) const
{
    if (historyData.contains(date)) {
        return historyData[date].meals;
    }
    return QList<DayMealEntry>();
}

QList<DayMealEntry> HistoryManager::getDayMeals(const QDate& date) const
{
    return getDayMeals(formatDate(date));
}

QStringList HistoryManager::getAvailableDates() const
{
    QStringList dates;
    for (auto it = historyData.constBegin(); it != historyData.constEnd(); ++it) {
        dates.append(it.key());
    }
    dates.sort();
    std::ranges::reverse(dates);
    return dates;
}

QList<QDate> HistoryManager::getAvailableDatesAsQDate() const
{
    QList<QDate> dates;
    for (auto it = historyData.constBegin(); it != historyData.constEnd(); ++it) {
        QDate date = parseDate(it.key());
        if (date.isValid()) {
            dates.append(date);
        }
    }
    std::ranges::sort(dates, std::greater{});
    return dates;
}

QMap<QString, double> HistoryManager::getWeeklyTotals(const QDate& startDate) const
{
    QMap<QString, double> totals;
    totals["calories"] = 0;
    totals["proteins"] = 0;
    totals["fats"] = 0;
    totals["carbs"] = 0;

    QDate endDate = startDate.addDays(6);
    QDate currentDate = startDate;

    while (currentDate <= endDate) {
        if (QString dateStr = formatDate(currentDate); historyData.contains(dateStr)) {
            const DaySummary& summary = historyData.value(dateStr);
            totals["calories"] += summary.totalCalories;
            totals["proteins"] += summary.totalProteins;
            totals["fats"] += summary.totalFats;
            totals["carbs"] += summary.totalCarbs;
        }
        currentDate = currentDate.addDays(1);
    }
    return totals;
}

QMap<QString, double> HistoryManager::getMonthlyTotals(const QDate& date) const
{
    QMap<QString, double> totals;
    totals["calories"] = 0;
    totals["proteins"] = 0;
    totals["fats"] = 0;
    totals["carbs"] = 0;

    QDate startDate(date.year(), date.month(), 1);
    QDate endDate = startDate.addMonths(1).addDays(-1);
    QDate currentDate = startDate;

    while (currentDate <= endDate) {
        if (QString dateStr = formatDate(currentDate); historyData.contains(dateStr)) {
            auto it = historyData.find(dateStr);
            if (it != historyData.end()) {
                const DaySummary& summary = it.value();
                totals["calories"] += summary.totalCalories;
                totals["proteins"] += summary.totalProteins;
                totals["fats"] += summary.totalFats;
                totals["carbs"] += summary.totalCarbs;
            }
        }
        currentDate = currentDate.addDays(1);
    }

    return totals;
}

double HistoryManager::getAverageDailyCalories(const QDate& startDate, const QDate& endDate) const
{
    double totalCalories = 0;
    int daysWithData = 0;

    QDate currentDate = startDate;
    while (currentDate <= endDate) {
        if (QString dateStr = formatDate(currentDate); historyData.contains(dateStr)) {
            totalCalories += historyData[dateStr].totalCalories;
            daysWithData++;
        }
        currentDate = currentDate.addDays(1);
    }

    return daysWithData > 0 ? totalCalories / daysWithData : 0.0;
}

bool HistoryManager::removeMealEntry(const QString& date, int index)
{
    constexpr double ZERO_VALUE = 0.0;
    
    if (!historyData.contains(date)) {
        return false;
    }

    DaySummary& summary = historyData[date];
    
    if (index < 0 || index >= summary.meals.size()) {
        return false;
    }

    if (const DayMealEntry& entry = summary.meals[index]; 
        entry.calories <= ZERO_VALUE && entry.proteins <= ZERO_VALUE && entry.fats <= ZERO_VALUE && entry.carbs <= ZERO_VALUE) {
        summary.meals.removeAt(index);
        if (summary.meals.isEmpty()) {
            historyData.remove(date);
        }
        return true;
    }

    double recalcCalories = ZERO_VALUE;
    double recalcProteins = ZERO_VALUE;
    double recalcFats = ZERO_VALUE;
    double recalcCarbs = ZERO_VALUE;

    QList<DayMealEntry> remainingMeals;
    
    if (summary.meals.isEmpty()) {
        return false;
    }

    for (int i = 0; i < summary.meals.size(); ++i) {
        if (const DayMealEntry& meal = summary.meals[i]; i != index && (meal.calories > 0 || meal.proteins > 0 || meal.fats > 0 || meal.carbs > 0)) {
            recalcCalories += meal.calories;
            recalcProteins += meal.proteins;
            recalcFats += meal.fats;
            recalcCarbs += meal.carbs;
            remainingMeals.append(meal);
        }
    }

    summary.totalCalories = recalcCalories;
    summary.totalProteins = recalcProteins;
    summary.totalFats = recalcFats;
    summary.totalCarbs = recalcCarbs;

    if (summary.totalCalories < ZERO_VALUE) summary.totalCalories = ZERO_VALUE;
    if (summary.totalProteins < ZERO_VALUE) summary.totalProteins = ZERO_VALUE;
    if (summary.totalFats < ZERO_VALUE) summary.totalFats = ZERO_VALUE;
    if (summary.totalCarbs < ZERO_VALUE) summary.totalCarbs = ZERO_VALUE;

    summary.meals = remainingMeals;

    if (summary.meals.isEmpty()) {
        historyData.remove(date);
    }

    return true;
}

bool HistoryManager::removeDayData(const QString& date)
{
    return historyData.remove(date) > 0;
}

bool HistoryManager::removeDayData(const QDate& date)
{
    return removeDayData(formatDate(date));
}

void HistoryManager::updateDaySummary(const QString& date, const DaySummary& summary)
{
    historyData[date] = summary;
}

bool HistoryManager::hasDataForDate(const QString& date) const
{
    return historyData.contains(date);
}

bool HistoryManager::hasDataForDate(const QDate& date) const
{
    return hasDataForDate(formatDate(date));
}

