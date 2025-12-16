#include "../headers/trackingwindow.h"
#include "../headers/user.h"
#include "../headers/nutrition_manager.h"
#include "../headers/nutrition_advisor.h"
#include "../headers/food_file_manager.h"
#include "../headers/history_manager.h"
#include "../headers/trend_analyzer.h"
#include <array>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollBar>
#include <QFrame>
#include <QTabWidget>
#include <QProgressBar>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QChart>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QFile>
#include <QApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QInputDialog>
#include <QDate>
#include <QTextStream>
#include <QStringConverter>
#include <QDebug>
#include <QTimer>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateTime>
#include <QDateEdit>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QTime>

TrackingWindow::TrackingWindow(User *user, NutritionManager *manager, NutritionAdvisor *advisor, QWidget *parent)
    : QWidget(parent),
    user(user),
    manager(manager),
    advisor(advisor),
    foodFileManager(new FoodFileManager("data/products.txt")),
    historyManager(new HistoryManager("data/nutrition_history.json"))
{
    if (foodFileManager) {
        if (foodFileManager->loadProductsFromFile()) {
            qDebug() << "Продукты успешно загружены, количество:" << foodFileManager->getProductCount();
        } else {
            qDebug() << "Ошибка загрузки продуктов";
        }
    }

    if (historyManager) {
        historyManager->loadHistoryFromFile();
    }

    if (historyManager && user) {
        trendAnalyzer = new TrendAnalyzer(historyManager, user);
    }

    setStyleSheet(R"(
        TrackingWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a1a2e, stop:0.5 #16213e, stop:1 #0f3460);
            font-family: 'Segoe UI', Arial, sans-serif;
            color: white;
        }
    )");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    tabWidget = new QTabWidget;
    tabWidget->setStyleSheet(R"(
        QTabWidget::pane {
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 15px;
            background: rgba(255, 255, 255, 0.05);
            margin: 10px;
        }
        QTabWidget::tab-bar {
            alignment: center;
        }
        QTabBar::tab {
            background: rgba(255, 255, 255, 0.1);
            border: 2px solid rgba(255, 255, 255, 0.2);
            border-bottom: none;
            border-top-left-radius: 10px;
            border-top-right-radius: 10px;
            padding: 15px 25px;
            font-size: 16px;
            font-weight: bold;
            color: rgba(255, 255, 255, 0.7);
            margin-right: 5px;
            min-width: 180px;
        }
        QTabBar::tab:selected {
            background: rgba(148, 87, 235, 0.3);
            border-color: rgba(148, 87, 235, 0.5);
            color: white;
        }
        QTabBar::tab:hover {
            background: rgba(148, 87, 235, 0.2);
            color: white;
        }
    )");

    auto *trackingTab = new QWidget;
    auto *statisticsTab = new QWidget;
    auto *trendsTab = new QWidget;

    tabWidget->addTab(trackingTab, " Отслеживание");
    tabWidget->addTab(statisticsTab, " Статистика");
    tabWidget->addTab(trendsTab, " Анализ трендов");

    setupTrackingTab(trackingTab);
    setupStatisticsTab(statisticsTab);
    setupTrendsTab(trendsTab);

    mainLayout->addWidget(tabWidget);
    connect(tabWidget, &QTabWidget::currentChanged, this, &TrackingWindow::updateStatistics);

    QTimer::singleShot(200, this, [this]() {
        if (dailyProgressChart && this->user && historyManager && this->user->get_daily_calories() > 0) {
            updateDailyProgressChart();
        }
    });
}

void TrackingWindow::setupTrackingTab(QWidget *tab)
{
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: rgba(255, 255, 255, 0.1);
            width: 12px;
            margin: 0px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background: #9457eb;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background: #8457db;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
        }
        QScrollBar:horizontal {
            background: rgba(255, 255, 255, 0.1);
            height: 12px;
            margin: 0px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal {
            background: #9457eb;
            border-radius: 6px;
            min-width: 20px;
        }
    )");

    auto *scrollContent = new QWidget;
    auto *tabLayout = new QVBoxLayout(scrollContent);
    tabLayout->setSpacing(15);
    tabLayout->setContentsMargins(20, 20, 20, 20);

    auto *titleLabel = new QLabel(" Отслеживание питания");
    titleLabel->setStyleSheet(R"(
        font-size: 24px;
        font-weight: bold;
        color: white;
        padding: 12px;
        text-align: center;
        background: rgba(148, 87, 235, 0.2);
        border-radius: 8px;
        margin: 5px;
    )");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto *dateGroup = new QGroupBox(" Выбор даты");
    dateGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 12px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 8px 0 8px;
            color: white;
            font-weight: bold;
            font-size: 14px;
        }
    )");
    auto *dateLayout = new QHBoxLayout(dateGroup);
    auto *dateLabel = new QLabel("Дата:");
    dateLabel->setStyleSheet("font-weight: 600; color: white; font-size: 12px;");
    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");
    dateEdit->setStyleSheet(R"(
        QDateEdit {
            background: rgba(255, 255, 255, 0.12);
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 6px;
            padding: 8px 10px;
            font-size: 12px;
            color: white;
            min-width: 120px;
        }
        QDateEdit:focus {
            border-color: #9457eb;
            background: rgba(255, 255, 255, 0.15);
        }
        QDateEdit::drop-down {
            border: none;
            width: 20px;
        }
    )");
    dateLayout->addWidget(dateLabel);
    dateLayout->addWidget(dateEdit);
    dateLayout->addStretch();
    previousDate = dateEdit->date().toString("yyyy-MM-dd");
    connect(dateEdit, &QDateEdit::dateChanged, this, &TrackingWindow::onDateChanged);

    auto *progressGroup = new QGroupBox(" Дневной прогресс");
    progressGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 12px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 8px 0 8px;
            color: white;
            font-weight: bold;
            font-size: 14px;
        }
    )");

    auto *progressLayout = new QVBoxLayout(progressGroup);
    progressLayout->setSpacing(8);

    QString progressStyle = R"(
        QProgressBar {
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 6px;
            background: rgba(255, 255, 255, 0.1);
            text-align: center;
            height: 25px;
            font-size: 11px;
            font-weight: bold;
            color: white;
            min-width: 200px;
        }
        QProgressBar::chunk {
            border-radius: 5px;
        }
    )";

    caloriesProgress = new QProgressBar;
    caloriesProgress->setRange(0, 100);
    caloriesProgress->setValue(0);
    caloriesProgress->setStyleSheet(progressStyle + "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e74c3c, stop:1 #c0392b); }");

    proteinsProgress = new QProgressBar;
    proteinsProgress->setRange(0, 100);
    proteinsProgress->setValue(0);
    proteinsProgress->setStyleSheet(progressStyle + "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3498db, stop:1 #2980b9); }");

    fatsProgress = new QProgressBar;
    fatsProgress->setRange(0, 100);
    fatsProgress->setValue(0);
    fatsProgress->setStyleSheet(progressStyle + "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #f39c12, stop:1 #e67e22); }");

    carbsProgress = new QProgressBar;
    carbsProgress->setRange(0, 100);
    carbsProgress->setValue(0);
    carbsProgress->setStyleSheet(progressStyle + "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #27ae60, stop:1 #229954); }");

    auto createProgressRow = [](const QString& label, QProgressBar* progress) {
        auto *rowLayout = new QHBoxLayout;
        auto *progressLabel = new QLabel(label);
        progressLabel->setStyleSheet("font-weight: 600; color: white; font-size: 12px; min-width: 80px;");
        progressLabel->setAlignment(Qt::AlignLeft);
        progress->setMinimumWidth(180);
        rowLayout->addWidget(progressLabel);
        rowLayout->addWidget(progress);
        rowLayout->addStretch();
        return rowLayout;
    };

    progressLayout->addLayout(createProgressRow(" Калории:", caloriesProgress));
    progressLayout->addLayout(createProgressRow(" Белки:", proteinsProgress));
    progressLayout->addLayout(createProgressRow(" Жиры:", fatsProgress));
    progressLayout->addLayout(createProgressRow(" Углеводы:", carbsProgress));

    auto *inputGroup = new QGroupBox(" Добавить прием пищи");
    inputGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 12px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 8px 0 8px;
            color: white;
            font-weight: bold;
            font-size: 14px;
        }
    )");

    auto *inputLayout = new QVBoxLayout(inputGroup);
    inputLayout->setSpacing(10);
    inputLayout->setContentsMargins(15, 15, 15, 15);

    QString labelStyle = "font-weight: 600; color: white; font-size: 12px; margin-bottom: 5px;";
    QString inputStyle = R"(
        QComboBox, QLineEdit {
            background: rgba(255, 255, 255, 0.12);
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 6px;
            padding: 8px 10px;
            font-size: 12px;
            color: white;
            min-height: 15px;
            margin-bottom: 5px;
        }
        QComboBox:focus, QLineEdit:focus {
            border-color: #9457eb;
            background: rgba(255, 255, 255, 0.15);
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            border-left: 3px solid transparent;
            border-right: 3px solid transparent;
            border-top: 3px solid white;
        }
        QLineEdit::placeholder {
            color: rgba(255, 255, 255, 0.5);
        }
    )";

    auto *mealLabel = new QLabel(" Прием пищи:");
    mealLabel->setStyleSheet(labelStyle);
    mealTypeComboBox = new QComboBox;
    mealTypeComboBox->addItems({"Завтрак", "Обед", "Ужин", "Перекус"});
    mealTypeComboBox->setStyleSheet(inputStyle);

    auto *productLabel = new QLabel(" Продукт:");
    productLabel->setStyleSheet(labelStyle);
    productNameEdit = new QLineEdit;
    productNameEdit->setPlaceholderText("Введите название продукта...");
    productNameEdit->setStyleSheet(inputStyle);

    auto *gramsLabel = new QLabel(" Количество (граммы):");
    gramsLabel->setStyleSheet(labelStyle);
    gramsEdit = new QLineEdit;
    gramsEdit->setPlaceholderText("Введите вес в граммах...");
    gramsEdit->setStyleSheet(inputStyle);
    gramsEdit->setValidator(new QDoubleValidator(1, 10000, 1, this));

    addMealButton = new QPushButton(" Добавить прием пищи");
    addMealButton->setStyleSheet(R"(
        QPushButton {
            background: #27ae60;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 10px 15px;
            font-size: 12px;
            font-weight: bold;
            margin-top: 8px;
        }
        QPushButton:hover {
            background: #229954;
        }
        QPushButton:pressed {
            background: #1e8449;
        }
    )");

    inputLayout->addWidget(mealLabel);
    inputLayout->addWidget(mealTypeComboBox);
    inputLayout->addSpacing(3);
    inputLayout->addWidget(productLabel);
    inputLayout->addWidget(productNameEdit);
    inputLayout->addSpacing(3);
    inputLayout->addWidget(gramsLabel);
    inputLayout->addWidget(gramsEdit);
    inputLayout->addSpacing(5);
    inputLayout->addWidget(addMealButton, 0, Qt::AlignCenter);

    auto *historyGroup = new QGroupBox(" Приемы пищи");
    historyGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 12px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 8px 0 8px;
            color: white;
            font-weight: bold;
            font-size: 14px;
        }
    )");

    auto *historyLayout = new QVBoxLayout(historyGroup);

    mealsTable = new QTableWidget;
    mealsTable->setColumnCount(8);
    mealsTable->setHorizontalHeaderLabels(QStringList() << "Время" << "Прием пищи" << "Продукт" << "Вес (г)" << "Ккал" << "Белки" << "Углеводы" << "Жиры");
    mealsTable->setStyleSheet(R"(
        QTableWidget {
            background: rgba(255, 255, 255, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 6px;
            color: white;
            gridline-color: rgba(255, 255, 255, 0.1);
            font-size: 11px;
        }
        QTableWidget::item {
            padding: 5px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }
        QTableWidget::item:selected {
            background: rgba(148, 87, 235, 0.3);
        }
        QHeaderView::section {
            background: rgba(148, 87, 235, 0.4);
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
            font-size: 11px;
        }
    )");

    mealsTable->horizontalHeader()->setStretchLastSection(true);
    mealsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mealsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mealsTable->setAlternatingRowColors(true);
    mealsTable->setMinimumHeight(200);

    historyLayout->addWidget(mealsTable);

    auto *buttonsLayout = new QHBoxLayout;

    removeMealButton = new QPushButton(" Удалить выбранное");
    removeMealButton->setStyleSheet(R"(
        QPushButton {
            background: #e74c3c;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 15px;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #c0392b;
        }
    )");

    auto *summaryButton = new QPushButton(" Промежуточные итоги");
    summaryButton->setStyleSheet(R"(
        QPushButton {
            background: #27ae60;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 15px;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #229954;
        }
    )");

    buttonsLayout->addWidget(summaryButton);
    buttonsLayout->addWidget(removeMealButton);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    tabLayout->addWidget(titleLabel);
    tabLayout->addWidget(dateGroup);
    tabLayout->addWidget(progressGroup);
    tabLayout->addWidget(inputGroup);
    tabLayout->addWidget(historyGroup);
    tabLayout->addLayout(buttonsLayout);

    scrollArea->setWidget(scrollContent);

    auto *mainTabLayout = new QVBoxLayout(tab);
    mainTabLayout->setSpacing(0);
    mainTabLayout->setContentsMargins(0, 0, 0, 0);
    mainTabLayout->addWidget(scrollArea);

    connect(addMealButton, &QPushButton::clicked, this, &TrackingWindow::onAddMealClicked);
    connect(summaryButton, &QPushButton::clicked, this, &TrackingWindow::onShowSummaryClicked);
    connect(removeMealButton, &QPushButton::clicked, this, &TrackingWindow::onRemoveMealClicked);
    connect(productNameEdit, &QLineEdit::textChanged, this, &TrackingWindow::onProductTextChanged);

    QTimer::singleShot(100, this, [this]() {
        populateProductSuggestions();
        qDebug() << "Подсказки продуктов обновлены, количество:" << (foodFileManager ? foodFileManager->getAllProductNames().size() : 0);
    });

    if (dateEdit) {
        previousDate = dateEdit->date().toString("yyyy-MM-dd");
    } else {
        previousDate = QDate::currentDate().toString("yyyy-MM-dd");
    }

    productNameEdit->setFocus();

    loadTrackingData();
}

