#ifndef TRACKINGWINDOW_H
#define TRACKINGWINDOW_H

#include <QWidget>
#include <QJsonObject>
#include <QDate>
#include <QTableWidget>
#include <QCompleter>
#include "user.h"
#include "nutrition_manager.h"
#include "meal_tracker.h"
#include "food_file_manager.h"
#include "history_manager.h"
#include "trend_analyzer.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QTabWidget;
class QProgressBar;
class QChartView;
class QTableWidget;
class QTableWidgetItem;
class QCompleter;
class QDateEdit;

class TrackingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TrackingWindow(User *user, NutritionManager *manager,
                            NutritionAdvisor *advisor, QWidget *parent = nullptr);
    ~TrackingWindow() override;

    void resetTracking();
    void loadDailyData();
    void saveTrackingData();
    QString getCurrentDate() const;
    HistoryManager* getHistoryManager() const { return historyManager; }

signals:
    void showSummary();

private slots:
    void onAddMealClicked();
    void onShowSummaryClicked();
    void updateStatistics();
    void addTestData();
    void onProductSearch();
    void onMealTypeChanged(int index);
    void onRemoveMealClicked();
    void onProductTextChanged(const QString &text);
    void onDateChanged(const QDate &date);
    void onUpdateTrends();

private:
    QString generateDailyStatsText(double calPercentage, double protPercentage, double fatsPercentage, double carbsPercentage) const;
    QString generateAnalysisText(double calPercentage, double protPercentage, double fatsPercentage, double carbsPercentage) const;
    QString generateWeeklyStatsText() const;
    QString generateMealStatsText() const;
    QString generateBJUFactText(double protPercentage, double fatsPercentage, double carbsPercentage) const;

    User* user = nullptr;
    NutritionManager* manager = nullptr;
    NutritionAdvisor* advisor = nullptr;
    MealTracker* mealTracker = nullptr;
    FoodFileManager* foodFileManager = nullptr;
    HistoryManager* historyManager = nullptr;
    TrendAnalyzer* trendAnalyzer = nullptr;

    double totalCalories = 0.0;
    double totalProteins = 0.0;
    double totalFats = 0.0;
    double totalCarbs = 0.0;

    QTabWidget* tabWidget = nullptr;
    QDateEdit* dateEdit = nullptr;
    QString previousDate;

    QComboBox* mealTypeComboBox = nullptr;
    QLineEdit* productNameEdit = nullptr;
    QLineEdit* gramsEdit = nullptr;
    QPushButton* addMealButton = nullptr;
    QPushButton* searchProductButton = nullptr;
    QPushButton* removeMealButton = nullptr;

    QTableWidget* mealsTable = nullptr;
    QTextEdit* productInfoDisplay = nullptr;
    QCompleter* productCompleter = nullptr;

    QChartView* caloriesChartView = nullptr;
    QChartView* macrosChartView = nullptr;
    QChartView* dailyProgressChart = nullptr;
    QTextEdit* statsDisplay = nullptr;
    QPushButton* updateTrendsButton = nullptr;
    QComboBox* trendPeriodComboBox = nullptr;

    QTableWidget* trendsSummaryTable = nullptr;
    QTableWidget* trendsTopProductsTable = nullptr;
    QTableWidget* trendsMealDistributionTable = nullptr;
    QTableWidget* trendsWeekdayTable = nullptr;
    QTableWidget* trendsStatsTable = nullptr;

    QProgressBar* caloriesProgress = nullptr;
    QProgressBar* proteinsProgress = nullptr;
    QProgressBar* fatsProgress = nullptr;
    QProgressBar* carbsProgress = nullptr;

    struct ProductInfo {
        QString name;
        double calories;
        double proteins;
        double fats;
        double carbs;

        ProductInfo() : calories(0), proteins(0), fats(0), carbs(0) {}
        ProductInfo(const QString &n, double cal, double prot, double fat, double carb)
            : name(n), calories(cal), proteins(prot), fats(fat), carbs(carb) {}
    };

    struct MealEntry {
        QString mealType;
        QString productName;
        double grams;
        double calories;
        double proteins;
        double fats;
        double carbs;
        QString timestamp;

        MealEntry() : grams(0), calories(0), proteins(0), fats(0), carbs(0) {}
        MealEntry(const QString &mt, const QString &pn, double g, double cal, double prot, double fat, double carb, const QString &ts)
            : mealType(mt), productName(pn), grams(g), calories(cal), proteins(prot), fats(fat), carbs(carb), timestamp(ts) {}
    };

    QList<MealEntry> mealEntries;
    QMap<QString, ProductInfo> productsDatabase;

    void setupTrackingTab(QWidget *tab);
    void setupStatisticsTab(QWidget *tab);
    void setupTrendsTab(QWidget *tab);
    void updateMealsTable();
    void updateProgressBars();
    bool addMeal(const QString &mealType, const QString &productName, double grams);
    void createCharts();
    void showProductInfo(const QString &productName);
    void populateProductSuggestions();
    bool loadProductsFromFile();
    ProductInfo findProduct(const QString &productName);
    void updateProductInfoDisplay(const QString &productName);

    void loadTrackingData();
    void autoSave();
    void createDailyProgressChart(QChartView *chartView, int days = 30);
    void updateDailyProgressChart();
};

#endif
