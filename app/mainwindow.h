#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//**************************************************************************
/*! \class MainWindow
 * \brief Main window that the user sees
 *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2024
 **************************************************************************/

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
    void addMissingBookValues();
    void configure();
    void createDB();
    void createSchema();
    void editTable();
    void exportCSV();
    void exportInventoryCSV();
    void findMissingImages();
    void getSchema();
    void openSQLWindow();
    void readCSV();
    void testing();

private:
    //void setupToolBar();

    //**************************************************************************
    /*! \brief Create the menu bar
     *
     ***************************************************************************/
    void setupMenuBar();

    //**************************************************************************
    /*! \brief Initialize and create the m_db variable; the interface to the database.
     *
     * if m_db is NOT null, it is not changed.
     * if m_db is NOT open, it is opened.
     * if the db fails to open, the user is allowed to select a different db and the process repeats.
     *
     *  \return True on success, False if could not create the worker.
     ***************************************************************************/
    bool createDBWorker();

    //**************************************************************************
    /*! \brief This currently does nothing.
     *
     ***************************************************************************/
    void initializeSettings();

    Ui::MainWindow *ui;
    StampDB* m_db;
};

#endif // MAINWINDOW_H