void TrackingWindow::setupStatisticsTab(QWidget *tab)
{
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: rgba(255, 255, 255, 0.1);
            width: 12px;
            margin: 0px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background: #9457eb;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background: #8457db;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
        }
    )");

    auto *scrollContent = new QWidget;
    auto *tabLayout = new QVBoxLayout(scrollContent);
    tabLayout->setSpacing(20);
    tabLayout->setContentsMargins(25, 25, 25, 25);

    auto *titleLabel = new QLabel(" Статистика питания");
    titleLabel->setStyleSheet(R"(
        font-size: 28px;
        font-weight: bold;
        color: white;
        padding: 15px;
        text-align: center;
        background: rgba(148, 87, 235, 0.2);
        border-radius: 10px;
        margin: 10px;
    )");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto *chartsGroup = new QGroupBox(" Визуализация данных");
    chartsGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 15px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 10px 0 10px;
            color: white;
            font-weight: bold;
            font-size: 16px;
        }
    )");

    auto *chartsLayout = new QVBoxLayout(chartsGroup);

    caloriesChartView = new QChartView();
    macrosChartView = new QChartView();

    caloriesChartView->setRenderHint(QPainter::Antialiasing);
    caloriesChartView->setMinimumSize(400, 250);
    caloriesChartView->setStyleSheet("background: transparent; border: none;");

    macrosChartView->setRenderHint(QPainter::Antialiasing);
    macrosChartView->setMinimumSize(400, 250);
    macrosChartView->setStyleSheet("background: transparent; border: none;");

    auto *chartsRow = new QHBoxLayout;
    chartsRow->addWidget(caloriesChartView);
    chartsRow->addWidget(macrosChartView);

    chartsLayout->addLayout(chartsRow);

    auto *statsGroup = new QGroupBox(" Детальная статистика");
    statsGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 15px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 10px 0 10px;
            color: white;
            font-weight: bold;
            font-size: 16px;
        }
    )");

    auto *statsLayout = new QVBoxLayout(statsGroup);
    statsDisplay = new QTextEdit;
    statsDisplay->setReadOnly(true);
    statsDisplay->setStyleSheet(R"(
        QTextEdit {
            background: rgba(255, 255, 255, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 8px;
            padding: 20px;
            font-size: 14px;
            line-height: 1.5;
            color: white;
            min-height: 150px;
        }
    )");
    statsLayout->addWidget(statsDisplay);

    auto *progressChartGroup = new QGroupBox(" Прогресс по дням");
    progressChartGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 15px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 10px 0 10px;
            color: white;
            font-weight: bold;
            font-size: 16px;
        }
    )");

    auto *progressChartLayout = new QVBoxLayout(progressChartGroup);
    progressChartLayout->setContentsMargins(0, 0, 0, 0);
    progressChartLayout->setSpacing(0);

    auto *chartScrollArea = new QScrollArea;
    chartScrollArea->setWidgetResizable(false);
    chartScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    chartScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chartScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartScrollArea->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:horizontal {
            background: rgba(255, 255, 255, 0.1);
            height: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal {
            background: #9457eb;
            border-radius: 6px;
            min-width: 20px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #8457db;
        }
        QScrollBar:vertical {
            width: 0px;
        }
    )");

    auto *periodLayout = new QHBoxLayout;
    auto *periodLabel = new QLabel("Период:");
    periodLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white; margin-right: 10px;");
    periodLayout->addWidget(periodLabel);

    auto *periodComboBox = new QComboBox;
    periodComboBox->addItem("3 дня", 3);
    periodComboBox->addItem("Неделя", 7);
    periodComboBox->addItem("Месяц", 30);
    periodComboBox->setCurrentIndex(2);
    periodComboBox->setStyleSheet(R"(
        QComboBox {
            background: rgba(148, 87, 235, 0.3);
            border: 2px solid rgba(148, 87, 235, 0.5);
            border-radius: 8px;
            padding: 8px 15px;
            color: white;
            font-size: 14px;
            font-weight: bold;
            min-width: 120px;
        }
        QComboBox:hover {
            background: rgba(148, 87, 235, 0.5);
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid white;
            width: 0;
            height: 0;
            margin-right: 10px;
        }
        QComboBox QAbstractItemView {
            background: rgba(45, 45, 65, 0.95);
            border: 2px solid rgba(148, 87, 235, 0.5);
            border-radius: 8px;
            color: white;
            selection-background-color: rgba(148, 87, 235, 0.5);
            padding: 5px;
        }
    )");
    periodLayout->addWidget(periodComboBox);
    periodLayout->addStretch();
    progressChartLayout->addLayout(periodLayout);

    dailyProgressChart = new QChartView();
    dailyProgressChart->setRenderHint(QPainter::Antialiasing);
    dailyProgressChart->setMinimumHeight(400);
    dailyProgressChart->setStyleSheet("background: transparent; border: none;");

    chartScrollArea->setWidget(dailyProgressChart);
    chartScrollArea->setMinimumHeight(400);
    progressChartLayout->addWidget(chartScrollArea, 1);

    connect(periodComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, periodComboBox, chartScrollArea]() {
        int days = periodComboBox->currentData().toInt();
        if (days > 7) {
            dailyProgressChart->setMinimumHeight(400);
            chartScrollArea->setMinimumHeight(400);
        } else {
            dailyProgressChart->setMinimumHeight(450);
            chartScrollArea->setMinimumHeight(450);
        }
        createDailyProgressChart(dailyProgressChart, days);
    });

    tabLayout->addWidget(titleLabel);
    tabLayout->addWidget(chartsGroup);
    tabLayout->addWidget(progressChartGroup);
    tabLayout->addWidget(statsGroup);

    scrollArea->setWidget(scrollContent);

    auto *mainTabLayout = new QVBoxLayout(tab);
    mainTabLayout->setSpacing(0);
    mainTabLayout->setContentsMargins(0, 0, 0, 0);
    mainTabLayout->addWidget(scrollArea);

    createCharts();
}

