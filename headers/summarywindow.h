#ifndef SUMMARYWINDOW_H
#define SUMMARYWINDOW_H

#include <QWidget>
#include "history_manager.h"

class QLabel;
class QPushButton;
class QProgressBar;
class QTableWidget;
class QChartView;
class User;

class SummaryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SummaryWindow(User *user, HistoryManager *historyManager = nullptr, QWidget *parent = nullptr);
    void displaySummary();
    void displaySummaryForDate(const QString& date = "");

    signals:
            void backToTracking();

private slots:
            void onBackClicked();

private:
    User *user;
    HistoryManager *historyManager;
    QLabel *summaryLabel;
    QLabel *dateLabel;
    QProgressBar *caloriesProgress;
    QProgressBar *proteinsProgress;
    QProgressBar *fatsProgress;
    QProgressBar *carbsProgress;
    QTableWidget *mealsTable;
    QPushButton *backButton;
    QChartView *progressChartView;

    void createProgressBar(QProgressBar *progressBar, const QString &text, double value, double max);
    void updateProgressBar(QProgressBar *progressBar, double value, double max);
    void createProgressChart(int days = 30);
};

#endif
