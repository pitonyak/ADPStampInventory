#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class StampDB;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QMenu *mainWindowMenu;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void createDB();
    void createSchema();
    void getSchema();
    void readCSV();
    void exportCSV();
    void openSQLWindow();
    void configure();


private:
    //void setupToolBar();
    void setupMenuBar();
    bool createDBWorker();
    void initializeSettings();

    Ui::MainWindow *ui;
    StampDB* m_db;
};

#endif // MAINWINDOW_H