void TrackingWindow::createCharts()
{
    if (!caloriesChartView || !macrosChartView || !user) {
        return;
    }

    auto *caloriesSet = new QBarSet("Калории");

    double currentCalories = totalCalories > 0 ? totalCalories : 0;
    double targetCalories = user->get_daily_calories() > 0 ? user->get_daily_calories() : 2000;

    *caloriesSet << currentCalories << targetCalories;

    auto *caloriesSeries = new QBarSeries();
    caloriesSeries->append(caloriesSet);

    auto *caloriesChart = new QChart();
    caloriesChart->addSeries(caloriesSeries);
    caloriesChart->setTitle(" Потребление калорий");
    caloriesChart->setAnimationOptions(QChart::SeriesAnimations);
    caloriesChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    caloriesChart->setBackgroundBrush(QBrush(QColor(45, 45, 65)));
    caloriesChart->setTitleBrush(QBrush(Qt::white));

    QStringList categories;
    categories << "Съедено" << "Цель";
    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(Qt::white);
    caloriesChart->addAxis(axisX, Qt::AlignBottom);
    caloriesSeries->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setLabelsColor(Qt::white);
    axisY->setRange(0, qMax(currentCalories, targetCalories) * 1.2);
    caloriesChart->addAxis(axisY, Qt::AlignLeft);
    caloriesSeries->attachAxis(axisY);

    caloriesChart->legend()->setVisible(false);
    caloriesChart->setBackgroundRoundness(10);

    caloriesChartView->setChart(caloriesChart);

    auto *proteinsSet = new QBarSet("Белки");
    auto *fatsSet = new QBarSet("Жиры");
    auto *carbsSet = new QBarSet("Углеводы");

    double currentProteins = totalProteins > 0 ? totalProteins : 0;
    double currentFats = totalFats > 0 ? totalFats : 0;
    double currentCarbs = totalCarbs > 0 ? totalCarbs : 0;

    double targetProteins = user->get_daily_proteins() > 0 ? user->get_daily_proteins() : 100;
    double targetFats = user->get_daily_fats() > 0 ? user->get_daily_fats() : 70;
    double targetCarbs = user->get_daily_carbs() > 0 ? user->get_daily_carbs() : 250;

    *proteinsSet << currentProteins << targetProteins;
    *fatsSet << currentFats << targetFats;
    *carbsSet << currentCarbs << targetCarbs;

    auto *macrosSeries = new QBarSeries();
    macrosSeries->append(proteinsSet);
    macrosSeries->append(fatsSet);
    macrosSeries->append(carbsSet);

    auto *macrosChart = new QChart();
    macrosChart->addSeries(macrosSeries);
    macrosChart->setTitle(" Баланс БЖУ");
    macrosChart->setAnimationOptions(QChart::SeriesAnimations);
    macrosChart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    macrosChart->setBackgroundBrush(QBrush(QColor(45, 45, 65)));
    macrosChart->setTitleBrush(QBrush(Qt::white));

    auto *macrosAxisX = new QBarCategoryAxis();
    macrosAxisX->append(categories);
    macrosAxisX->setLabelsColor(Qt::white);
    macrosChart->addAxis(macrosAxisX, Qt::AlignBottom);
    macrosSeries->attachAxis(macrosAxisX);

    auto *macrosAxisY = new QValueAxis();
    macrosAxisY->setLabelsColor(Qt::white);

    double maxMacro = qMax(qMax(currentProteins, targetProteins),
                           qMax(qMax(currentFats, targetFats),
                                qMax(currentCarbs, targetCarbs)));
    macrosAxisY->setRange(0, maxMacro * 1.2);

    macrosChart->addAxis(macrosAxisY, Qt::AlignLeft);
    macrosSeries->attachAxis(macrosAxisY);

    macrosChart->legend()->setVisible(true);
    macrosChart->legend()->setAlignment(Qt::AlignBottom);
    macrosChart->legend()->setLabelColor(Qt::white);
    macrosChart->setBackgroundRoundness(10);

    macrosChartView->setChart(macrosChart);
}

QString TrackingWindow::getCurrentDate() const
{
    if (dateEdit) {
        return dateEdit->date().toString("yyyy-MM-dd");
    }
    return QDate::currentDate().toString("yyyy-MM-dd");
}

void TrackingWindow::saveTrackingData()
{
    if (!historyManager) {
        qDebug() << "HistoryManager не инициализирован";
        return;
    }

    QString currentDate = getCurrentDate();
    qDebug() << "Сохранение данных за дату:" << currentDate;

    double recalcCalories = 0;
    double recalcProteins = 0;
    double recalcFats = 0;
    double recalcCarbs = 0;

    QList<DayMealEntry> dayMeals;
    for (const auto& entry : mealEntries) {
        DayMealEntry dayEntry(currentDate, entry.mealType, entry.productName,
                              entry.grams, entry.calories, entry.proteins,
                              entry.fats, entry.carbs, entry.timestamp);
        dayMeals.append(dayEntry);

        recalcCalories += entry.calories;
        recalcProteins += entry.proteins;
        recalcFats += entry.fats;
        recalcCarbs += entry.carbs;
    }

    totalCalories = recalcCalories;
    totalProteins = recalcProteins;
    totalFats = recalcFats;
    totalCarbs = recalcCarbs;

    DaySummary summary(currentDate);
    summary.totalCalories = totalCalories;
    summary.totalProteins = totalProteins;
    summary.totalFats = totalFats;
    summary.totalCarbs = totalCarbs;
    summary.meals = dayMeals;

    // Сохраняем в менеджер истории
    historyManager->updateDaySummary(currentDate, summary);

    // Немедленно сохраняем в файл
    if (!historyManager->saveHistoryToFile()) {
        qDebug() << "Ошибка сохранения истории в файл";
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить данные в файл");
    } else {
        qDebug() << "Данные успешно сохранены";
    }
}

void TrackingWindow::loadTrackingData()
{
    if (!historyManager) {
        qDebug() << "HistoryManager не инициализирован";
        return;
    }

    QString currentDate = getCurrentDate();
    qDebug() << "Загрузка данных за дату:" << currentDate;

    DaySummary daySummary = historyManager->getDaySummary(currentDate);

    mealEntries.clear();
    double recalcCalories = 0;
    double recalcProteins = 0;
    double recalcFats = 0;
    double recalcCarbs = 0;

    if (daySummary.date == currentDate) {
        for (const DayMealEntry& dayEntry : daySummary.meals) {
            MealEntry entry;
            entry.mealType = dayEntry.mealType;
            entry.productName = dayEntry.productName;
            entry.grams = dayEntry.grams;
            entry.calories = dayEntry.calories;
            entry.proteins = dayEntry.proteins;
            entry.fats = dayEntry.fats;
            entry.carbs = dayEntry.carbs;
            entry.timestamp = dayEntry.timestamp;
            mealEntries.append(entry);

            recalcCalories += dayEntry.calories;
            recalcProteins += dayEntry.proteins;
            recalcFats += dayEntry.fats;
            recalcCarbs += dayEntry.carbs;
        }
    }

    // Обновляем totals
    totalCalories = recalcCalories;
    totalProteins = recalcProteins;
    totalFats = recalcFats;
    totalCarbs = recalcCarbs;

    qDebug() << "Загружено записей:" << mealEntries.size();
    qDebug() << "Калории:" << totalCalories;

    // Обновляем интерфейс
    if (mealsTable && caloriesProgress) {
        updateMealsTable();
        updateProgressBars();
        updateStatistics();
        updateDailyProgressChart();
    }
}

void TrackingWindow::onDateChanged(const QDate &date)
{
    if (!date.isValid()) {
        return;
    }

    if (!previousDate.isEmpty()) {
        qDebug() << "Сохранение данных за предыдущую дату:" << previousDate;

        DaySummary oldSummary(previousDate);
        oldSummary.totalCalories = totalCalories;
        oldSummary.totalProteins = totalProteins;
        oldSummary.totalFats = totalFats;
        oldSummary.totalCarbs = totalCarbs;

        for (const auto& entry : mealEntries) {
            DayMealEntry dayEntry(previousDate, entry.mealType, entry.productName,
                                  entry.grams, entry.calories, entry.proteins,
                                  entry.fats, entry.carbs, entry.timestamp);
            oldSummary.meals.append(dayEntry);
        }

        if (historyManager) {
            historyManager->updateDaySummary(previousDate, oldSummary);
        }
    }

    previousDate = date.toString("yyyy-MM-dd");
    qDebug() << "Смена даты на:" << previousDate;

    loadTrackingData();
}

