#include "../headers/menuwindow.h"
#include "../headers/manager.h"
#include "../headers/user.h"
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>
#include <QScrollBar>

              MenuWindow::MenuWindow(Manager *manager, User *user, QWidget *parent)
    : QWidget(parent), manager(manager), user(user)
{
    setStyleSheet(R"(
        MenuWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a1a2e, stop:0.5 #16213e, stop:1 #0f3460);
            font-family: 'Segoe UI', Arial, sans-serif;
            color: white;
        }
    )");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(25, 25, 25, 25);

    // Заголовок
    auto *titleLabel = new QLabel(" Примерное меню на 3 дня");
    titleLabel->setStyleSheet(R"(
        font-size: 32px;
        font-weight: bold;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #9457eb, stop:1 #f39c12);
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
        padding: 20px;
        text-align: center;
    )");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto *menuGroup = new QGroupBox(" План питания");
    menuGroup->setStyleSheet(R"(
        QGroupBox {
            background: rgba(255, 255, 255, 0.08);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 15px;
            margin-top: 10px;
            padding-top: 20px;
            color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 12px 0 12px;
            color: white;
            font-weight: bold;
            font-size: 18px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #9457eb, stop:1 #f39c12);
            border-radius: 8px;
        }
    )");

    auto *menuLayout = new QVBoxLayout(menuGroup);
    menuDisplay = new QTextEdit;
    menuDisplay->setReadOnly(true);
    menuDisplay->setStyleSheet(R"(
        QTextEdit {
            background: rgba(255, 255, 255, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 12px;
            padding: 20px;
            font-size: 14px;
            line-height: 1.5;
            color: white;
        }
        QScrollBar:vertical {
            background: rgba(255, 255, 255, 0.1);
            width: 15px;
            margin: 0px;
            border-radius: 7px;
        }
        QScrollBar::handle:vertical {
            background: #9457eb;
            border-radius: 7px;
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
    menuDisplay->setFixedHeight(450);
    menuLayout->addWidget(menuDisplay);

    startTrackingButton = new QPushButton(" Начать отслеживание питания");
    startTrackingButton->setStyleSheet(R"(
        QPushButton {
            background: rgba(255, 255, 255, 0.15);
            color: white;
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 12px;
            padding: 18px 35px;
            font-size: 16px;
            font-weight: bold;
            margin: 10px;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.25);
            border-color: rgba(255, 255, 255, 0.4);
        }
        QPushButton:pressed {
            background: rgba(255, 255, 255, 0.1);
        }
    )");

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(menuGroup, 1);
    mainLayout->addWidget(startTrackingButton);
    mainLayout->setAlignment(startTrackingButton, Qt::AlignCenter);

    connect(startTrackingButton, &QPushButton::clicked, this, &MenuWindow::onStartTrackingClicked);
}

void MenuWindow::displayMenu()
{
    if (!user) {
        return;
    }
    
    QString goal = QString::fromStdString(user->get_goal());
    double targetCalories = user->get_daily_calories();
    double targetProteins = user->get_daily_proteins();
    double targetFats = user->get_daily_fats();
    double targetCarbs = user->get_daily_carbs();
    
    QString menuText;
    menuText += "<div style='font-family: \"Segoe UI\", Arial, sans-serif; color: white; line-height: 1.6;'>";
    
    // Заголовок в зависимости от цели
    QString titleColor;
    QString goalDescription;
    if (goal == "похудение") {
        titleColor = "#e74c3c";
        goalDescription = "для похудения";
    } else if (goal == "набор") {
        titleColor = "#3498db";
        goalDescription = "для набора массы";
    } else {
        titleColor = "#27ae60";
        goalDescription = "для поддержания веса";
    }
    
    menuText += QString("<h1 style='color: %1; text-align: center; margin-bottom: 25px; font-size: 24px;'>План питания на 3 дня (%2)</h1>").arg(titleColor).arg(goalDescription);
    menuText += QString("<div style='background: rgba(255,255,255,0.05); padding: 15px; border-radius: 10px; margin-bottom: 20px; text-align: center;'>");
    menuText += QString("<b style='color: %1;'>Ваша дневная норма:</b> <span style='color: white;'>%2 ккал | Белки: %3г | Жиры: %4г | Углеводы: %5г</span>").arg(titleColor).arg(targetCalories, 0, 'f', 0).arg(targetProteins, 0, 'f', 1).arg(targetFats, 0, 'f', 1).arg(targetCarbs, 0, 'f', 1);
    menuText += "</div>";
    
    // Меню в зависимости от цели
    if (goal == "похудение") {
        // Меню для похудения - низкокалорийное, сбалансированное
        menuText += "<div style='background: linear-gradient(135deg, rgba(231, 76, 60, 0.15), rgba(192, 57, 43, 0.15)); padding: 20px; border-radius: 12px; margin: 15px 0; border: 1px solid rgba(255, 255, 255, 0.1);'>";
        menuText += "<h2 style='color: #e74c3c; margin-bottom: 12px; font-size: 18px;'>День 1 - Легкий старт</h2>";
        menuText += "<ul style='color: rgba(255, 255, 255, 0.9); line-height: 1.7; font-size: 13px; margin-left: 10px;'>";
        menuText += "<li style='margin-bottom: 8px;'><b>Завтрак:</b> Овсяная каша на воде с ягодами (150г каша + 50г ягод) - 220 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Яблоко зеленое (150г) - 78 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Обед:</b> Куриная грудка на пару (120г) + овощной салат (200г) - 280 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Творог обезжиренный (150г) - 130 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Ужин:</b> Рыба запеченная (150г) + овощи на пару (150г) - 230 ккал</li>";
        menuText += "</ul>";
        double day1Cal = targetCalories * 0.85;
        double day1Prot = targetProteins * 0.95;
        double day1Fats = targetFats * 0.80;
        double day1Carbs = targetCarbs * 0.75;
        menuText += QString("<div style='background: rgba(255, 255, 255, 0.1); padding: 10px; border-radius: 6px; margin-top: 12px; border-left: 3px solid #e74c3c;'>");
        menuText += QString("<b style='color: #e74c3c;'>Итого:</b> <span style='color: white;'>~%1 ккал | Белки: ~%2г | Жиры: ~%3г | Углеводы: ~%4г</span>").arg(day1Cal, 0, 'f', 0).arg(day1Prot, 0, 'f', 1).arg(day1Fats, 0, 'f', 1).arg(day1Carbs, 0, 'f', 1);
        menuText += "</div></div>";
        
        menuText += "<div style='background: linear-gradient(135deg, rgba(231, 76, 60, 0.15), rgba(192, 57, 43, 0.15)); padding: 20px; border-radius: 12px; margin: 15px 0; border: 1px solid rgba(255, 255, 255, 0.1);'>";
        menuText += "<h2 style='color: #e74c3c; margin-bottom: 12px; font-size: 18px;'>День 2 - Белковый акцент</h2>";
        menuText += "<ul style='color: rgba(255, 255, 255, 0.9); line-height: 1.7; font-size: 13px; margin-left: 10px;'>";
        menuText += "<li style='margin-bottom: 8px;'><b>Завтрак:</b> Омлет из 2 яиц с овощами (180г) - 250 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Грейпфрут (200г) - 70 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Обед:</b> Индейка отварная (150г) + салат из капусты (200г) - 270 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Кефир 1% (200г) - 90 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Ужин:</b> Творог обезжиренный (200г) + огурец (100г) - 165 ккал</li>";
        menuText += "</ul>";
        double day2Cal = targetCalories * 0.82;
        double day2Prot = targetProteins * 1.05;
        double day2Fats = targetFats * 0.75;
        double day2Carbs = targetCarbs * 0.70;
        menuText += QString("<div style='background: rgba(255, 255, 255, 0.1); padding: 10px; border-radius: 6px; margin-top: 12px; border-left: 3px solid #e74c3c;'>");
        menuText += QString("<b style='color: #e74c3c;'>Итого:</b> <span style='color: white;'>~%1 ккал | Белки: ~%2г | Жиры: ~%3г | Углеводы: ~%4г</span>").arg(day2Cal, 0, 'f', 0).arg(day2Prot, 0, 'f', 1).arg(day2Fats, 0, 'f', 1).arg(day2Carbs, 0, 'f', 1);
        menuText += "</div></div>";
        
        menuText += "<div style='background: linear-gradient(135deg, rgba(231, 76, 60, 0.15), rgba(192, 57, 43, 0.15)); padding: 20px; border-radius: 12px; margin: 15px 0; border: 1px solid rgba(255, 255, 255, 0.1);'>";
        menuText += "<h2 style='color: #e74c3c; margin-bottom: 12px; font-size: 18px;'>День 3 - Сбалансированный</h2>";
        menuText += "<ul style='color: rgba(255, 255, 255, 0.9); line-height: 1.7; font-size: 13px; margin-left: 10px;'>";
        menuText += "<li style='margin-bottom: 8px;'><b>Завтрак:</b> Гречневая каша (150г) + яйцо всмятку (1 шт) - 290 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Йогурт натуральный без сахара (150г) - 90 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Обед:</b> Рыба запеченная (180г) + овощи гриль (200г) - 310 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Творог 5% (150г) + зелень - 145 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Ужин:</b> Салат из свежих овощей с курицей (250г) - 200 ккал</li>";
        menuText += "</ul>";
        double day3Cal = targetCalories * 0.88;
        double day3Prot = targetProteins * 0.98;
        double day3Fats = targetFats * 0.85;
        double day3Carbs = targetCarbs * 0.78;
        menuText += QString("<div style='background: rgba(255, 255, 255, 0.1); padding: 10px; border-radius: 6px; margin-top: 12px; border-left: 3px solid #e74c3c;'>");
        menuText += QString("<b style='color: #e74c3c;'>Итого:</b> <span style='color: white;'>~%1 ккал | Белки: ~%2г | Жиры: ~%3г | Углеводы: ~%4г</span>").arg(day3Cal, 0, 'f', 0).arg(day3Prot, 0, 'f', 1).arg(day3Fats, 0, 'f', 1).arg(day3Carbs, 0, 'f', 1);
        menuText += "</div></div>";
        
    } else if (goal == "набор") {
        // Меню для набора массы - высококалорийное, белковое
        menuText += "<div style='background: linear-gradient(135deg, rgba(52, 152, 219, 0.15), rgba(41, 128, 185, 0.15)); padding: 20px; border-radius: 12px; margin: 15px 0; border: 1px solid rgba(255, 255, 255, 0.1);'>";
        menuText += "<h2 style='color: #3498db; margin-bottom: 12px; font-size: 18px;'>День 1 - Энергичный</h2>";
        menuText += "<ul style='color: rgba(255, 255, 255, 0.9); line-height: 1.7; font-size: 13px; margin-left: 10px;'>";
        menuText += "<li style='margin-bottom: 8px;'><b>Завтрак:</b> Овсяная каша на молоке с бананом и орехами (200г каша + 100г банан + 30г орехи) - 680 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Творог 9% (200г) + мед (30г) - 320 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Обед:</b> Говядина тушеная (200г) + рис (150г) + овощной салат (150г) - 780 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Протеиновый коктейль (300мл) + банан (100г) - 350 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Ужин:</b> Куриная грудка запеченная (200г) + картофель отварной (200г) + овощи (150г) - 570 ккал</li>";
        menuText += "</ul>";
        double day1Cal = targetCalories * 1.10;
        double day1Prot = targetProteins * 1.15;
        double day1Fats = targetFats * 1.05;
        double day1Carbs = targetCarbs * 1.12;
        menuText += QString("<div style='background: rgba(255, 255, 255, 0.1); padding: 10px; border-radius: 6px; margin-top: 12px; border-left: 3px solid #3498db;'>");
        menuText += QString("<b style='color: #3498db;'>Итого:</b> <span style='color: white;'>~%1 ккал | Белки: ~%2г | Жиры: ~%3г | Углеводы: ~%4г</span>").arg(day1Cal, 0, 'f', 0).arg(day1Prot, 0, 'f', 1).arg(day1Fats, 0, 'f', 1).arg(day1Carbs, 0, 'f', 1);
        menuText += "</div></div>";
        
        menuText += "<div style='background: linear-gradient(135deg, rgba(52, 152, 219, 0.15), rgba(41, 128, 185, 0.15)); padding: 20px; border-radius: 12px; margin: 15px 0; border: 1px solid rgba(255, 255, 255, 0.1);'>";
        menuText += "<h2 style='color: #3498db; margin-bottom: 12px; font-size: 18px;'>День 2 - Белковый</h2>";
        menuText += "<ul style='color: rgba(255, 255, 255, 0.9); line-height: 1.7; font-size: 13px; margin-left: 10px;'>";
        menuText += "<li style='margin-bottom: 8px;'><b>Завтрак:</b> Яичница из 3 яиц + цельнозерновой хлеб (100г) + сыр (50г) - 520 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Орехи смешанные (50г) + сухофрукты (50г) - 350 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Обед:</b> Стейк из говядины (250г) + макароны из твердых сортов (200г) + овощи (150г) - 920 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Творог 9% (250г) + сметана 15% (50г) + ягоды (100г) - 420 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Ужин:</b> Лосось запеченный (200г) + бурый рис (150г) + авокадо (100г) - 650 ккал</li>";
        menuText += "</ul>";
        double day2Cal = targetCalories * 1.15;
        double day2Prot = targetProteins * 1.20;
        double day2Fats = targetFats * 1.10;
        double day2Carbs = targetCarbs * 1.08;
        menuText += QString("<div style='background: rgba(255, 255, 255, 0.1); padding: 10px; border-radius: 6px; margin-top: 12px; border-left: 3px solid #3498db;'>");
        menuText += QString("<b style='color: #3498db;'>Итого:</b> <span style='color: white;'>~%1 ккал | Белки: ~%2г | Жиры: ~%3г | Углеводы: ~%4г</span>").arg(day2Cal, 0, 'f', 0).arg(day2Prot, 0, 'f', 1).arg(day2Fats, 0, 'f', 1).arg(day2Carbs, 0, 'f', 1);
        menuText += "</div></div>";
        
        menuText += "<div style='background: linear-gradient(135deg, rgba(52, 152, 219, 0.15), rgba(41, 128, 185, 0.15)); padding: 20px; border-radius: 12px; margin: 15px 0; border: 1px solid rgba(255, 255, 255, 0.1);'>";
        menuText += "<h2 style='color: #3498db; margin-bottom: 12px; font-size: 18px;'>День 3 - Сбалансированный</h2>";
        menuText += "<ul style='color: rgba(255, 255, 255, 0.9); line-height: 1.7; font-size: 13px; margin-left: 10px;'>";
        menuText += "<li style='margin-bottom: 8px;'><b>Завтрак:</b> Творожная запеканка с изюмом (250г) + сметана (50г) - 480 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Молочный коктейль с бананом (400мл) - 320 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Обед:</b> Индейка тушеная (200г) + гречка (180г) + салат (150г) - 720 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Орехи грецкие (60г) + мед (40г) - 480 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Ужин:</b> Куриные бедра запеченные (250г) + овощи на гриле (200г) - 520 ккал</li>";
        menuText += "</ul>";
        double day3Cal = targetCalories * 1.08;
        double day3Prot = targetProteins * 1.12;
        double day3Fats = targetFats * 1.08;
        double day3Carbs = targetCarbs * 1.05;
        menuText += QString("<div style='background: rgba(255, 255, 255, 0.1); padding: 10px; border-radius: 6px; margin-top: 12px; border-left: 3px solid #3498db;'>");
        menuText += QString("<b style='color: #3498db;'>Итого:</b> <span style='color: white;'>~%1 ккал | Белки: ~%2г | Жиры: ~%3г | Углеводы: ~%4г</span>").arg(day3Cal, 0, 'f', 0).arg(day3Prot, 0, 'f', 1).arg(day3Fats, 0, 'f', 1).arg(day3Carbs, 0, 'f', 1);
        menuText += "</div></div>";
        
    } else {
        // Меню для поддержания веса - сбалансированное
        menuText += "<div style='background: linear-gradient(135deg, rgba(39, 174, 96, 0.15), rgba(46, 204, 113, 0.15)); padding: 20px; border-radius: 12px; margin: 15px 0; border: 1px solid rgba(255, 255, 255, 0.1);'>";
        menuText += "<h2 style='color: #27ae60; margin-bottom: 12px; font-size: 18px;'>День 1 - Сбалансированный</h2>";
        menuText += "<ul style='color: rgba(255, 255, 255, 0.9); line-height: 1.7; font-size: 13px; margin-left: 10px;'>";
        menuText += "<li style='margin-bottom: 8px;'><b>Завтрак:</b> Овсяная каша на молоке с фруктами (200г каша + 100г фрукты) - 420 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Йогурт натуральный (150г) + орехи (30г) - 240 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Обед:</b> Куриная грудка запеченная (180г) + гречка (150г) + овощной салат (150г) - 580 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Яблоко (180г) + сыр (40г) - 180 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Ужин:</b> Рыба запеченная (150г) + овощи на пару (200г) - 310 ккал</li>";
        menuText += "</ul>";
        double day1Cal = targetCalories * 0.98;
        double day1Prot = targetProteins * 1.0;
        double day1Fats = targetFats * 0.95;
        double day1Carbs = targetCarbs * 0.97;
        menuText += QString("<div style='background: rgba(255, 255, 255, 0.1); padding: 10px; border-radius: 6px; margin-top: 12px; border-left: 3px solid #27ae60;'>");
        menuText += QString("<b style='color: #27ae60;'>Итого:</b> <span style='color: white;'>~%1 ккал | Белки: ~%2г | Жиры: ~%3г | Углеводы: ~%4г</span>").arg(day1Cal, 0, 'f', 0).arg(day1Prot, 0, 'f', 1).arg(day1Fats, 0, 'f', 1).arg(day1Carbs, 0, 'f', 1);
        menuText += "</div></div>";
        
        menuText += "<div style='background: linear-gradient(135deg, rgba(39, 174, 96, 0.15), rgba(46, 204, 113, 0.15)); padding: 20px; border-radius: 12px; margin: 15px 0; border: 1px solid rgba(255, 255, 255, 0.1);'>";
        menuText += "<h2 style='color: #27ae60; margin-bottom: 12px; font-size: 18px;'>День 2 - Разнообразный</h2>";
        menuText += "<ul style='color: rgba(255, 255, 255, 0.9); line-height: 1.7; font-size: 13px; margin-left: 10px;'>";
        menuText += "<li style='margin-bottom: 8px;'><b>Завтрак:</b> Омлет из 2 яиц с овощами и сыром (220г) - 380 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Творог 5% (180г) + ягоды (100г) - 250 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Обед:</b> Индейка тушеная (200г) + рис (160г) + салат (150г) - 640 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Орехи (40г) + сухофрукты (40г) - 300 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Ужин:</b> Творог 5% (200г) + зелень и овощи (100г) - 240 ккал</li>";
        menuText += "</ul>";
        double day2Cal = targetCalories * 1.02;
        double day2Prot = targetProteins * 1.05;
        double day2Fats = targetFats * 1.0;
        double day2Carbs = targetCarbs * 1.0;
        menuText += QString("<div style='background: rgba(255, 255, 255, 0.1); padding: 10px; border-radius: 6px; margin-top: 12px; border-left: 3px solid #27ae60;'>");
        menuText += QString("<b style='color: #27ae60;'>Итого:</b> <span style='color: white;'>~%1 ккал | Белки: ~%2г | Жиры: ~%3г | Углеводы: ~%4г</span>").arg(day2Cal, 0, 'f', 0).arg(day2Prot, 0, 'f', 1).arg(day2Fats, 0, 'f', 1).arg(day2Carbs, 0, 'f', 1);
        menuText += "</div></div>";
        
        menuText += "<div style='background: linear-gradient(135deg, rgba(39, 174, 96, 0.15), rgba(46, 204, 113, 0.15)); padding: 20px; border-radius: 12px; margin: 15px 0; border: 1px solid rgba(255, 255, 255, 0.1);'>";
        menuText += "<h2 style='color: #27ae60; margin-bottom: 12px; font-size: 18px;'>День 3 - Классический</h2>";
        menuText += "<ul style='color: rgba(255, 255, 255, 0.9); line-height: 1.7; font-size: 13px; margin-left: 10px;'>";
        menuText += "<li style='margin-bottom: 8px;'><b>Завтрак:</b> Гречневая каша (180г) + яйцо (1 шт) + овощи (100г) - 450 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Кефир (250мл) + банан (120г) - 250 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Обед:</b> Рыба запеченная (200г) + картофель отварной (200г) + салат (150г) - 580 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Перекус:</b> Орехи (30г) + яблоко (150г) - 230 ккал</li>";
        menuText += "<li style='margin-bottom: 8px;'><b>Ужин:</b> Куриная грудка (150г) + овощи на гриле (200г) - 280 ккал</li>";
        menuText += "</ul>";
        double day3Cal = targetCalories * 1.0;
        double day3Prot = targetProteins * 0.98;
        double day3Fats = targetFats * 1.0;
        double day3Carbs = targetCarbs * 1.03;
        menuText += QString("<div style='background: rgba(255, 255, 255, 0.1); padding: 10px; border-radius: 6px; margin-top: 12px; border-left: 3px solid #27ae60;'>");
        menuText += QString("<b style='color: #27ae60;'>Итого:</b> <span style='color: white;'>~%1 ккал | Белки: ~%2г | Жиры: ~%3г | Углеводы: ~%4г</span>").arg(day3Cal, 0, 'f', 0).arg(day3Prot, 0, 'f', 1).arg(day3Fats, 0, 'f', 1).arg(day3Carbs, 0, 'f', 1);
        menuText += "</div></div>";
    }
    
    menuText += "</div>";

    menuDisplay->setHtml(menuText);
    menuDisplay->verticalScrollBar()->setValue(0);
}

void MenuWindow::onStartTrackingClicked()
{
    emit startTracking();
}

