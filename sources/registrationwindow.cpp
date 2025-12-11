#include "../headers/registrationwindow.h"
#include "../headers/user.h"
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFrame>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

RegistrationWindow::RegistrationWindow(User *user, QWidget *parent)
    : QWidget(parent), user(user)
{
    setStyleSheet(R"(
        RegistrationWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a1a2e, stop:0.5 #16213e, stop:1 #0f3460);
            font-family: 'Segoe UI', Arial, sans-serif;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    mainFrame = new QFrame;
    mainFrame->setStyleSheet(R"(
        QFrame {
            background: rgba(45, 45, 65, 0.95);
            border: 2px solid #9457eb;
            border-radius: 20px;
        }
    )");

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect;
    shadowEffect->setBlurRadius(25);
    shadowEffect->setColor(QColor(148, 87, 235, 120));
    shadowEffect->setOffset(0, 8);
    mainFrame->setGraphicsEffect(shadowEffect);

    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->setSpacing(25);
    frameLayout->setContentsMargins(40, 40, 40, 40);

    QLabel *titleLabel = new QLabel("👤 Создание профиля");
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 32px;
            font-weight: bold;
            color: white;
            padding: 10px;
            text-align: center;
            background: transparent;
        }
    )");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *subtitleLabel = new QLabel("Создайте персональный профиль для точных рекомендаций по питанию");
    subtitleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 14px;
            color: rgba(255, 255, 255, 0.8);
            padding: 5px;
            text-align: center;
            background: transparent;
        }
    )");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);

    QFrame *formContainer = new QFrame;
    formContainer->setStyleSheet("background: transparent; border: none;");

    QFormLayout *formLayout = new QFormLayout(formContainer);
    formLayout->setSpacing(20);
    formLayout->setContentsMargins(20, 20, 20, 20);
    formLayout->setVerticalSpacing(20);
    formLayout->setHorizontalSpacing(25);

    QString inputStyle = R"(
        QLineEdit {
            background: #2d2d3a;
            border: 2px solid #555566;
            border-radius: 10px;
            padding: 12px 15px;
            font-size: 14px;
            color: white;
            min-width: 250px;
            min-height: 20px;
        }
        QLineEdit:focus {
            border-color: #9457eb;
            background: #353545;
        }
        QLineEdit::placeholder {
            color: #888899;
        }
    )";

    QString labelStyle = R"(
        QLabel {
            color: white;
            font-size: 14px;
            font-weight: bold;
            padding: 5px;
            background: transparent;
        }
    )";

    QString comboBoxStyle = R"(
        QComboBox {
            background: #2d2d3a;
            border: 2px solid #555566;
            border-radius: 10px;
            padding: 12px 15px;
            font-size: 14px;
            color: white;
            min-width: 250px;
        }
        QComboBox:focus {
            border-color: #9457eb;
            background: #353545;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid white;
        }
        QComboBox QAbstractItemView {
            background: #2d2d3a;
            border: 1px solid #555566;
            color: white;
            selection-background-color: #9457eb;
        }
    )";

    QLabel *nameLabel = new QLabel(" Имя:");
    nameLabel->setStyleSheet(labelStyle);
    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Введите ваше имя");
    nameEdit->setStyleSheet(inputStyle);
    formLayout->addRow(nameLabel, nameEdit);

    QLabel *ageLabel = new QLabel(" Возраст:");
    ageLabel->setStyleSheet(labelStyle);
    ageEdit = new QLineEdit;
    ageEdit->setPlaceholderText("Ваш возраст в годах");
    ageEdit->setStyleSheet(inputStyle);
    ageEdit->setValidator(new QIntValidator(1, 120, this));
    formLayout->addRow(ageLabel, ageEdit);

    QLabel *heightLabel = new QLabel(" Рост (см):");
    heightLabel->setStyleSheet(labelStyle);
    heightEdit = new QLineEdit;
    heightEdit->setPlaceholderText("Рост в сантиметрах");
    heightEdit->setStyleSheet(inputStyle);
    heightEdit->setValidator(new QIntValidator(50, 250, this));
    formLayout->addRow(heightLabel, heightEdit);

    QLabel *weightLabel = new QLabel(" Вес (кг):");
    weightLabel->setStyleSheet(labelStyle);
    weightEdit = new QLineEdit;
    weightEdit->setPlaceholderText("Вес в килограммах");
    weightEdit->setStyleSheet(inputStyle);
    weightEdit->setValidator(new QDoubleValidator(20, 300, 1, this));
    formLayout->addRow(weightLabel, weightEdit);

    QLabel *goalLabel = new QLabel(" Цель:");
    goalLabel->setStyleSheet(labelStyle);
    goalComboBox = new QComboBox;
    goalComboBox->addItem(" Похудение");
    goalComboBox->addItem(" Набор массы");
    goalComboBox->addItem(" Поддержание веса");
    goalComboBox->setStyleSheet(comboBoxStyle);
    goalComboBox->setCurrentIndex(2);
    formLayout->addRow(goalLabel, goalComboBox);

    registerButton = new QPushButton(" Начать!");
    registerButton->setStyleSheet(R"(
        QPushButton {
            background: rgba(255, 255, 255, 0.15);
            color: white;
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 10px;
            padding: 15px 30px;
            font-size: 16px;
            font-weight: bold;
            margin-top: 10px;
            min-height: 25px;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 0.25);
            border-color: rgba(255, 255, 255, 0.4);
        }
        QPushButton:pressed {
            background: rgba(255, 255, 255, 0.1);
        }
    )");


    /* QPushButton *autoFillButton = new QPushButton(" Автозаполнение ");
    autoFillButton->setStyleSheet(R"(
        QPushButton {
            background: #8e44ad;
            color: white;
            border: none;
            border-radius: 10px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
            margin-top: 5px;
        }
        QPushButton:hover {
            background: #7d3c98;
        }
    )");*/

    frameLayout->addWidget(titleLabel);
    frameLayout->addWidget(subtitleLabel);
    frameLayout->addWidget(formContainer);
    frameLayout->addWidget(registerButton);
    frameLayout->setAlignment(registerButton, Qt::AlignCenter);

    mainLayout->addWidget(mainFrame);

    connect(nameEdit, &QLineEdit::returnPressed, this, &RegistrationWindow::onNameEnterPressed);
    connect(ageEdit, &QLineEdit::returnPressed, this, &RegistrationWindow::onAgeEnterPressed);
    connect(heightEdit, &QLineEdit::returnPressed, this, &RegistrationWindow::onHeightEnterPressed);
    connect(weightEdit, &QLineEdit::returnPressed, this, &RegistrationWindow::onWeightEnterPressed);
    connect(registerButton, &QPushButton::clicked, this, &RegistrationWindow::registerUser);

    nameEdit->setFocus();
}