void TrackingWindow::updateStatistics()
{
    if (!statsDisplay || !user) {
        return;
    }

    createCharts();

    double dailyCalories = user->get_daily_calories() > 0 ? user->get_daily_calories() : 1;
    double dailyProteins = user->get_daily_proteins() > 0 ? user->get_daily_proteins() : 1;
    double dailyFats = user->get_daily_fats() > 0 ? user->get_daily_fats() : 1;
    double dailyCarbs = user->get_daily_carbs() > 0 ? user->get_daily_carbs() : 1;

    double calPercentage = (totalCalories / dailyCalories) * 100;
    double protPercentage = (totalProteins / dailyProteins) * 100;
    double fatsPercentage = (totalFats / dailyFats) * 100;
    double carbsPercentage = (totalCarbs / dailyCarbs) * 100;

    QString statsText = generateDailyStatsText(calPercentage, protPercentage, fatsPercentage, carbsPercentage);
    statsText += generateAnalysisText(calPercentage, protPercentage, fatsPercentage, carbsPercentage);

    if (historyManager) {
        statsText += generateWeeklyStatsText();
        statsText += generateMealStatsText();
        statsText += generateBJUFactText(protPercentage, fatsPercentage, carbsPercentage);
    }

    statsDisplay->setHtml(statsText);
}

QString TrackingWindow::generateDailyStatsText(double calPercentage, double protPercentage, double fatsPercentage, double carbsPercentage) const
{
    QString calColor;
    if (calPercentage > 100) {
        calColor = "#e74c3c";
    } else if (calPercentage < 80) {
        calColor = "#f39c12";
    } else {
        calColor = "#27ae60";
    }

    QString protColor;
    if (protPercentage > 120) {
        protColor = "#e74c3c";
    } else if (protPercentage < 80) {
        protColor = "#f39c12";
    } else {
        protColor = "#27ae60";
    }

    QString fatsColor;
    if (fatsPercentage > 120) {
        fatsColor = "#e74c3c";
    } else if (fatsPercentage < 80) {
        fatsColor = "#f39c12";
    } else {
        fatsColor = "#27ae60";
    }

    QString carbsColor;
    if (carbsPercentage > 120) {
        carbsColor = "#e74c3c";
    } else if (carbsPercentage < 80) {
        carbsColor = "#f39c12";
    } else {
        carbsColor = "#27ae60";
    }

    QString text = "<h3 style='color: #9457eb; margin-bottom: 15px;'> Статистика за день:</h3>";
    text += QString("<b>Калории:</b> %1/%2 ккал (<span style='color:%4'>%3%</span>)<br>")
                .arg(totalCalories, 0, 'f', 1)
                .arg(user->get_daily_calories(), 0, 'f', 0)
                .arg(calPercentage, 0, 'f', 1)
                .arg(calColor);
    text += QString("<b>Белки:</b> %1/%2 г (<span style='color:%4'>%3%</span>)<br>")
                .arg(totalProteins, 0, 'f', 1)
                .arg(user->get_daily_proteins(), 0, 'f', 1)
                .arg(protPercentage, 0, 'f', 1)
                .arg(protColor);
    text += QString("<b>Жиры:</b> %1/%2 г (<span style='color:%4'>%3%</span>)<br>")
                .arg(totalFats, 0, 'f', 1)
                .arg(user->get_daily_fats(), 0, 'f', 1)
                .arg(fatsPercentage, 0, 'f', 1)
                .arg(fatsColor);
    text += QString("<b>Углеводы:</b> %1/%2 г (<span style='color:%4'>%3%</span>)<br><br>")
                .arg(totalCarbs, 0, 'f', 1)
                .arg(user->get_daily_carbs(), 0, 'f', 1)
                .arg(carbsPercentage, 0, 'f', 1)
                .arg(carbsColor);
    return text;
}

QString TrackingWindow::generateAnalysisText(double calPercentage, double protPercentage, double fatsPercentage, double carbsPercentage) const
{
    QString text = "<h3 style='color: #f39c12; margin-bottom: 15px;'> Анализ:</h3>";

    if (calPercentage < 70) {
        text += "• Калорийность: <span style='color:#e74c3c'>ниже нормы</span><br>";
    } else if (calPercentage > 130) {
        text += "• Калорийность: <span style='color:#e74c3c'>превышена</span><br>";
    } else {
        text += "• Калорийность: <span style='color:#27ae60'>в норме</span><br>";
    }

    if (protPercentage < 80) {
        text += "• Белки: <span style='color:#e74c3c'>недостаточно</span><br>";
    } else if (protPercentage > 120) {
        text += "• Белки: <span style='color:#f39c12'>избыток</span><br>";
    } else {
        text += "• Белки: <span style='color:#27ae60'>сбалансированы</span><br>";
    }

    if (fatsPercentage < 80) {
        text += "• Жиры: <span style='color:#e74c3c'>недостаточно</span><br>";
    } else if (fatsPercentage > 120) {
        text += "• Жиры: <span style='color:#f39c12'>избыток</span><br>";
    } else {
        text += "• Жиры: <span style='color:#27ae60'>сбалансированы</span><br>";
    }

    if (carbsPercentage < 80) {
        text += "• Углеводы: <span style='color:#e74c3c'>недостаточно</span><br>";
    } else if (carbsPercentage > 120) {
        text += "• Углеводы: <span style='color:#f39c12'>избыток</span><br>";
    } else {
        text += "• Углеводы: <span style='color:#27ae60'>сбалансированы</span><br>";
    }

    return text;
}

QString TrackingWindow::generateWeeklyStatsText() const
{
    if (!historyManager) {
        return "";
    }

    QString text = "<br><h3 style='color: #9457eb; margin-bottom: 15px; margin-top: 20px;'> Дополнительная статистика:</h3>";

    QDate today = QDate::currentDate();
    double weeklyAvgCalories = 0;
    double weeklyAvgProteins = 0;
    double weeklyAvgFats = 0;
    double weeklyAvgCarbs = 0;
    int daysWithData = 0;

    for (int i = 0; i < 7; ++i) {
        QDate date = today.addDays(-i);
        QString dateStr = date.toString("yyyy-MM-dd");
        DaySummary summary = historyManager->getDaySummary(dateStr);
        if (!summary.date.isEmpty() && summary.totalCalories > 0) {
            weeklyAvgCalories += summary.totalCalories;
            weeklyAvgProteins += summary.totalProteins;
            weeklyAvgFats += summary.totalFats;
            weeklyAvgCarbs += summary.totalCarbs;
            daysWithData++;
        }
    }

    if (daysWithData > 0) {
        weeklyAvgCalories /= daysWithData;
        weeklyAvgProteins /= daysWithData;
        weeklyAvgFats /= daysWithData;
        weeklyAvgCarbs /= daysWithData;

        text += QString("<div style='background: rgba(148, 87, 235, 0.1); padding: 12px; border-radius: 8px; margin-bottom: 8px; border-left: 3px solid #9457eb;'>");
        text += QString("<b style='color: #9457eb;'>Средние показатели за последние 7 дней (%1 дня с данными):</b><br>").arg(daysWithData);
        text += QString("• Калории: <span style='color: white;'>%1 ккал/день</span><br>").arg(weeklyAvgCalories, 0, 'f', 0);
        text += QString("• Белки: <span style='color: white;'>%1 г/день</span><br>").arg(weeklyAvgProteins, 0, 'f', 1);
        text += QString("• Жиры: <span style='color: white;'>%1 г/день</span><br>").arg(weeklyAvgFats, 0, 'f', 1);
        text += QString("• Углеводы: <span style='color: white;'>%1 г/день</span>").arg(weeklyAvgCarbs, 0, 'f', 1);
        text += "</div>";

        double calDiff = totalCalories - weeklyAvgCalories;
        if (qAbs(calDiff) > 50) {
            QString trendColor = calDiff > 0 ? "#f39c12" : "#3498db";
            QString trendText = calDiff > 0 ? "выше" : "ниже";
            text += QString("<div style='background: rgba(243, 156, 18, 0.1); padding: 12px; border-radius: 8px; margin-top: 8px; border-left: 3px solid #f39c12;'>");
            text += QString("<b style='color: #f39c12;'>Тренд:</b> Сегодня калорийность <span style='color: %1;'>%2</span> среднего на <span style='color: white;'>%3 ккал</span>").arg(trendColor).arg(trendText).arg(qAbs(calDiff), 0, 'f', 0);
            text += "</div>";
        }
    }

    return text;
}

