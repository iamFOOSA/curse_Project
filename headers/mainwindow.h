#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "user.h"
#include "nutrition_manager.h"
#include "nutrition_advisor.h"
#include "meal_tracker.h"
#include "manager.h" // Добавляем

// Предварительное объявление классов
class RegistrationWindow;
class MenuWindow;
class TrackingWindow;
class SummaryWindow;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void showRegistration();
    void showMenu();
    void showTracking();
    void showSummary();
    void showHistory();
    void onUserRegistered();
    void initializeData();

private:
    void cleanup();

    User *user;
    NutritionManager *manager;
    Manager *basicManager; // Добавляем
    NutritionAdvisor *advisor;
    MealTracker *mealTracker;

    QStackedWidget *stackedWidget;
    RegistrationWindow *registrationWindow;
    MenuWindow *menuWindow;
    TrackingWindow *trackingWindow;
    SummaryWindow *summaryWindow;
};

#endif // MAINWINDOW_H