void RegistrationWindow::onNameEnterPressed()
{
    ageEdit->setFocus();
    ageEdit->selectAll();
}

void RegistrationWindow::onAgeEnterPressed()
{
    heightEdit->setFocus();
    heightEdit->selectAll();
}

void RegistrationWindow::onHeightEnterPressed()
{
    weightEdit->setFocus();
    weightEdit->selectAll();
}

void RegistrationWindow::onWeightEnterPressed()
{
    goalComboBox->setFocus();
    goalComboBox->showPopup();
}

void RegistrationWindow::registerUser()
{
    if (nameEdit->text().isEmpty() || ageEdit->text().isEmpty() ||
        heightEdit->text().isEmpty() || weightEdit->text().isEmpty()) {

        QMessageBox::warning(this, "Неполные данные",
                             "Пожалуйста, заполните все поля!\n\n"
                             "• Имя\n• Возраст\n• Рост\n• Вес");
        return;
    }

    bool ageOk, heightOk, weightOk;
    int age = ageEdit->text().toInt(&ageOk);
    double height = heightEdit->text().toDouble(&heightOk);
    double weight = weightEdit->text().toDouble(&weightOk);

    if (!ageOk || age <= 0 || age > 120) {
        showError("Возраст", "Пожалуйста, введите корректный возраст от 1 до 120 лет!");
        ageEdit->setFocus();
        ageEdit->selectAll();
        return;
    }

    if (!heightOk || height <= 0 || height > 250) {
        showError("Рост", "Пожалуйста, введите корректный рост от 50 до 250 см!");
        heightEdit->setFocus();
        heightEdit->selectAll();
        return;
    }

    if (!weightOk || weight <= 0 || weight > 300) {
        showError("Вес", "Пожалуйста, введите корректный вес от 20 до 300 кг!");
        weightEdit->setFocus();
        weightEdit->selectAll();
        return;
    }

    QString goal;
    switch(goalComboBox->currentIndex()) {
    case 0: goal = "похудение"; break;
    case 1: goal = "набор"; break;
    case 2: goal = "поддержание"; break;
    default: goal = "поддержание";
    }

    *user = User(nameEdit->text().toStdString(),
                 age,
                 height,
                 weight,
                 goal.toStdString());

    emit registrationCompleted();
}

void RegistrationWindow::showError(const QString &field, const QString &message)
{
    QMessageBox::critical(this, "Ошибка в поле: " + field, message);
}

void RegistrationWindow::autoFillRegistration()
{
    // Автоматически заполняем поля тестовыми данными
    nameEdit->setText("даня");
    ageEdit->setText("19");
    heightEdit->setText("179");
    weightEdit->setText("79");
    goalComboBox->setCurrentIndex(2); // Поддержание веса

}


