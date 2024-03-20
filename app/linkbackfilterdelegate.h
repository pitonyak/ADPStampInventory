#ifndef LINKBACKFILTERDELEGATE_H
#define LINKBACKFILTERDELEGATE_H

#include <QStyledItemDelegate>

//**************************************************************************
/*! \class LinkBackFilterDelegate
 *  \brief Delegate for the dialog for editing filters.
 *
 *  As the data grid is edited, this object is called.
 *  This delegate is associated to the filters table view in the BackupSetDialog.
 *  The data is stored in a FilterTableModel.
 *
 *  Other edit things were found that can use this with good effect, so this is used
 *  in other places as well; for example, editing a log router information object.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2014
 ***************************************************************************/
class LinkBackFilterDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  explicit LinkBackFilterDelegate(QObject *parent = 0);

  //**************************************************************************
  /*! \brief Returns the widget used to edit the item specified by index for editing.
   *  \param [in, out] parent Used to control how the editor widget appears.
   *  \param [in] option Used to control how the editor widget appears.
   *  \param [in] index Indicates which column is of interest.
   *  \return Widget used to edit the specified data (so the editor is specific to the column).
   ***************************************************************************/
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    //**************************************************************************
    /*! \brief Sets the data to be displayed and edited by the editor from the data model item specified by the model index.
     *
     *  \param [in, out] editor will contain the data on exit.
     *  \param [in] index is the row and column of interest.
     ***************************************************************************/
    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    //**************************************************************************
    /*! \brief Gets data from the editor widget and stores it in the specified model at the item index.
     *
     *  \param [in, out] editor from which the data is taken.
     *  \param [in] model contains the data that is displayed.
     *  \param [in] index is the row and column of interest.
     ***************************************************************************/
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    //**************************************************************************
    /*! \brief Format the value for display purposes.
     *
     * The default implementaiotn displays dates with a two digit year, which was hiding incorrect
     * dates; for example, while entering "96" for 1996, this was entered as 2096. Also, old data
     * had been exported from the original data set as a two digit year (I am guessing) and then imported
     * into another data set that became the data used by this program. Dates in the 1800's particularly
     * had a problem on import.
     *
     * Date, Time, and DateTime objects are formatted based on the format strings contained in this
     * class if those strings have length greater than zero. If the format string is empty, then
     * the base class implmentation is used.
     *
     *  \param [in] value Value to display when not in edit mode.
     *  \param [in] locale Locale to use when displaying this.
     *  \return Formatted string used for display purposes.
     ***************************************************************************/
    virtual QString displayText(const QVariant & value, const QLocale & locale ) const;

    /*! \brief If not empty, this string is used to format Date, otherwise, the default implementaiton is used */
    QString getDateFormatString() const { return m_dateFormatString; }

    /*! \brief If not empty, this string is used to format Time, otherwise, the default implementaiton is used */
    QString getTimeFormatString() const { return m_timeFormatString; }

    /*! \brief If not empty, this string is used to format DateTime, otherwise, the default implementaiton is used */
    QString getDateTimeFormatString() const { return m_dateTimeFormatString; }

    void setDateFormatString(const QString& formatString) { m_dateFormatString = formatString; }
    void setTimeFormatString(const QString& formatString) { m_timeFormatString = formatString; }
    void setDateTimeFormatString(const QString& formatString) { m_dateTimeFormatString = formatString; }

    /*! \brief If not empty, this string is used to format Boolean true, otherwise, the default implementaiton is used */
    QString getBoolTrue() const { return m_boolTrue; }

    /*! \brief If not empty, this string is used to format Boolean false, otherwise, the default implementaiton is used */
    QString getBoolFalse() const { return m_boolFalse; }
    void setBoolTrue(const QString& displayValue) { m_boolTrue = displayValue; }
    void setBoolFalse(const QString& displayValue) { m_boolFalse = displayValue; }


signals:

public slots:

private:
    QString m_dateFormatString;
    QString m_timeFormatString;
    QString m_dateTimeFormatString;

    QString m_boolTrue;
    QString m_boolFalse;


};

#endif // LINKBACKFILTERDELEGATE_H