QString TrackingWindow::generateMealStatsText() const
{
    if (!historyManager || mealEntries.isEmpty()) {
        return "";
    }

    // Используем std::array вместо C-style массива
    std::array<int, 4> mealTypesCount = {0};

    for (const auto& entry : mealEntries) {
        if (entry.mealType == "Завтрак") {
            mealTypesCount[0]++;
        } else if (entry.mealType == "Обед") {
            mealTypesCount[1]++;
        } else if (entry.mealType == "Ужин") {
            mealTypesCount[2]++;
        } else if (entry.mealType == "Перекус") {
            mealTypesCount[3]++;
        }
    }

    QString text = "<div style='background: rgba(148, 87, 235, 0.1); padding: 12px; border-radius: 8px; margin-top: 8px; border-left: 3px solid #9457eb;'>";
    text += QString("<b style='color: #9457eb;'>Приемы пищи сегодня:</b><br>");
    text += QString("• Всего записей: <span style='color: white;'>%1</span><br>").arg(mealEntries.size());

    if (mealTypesCount[0] > 0) {
        text += QString("• Завтраки: <span style='color: white;'>%1</span><br>").arg(mealTypesCount[0]);
    }
    if (mealTypesCount[1] > 0) {
        text += QString("• Обеды: <span style='color: white;'>%1</span><br>").arg(mealTypesCount[1]);
    }
    if (mealTypesCount[2] > 0) {
        text += QString("• Ужины: <span style='color: white;'>%1</span><br>").arg(mealTypesCount[2]);
    }
    if (mealTypesCount[3] > 0) {
        text += QString("• Перекусы: <span style='color: white;'>%1</span>").arg(mealTypesCount[3]);
    }

    text += "</div>";
    return text;
}

QString TrackingWindow::generateBJUFactText(double protPercentage, double fatsPercentage, double carbsPercentage) const
{
    QString text = "<div style='background: rgba(243, 156, 18, 0.15); padding: 12px; border-radius: 8px; margin-top: 12px; border-left: 3px solid #f39c12;'>";
    text += "<b style='color: #f39c12;'>Интересный факт:</b><br>";
    if (double bjuBalance = (protPercentage + fatsPercentage + carbsPercentage) / 3.0; bjuBalance >= 90 && bjuBalance <= 110) {
        text += "<span style='color: white;'>Отличный баланс БЖУ! Ваше питание сбалансировано.</span>";
    } else if (bjuBalance < 90) {
        text += "<span style='color: white;'>Рекомендуется добавить больше разнообразия в рацион для лучшего баланса.</span>";
    } else {
        text += "<span style='color: white;'>Следите за перееданием - старайтесь распределять питание равномерно.</span>";
    }
    text += "</div>";
    return text;
}

void TrackingWindow::updateProgressBars()
{
    if (!caloriesProgress || !proteinsProgress || !fatsProgress || !carbsProgress || !user) {
        return;
    }

    caloriesProgress->setRange(0, 100);
    proteinsProgress->setRange(0, 100);
    fatsProgress->setRange(0, 100);
    carbsProgress->setRange(0, 100);

    double dailyCalories = user->get_daily_calories() > 0 ? user->get_daily_calories() : 1;
    double dailyProteins = user->get_daily_proteins() > 0 ? user->get_daily_proteins() : 1;
    double dailyFats = user->get_daily_fats() > 0 ? user->get_daily_fats() : 1;
    double dailyCarbs = user->get_daily_carbs() > 0 ? user->get_daily_carbs() : 1;

    qDebug() << "updateProgressBars: totalCalories=" << totalCalories << "dailyCalories=" << dailyCalories;
    qDebug() << "updateProgressBars: totalProteins=" << totalProteins << "dailyProteins=" << dailyProteins;

    double calPercentage = (totalCalories / dailyCalories) * 100;
    double protPercentage = (totalProteins / dailyProteins) * 100;
    double fatsPercentage = (totalFats / dailyFats) * 100;
    double carbsPercentage = (totalCarbs / dailyCarbs) * 100;

    int calValue = qMin(static_cast<int>(calPercentage), 100);
    int protValue = qMin(static_cast<int>(protPercentage), 100);
    int fatsValue = qMin(static_cast<int>(fatsPercentage), 100);
    int carbsValue = qMin(static_cast<int>(carbsPercentage), 100);

    caloriesProgress->setValue(calValue);
    proteinsProgress->setValue(protValue);
    fatsProgress->setValue(fatsValue);
    carbsProgress->setValue(carbsValue);

    caloriesProgress->setFormat(QString("Калории: %1/%2 (%3%)").arg(totalCalories, 0, 'f', 1).arg(user->get_daily_calories(), 0, 'f', 0).arg(calPercentage, 0, 'f', 1));
    proteinsProgress->setFormat(QString("Белки: %1/%2 г (%3%)").arg(totalProteins, 0, 'f', 1).arg(user->get_daily_proteins(), 0, 'f', 1).arg(protPercentage, 0, 'f', 1));
    fatsProgress->setFormat(QString("Жиры: %1/%2 г (%3%)").arg(totalFats, 0, 'f', 1).arg(user->get_daily_fats(), 0, 'f', 1).arg(fatsPercentage, 0, 'f', 1));
    carbsProgress->setFormat(QString("Углеводы: %1/%2 г (%3%)").arg(totalCarbs, 0, 'f', 1).arg(user->get_daily_carbs(), 0, 'f', 1).arg(carbsPercentage, 0, 'f', 1));
    caloriesProgress->repaint();
    proteinsProgress->repaint();
    fatsProgress->repaint();
    carbsProgress->repaint();

    if (caloriesProgress->parentWidget()) {
        caloriesProgress->parentWidget()->update();
    }

    QApplication::processEvents();
}

void TrackingWindow::resetTracking()
{
    totalCalories = 0;
    totalProteins = 0;
    totalFats = 0;
    totalCarbs = 0;

    mealEntries.clear();

    if (historyManager) {
        QString currentDate = getCurrentDate();
        historyManager->removeDayData(currentDate);
    }

    updateMealsTable();
    updateProgressBars();
    updateStatistics();
    saveTrackingData();
}

void TrackingWindow::onAddMealClicked()
{
    QString productName = productNameEdit->text().trimmed();
    QString gramsText = gramsEdit->text().trimmed();
    QString mealType = mealTypeComboBox->currentText();

    if (productName.isEmpty() || gramsText.isEmpty()) {
        return;
    }

    bool ok;
    double grams = gramsText.toDouble(&ok);
    if (!ok || grams <= 0) {
        return;
    }

    if (addMeal(mealType, productName, grams)) {
        productNameEdit->clear();
        gramsEdit->clear();
        productNameEdit->setFocus();
    }
}

void TrackingWindow::onShowSummaryClicked()
{
    saveTrackingData();
    emit showSummary();
}

bool TrackingWindow::addMeal(const QString &mealType, const QString &productName, double grams)
{
    if (!foodFileManager || productName.trimmed().isEmpty() || grams <= 0) {
        return false;
    }

    FoodFileManager::ProductData product = foodFileManager->findProduct(productName);

    qDebug() << "addMeal: поиск продукта '" << productName << "'";
    qDebug() << "addMeal: найден продукт:" << (product.name.isEmpty() ? "НЕ НАЙДЕН" : product.name);

    if (!product.name.isEmpty()) {
        qDebug() << "addMeal: данные продукта из БД - Ккал:" << product.calories << "Б:" << product.proteins << "Ж:" << product.fats << "У:" << product.carbs << "на 100г";
    }

    if (product.name.isEmpty()) {
        qDebug() << "addMeal: продукт не найден в базе, проверяем существование...";
        if (!foodFileManager->productExists(productName)) {
            qDebug() << "addMeal: продукт не существует, добавляем новый с дефолтными значениями:" << productName;
            foodFileManager->addProduct(productName, 100, 10, 5, 20);
            foodFileManager->saveProductsToFile();
            foodFileManager->loadProductsFromFile();
        }
        product = foodFileManager->findProduct(productName);
        if (product.name.isEmpty()) {
            qDebug() << "addMeal: КРИТИЧЕСКАЯ ОШИБКА - продукт не найден после добавления!";
            QMessageBox::warning(this, "Ошибка", QString("Не удалось найти или добавить продукт: %1").arg(productName));
            return false;
        }
        qDebug() << "addMeal: продукт найден после добавления:" << product.name;
    }

    if (product.calories <= 0 && product.proteins <= 0 && product.fats <= 0 && product.carbs <= 0) {
        qDebug() << "addMeal: ВНИМАНИЕ - продукт имеет нулевые значения БЖУ!";
    }

    double mealCalories = (product.calories * grams) / 100.0;
    double mealProteins = (product.proteins * grams) / 100.0;
    double mealFats = (product.fats * grams) / 100.0;
    double mealCarbs = (product.carbs * grams) / 100.0;

    qDebug() << "addMeal: расчет для" << grams << "г продукта '" << product.name << "'";
    qDebug() << "addMeal: Исходные данные (на 100г): Ккал:" << product.calories << "Б:" << product.proteins << "Ж:" << product.fats << "У:" << product.carbs;
    qDebug() << "addMeal: Рассчитано (на" << grams << "г): Ккал:" << mealCalories << "Б:" << mealProteins << "Ж:" << mealFats << "У:" << mealCarbs;
    qDebug() << "addMeal: Ккал:" << mealCalories << "Б:" << mealProteins << "Ж:" << mealFats << "У:" << mealCarbs;
    totalCalories += mealCalories;
    totalProteins += mealProteins;
    totalFats += mealFats;
    totalCarbs += mealCarbs;

    QString timestamp = QDateTime::currentDateTime().toString("hh:mm");
    QString date = getCurrentDate();

    MealEntry newEntry;
    newEntry.mealType = mealType;
    newEntry.productName = product.name;
    newEntry.grams = grams;
    newEntry.calories = mealCalories;
    newEntry.proteins = mealProteins;
    newEntry.fats = mealFats;
    newEntry.carbs = mealCarbs;
    newEntry.timestamp = timestamp;

    mealEntries.append(newEntry);

    if (historyManager) {
        HistoryManager::MealEntryParams params;
        params.date = date;
        params.mealType = mealType;
        params.productName = product.name;
        params.grams = grams;
        params.calories = mealCalories;
        params.proteins = mealProteins;
        params.fats = mealFats;
        params.carbs = mealCarbs;
        params.timestamp = timestamp;
        historyManager->addMealEntry(params);
    }

    updateMealsTable();
    updateProgressBars();
    updateStatistics();
    updateDailyProgressChart();
    saveTrackingData();


    return true;
}



