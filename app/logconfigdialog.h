#ifndef LOGCONFIGDIALOG_H
#define LOGCONFIGDIALOG_H

#include <QDialog>
#include "simpleloggeradp.h"

//**************************************************************************
/*! \class LogConfigDialog
 *  \brief Configure
 *
 * It seems that size() was removed from the QAbstractTableModel in QT 6.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2020
 *
 **************************************************************************/

class QLineEdit;
class QTableView;
class LogRoutingInfoTableModel;

class LogConfigDialog : public QDialog
{
  Q_OBJECT
public:
    //**************************************************************************
    /*! \brief Default constructor with the specified parent.
     *
     *  All objects owned by the parent are destroyed in the destructor.
     *  \param [in] parent The object's owner.
     ***************************************************************************/
  explicit LogConfigDialog(QWidget *parent = 0);

    //**************************************************************************
    /*! \brief Destructor in case I need any special clean-up.
     **************************************************************************/
  ~LogConfigDialog();

  void configureDialog(const SimpleLoggerADP& logger);
  void configureLogger(SimpleLoggerADP& logger);

  void setConfigFilePath(const QString& path);
  QString getConfigFilePath() const;

  void setLogFilePath(const QString& path);
  QString getLogFilePath() const;

  bool isRoutingSelected() const;
  int getSelectedRouting() const;

signals:
  
public slots:
  void editSelectedRouting();
  void delSelectedRouting();
  void copyRouting();
  void addRouting();
  void loadLogConfiguration();
  void saveLogConfiguration();
  void selectLogFile();

private:
  void buildDialog();

  QLineEdit* m_configFilePath;
  QLineEdit* m_logFilePath;
  QTableView* m_routingTableView;
  LogRoutingInfoTableModel* m_tableModel;
};

#endif // LOGCONFIGDIALOG_H
