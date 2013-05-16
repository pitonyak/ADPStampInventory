#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationDomain("pitonyak.org");
    QCoreApplication::setOrganizationName("Pitonyak");
    QCoreApplication::setApplicationName("Stamp Inventory ADP");
    QCoreApplication::setApplicationVersion("1.0.0");

    MainWindow w;
    w.show();

    return a.exec();
}
