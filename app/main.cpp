#include <QApplication>
#include "mainwindow.h"
#include "globals.h"
#include <QLoggingCategory>
#include <iostream>

QtEnumMapper enumMapper;

//
// I can write my own message handler, and then install it, but that seems very heavy handed.
// But this is vaguely how I would do it.
//

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Ignore the qt events but allow my events to still be printed.
    if (type == QtDebugMsg && context.category != nullptr) {
      QString catStr(context.category);
      if (catStr.startsWith("qt.")) {
        return;
      }
    }

    // consider these:
    //const char *file = context.file ? context.file : "";
    //const char *function = context.function ? context.function : "";

    // Write the date of recording
    std::cout << qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz "));
    // By type determine to what level belongs message
    switch (type)
    {
    case QtInfoMsg:     std::cout << "INF "; break;
    case QtDebugMsg:    std::cout << "DBG "; break;
    case QtWarningMsg:  std::cout << "WRN "; break;
    case QtCriticalMsg: std::cout << "CRT "; break;
    case QtFatalMsg:    std::cout << "FTL "; break;
    }
    // Write to the output category of the message and the message itself
    std::cout << context.category << ": " << qPrintable(msg) << std::endl;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // qInstallMessageHandler(messageHandler);

    // Fedora disables qDebug output at the moment
    // New configuration thing, so, re-enable it as follows (if you want).
    // But, you do NOT want all those mouse movement messages
    QLoggingCategory::setFilterRules("*.debug=true\n"
                                     "qt.gui.*=false\n"
                                     "qt.qpa.*=false\n"
                                     "qt.pointer*.debug=false\n"
                                     "qt.accessibility*.debug=false\n"
                                     "qt.text.layout*.debug=false\n"
                                     "qt.text*.debug=false\n"
                                     "qt.svg.timing=false\n"
                                     "qt.widgets.gestures*.debug=false");
// text.layout drawing

    QCoreApplication::setOrganizationDomain("pitonyak.org");
    QCoreApplication::setOrganizationName("Pitonyak");
    QCoreApplication::setApplicationName("Stamp Inventory ADP");
    QCoreApplication::setApplicationVersion("1.0.0");

    MainWindow w;
    w.show();

    return a.exec();
}

QtEnumMapper& getEnumMapper()
{
  return enumMapper;
}
