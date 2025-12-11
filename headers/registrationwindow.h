#ifndef REGISTRATIONWINDOW_H
#define REGISTRATIONWINDOW_H

#include <QWidget>

class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;
class QFrame;
class User;

class RegistrationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationWindow(User *user, QWidget *parent = nullptr);

signals:
    void registrationCompleted();

private slots:
    void registerUser();
    void onNameEnterPressed();
    void onAgeEnterPressed();
    void onHeightEnterPressed();
    void onWeightEnterPressed();

     void autoFillRegistration();

private:
    User *user;
    QLineEdit *nameEdit;
    QLineEdit *ageEdit;
    QLineEdit *heightEdit;
    QLineEdit *weightEdit;
    QComboBox *goalComboBox;
    QPushButton *registerButton;
    QFrame *mainFrame;

    void showError(const QString &field, const QString &message);
};

#endif
