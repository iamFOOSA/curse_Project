#include "../headers/summarywindow.h"
#include "../headers/user.h"
#include "../headers/history_manager.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QGridLayout>
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>
#include <QTableWidget>
#include <QHeaderView>
#include <QDate>
#include <QTableWidgetItem>
#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QBarSeries>
#include <QComboBox>
#include <QBarSet>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QDateTime>
#include <QTime>
#include <QPainter>
#include <QFrame>

SummaryWindow::SummaryWindow(User *user, HistoryManager *historyManager, QWidget *parent)
    : QWidget(parent), user(user), historyManager(historyManager)
{
    if (!historyManager) {
        this->historyManager = new HistoryManager("data/nutrition_history.json");
    }
    
    setStyleSheet(R"(
        SummaryWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a1a2e, stop:0.5 #16213e, stop:1 #0f3460);
            font-family: 'Segoe UI', Arial, sans-serif;
            color: white;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea;
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
    )");

    QWidget *scrollContent = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(20);
    scrollLayout->setContentsMargins(20, 20, 20, 20);

    // Заголовок с датой
    QFrame *headerFrame = new QFrame;
    headerFrame->setStyleSheet(R"(
        QFrame {
            background: rgba(148, 87, 235, 0.2);
            border-radius: 15px;
            padding: 20px;
        }
    )");
    QVBoxLayout *headerLayout = new QVBoxLayout(headerFrame);
    
    QLabel *titleLabel = new QLabel("Итоги дня");
    titleLabel->setStyleSheet(R"(
        font-size: 32px;
        font-weight: bold;
        color: white;
        padding: 10px;
    )");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    dateLabel = new QLabel;
    dateLabel->setStyleSheet(R"(
        font-size: 16px;
        color: rgba(255, 255, 255, 0.8);
        padding: 5px;
    )");
    dateLabel->setAlignment(Qt::AlignCenter);
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(dateLabel);

    // Карточки метрик в строку
    QHBoxLayout *metricsLayout = new QHBoxLayout;
    metricsLayout->setSpacing(15);
    
    auto createMetricCard = [](const QString& title, const QString& color) -> QFrame* {
        QFrame *card = new QFrame;
        card->setStyleSheet(QString(R"(
            QFrame {
                background: rgba(255, 255, 255, 0.1);
                border: 2px solid %1;
                border-radius: 15px;
                padding: 15px;
            }
        )").arg(color));
        card->setMinimumHeight(120);
        return card;
    };
    
    QFrame *caloriesCard = createMetricCard("Калории", "#e74c3c");
    QFrame *proteinsCard = createMetricCard("Белки", "#3498db");
    QFrame *fatsCard = createMetricCard("Жиры", "#f39c12");
    QFrame *carbsCard = createMetricCard("Углеводы", "#27ae60");
    
    // Для каждой карточки создаем layout с метриками
    // Калории
    QVBoxLayout *caloriesCardLayout = new QVBoxLayout(caloriesCard);
    caloriesCardLayout->setSpacing(10);
    caloriesCardLayout->setContentsMargins(10, 10, 10, 10);
    QLabel *caloriesLabel = new QLabel("Калории");
    caloriesLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white;");
    caloriesCardLayout->addWidget(caloriesLabel);
    caloriesProgress = new QProgressBar;
    caloriesProgress->setStyleSheet(R"(
        QProgressBar {
            border: 1px solid rgba(255,255,255,0.2);
            border-radius: 6px;
            background: rgba(255,255,255,0.05);
            height: 30px;
            font-size: 12px;
            font-weight: bold;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e74c3c, stop:1 #c0392b);
            border-radius: 5px;
        }
    )");
    caloriesCardLayout->addWidget(caloriesProgress);
    caloriesCardLayout->addStretch();
    
    // Белки
    QVBoxLayout *proteinsCardLayout = new QVBoxLayout(proteinsCard);
    proteinsCardLayout->setSpacing(10);
    proteinsCardLayout->setContentsMargins(10, 10, 10, 10);
    QLabel *proteinsLabel = new QLabel("Белки");
    proteinsLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white;");
    proteinsCardLayout->addWidget(proteinsLabel);
    proteinsProgress = new QProgressBar;
    proteinsProgress->setStyleSheet(R"(
        QProgressBar {
            border: 1px solid rgba(255,255,255,0.2);
            border-radius: 6px;
            background: rgba(255,255,255,0.05);
            height: 30px;
            font-size: 12px;
            font-weight: bold;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3498db, stop:1 #2980b9);
            border-radius: 5px;
        }
    )");
    proteinsCardLayout->addWidget(proteinsProgress);
    proteinsCardLayout->addStretch();
    
    // Жиры
    QVBoxLayout *fatsCardLayout = new QVBoxLayout(fatsCard);
    fatsCardLayout->setSpacing(10);
    fatsCardLayout->setContentsMargins(10, 10, 10, 10);
    QLabel *fatsLabel = new QLabel("Жиры");
    fatsLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white;");
    fatsCardLayout->addWidget(fatsLabel);
    fatsProgress = new QProgressBar;
    fatsProgress->setStyleSheet(R"(
            QProgressBar {
            border: 1px solid rgba(255,255,255,0.2);
            border-radius: 6px;
            background: rgba(255,255,255,0.05);
                height: 30px;
                font-size: 12px;
                font-weight: bold;
            }
            QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #f39c12, stop:1 #e67e22);
            border-radius: 5px;
        }
    )");
    fatsCardLayout->addWidget(fatsProgress);
    fatsCardLayout->addStretch();
    
    // Углеводы
    QVBoxLayout *carbsCardLayout = new QVBoxLayout(carbsCard);
    carbsCardLayout->setSpacing(10);
    carbsCardLayout->setContentsMargins(10, 10, 10, 10);
    QLabel *carbsLabel = new QLabel("Углеводы");
    carbsLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white;");
    carbsCardLayout->addWidget(carbsLabel);
    carbsProgress = new QProgressBar;
    carbsProgress->setStyleSheet(R"(
        QProgressBar {
            border: 1px solid rgba(255,255,255,0.2);
            border-radius: 6px;
            background: rgba(255,255,255,0.05);
            height: 30px;
            font-size: 12px;
            font-weight: bold;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #27ae60, stop:1 #229954);
            border-radius: 5px;
        }
    )");
    carbsCardLayout->addWidget(carbsProgress);
    carbsCardLayout->addStretch();
    
    metricsLayout->addWidget(caloriesCard);
    metricsLayout->addWidget(proteinsCard);
    metricsLayout->addWidget(fatsCard);
    metricsLayout->addWidget(carbsCard);

    // Анализ и рекомендации
    QFrame *analysisFrame = new QFrame;
    analysisFrame->setStyleSheet(R"(
        QFrame {
            background: rgba(255, 255, 255, 0.08);
            border: 2px solid rgba(148, 87, 235, 0.5);
            border-radius: 15px;
            padding: 20px;
        }
    )");
    QVBoxLayout *analysisLayout = new QVBoxLayout(analysisFrame);

    QLabel *analysisTitle = new QLabel("Анализ дня");
    analysisTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #9457eb; padding: 10px 0;");
    analysisLayout->addWidget(analysisTitle);

    summaryLabel = new QLabel;
    summaryLabel->setStyleSheet(R"(
        font-size: 14px;
        color: rgba(255, 255, 255, 0.9);
        padding: 15px;
        background: rgba(255, 255, 255, 0.05);
        border-radius: 10px;
        line-height: 1.8;
    )");
    summaryLabel->setWordWrap(true);
    analysisLayout->addWidget(summaryLabel);

    // Кнопка возврата
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    backButton = new QPushButton("← Вернуться к отслеживанию");
    backButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #9457eb, stop:1 #f39c12);
            color: white;
            border: none;
            border-radius: 12px;
            padding: 15px 40px;
            font-size: 15px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #8457db, stop:1 #e38c02);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7457cb, stop:1 #d37c02);
        }
    )");
    buttonLayout->addStretch();
    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch();

    // График прогресса по дням - улучшенный дизайн
    QFrame *chartFrame = new QFrame;
    chartFrame->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(243, 156, 18, 0.12), stop:1 rgba(148, 87, 235, 0.08));
            border: 2px solid rgba(243, 156, 18, 0.6);
            border-radius: 15px;
            padding: 20px;
        }
    )");
    QVBoxLayout *chartLayout = new QVBoxLayout(chartFrame);
    chartLayout->setContentsMargins(0, 0, 0, 0);
    chartLayout->setSpacing(0);
    
    QFrame *chartTitleFrame = new QFrame;
    chartTitleFrame->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(243, 156, 18, 0.3), stop:1 rgba(148, 87, 235, 0.3));
            border-radius: 10px;
            padding: 8px;
        }
    )");
    QHBoxLayout *chartTitleLayout = new QHBoxLayout(chartTitleFrame);
    chartTitleLayout->setContentsMargins(15, 8, 15, 8);
    
    QLabel *chartTitle = new QLabel("📊 Прогресс по дням");
    chartTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: white; background: transparent;");
    chartTitleLayout->addWidget(chartTitle);
    chartTitleLayout->addStretch();
    chartLayout->addWidget(chartTitleFrame);
    
    // Создаем ScrollArea для графика с горизонтальной прокруткой
    QScrollArea *chartScrollArea = new QScrollArea;
    chartScrollArea->setWidgetResizable(false);
    chartScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    chartScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chartScrollArea->setFixedHeight(400); // Фиксированная высота, чтобы не было вертикального скролла
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
    
    // Кнопки выбора периода
    QHBoxLayout *periodLayout = new QHBoxLayout;
    QLabel *periodLabel = new QLabel("Период:");
    periodLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: white; margin-right: 10px;");
    periodLayout->addWidget(periodLabel);
    
    QComboBox *periodComboBox = new QComboBox;
    periodComboBox->addItem("3 дня", 3);
    periodComboBox->addItem("Неделя", 7);
    periodComboBox->addItem("Месяц", 30);
    periodComboBox->setCurrentIndex(2); // По умолчанию месяц
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
    chartLayout->addLayout(periodLayout);
    
    progressChartView = new QChartView();
    progressChartView->setRenderHint(QPainter::Antialiasing);
    progressChartView->setFixedHeight(400); // Фиксированная высота без вертикального скролла
    progressChartView->setStyleSheet("background: transparent; border: none;");
    
    chartScrollArea->setWidget(progressChartView);
    chartScrollArea->setFixedHeight(400); // Фиксированная высота
    chartLayout->addWidget(chartScrollArea, 1);
    
    // Подключаем изменение периода к обновлению графика
    connect(periodComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, periodComboBox]() {
        int days = periodComboBox->currentData().toInt();
        createProgressChart(days);
    });

    // Таблица приемов пищи - улучшенный дизайн
    QFrame *mealsFrame = new QFrame;
    mealsFrame->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(52, 152, 219, 0.15), stop:1 rgba(52, 152, 219, 0.05));
            border: 2px solid rgba(52, 152, 219, 0.6);
            border-radius: 15px;
            padding: 20px;
        }
    )");
    QVBoxLayout *mealsLayout = new QVBoxLayout(mealsFrame);
    
    QFrame *mealsTitleFrame = new QFrame;
    mealsTitleFrame->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(52, 152, 219, 0.3), stop:1 rgba(148, 87, 235, 0.3));
            border-radius: 10px;
            padding: 10px;
        }
    )");
    QHBoxLayout *mealsTitleLayout = new QHBoxLayout(mealsTitleFrame);
    mealsTitleLayout->setContentsMargins(15, 10, 15, 10);
    
    QLabel *mealsIcon = new QLabel("🍽️");
    mealsIcon->setStyleSheet("font-size: 24px; background: transparent;");
    mealsTitleLayout->addWidget(mealsIcon);
    
    QLabel *mealsTitle = new QLabel("Приемы пищи");
    mealsTitle->setStyleSheet(R"(
        font-size: 22px;
        font-weight: bold;
        color: white;
        background: transparent;
        padding: 5px 0;
    )");
    mealsTitleLayout->addWidget(mealsTitle);
    mealsTitleLayout->addStretch();
    mealsLayout->addWidget(mealsTitleFrame);
    
    mealsTable = new QTableWidget;
    mealsTable->setColumnCount(8);
    mealsTable->setHorizontalHeaderLabels(QStringList() << "Время" << "Прием пищи" << "Продукт" << "Вес (г)" << "Ккал" << "Белки (г)" << "Жиры (г)" << "Углеводы (г)");
    mealsTable->setStyleSheet(R"(
        QTableWidget {
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(52, 152, 219, 0.4);
            border-radius: 12px;
            color: white;
            gridline-color: rgba(52, 152, 219, 0.2);
            font-size: 13px;
        }
        QHeaderView::section {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(52, 152, 219, 0.8), stop:1 rgba(148, 87, 235, 0.8));
            color: white;
            padding: 12px 8px;
            border: none;
            border-bottom: 2px solid rgba(255, 255, 255, 0.3);
            font-weight: bold;
            font-size: 13px;
            border-top-left-radius: 10px;
            border-top-right-radius: 10px;
        }
        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }
        QTableWidget::item:selected {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 rgba(148, 87, 235, 0.4), stop:1 rgba(52, 152, 219, 0.4));
            color: white;
        }
        QTableWidget::item:hover {
            background: rgba(148, 87, 235, 0.2);
        }
    )");
    mealsTable->horizontalHeader()->setStretchLastSection(true);
    mealsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mealsTable->setAlternatingRowColors(true);
    mealsLayout->addWidget(mealsTable);

    scrollLayout->addWidget(headerFrame);
    scrollLayout->addLayout(metricsLayout);
    scrollLayout->addWidget(chartFrame);
    scrollLayout->addWidget(mealsFrame);
    scrollLayout->addWidget(analysisFrame);
    scrollLayout->addLayout(buttonLayout);
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    connect(backButton, &QPushButton::clicked, this, &SummaryWindow::onBackClicked);
}

