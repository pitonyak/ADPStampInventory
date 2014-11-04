#ifndef CSVLINE_H
#define CSVLINE_H

#include <QObject>
#include <QStringList>
#include "csvcolumn.h"

//**************************************************************************
/*! \class CSVLine
 * \brief Represent a single row from a CSV file. Contains the original line as well as the parsed columns.
 *
 **************************************************************************/

class CSVLine : public QObject
{
  Q_OBJECT
public:
  //**************************************************************************
  /*! \brief Constructor
   *
   * \param [in, out] parent The object's owner. The parent's destructor destroys all child objects.
   *
   ***************************************************************************/
  explicit CSVLine(QObject *parent = nullptr);

  //**************************************************************************
  /*! \brief Constructor
   *
   * \param [in] x Object to copy into the newly constructed object.
   * \param [in, out] parent The object's owner. The parent's destructor destroys all child objects.
   *
   ***************************************************************************/
  explicit CSVLine(const CSVLine& x, QObject *parent = nullptr);

  //**************************************************************************
  /*! \brief Constructor
   *
   * \param [in] originalLine Unparsed line that was read.
   * \param [in, out] parent The object's owner. The parent's destructor destroys all child objects.
   *
   ***************************************************************************/
  explicit CSVLine(const QString& originalLine, QObject *parent = nullptr);

  //**************************************************************************
  /*! \brief Virtual destructor... Cleanup any extra memory.
   *
   *
   ***************************************************************************/
  virtual ~CSVLine();

  //**************************************************************************
  /*! \brief Determine if the original line has been set.
   *
   * \returns True if there is an original to return.
   *
   ***************************************************************************/
  bool containsOriginalLine() const;

  //**************************************************************************
  /*! \brief Get the original line.
   *
   * \returns The original line, or an empty string if it was not set.
   *
   ***************************************************************************/
  QString getOrignalLine() const;

  //**************************************************************************
  /*! \brief Set the original line.
   *
   * \param [in] originalLine Unparsed line that was read.
   *
   ***************************************************************************/
  void setOriginalLine(const QString& originalLine);

  //**************************************************************************
  /*! \brief Remove any original line.
   *
   *
   ***************************************************************************/
  void clearOriginalLine();

  //**************************************************************************
  /*! \brief Remove columns and original lines.
   *
   *
   ***************************************************************************/
  void clear();

  //**************************************************************************
  /*! \brief Remove columns.
   *
   *
   ***************************************************************************/
  void clearColumns();

  //**************************************************************************
  /*!
   *
   * \returns Number of columns already added to this line.
   *
   ***************************************************************************/
  int count() const;

  //**************************************************************************
  /*! \brief Find the index for a column with the given value.
   *
   * \param [in] value Value to search for.
   * \param [in] from First location to search.
   * \param [in] cs Case sensitivity flag, is the search case sensitive.
   * \returns Index that value is found, or -1 if not found.
   *
   ***************************************************************************/
  int indexOf(const QString& value, const int from = 0, const Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

  //**************************************************************************
  /*! \brief Get a specific object. If the index is out of range, an error occurs.
   *
   * \param [in] index Which object to return.
   * \returns The object at the index.
   *
   ***************************************************************************/
  CSVColumn& operator[](const int index);

  //**************************************************************************
  /*! \brief Get a specific object. If the index is out of range, an error occurs.
   *
   * \param [in] index Which object to return.
   * \returns The object at the index.
   *
   ***************************************************************************/
  const CSVColumn& operator[](const int index) const;

  //**************************************************************************
  /*! \brief Append a column object to the line.
   *
   * \param [in] column
   *
   ***************************************************************************/
  void append(const CSVColumn& column);

  //**************************************************************************
  /*! \brief Assign the parameter to this object.
   *
   * \param [in] x Object to copy from.
   * \returns A reference to this object.
   *
   ***************************************************************************/
  const CSVLine& operator=(const CSVLine& x);

  //**************************************************************************
  /*! \brief Assign the parameter to this object.
   *
   * \param [in] x Object to copy from.
   * \returns A reference to this object.
   *
   ***************************************************************************/
  const CSVLine& copyFrom(const CSVLine& x);

  //**************************************************************************
  /*! \brief Return the line as a "comma" delimited set of values.
   *
   * \param [in] brief When True, the value is printed (strings have delimiters).
   *                   When False, includes type and value.
   *
   * \returns Line as a delimited string.
   *
   ***************************************************************************/
  QString toString(const bool brief=true) const;

  //**************************************************************************
  /*! \brief Return the colunmns as a list of strings.
   *
   * \returns Column values as a list of strings.
   *
   ***************************************************************************/
  QStringList toStringList() const;

signals:

public slots:

private:
  QString* m_originalLine;
  QList<CSVColumn> m_columns;
};

inline bool CSVLine::containsOriginalLine() const
{
  return m_originalLine != nullptr;
}

inline QString CSVLine::getOrignalLine() const
{
  return containsOriginalLine() ? *m_originalLine : QString();
}


inline CSVColumn& CSVLine::operator[](const int index)
{
  return m_columns[index];
}

inline const CSVColumn& CSVLine::operator[](const int index) const
{
  return m_columns[index];
}

inline void CSVLine::append(const CSVColumn& column)
{
  m_columns.append(column);
}

inline void CSVLine::clear()
{
  clearColumns();
  clearOriginalLine();
}

inline void CSVLine::clearColumns()
{
  m_columns.clear();
}


#endif // CSVLINE_H
