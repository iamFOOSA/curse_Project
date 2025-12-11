#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QWidget>

class QTextEdit;
class QPushButton;
class Manager;
class User;

class MenuWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MenuWindow(Manager *manager, User *user, QWidget *parent = nullptr);
    void displayMenu();

    signals:
            void startTracking();

private slots:
            void onStartTrackingClicked();

private:
    Manager *manager;
    User *user;
    QTextEdit *menuDisplay;
    QPushButton *startTrackingButton;
};

#endif
