#ifndef TABLESORTFIELDDIALOG_H
#define TABLESORTFIELDDIALOG_H

#include <QDialog>

#include "tablesortfieldtablemodel.h"
#include "genericdatacollection.h"
#include "typemapper.h"

class QTableView;
class QLineEdit;

//**************************************************************************
/*! \class TableSortFieldDialog
 *
 * \brief Edit a collection of sort fields.
 *
 * The sort fields are based on a table so that the column names are known.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2014-2014
 ***************************************************************************/
class TableSortFieldDialog : public QDialog
{
  Q_OBJECT
public:
  //**************************************************************************
  //! Default Constructor
  /*!
   * \param [in] dataCollection Defines the valid columns and types for sorting.
   * \param [in] parent This is a QObject, so you can set a parent that will control the lifetime of this object.
   ***************************************************************************/
  explicit TableSortFieldDialog(const GenericDataCollection* dataCollection, QWidget *parent = nullptr);

  /*! \brief Destructor saves dialog geometry. */
  virtual ~TableSortFieldDialog();

  void setConfigFilePath(const QString& path);
  QString getConfigFilePath() const;

  int getSelectedRoW() const;


signals:

public slots:
  void delSelectedRow();
  void copyRow();
  void addRow();
  void loadConfiguration();
  void saveConfiguration();

private:
  /*! \brief Set list columns, delegates, and initial values. */
  void initialize();
  void buildDialog();

  /*! \brief Get the index of the current row. */
  int getSelectedRow() const;

  /*! \brief Enable/disable buttons based on dialog values. */
  void enableButtons();

  /*! \brief Return True if a row is currently selected. */
  bool isRowSelected() const;

  /*! \brief Identifies the columns and the types. */
  const GenericDataCollection* m_dataCollection;
  QTableView* m_tableView;
  QLineEdit* m_configFilePath;
  TableSortFieldTableModel* m_tableModel;
  TypeMapper m_mapper;
};

#endif // TABLESORTFIELDDIALOG_H
