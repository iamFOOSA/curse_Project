#include "../headers/mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QStyleFactory>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(25, 25, 35));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(35, 35, 45));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 55));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(55, 55, 65));
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(65, 65, 85));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(148, 87, 235));
    darkPalette.setColor(QPalette::Highlight, QColor(148, 87, 235));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    QApplication::setPalette(darkPalette);

    a.setStyleSheet(R"(
        QToolTip {
            background-color: #373747;
            color: white;
            border: 1px solid #9457eb;
            border-radius: 5px;
            padding: 5px;
        }

        QMessageBox {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a1a2e, stop:0.5 #16213e, stop:1 #0f3460);
            color: white;
        }

        QMessageBox QLabel {
            color: white;
        }
    )");

    QApplication::setApplicationName(" Дневник питания");
    QApplication::setApplicationVersion("2.0");
    QApplication::setOrganizationName("HealthTech");

    MainWindow w;
    w.show();

    return QApplication::exec();
}