void TrackingWindow::updateMealsTable()
{
    if (!mealsTable) {
        return;
    }

    mealsTable->setRowCount(mealEntries.size());

    for (int i = 0; i < mealEntries.size(); ++i) {
        const auto &entry = mealEntries[i];

        mealsTable->setItem(i, 0, new QTableWidgetItem(entry.timestamp));
        mealsTable->setItem(i, 1, new QTableWidgetItem(entry.mealType));
        mealsTable->setItem(i, 2, new QTableWidgetItem(entry.productName));
        mealsTable->setItem(i, 3, new QTableWidgetItem(QString::number(entry.grams, 'f', 1)));
        mealsTable->setItem(i, 4, new QTableWidgetItem(QString::number(entry.calories, 'f', 1)));
        mealsTable->setItem(i, 5, new QTableWidgetItem(QString::number(entry.proteins, 'f', 1)));
        mealsTable->setItem(i, 6, new QTableWidgetItem(QString::number(entry.carbs, 'f', 1)));
        mealsTable->setItem(i, 7, new QTableWidgetItem(QString::number(entry.fats, 'f', 1)));
    }

    mealsTable->resizeColumnsToContents();
}

void TrackingWindow::onRemoveMealClicked()
{
    constexpr double ZERO_VALUE = 0.0;

    if (!historyManager || !mealsTable) {
        return;
    }

    if (QString currentDate = getCurrentDate(); currentDate.isEmpty()) {
        return;
    }

    int currentRow = mealsTable->currentRow();

    if (currentRow < 0 || currentRow >= mealEntries.size()) {
        return;
    }

    if (const auto &entry = mealEntries[currentRow]; entry.calories <= ZERO_VALUE && entry.proteins <= ZERO_VALUE && entry.fats <= ZERO_VALUE && entry.carbs <= ZERO_VALUE) {
        mealEntries.removeAt(currentRow);
        updateMealsTable();
        return;
    }

    double recalcCalories = ZERO_VALUE;
    double recalcProteins = ZERO_VALUE;
    double recalcFats = ZERO_VALUE;
    double recalcCarbs = ZERO_VALUE;

    QList<MealEntry> remainingMeals;

    if (mealEntries.isEmpty()) {
        return;
    }

    for (int i = 0; i < mealEntries.size(); ++i) {
        if (i != currentRow) {
            const auto &meal = mealEntries[i];
            if (meal.calories > 0 || meal.proteins > 0 || meal.fats > 0 || meal.carbs > 0) {
                recalcCalories += meal.calories;
                recalcProteins += meal.proteins;
                recalcFats += meal.fats;
                recalcCarbs += meal.carbs;
                remainingMeals.append(meal);
            }
        }
    }

    totalCalories = recalcCalories;
    totalProteins = recalcProteins;
    totalFats = recalcFats;
    totalCarbs = recalcCarbs;

    if (totalCalories < ZERO_VALUE) totalCalories = ZERO_VALUE;
    if (totalProteins < ZERO_VALUE) totalProteins = ZERO_VALUE;
    if (totalFats < ZERO_VALUE) totalFats = ZERO_VALUE;
    if (totalCarbs < ZERO_VALUE) totalCarbs = ZERO_VALUE;

    mealEntries = remainingMeals;

    QString currentDate = getCurrentDate();
    if (historyManager) {
        historyManager->removeMealEntry(currentDate, currentRow);
    }

    saveTrackingData();

    updateMealsTable();
    updateProgressBars();
    updateStatistics();
    updateDailyProgressChart();
}

void TrackingWindow::onProductTextChanged(const QString &text)
{
    Q_UNUSED(text)
}

TrackingWindow::~TrackingWindow() {
    if (historyManager) {
        historyManager->saveHistoryToFile();
        delete historyManager;
        historyManager = nullptr;
    }
    if (foodFileManager) {
        delete foodFileManager;
        foodFileManager = nullptr;
    }
    if (trendAnalyzer) {
        delete trendAnalyzer;
        trendAnalyzer = nullptr;
    }
    if (productCompleter) {
        delete productCompleter;
        productCompleter = nullptr;
    }
}

void TrackingWindow::addTestData() {
}

void TrackingWindow::onProductSearch() {
}

void TrackingWindow::onMealTypeChanged(int index) {
    Q_UNUSED(index)
}

void TrackingWindow::loadDailyData() {
    loadTrackingData();
}

void TrackingWindow::populateProductSuggestions() {
    if (!foodFileManager || !productNameEdit) {
        qDebug() << "populateProductSuggestions: foodFileManager или productNameEdit не инициализирован";
        return;
    }

    QStringList productNames = foodFileManager->getAllProductNames();
    qDebug() << "populateProductSuggestions: найдено продуктов:" << productNames.size();

    if (productNames.isEmpty()) {
        qDebug() << "populateProductSuggestions: список продуктов пуст!";
        return;
    }

    if (productCompleter) {
        productNameEdit->setCompleter(nullptr);
        delete productCompleter;
        productCompleter = nullptr;
    }

    productCompleter = new QCompleter(productNames, this);
    productCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    productCompleter->setFilterMode(Qt::MatchContains);
    productCompleter->setCompletionMode(QCompleter::PopupCompletion);
    productCompleter->setMaxVisibleItems(10);
    productNameEdit->setCompleter(productCompleter);

    qDebug() << "populateProductSuggestions: completer настроен для" << productNames.size() << "продуктов";
}

void TrackingWindow::showProductInfo(const QString &productName) {
    Q_UNUSED(productName)
}

void TrackingWindow::autoSave() {
}

void TrackingWindow::createDailyProgressChart(QChartView *chartView, int days)
{
    if (!chartView || !historyManager || !user) {
        return;
    }

    constexpr int DEFAULT_DAYS = 30;
    constexpr double DEFAULT_CALORIES = 2000.0;

    if (days <= 0) {
        days = DEFAULT_DAYS;
    }

    double targetCalories = user->get_daily_calories() > 0 ? user->get_daily_calories() : DEFAULT_CALORIES;

    QDate today = QDate::currentDate();
    QDate startDate = today.addDays(-(days - 1));

    auto *actualSet = new QBarSet("Фактическое");
    auto *targetSet = new QBarSet("Цель");

    QStringList dateLabels;
    double maxCalories = targetCalories;
    double minCalories = 0;

    for (int i = 0; i < days; ++i) {
        QDate currentDate = startDate.addDays(i);
        QString dateStr = currentDate.toString("yyyy-MM-dd");

        DaySummary summary = historyManager->getDaySummary(dateStr);
        double calories = summary.totalCalories;

        *actualSet << calories;
        *targetSet << targetCalories;

        dateLabels << currentDate.toString("dd.MM");
        maxCalories = qMax(maxCalories, calories);
        minCalories = qMin(minCalories, calories);
    }

    actualSet->setColor(QColor(148, 87, 235));
    targetSet->setColor(QColor(243, 156, 18, 100));

    auto *series = new QBarSeries();
    series->append(actualSet);
    series->append(targetSet);
    series->setBarWidth(0.7);

    auto *chart = new QChart();
    chart->addSeries(series);
    QString periodText;
    if (days == 3) periodText = "3 дня";
    else if (days == 7) periodText = "неделю";
    else if (days == 30) periodText = "месяц";
    else periodText = QString::number(days) + " дней";

    chart->setTitle(QString("Прогресс по калориям за последние %1").arg(periodText));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    chart->setBackgroundBrush(QBrush(QColor(45, 45, 65)));
    chart->setTitleBrush(QBrush(Qt::white));

    auto *axisX = new QBarCategoryAxis();
    axisX->append(dateLabels);
    axisX->setLabelsColor(Qt::white);
    axisX->setTitleText("Дата");
    axisX->setTitleBrush(QBrush(Qt::white));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    double range = maxCalories - minCalories;
    if (range < 500) range = 500;
    axisY->setRange(0, maxCalories + range * 0.3);
    axisY->setLabelsColor(Qt::white);
    axisY->setTitleText("Калории (ккал)");
    axisY->setTitleBrush(QBrush(Qt::white));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(Qt::white);
    chart->setBackgroundRoundness(10);

    constexpr int DAYS_THRESHOLD_SHORT = 3;
    constexpr int DAYS_THRESHOLD_MEDIUM = 7;
    constexpr int PIXELS_PER_DAY_SHORT = 70;
    constexpr int PIXELS_PER_DAY_MEDIUM = 60;
    constexpr int PIXELS_PER_DAY_LONG = 65;
    constexpr int MIN_CHART_WIDTH = 800;

    int chartWidth;
    if (days <= DAYS_THRESHOLD_SHORT) {
        chartWidth = days * PIXELS_PER_DAY_SHORT;
    } else if (days <= DAYS_THRESHOLD_MEDIUM) {
        chartWidth = days * PIXELS_PER_DAY_MEDIUM;
    } else {
        chartWidth = qMax(MIN_CHART_WIDTH, days * PIXELS_PER_DAY_LONG);
    }
    chartView->setMinimumWidth(chartWidth);
    chartView->setChart(chart);
}

