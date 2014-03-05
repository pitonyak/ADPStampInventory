#ifndef CSVREADERDIALOG_H
#define CSVREADERDIALOG_H

#include <QDialog>
#include <QString>
#include <QMap>

class QGroupBox;
class QVBoxLayout;
class QComboBox;
class QSpinBox;
class QCheckBox;
class QCheckBox;
class QLineEdit;
class QRadioButton;
class QLabel;
class QTableWidget;
class CSVReader;

//**************************************************************************
/*! \class CSVReaderDialog
 *  \brief Configure how a CSV file should be read.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2013
 *
 **************************************************************************/

class CSVReaderDialog : public QDialog
{
  Q_OBJECT
public:

  /*! \brief Constructor. Dialog geometry is set from the settings to the last size used.
   *
   *  \param [in,out] reader Dialog uses this to set CSV parameters.
   *  \param [in,out] parent Objects owner.
   */
  explicit CSVReaderDialog(CSVReader* reader = nullptr, QWidget* parent = nullptr);

  /*! \brief Destructor saves dialog geometry. */
  virtual ~CSVReaderDialog();

  /*! \brief Configure the table view based on the CSV reqader.
   *
   * The vBoxLayout is not null the table widget is added to it,
   * so, it must not be null the first time it is called, but
   * should be called each time afterwards.
   *
   *  \param [in,out] vBoxLayout Will contain the table widget.
   */
  void configureFromReader(QVBoxLayout* vBoxLayout = nullptr);

signals:

public slots:
  void separatorToggled(bool enabled);

  void tableCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
  void tableSelectionChanged();
  void columnTypeChanged(const QString& text);


private:
  void buildDialog();
  void createImportGroupBox();
  void createSeparatorOptionsGroupBox();
  void creatFieldsGroupBox();
  int getCurrentColumn() const;

  QGroupBox* m_importGroupBox;
  QGroupBox* m_separatorOptionsGroupBox;
  QGroupBox* m_fieldsGroupBox;
  QComboBox* m_characterSet;
  QSpinBox* m_firstRow;
  QMap<QString, QCheckBox*>* m_separatedMap;
  QCheckBox* m_otherSeparatorBox;
  QLineEdit* m_otherSeparator;
  QCheckBox* m_mergeDelimiters;
  QComboBox* m_textDelimiter;
  QRadioButton* m_fixedWidthRB;
  QRadioButton* m_separatedByRB;
  QLabel* m_textDelimiterLabel;
  QComboBox* m_columnType;
  QLabel* m_columnTypeLabel;
  QTableWidget* m_tableWidget;
  CSVReader* m_reader;
  QMap<QMetaType::Type, QStringList> m_typeToTxt;
  QMap<QString, QList<QMetaType::Type> > m_txtToType;
  QStringList m_typeNames;
  QList<QMetaType::Type> m_FieldType;

};

#endif // CSVREADERDIALOG_H
