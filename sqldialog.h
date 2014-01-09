#ifndef SQLDIALOG_H
#define SQLDIALOG_H

#include <QDialog>

class QPlainTextEdit;
class QTableWidget;
class QStatusBar;
class StampDB;

//**************************************************************************
/*! \class SQLDialog
 * \brief Enter SQL commands in the text box, execute them, show results in a table. *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2014
 **************************************************************************/

class SQLDialog : public QDialog
{
  Q_OBJECT
public:
    /*! \brief Constructor. Restores geometry and editor text.
     *
     *  \param [in,out] StampDB Database against which queries are run.
     *
     *  \param [in,out] parent This is a QObject, so you can set a parent that will control the lifetime of this object.
     */
  explicit SQLDialog(StampDB& db, QWidget *parent = 0);

    /*! \brief Destructor. Saves dialog geometry and editor text. */
  virtual ~SQLDialog();

    /*! \brief Get all of the text in the text editor. Used to save the text when the dialog closes.
     *
     *  \return All of the text in the text editor.
     */
  QString getSqlText() const;

  /*! \brief Execute the current line (or the selected text) as SQL and show the result in the table.
   *
   *  The table is auto-adjusted to size to show all of the text.
   */
  void executeSql(const QString& sqlString);

signals:
  
public slots:
  void closeButtonPressed();
  void sqlButtonPressed();

private:
  StampDB& m_db;
  QPlainTextEdit* m_textEdit;
  QTableWidget* m_tableWidget;
  QStatusBar* m_statusBar;

};

#endif // SQLDIALOG_H