void TrackingWindow::updateDailyProgressChart()
{
    if (!dailyProgressChart || !historyManager || !user) {
        return;
    }

    QWidget *parentWidget = dailyProgressChart->parentWidget();
    if (!parentWidget) {
        parentWidget = this;
    }
    QComboBox *periodComboBox = parentWidget->findChild<QComboBox*>();

    constexpr int DEFAULT_DAYS_PERIOD = 30;
    int days = DEFAULT_DAYS_PERIOD;
    if (periodComboBox) {
        days = periodComboBox->currentData().toInt();
    }

    createDailyProgressChart(dailyProgressChart, days);
}

void TrackingWindow::setupTrendsTab(QWidget *tab)
{
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: rgba(255, 255, 255, 0.1);
            width: 12px;
            margin: 0px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background: #9457eb;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background: #8457db;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
        }
    )");

    auto *scrollContent = new QWidget;
    auto *tabLayout = new QVBoxLayout(scrollContent);
    tabLayout->setSpacing(20);
    tabLayout->setContentsMargins(25, 25, 25, 25);

    auto *titleLabel = new QLabel(" Анализ трендов питания");
    titleLabel->setStyleSheet(R"(
        font-size: 28px;
        font-weight: bold;
        color: white;
        padding: 15px;
        text-align: center;
        background: rgba(148, 87, 235, 0.2);
        border-radius: 10px;
        margin: 10px;
    )");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto *controlsGroup = new QGroupBox(" Настройки анализа");
    controlsGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 15px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 10px 0 10px;
            color: white;
            font-weight: bold;
            font-size: 16px;
        }
    )");

    auto *controlsLayout = new QHBoxLayout(controlsGroup);
    auto *periodLabel = new QLabel("Период анализа:");
    periodLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white; margin-right: 10px;");

    trendPeriodComboBox = new QComboBox;
    trendPeriodComboBox->addItem("7 дней", 7);
    trendPeriodComboBox->addItem("14 дней", 14);
    trendPeriodComboBox->addItem("30 дней", 30);
    trendPeriodComboBox->addItem("60 дней", 60);
    constexpr int DEFAULT_PERIOD_INDEX = 2;
    trendPeriodComboBox->setCurrentIndex(DEFAULT_PERIOD_INDEX);
    trendPeriodComboBox->setStyleSheet(R"(
        QComboBox {
            background: rgba(148, 87, 235, 0.3);
            border: 2px solid rgba(148, 87, 235, 0.5);
            border-radius: 8px;
            padding: 8px 15px;
            color: white;
            font-size: 14px;
            font-weight: bold;
            min-width: 150px;
        }
        QComboBox:hover {
            background: rgba(148, 87, 235, 0.5);
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid white;
            width: 0;
            height: 0;
            margin-right: 10px;
        }
        QComboBox QAbstractItemView {
            background: rgba(45, 45, 65, 0.95);
            border: 2px solid rgba(148, 87, 235, 0.5);
            border-radius: 8px;
            color: white;
            selection-background-color: rgba(148, 87, 235, 0.5);
            padding: 5px;
        }
    )");

    updateTrendsButton = new QPushButton("Обновить анализ");
    updateTrendsButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #9457eb, stop:1 #8457db);
            border: none;
            border-radius: 8px;
            padding: 12px 25px;
            font-size: 14px;
            font-weight: bold;
            color: white;
            min-width: 200px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #8457db, stop:1 #7457cb);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7457cb, stop:1 #6457bb);
        }
    )");
    connect(updateTrendsButton, &QPushButton::clicked, this, &TrackingWindow::onUpdateTrends);

    controlsLayout->addWidget(periodLabel);
    controlsLayout->addWidget(trendPeriodComboBox);
    controlsLayout->addWidget(updateTrendsButton);
    controlsLayout->addStretch();

    auto *trendsGroup = new QGroupBox(" Результаты анализа");
    trendsGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.15);
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 15px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 10px 0 10px;
            color: white;
            font-weight: bold;
            font-size: 16px;
        }
    )");

    auto *trendsLayout = new QVBoxLayout(trendsGroup);

    QString tableStyle = R"(
        QTableWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(30, 30, 50, 0.9), stop:1 rgba(20, 20, 40, 0.9));
            border: 2px solid rgba(148, 87, 235, 0.3);
            border-radius: 12px;
            gridline-color: rgba(148, 87, 235, 0.2);
            font-size: 15px;
            color: white;
            selection-background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(148, 87, 235, 0.4), stop:1 rgba(132, 87, 219, 0.4));
            outline: none;
        }
        QTableWidget::item {
            padding: 18px;
            border: none;
            border-bottom: 1px solid rgba(148, 87, 235, 0.15);
        }
        QTableWidget::item:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(148, 87, 235, 0.2), stop:1 rgba(132, 87, 219, 0.2));
        }
        QTableWidget::item:selected {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(148, 87, 235, 0.5), stop:1 rgba(132, 87, 219, 0.5));
            color: white;
            font-weight: bold;
        }
        QTableWidget::item:alternate {
            background: rgba(148, 87, 235, 0.08);
        }
        QHeaderView::section {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(148, 87, 235, 0.6), stop:1 rgba(132, 87, 219, 0.6));
            color: white;
            padding: 18px;
            border: none;
            border-bottom: 2px solid rgba(148, 87, 235, 0.8);
            font-weight: bold;
            font-size: 17px;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        QHeaderView::section:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(148, 87, 235, 0.8), stop:1 rgba(132, 87, 219, 0.8));
        }
        QTableCornerButton::section {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(148, 87, 235, 0.6), stop:1 rgba(132, 87, 219, 0.6));
            border: none;
            border-bottom: 2px solid rgba(148, 87, 235, 0.8);
        }
    )";

    auto *summaryLabel = new QLabel("Средние значения и тренды:");
    summaryLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white; margin-top: 10px;");
    trendsSummaryTable = new QTableWidget(4, 3);
    trendsSummaryTable->setHorizontalHeaderLabels(QStringList() << "Макронутриент" << "Среднее значение" << "Тренд");
    trendsSummaryTable->verticalHeader()->setVisible(false);
    trendsSummaryTable->setStyleSheet(tableStyle);
    trendsSummaryTable->horizontalHeader()->setStretchLastSection(true);
    trendsSummaryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    trendsSummaryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    trendsSummaryTable->setAlternatingRowColors(true);
    trendsSummaryTable->verticalHeader()->setDefaultSectionSize(60);
    trendsSummaryTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    trendsSummaryTable->setMinimumHeight(250);

    auto *topProductsLabel = new QLabel("Топ-10 наиболее часто употребляемых продуктов:");
    topProductsLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white; margin-top: 15px;");
    trendsTopProductsTable = new QTableWidget(0, 3);
    trendsTopProductsTable->setHorizontalHeaderLabels(QStringList() << "№" << "Продукт" << "Количество раз");
    trendsTopProductsTable->verticalHeader()->setVisible(false);
    trendsTopProductsTable->setStyleSheet(tableStyle);
    trendsTopProductsTable->horizontalHeader()->setStretchLastSection(true);
    trendsTopProductsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    trendsTopProductsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    trendsTopProductsTable->setAlternatingRowColors(true);
    trendsTopProductsTable->verticalHeader()->setDefaultSectionSize(50);
    trendsTopProductsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    trendsTopProductsTable->setMinimumHeight(400);

    auto *mealDistLabel = new QLabel("Распределение калорий по приемам пищи:");
    mealDistLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white; margin-top: 15px;");
    trendsMealDistributionTable = new QTableWidget(0, 2);
    trendsMealDistributionTable->setHorizontalHeaderLabels(QStringList() << "Прием пищи" << "Средние калории");
    trendsMealDistributionTable->verticalHeader()->setVisible(false);
    trendsMealDistributionTable->setStyleSheet(tableStyle);
    trendsMealDistributionTable->horizontalHeader()->setStretchLastSection(true);
    trendsMealDistributionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    trendsMealDistributionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    trendsMealDistributionTable->setAlternatingRowColors(true);
    trendsMealDistributionTable->verticalHeader()->setDefaultSectionSize(60);
    trendsMealDistributionTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    trendsMealDistributionTable->setMinimumHeight(200);

    auto *weekdayLabel = new QLabel("Средние калории по дням недели:");
    weekdayLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white; margin-top: 15px;");
    trendsWeekdayTable = new QTableWidget(0, 2);
    trendsWeekdayTable->setHorizontalHeaderLabels(QStringList() << "День недели" << "Средние калории");
    trendsWeekdayTable->verticalHeader()->setVisible(false);
    trendsWeekdayTable->setStyleSheet(tableStyle);
    trendsWeekdayTable->horizontalHeader()->setStretchLastSection(true);
    trendsWeekdayTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    trendsWeekdayTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    trendsWeekdayTable->setAlternatingRowColors(true);
    trendsWeekdayTable->verticalHeader()->setDefaultSectionSize(55);
    trendsWeekdayTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    trendsWeekdayTable->setMinimumHeight(350);

    auto *statsLabel = new QLabel("Общая статистика:");
    statsLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white; margin-top: 15px;");
    trendsStatsTable = new QTableWidget(0, 2);
    trendsStatsTable->setHorizontalHeaderLabels(QStringList() << "Показатель" << "Значение");
    trendsStatsTable->verticalHeader()->setVisible(false);
    trendsStatsTable->setStyleSheet(tableStyle);
    trendsStatsTable->horizontalHeader()->setStretchLastSection(true);
    trendsStatsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    trendsStatsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    trendsStatsTable->setAlternatingRowColors(true);
    trendsStatsTable->verticalHeader()->setDefaultSectionSize(55);
    trendsStatsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    trendsStatsTable->setMinimumHeight(250);

    trendsLayout->addWidget(summaryLabel);
    trendsLayout->addWidget(trendsSummaryTable);

    trendsLayout->addWidget(topProductsLabel);
    trendsLayout->addWidget(trendsTopProductsTable, 1);

    trendsLayout->addWidget(mealDistLabel);
    trendsLayout->addWidget(trendsMealDistributionTable);

    trendsLayout->addWidget(weekdayLabel);
    trendsLayout->addWidget(trendsWeekdayTable);

    trendsLayout->addWidget(statsLabel);
    trendsLayout->addWidget(trendsStatsTable);

    tabLayout->addWidget(titleLabel);
    tabLayout->addWidget(controlsGroup);
    tabLayout->addWidget(trendsGroup);
    tabLayout->addStretch();

    scrollArea->setWidget(scrollContent);

    auto *mainTabLayout = new QVBoxLayout(tab);
    mainTabLayout->setSpacing(0);
    mainTabLayout->setContentsMargins(0, 0, 0, 0);
    mainTabLayout->addWidget(scrollArea);
}