void SummaryWindow::displaySummary()
{
    displaySummaryForDate(QDate::currentDate().toString("yyyy-MM-dd"));
}

void SummaryWindow::displaySummaryForDate(const QString& date)
{
    QString targetDate = date.isEmpty() ? QDate::currentDate().toString("yyyy-MM-dd") : date;
    
    DaySummary summary = historyManager->getDaySummary(targetDate);
    
    // Если нет данных для выбранной даты, создаем пустую сводку
    if (summary.date.isEmpty()) {
        summary = DaySummary(targetDate);
    }

    QDate summaryDate = QDate::fromString(targetDate, "yyyy-MM-dd");
    if (!summaryDate.isValid()) {
        summaryDate = QDate::currentDate();
        targetDate = summaryDate.toString("yyyy-MM-dd");
        summary = historyManager->getDaySummary(targetDate);
        if (summary.date.isEmpty()) {
            summary = DaySummary(targetDate);
        }
    }
    
    QString dateDisplay = summaryDate.toString("dd.MM.yyyy");
    QString dayOfWeek;
    switch (summaryDate.dayOfWeek()) {
        case 1: dayOfWeek = "Понедельник"; break;
        case 2: dayOfWeek = "Вторник"; break;
        case 3: dayOfWeek = "Среда"; break;
        case 4: dayOfWeek = "Четверг"; break;
        case 5: dayOfWeek = "Пятница"; break;
        case 6: dayOfWeek = "Суббота"; break;
        case 7: dayOfWeek = "Воскресенье"; break;
        default: dayOfWeek = ""; break;
    }
    dateLabel->setText(QString("%1 • %2").arg(dateDisplay, dayOfWeek));

    double consumedCalories = summary.totalCalories;
    double consumedProteins = summary.totalProteins;
    double consumedFats = summary.totalFats;
    double consumedCarbs = summary.totalCarbs;

    double targetCalories = user->get_daily_calories() > 0 ? user->get_daily_calories() : 2000;
    double targetProteins = user->get_daily_proteins() > 0 ? user->get_daily_proteins() : 100;
    double targetFats = user->get_daily_fats() > 0 ? user->get_daily_fats() : 70;
    double targetCarbs = user->get_daily_carbs() > 0 ? user->get_daily_carbs() : 250;

    // Обновляем прогресс-бары (без текста в формате)
    updateProgressBar(caloriesProgress, consumedCalories, targetCalories);
    updateProgressBar(proteinsProgress, consumedProteins, targetProteins);
    updateProgressBar(fatsProgress, consumedFats, targetFats);
    updateProgressBar(carbsProgress, consumedCarbs, targetCarbs);

    // Статистика дня
    QString summaryText = "<div style='margin-bottom: 20px;'>";
    summaryText += "<h3 style='color: #9457eb; margin-bottom: 15px;'>Статистика дня:</h3>";
    summaryText += QString("<div style='background: rgba(255,255,255,0.05); padding: 15px; border-radius: 10px; margin-bottom: 10px;'>");
    summaryText += QString("<b>Всего приемов пищи:</b> <span style='color: #9457eb;'>%1</span><br>").arg(summary.meals.size());
    
    int mealTypesCount[4] = {0};
    for (const DayMealEntry& meal : summary.meals) {
        if (meal.mealType == "Завтрак") mealTypesCount[0]++;
        else if (meal.mealType == "Обед") mealTypesCount[1]++;
        else if (meal.mealType == "Ужин") mealTypesCount[2]++;
        else if (meal.mealType == "Перекус") mealTypesCount[3]++;
    }
    
    if (mealTypesCount[0] > 0) summaryText += QString("• Завтраки: %1<br>").arg(mealTypesCount[0]);
    if (mealTypesCount[1] > 0) summaryText += QString("• Обеды: %1<br>").arg(mealTypesCount[1]);
    if (mealTypesCount[2] > 0) summaryText += QString("• Ужины: %1<br>").arg(mealTypesCount[2]);
    if (mealTypesCount[3] > 0) summaryText += QString("• Перекусы: %1<br>").arg(mealTypesCount[3]);
    summaryText += "</div>";
    summaryText += "</div>";

    summaryText += "<h3 style='color: #9457eb; margin-bottom: 15px;'>Анализ выполнения цели:</h3>";

    double calPercentage = targetCalories > 0 ? (consumedCalories / targetCalories) * 100 : 0;
    QString calStatus;
    if (calPercentage < 80) {
        calStatus = "<span style='color:#e74c3c'>Недостаточно</span>";
    } else if (calPercentage > 120) {
        calStatus = "<span style='color:#e74c3c'>Превышено</span>";
    } else {
        calStatus = "<span style='color:#27ae60'>Отлично</span>";
    }
    summaryText += QString("<div style='background: rgba(255,255,255,0.05); padding: 12px; border-radius: 8px; margin-bottom: 8px;'>");
    summaryText += QString("<b>Калории:</b> %1 ккал / %2 ккал (%3%%) - %4").arg(consumedCalories, 0, 'f', 0).arg(targetCalories, 0, 'f', 0).arg(calPercentage, 0, 'f', 1).arg(calStatus);
    summaryText += "</div>";

    double protPercentage = targetProteins > 0 ? (consumedProteins / targetProteins) * 100 : 0;
    QString protStatus;
    if (protPercentage < 80) {
        protStatus = "<span style='color:#e74c3c'>Недостаточно</span>";
    } else if (protPercentage > 120) {
        protStatus = "<span style='color:#f39c12'>Избыток</span>";
    } else {
        protStatus = "<span style='color:#27ae60'>В норме</span>";
    }
    summaryText += QString("<div style='background: rgba(255,255,255,0.05); padding: 12px; border-radius: 8px; margin-bottom: 8px;'>");
    summaryText += QString("<b>Белки:</b> %1 г / %2 г (%3%%) - %4").arg(consumedProteins, 0, 'f', 1).arg(targetProteins, 0, 'f', 1).arg(protPercentage, 0, 'f', 1).arg(protStatus);
    summaryText += "</div>";

    double fatsPercentage = targetFats > 0 ? (consumedFats / targetFats) * 100 : 0;
    QString fatsStatus;
    if (fatsPercentage < 80) {
        fatsStatus = "<span style='color:#e74c3c'>Недостаточно</span>";
    } else if (fatsPercentage > 120) {
        fatsStatus = "<span style='color:#f39c12'>Избыток</span>";
    } else {
        fatsStatus = "<span style='color:#27ae60'>В норме</span>";
    }
    summaryText += QString("<div style='background: rgba(255,255,255,0.05); padding: 12px; border-radius: 8px; margin-bottom: 8px;'>");
    summaryText += QString("<b>Жиры:</b> %1 г / %2 г (%3%%) - %4").arg(consumedFats, 0, 'f', 1).arg(targetFats, 0, 'f', 1).arg(fatsPercentage, 0, 'f', 1).arg(fatsStatus);
    summaryText += "</div>";

    double carbsPercentage = targetCarbs > 0 ? (consumedCarbs / targetCarbs) * 100 : 0;
    QString carbsStatus;
    if (carbsPercentage < 80) {
        carbsStatus = "<span style='color:#e74c3c'>Недостаточно</span>";
    } else if (carbsPercentage > 120) {
        carbsStatus = "<span style='color:#f39c12'>Избыток</span>";
    } else {
        carbsStatus = "<span style='color:#27ae60'>В норме</span>";
    }
    summaryText += QString("<div style='background: rgba(255,255,255,0.05); padding: 12px; border-radius: 8px; margin-bottom: 15px;'>");
    summaryText += QString("<b>Углеводы:</b> %1 г / %2 г (%3%%) - %4").arg(consumedCarbs, 0, 'f', 1).arg(targetCarbs, 0, 'f', 1).arg(carbsPercentage, 0, 'f', 1).arg(carbsStatus);
    summaryText += "</div>";

    summaryText += "<h3 style='color: #f39c12; margin-bottom: 15px;'>Рекомендации:</h3>";

    if (calPercentage < 80) {
        summaryText += "<div style='background: rgba(231, 76, 60, 0.1); padding: 12px; border-radius: 8px; border-left: 4px solid #e74c3c; margin-bottom: 8px;'>";
        summaryText += "• <b>Калории:</b> Добавьте полезные перекусы или увеличите порции основных приемов пищи<br>";
        summaryText += "</div>";
    } else if (calPercentage > 120) {
        summaryText += "<div style='background: rgba(231, 76, 60, 0.1); padding: 12px; border-radius: 8px; border-left: 4px solid #e74c3c; margin-bottom: 8px;'>";
        summaryText += "• <b>Калории:</b> Превышена норма. Следующий день можно сделать разгрузочным<br>";
        summaryText += "</div>";
    } else {
        summaryText += "<div style='background: rgba(39, 174, 96, 0.1); padding: 12px; border-radius: 8px; border-left: 4px solid #27ae60; margin-bottom: 8px;'>";
        summaryText += "• <b>Калории:</b> Отличный баланс! Продолжайте в том же духе<br>";
        summaryText += "</div>";
    }

    if (protPercentage < 80) {
        summaryText += "<div style='background: rgba(243, 156, 18, 0.1); padding: 12px; border-radius: 8px; border-left: 4px solid #f39c12; margin-bottom: 8px;'>";
        summaryText += "• <b>Белки:</b> Добавьте белковых продуктов (мясо, рыба, творог, яйца, бобовые)<br>";
        summaryText += "</div>";
    } else if (protPercentage > 120) {
        summaryText += "<div style='background: rgba(243, 156, 18, 0.1); padding: 12px; border-radius: 8px; border-left: 4px solid #f39c12; margin-bottom: 8px;'>";
        summaryText += "• <b>Белки:</b> Белки превышают норму. Следите за балансом<br>";
        summaryText += "</div>";
    }

    if (fatsPercentage < 80) {
        summaryText += "<div style='background: rgba(243, 156, 18, 0.1); padding: 12px; border-radius: 8px; border-left: 4px solid #f39c12; margin-bottom: 8px;'>";
        summaryText += "• <b>Жиры:</b> Не хватает полезных жиров (орехи, авокадо, оливковое масло, рыба)<br>";
        summaryText += "</div>";
    } else if (fatsPercentage > 120) {
        summaryText += "<div style='background: rgba(243, 156, 18, 0.1); padding: 12px; border-radius: 8px; border-left: 4px solid #f39c12; margin-bottom: 8px;'>";
        summaryText += "• <b>Жиры:</b> Жиры превышают норму. Ограничьте жирные продукты<br>";
        summaryText += "</div>";
    }

    if (carbsPercentage < 80) {
        summaryText += "<div style='background: rgba(243, 156, 18, 0.1); padding: 12px; border-radius: 8px; border-left: 4px solid #f39c12; margin-bottom: 8px;'>";
        summaryText += "• <b>Углеводы:</b> Нужно больше углеводов (крупы, цельнозерновой хлеб, фрукты, овощи)<br>";
        summaryText += "</div>";
    } else if (carbsPercentage > 120) {
        summaryText += "<div style='background: rgba(243, 156, 18, 0.1); padding: 12px; border-radius: 8px; border-left: 4px solid #f39c12; margin-bottom: 8px;'>";
        summaryText += "• <b>Углеводы:</b> Углеводы превышают норму. Ограничьте сладкое и мучное<br>";
        summaryText += "</div>";
    }

    summaryText += "<div style='background: rgba(148, 87, 235, 0.15); padding: 15px; border-radius: 10px; border-left: 4px solid #9457eb; margin-top: 15px;'>";
    summaryText += "<b style='color: #9457eb; font-size: 16px;'>Мотивация:</b><br>";
    if (calPercentage >= 80 && calPercentage <= 120 && 
        protPercentage >= 80 && protPercentage <= 120 &&
        fatsPercentage >= 80 && fatsPercentage <= 120 &&
        carbsPercentage >= 80 && carbsPercentage <= 120) {
        summaryText += "Отличная работа! Вы идеально соблюдаете баланс БЖУ. Продолжайте в том же духе!";
    } else {
        summaryText += "Каждый день - это новый шанс стать лучше. Следите за балансом БЖУ, пейте воду и оставайтесь активными!";
    }
    summaryText += "</div>";

    summaryLabel->setText(summaryText);

    mealsTable->setRowCount(summary.meals.size());
    for (int i = 0; i < summary.meals.size(); ++i) {
        const DayMealEntry& entry = summary.meals[i];
        mealsTable->setItem(i, 0, new QTableWidgetItem(entry.timestamp));
        mealsTable->setItem(i, 1, new QTableWidgetItem(entry.mealType));
        mealsTable->setItem(i, 2, new QTableWidgetItem(entry.productName));
        mealsTable->setItem(i, 3, new QTableWidgetItem(QString::number(entry.grams, 'f', 1)));
        mealsTable->setItem(i, 4, new QTableWidgetItem(QString::number(entry.calories, 'f', 1)));
        mealsTable->setItem(i, 5, new QTableWidgetItem(QString::number(entry.proteins, 'f', 1)));
        mealsTable->setItem(i, 6, new QTableWidgetItem(QString::number(entry.fats, 'f', 1)));
        mealsTable->setItem(i, 7, new QTableWidgetItem(QString::number(entry.carbs, 'f', 1)));
    }
    mealsTable->resizeColumnsToContents();
    
    createProgressChart(30);
}

