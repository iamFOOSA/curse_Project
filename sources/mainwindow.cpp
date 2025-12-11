#include "../headers/mainwindow.h"
#include "../headers/registrationwindow.h"
#include "../headers/menuwindow.h"
#include "../headers/trackingwindow.h"
#include "../headers/summarywindow.h"
#include "../headers/nutrition_manager.h"
#include "../headers/nutrition_advisor.h"
#include "../headers/meal_tracker.h"
#include "../headers/manager.h"
#include "../headers/history_manager.h"
#include <QStackedWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QDate>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), manager(nullptr), basicManager(nullptr), user(nullptr), advisor(nullptr), mealTracker(nullptr)
{
    setWindowTitle("Дневник питания - FatSecret");
    setMinimumSize(1000, 700);
    resize(1100, 800);

    user = new User();
    advisor = new NutritionAdvisor();
    manager = new NutritionManager(advisor);
    basicManager = new Manager();


    basicManager->load_products_from_file();

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    registrationWindow = new RegistrationWindow(user, this);
    menuWindow = new MenuWindow(basicManager, user, this);
    trackingWindow = new TrackingWindow(user, manager, advisor, this);
    
    // Создаем SummaryWindow после trackingWindow, чтобы передать historyManager
    HistoryManager *historyManager = trackingWindow->getHistoryManager();
    summaryWindow = new SummaryWindow(user, historyManager, this);

    stackedWidget->addWidget(registrationWindow);
    stackedWidget->addWidget(menuWindow);
    stackedWidget->addWidget(trackingWindow);
    stackedWidget->addWidget(summaryWindow);

    connect(registrationWindow, &RegistrationWindow::registrationCompleted,
            this, &MainWindow::showMenu);
    connect(menuWindow, &MenuWindow::startTracking,
            this, &MainWindow::showTracking);
    connect(trackingWindow, &TrackingWindow::showSummary,
            this, &MainWindow::showSummary);
    connect(summaryWindow, &SummaryWindow::backToTracking,
            this, &MainWindow::showTracking);

    showRegistration();
}

MainWindow::~MainWindow()
{
    cleanup();
}

void MainWindow::cleanup()
{
    delete mealTracker;
    delete manager;
    delete basicManager;
    delete advisor;
    delete user;
}

void MainWindow::showRegistration()
{
    stackedWidget->setCurrentWidget(registrationWindow);
}

void MainWindow::showMenu()
{
    menuWindow->displayMenu();
    stackedWidget->setCurrentWidget(menuWindow);
}

void MainWindow::showTracking()
{
    stackedWidget->setCurrentWidget(trackingWindow);
}

void MainWindow::showSummary()
{
    trackingWindow->saveTrackingData();
    QString currentDate = trackingWindow->getCurrentDate();
    summaryWindow->displaySummaryForDate(currentDate);
    stackedWidget->setCurrentWidget(summaryWindow);
}