void TrackingWindow::onUpdateTrends()
{
    if (!trendAnalyzer || !trendsSummaryTable) {
        return;
    }

    if (!trendAnalyzer->isValid()) {
        constexpr int ERROR_ROW_COUNT = 1;
        trendsSummaryTable->setRowCount(ERROR_ROW_COUNT);
        trendsSummaryTable->setItem(0, 0, new QTableWidgetItem("Ошибка"));
        trendsSummaryTable->setItem(0, 1, new QTableWidgetItem("Недостаточно данных для анализа"));
        trendsSummaryTable->setItem(0, 2, new QTableWidgetItem(""));
        return;
    }

    int days = trendPeriodComboBox->currentData().toInt();

    TrendAnalysis analysis = trendAnalyzer->analyzeTrends(days);

    constexpr int SUMMARY_TABLE_ROWS = 4;
    trendsSummaryTable->setRowCount(SUMMARY_TABLE_ROWS);
    auto *calItem = new QTableWidgetItem("Калории");
    calItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trendsSummaryTable->setItem(0, 0, calItem);
    trendsSummaryTable->setItem(0, 1, new QTableWidgetItem(QString("%1 ккал").arg(static_cast<int>(analysis.avgCalories))));
    trendsSummaryTable->setItem(0, 2, new QTableWidgetItem(analysis.caloriesTrend));

    auto *protItem = new QTableWidgetItem("Белки");
    protItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trendsSummaryTable->setItem(1, 0, protItem);
    trendsSummaryTable->setItem(1, 1, new QTableWidgetItem(QString("%1 г").arg(static_cast<int>(analysis.avgProteins))));
    trendsSummaryTable->setItem(1, 2, new QTableWidgetItem(analysis.proteinsTrend));

    auto *fatItem = new QTableWidgetItem("Жиры");
    fatItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trendsSummaryTable->setItem(2, 0, fatItem);
    trendsSummaryTable->setItem(2, 1, new QTableWidgetItem(QString("%1 г").arg(static_cast<int>(analysis.avgFats))));
    trendsSummaryTable->setItem(2, 2, new QTableWidgetItem(analysis.fatsTrend));

    auto *carbItem = new QTableWidgetItem("Углеводы");
    carbItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trendsSummaryTable->setItem(3, 0, carbItem);
    trendsSummaryTable->setItem(3, 1, new QTableWidgetItem(QString("%1 г").arg(static_cast<int>(analysis.avgCarbs))));
    trendsSummaryTable->setItem(3, 2, new QTableWidgetItem(analysis.carbsTrend));

    trendsTopProductsTable->setRowCount(analysis.topProducts.size());
    for (int i = 0; i < analysis.topProducts.size(); ++i) {
        auto *numItem = new QTableWidgetItem(QString::number(i + 1));
        numItem->setTextAlignment(Qt::AlignCenter);
        trendsTopProductsTable->setItem(i, 0, numItem);

        auto *nameItem = new QTableWidgetItem(analysis.topProducts[i].first);
        nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        trendsTopProductsTable->setItem(i, 1, nameItem);

        auto *countItem = new QTableWidgetItem(QString("%1 раз").arg(analysis.topProducts[i].second));
        countItem->setTextAlignment(Qt::AlignCenter);
        trendsTopProductsTable->setItem(i, 2, countItem);
    }

    trendsMealDistributionTable->setRowCount(analysis.mealDistribution.size());
    int row = 0;
    for (auto it = analysis.mealDistribution.constBegin(); it != analysis.mealDistribution.constEnd(); ++it) {
        auto *mealItem = new QTableWidgetItem(it.key());
        mealItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        trendsMealDistributionTable->setItem(row, 0, mealItem);
        auto *mealCaloriesItem = new QTableWidgetItem(QString("%1 ккал").arg(static_cast<int>(it.value())));
        mealCaloriesItem->setTextAlignment(Qt::AlignCenter);
        trendsMealDistributionTable->setItem(row, 1, mealCaloriesItem);
        row++;
    }

    QStringList weekdayOrder = {"Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье"};
    int weekdayRowCount = 0;
    for (const QString& day : weekdayOrder) {
        if (analysis.weekdayCalories.contains(day)) {
            weekdayRowCount++;
        }
    }
    trendsWeekdayTable->setRowCount(weekdayRowCount);
    int weekdayRow = 0;  // Используем другое имя переменной
    for (const QString& day : weekdayOrder) {
        if (analysis.weekdayCalories.contains(day)) {
            auto *dayItem = new QTableWidgetItem(day);
            dayItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            trendsWeekdayTable->setItem(weekdayRow, 0, dayItem);

            double calories = analysis.weekdayCalories[day];
            QString caloriesText = QString("%1 ккал").arg(static_cast<int>(calories));
            if (day == analysis.mostCaloricDay) {
                caloriesText += " (максимум)";
            } else if (day == analysis.leastCaloricDay) {
                caloriesText += " (минимум)";
            }
            auto *weekdayCalItem = new QTableWidgetItem(caloriesText);  // Переименовано
            weekdayCalItem->setTextAlignment(Qt::AlignCenter);
            trendsWeekdayTable->setItem(weekdayRow, 1, weekdayCalItem);
            weekdayRow++;
        }
    }

    constexpr int STATS_TABLE_ROWS = 5;
    trendsStatsTable->setRowCount(STATS_TABLE_ROWS);

    auto *periodItem = new QTableWidgetItem("Период анализа");
    periodItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trendsStatsTable->setItem(0, 0, periodItem);
    trendsStatsTable->setItem(0, 1, new QTableWidgetItem(
                                        QString("%1 - %2 (%3 дней)").arg(analysis.startDate.toString("dd.MM.yyyy"))
                                            .arg(analysis.endDate.toString("dd.MM.yyyy"))
                                            .arg(analysis.totalDays)));

    auto *dataItem = new QTableWidgetItem("Дней с данными");
    dataItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trendsStatsTable->setItem(1, 0, dataItem);
    auto *dataValue = new QTableWidgetItem(QString::number(analysis.daysWithData));
    dataValue->setTextAlignment(Qt::AlignCenter);
    trendsStatsTable->setItem(1, 1, dataValue);

    auto *stabilityItem = new QTableWidgetItem("Стабильность питания");
    stabilityItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trendsStatsTable->setItem(2, 0, stabilityItem);
    auto *stabilityValue = new QTableWidgetItem(QString("%1/100").arg(static_cast<int>(analysis.stabilityScore)));
    stabilityValue->setTextAlignment(Qt::AlignCenter);
    trendsStatsTable->setItem(2, 1, stabilityValue);

    auto *overItem = new QTableWidgetItem("Дней превышения нормы");
    overItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trendsStatsTable->setItem(3, 0, overItem);
    auto *overValue = new QTableWidgetItem(QString::number(analysis.daysOverTarget));
    overValue->setTextAlignment(Qt::AlignCenter);
    trendsStatsTable->setItem(3, 1, overValue);

    auto *underItem = new QTableWidgetItem("Дней ниже нормы");
    underItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trendsStatsTable->setItem(4, 0, underItem);
    auto *underValue = new QTableWidgetItem(QString::number(analysis.daysUnderTarget));
    underValue->setTextAlignment(Qt::AlignCenter);
    trendsStatsTable->setItem(4, 1, underValue);

    trendsSummaryTable->resizeColumnsToContents();
    trendsSummaryTable->resizeRowsToContents();

    trendsTopProductsTable->resizeColumnsToContents();
    trendsTopProductsTable->resizeRowsToContents();

    trendsMealDistributionTable->resizeColumnsToContents();
    trendsMealDistributionTable->resizeRowsToContents();

    trendsWeekdayTable->resizeColumnsToContents();
    trendsWeekdayTable->resizeRowsToContents();

    trendsStatsTable->resizeColumnsToContents();
    trendsStatsTable->resizeRowsToContents();

    for (int i = 0; i < trendsSummaryTable->columnCount(); ++i) {
        trendsSummaryTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    for (int i = 0; i < trendsTopProductsTable->columnCount(); ++i) {
        trendsTopProductsTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    for (int i = 0; i < trendsMealDistributionTable->columnCount(); ++i) {
        trendsMealDistributionTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    for (int i = 0; i < trendsWeekdayTable->columnCount(); ++i) {
        trendsWeekdayTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    for (int i = 0; i < trendsStatsTable->columnCount(); ++i) {
        trendsStatsTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
}