void SummaryWindow::onBackClicked()
{
    emit backToTracking();
}

void SummaryWindow::createProgressChart(int days)
{
    if (!progressChartView || !historyManager || !user) {
        return;
    }
    
    if (days <= 0) {
        days = 30; // По умолчанию месяц
    }
    
    QStringList dates = historyManager->getAvailableDates();
    double targetCalories = user->get_daily_calories() > 0 ? user->get_daily_calories() : 2000;
    
    // Определяем диапазон дат
    QDate today = QDate::currentDate();
    QDate startDate = today.addDays(-(days - 1)); // Включая сегодня
    
    QBarSet *actualSet = new QBarSet("Фактическое");
    QBarSet *targetSet = new QBarSet("Цель");
    
    QStringList dateLabels;
    double maxCalories = targetCalories;
    double minCalories = 0;
    
    // Проходим по всем дням последовательно
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
    
    QBarSeries *series = new QBarSeries();
    series->append(actualSet);
    series->append(targetSet);
    series->setBarWidth(0.7);
    
    QChart *chart = new QChart();
    chart->addSeries(series);
    QString periodText;
    if (days == 3) periodText = "3 дня";
    else if (days == 7) periodText = "неделю";
    else if (days == 30) periodText = "месяц";
    else periodText = QString::number(days) + " дней";
    
    chart->setTitle(QString("Прогресс по калориям за последние %1").arg(periodText));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitleFont(QFont("Segoe UI", 14, QFont::Bold));
    chart->setBackgroundBrush(QBrush(QColor(45, 45, 65)));
    chart->setTitleBrush(QBrush(Qt::white));
    
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(dateLabels);
    axisX->setLabelsColor(Qt::white);
    axisX->setTitleText("Дата");
    axisX->setTitleBrush(QBrush(Qt::white));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis();
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
    
    // Устанавливаем минимальную ширину для графика
    // Для 3 дней и недели делаем более сжатым, для месяца - широким
    int chartWidth;
    if (days <= 3) {
        chartWidth = days * 70; // ~70px на день для 3 дней - более компактно
    } else if (days <= 7) {
        chartWidth = days * 60; // ~60px на день для недели - более компактно
    } else {
        chartWidth = qMax(800, days * 65); // ~65px на день для месяца
    }
    progressChartView->setMinimumWidth(chartWidth);
    progressChartView->setChart(chart);
}

void SummaryWindow::createProgressBar(QProgressBar *progressBar, const QString &text, double value, double max)
{
    if (max <= 0) {
        max = 1;
    }
    int percentage = qMin(static_cast<int>((value / max) * 100), 200);
    progressBar->setRange(0, 200);
    progressBar->setValue(qMin(percentage, 200));

    QString color;
    if (percentage < 80) {
        color = "#e74c3c";
    } else if (percentage > 120) {
        color = "#f39c12";
    } else {
        color = "#27ae60";
    }

    progressBar->setFormat(QString("<span style='color: %4'>%1: %2/%3 (%5%)</span>")
                               .arg(text)
                               .arg(value, 0, 'f', 1)
                               .arg(max, 0, 'f', 1)
                               .arg(color)
                               .arg(percentage));
}

void SummaryWindow::updateProgressBar(QProgressBar *progressBar, double value, double max)
{
    if (max <= 0) {
        max = 1;
    }
    int percentage = qMin(static_cast<int>((value / max) * 100), 200);
    progressBar->setRange(0, 200);
    progressBar->setValue(qMin(percentage, 200));
    
    // Устанавливаем пустой формат - без текста
    progressBar->setFormat("");
}