void MainWindow::showHistory()
{
    QDialog historyDialog(this);
    historyDialog.setWindowTitle("История питания");
    historyDialog.setMinimumSize(900, 700);
    historyDialog.setStyleSheet(R"(
        QDialog {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a1a2e, stop:0.5 #16213e, stop:1 #0f3460);
            color: white;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        QLabel {
            color: white;
        }
        QComboBox {
            background: rgba(255, 255, 255, 0.12);
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 6px;
            padding: 8px;
            color: white;
            min-width: 200px;
        }
        QComboBox:focus {
            border-color: #9457eb;
        }
        QComboBox::drop-down {
            border: none;
        }
        QComboBox::down-arrow {
            border-left: 3px solid transparent;
            border-right: 3px solid transparent;
            border-top: 3px solid white;
        }
        QTableWidget {
            background: rgba(255, 255, 255, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 6px;
            color: white;
            gridline-color: rgba(255, 255, 255, 0.1);
        }
        QHeaderView::section {
            background: rgba(148, 87, 235, 0.4);
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        QPushButton {
            background: #9457eb;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #8457db;
        }
    )");

    HistoryManager historyManager("data/nutrition_history.json");
    QStringList dates = historyManager.getAvailableDates();

    if (dates.isEmpty()) {
        return;
    }

    auto mainLayout = new QVBoxLayout(&historyDialog);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel("История питания", &historyDialog);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *dateLayout = new QHBoxLayout;
    auto dateSelectLabel = new QLabel("Выберите дату:", &historyDialog);
    dateSelectLabel->setStyleSheet("font-size: 14px; color: white;");
    auto dateComboBox = new QComboBox(&historyDialog);
    auto viewButton = new QPushButton("Показать", &historyDialog);
    auto viewTodayButton = new QPushButton("Сегодня", &historyDialog);

    for (const QString& date : dates) {
        QDate qdate = QDate::fromString(date, "yyyy-MM-dd");
        QString displayDate = qdate.toString("dd.MM.yyyy");
        dateComboBox->addItem(displayDate, date);
    }

    dateLayout->addWidget(dateSelectLabel);
    dateLayout->addWidget(dateComboBox);
    dateLayout->addWidget(viewButton);
    dateLayout->addWidget(viewTodayButton);
    dateLayout->addStretch();

    auto summaryTable = new QTableWidget(&historyDialog);
    summaryTable->setColumnCount(6);
    summaryTable->setHorizontalHeaderLabels(QStringList() << "Дата" << "Калории" << "Белки" << "Жиры" << "Углеводы" << "Приемов пищи");
    summaryTable->setRowCount(dates.size());
    summaryTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    for (int i = 0; i < dates.size(); ++i) {
        DaySummary summary = historyManager.getDaySummary(dates[i]);
        QDate qdate = QDate::fromString(dates[i], "yyyy-MM-dd");
        
        summaryTable->setItem(i, 0, new QTableWidgetItem(qdate.toString("dd.MM.yyyy")));
        summaryTable->setItem(i, 1, new QTableWidgetItem(QString::number(summary.totalCalories, 'f', 1)));
        summaryTable->setItem(i, 2, new QTableWidgetItem(QString::number(summary.totalProteins, 'f', 1)));
        summaryTable->setItem(i, 3, new QTableWidgetItem(QString::number(summary.totalFats, 'f', 1)));
        summaryTable->setItem(i, 4, new QTableWidgetItem(QString::number(summary.totalCarbs, 'f', 1)));
        summaryTable->setItem(i, 5, new QTableWidgetItem(QString::number(summary.meals.size())));
    }

    summaryTable->horizontalHeader()->setStretchLastSection(true);
    summaryTable->resizeColumnsToContents();

    auto detailLabel = new QLabel("Детали выбранного дня:", &historyDialog);
    detailLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white; padding: 10px;");

    auto detailTable = new QTableWidget(&historyDialog);
    detailTable->setColumnCount(8);
    detailTable->setHorizontalHeaderLabels(QStringList() << "Время" << "Прием пищи" << "Продукт" << "Вес (г)" << "Ккал" << "Белки" << "Жиры" << "Углеводы");
    detailTable->horizontalHeader()->setStretchLastSection(true);
    detailTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    detailTable->setMinimumHeight(200);

    auto updateDetailTable = [&](const QString& dateStr) {
        DaySummary summary = historyManager.getDaySummary(dateStr);
        detailTable->setRowCount(summary.meals.size());
        
        for (int i = 0; i < summary.meals.size(); ++i) {
            const DayMealEntry& entry = summary.meals[i];
            detailTable->setItem(i, 0, new QTableWidgetItem(entry.timestamp));
            detailTable->setItem(i, 1, new QTableWidgetItem(entry.mealType));
            detailTable->setItem(i, 2, new QTableWidgetItem(entry.productName));
            detailTable->setItem(i, 3, new QTableWidgetItem(QString::number(entry.grams, 'f', 1)));
            detailTable->setItem(i, 4, new QTableWidgetItem(QString::number(entry.calories, 'f', 1)));
            detailTable->setItem(i, 5, new QTableWidgetItem(QString::number(entry.proteins, 'f', 1)));
            detailTable->setItem(i, 6, new QTableWidgetItem(QString::number(entry.fats, 'f', 1)));
            detailTable->setItem(i, 7, new QTableWidgetItem(QString::number(entry.carbs, 'f', 1)));
        }
        detailTable->resizeColumnsToContents();
    };

    if (!dates.isEmpty()) {
        updateDetailTable(dates.first());
        dateComboBox->setCurrentIndex(0);
    }

    connect(summaryTable, &QTableWidget::cellDoubleClicked, [&](int row, int) {
        if (row >= 0 && row < dates.size()) {
            QString selectedDate = dates[row];
            int index = dateComboBox->findData(selectedDate);
            if (index >= 0) {
                dateComboBox->setCurrentIndex(index);
                updateDetailTable(selectedDate);
            }
        }
    });

    connect(viewButton, &QPushButton::clicked, [&]() {
        QString selectedDate = dateComboBox->currentData().toString();
        if (!selectedDate.isEmpty()) {
            updateDetailTable(selectedDate);
        }
    });

    connect(viewTodayButton, &QPushButton::clicked, [&]() {
        QString todayDate = QDate::currentDate().toString("yyyy-MM-dd");
        int index = dateComboBox->findData(todayDate);
        if (index >= 0) {
            dateComboBox->setCurrentIndex(index);
            updateDetailTable(todayDate);
        }
    });

    auto closeButton = new QPushButton("Закрыть", &historyDialog);
    connect(closeButton, &QPushButton::clicked, &historyDialog, &QDialog::accept);

    auto showSummaryButton = new QPushButton("Показать итоги дня", &historyDialog);
    connect(showSummaryButton, &QPushButton::clicked, [&]() {
        QString selectedDate = dateComboBox->currentData().toString();
        if (!selectedDate.isEmpty()) {
            historyDialog.accept();
            summaryWindow->displaySummaryForDate(selectedDate);
            stackedWidget->setCurrentWidget(summaryWindow);
        }
    });

    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(dateLayout);
    mainLayout->addWidget(summaryTable, 2);
    mainLayout->addWidget(detailLabel);
    mainLayout->addWidget(detailTable, 2);
    
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(showSummaryButton);
    buttonsLayout->addWidget(closeButton);
    buttonsLayout->setAlignment(Qt::AlignRight);
    mainLayout->addLayout(buttonsLayout);

    historyDialog.exec();
}

void MainWindow::onUserRegistered()
{
    showMenu();
}

void MainWindow::initializeData()
{
}
