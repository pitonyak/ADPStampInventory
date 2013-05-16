#ifndef CSVLINE_H
#define CSVLINE_H

#include <QObject>
#include <QStringList>
#include "csvcolumn.h"

//**************************************************************************
//! Represent a single row.
/*!
 *
 **************************************************************************/

class CSVLine : public QObject
{
    Q_OBJECT
public:
    //**************************************************************************
    //! Constructor
    /*!
     * \param parent The object's owner. The parent's destructor destroys all child objects.
     *
     ***************************************************************************/
    explicit CSVLine(QObject *parent = nullptr);

    //**************************************************************************
    //! Constructor
    /*!
     * \param x Object to copy into the newly constructed object.
     * \param parent The object's owner. The parent's destructor destroys all child objects.
     *
     ***************************************************************************/
    explicit CSVLine(const CSVLine& x, QObject *parent = nullptr);

    //**************************************************************************
    //! Constructor
    /*!
     * \param originalLine Unparsed line that was read.
     * \param parent The object's owner. The parent's destructor destroys all child objects.
     *
     ***************************************************************************/
    explicit CSVLine(const QString& originalLine, QObject *parent = nullptr);

    //**************************************************************************
    //! Virtual destructor... Cleanup any extra memory.
    /*!
     *
     ***************************************************************************/
    virtual ~CSVLine();

    //**************************************************************************
    //! Determine if the original line has been set.
    /*!
     * \returns True if there is an original to return.
     *
     ***************************************************************************/
    bool hasOriginalLine() const;

    //**************************************************************************
    //! Get the original line.
    /*!
     * \returns The original line, or an empty string if it was not set.
     *
     ***************************************************************************/
    QString getOrignalLine() const;

    //**************************************************************************
    //! Set the original line.
    /*!
     * \param originalLine Unparsed line that was read.
     *
     ***************************************************************************/
    void setOriginalLine(const QString& originalLine);

    //**************************************************************************
    //! Remove any original line.
    /*!
     *
     ***************************************************************************/
    void clearOriginalLine();

    //**************************************************************************
    //! Remove columns and original lines.
    /*!
     *
     ***************************************************************************/
    void clear();

    //**************************************************************************
    //! Remove columns.
    /*!
     *
     ***************************************************************************/
    void clearColumns();

    //**************************************************************************
    //!
    /*!
     * \returns Number of columns already added to this line.
     *
     ***************************************************************************/
    int count() const;

    //**************************************************************************
    //! Find the index for a column with the given value.
    /*!
     * \param value Value to search for.
     * \param from First location to search.
     * \returns Index that value is found, or -1 if not found.
     *
     ***************************************************************************/
    int indexOf(const QString& value, int from = 0) const;

    //**************************************************************************
    //! Get a specific object. If the index is out of range, an error occurs.
    /*!
     * \param index Which object to return.
     * \returns The object at the index.
     *
     ***************************************************************************/
    CSVColumn& operator[](const int index);

    //**************************************************************************
    //! Get a specific object. If the index is out of range, an error occurs.
    /*!
     * \param index Which object to return.
     * \returns The object at the index.
     *
     ***************************************************************************/
    const CSVColumn& operator[](const int index) const;

    //**************************************************************************
    //! Append a column object to the line.
    /*!
     * \param column
     *
     ***************************************************************************/
    void append(const CSVColumn& column);

    //**************************************************************************
    //! Assign the parameter to this object.
    /*!
     * \param x Object to copy from.
     * \returns A reference to this object.
     *
     ***************************************************************************/
    const CSVLine& operator=(const CSVLine& x);

    //**************************************************************************
    //! Assign the parameter to this object.
    /*!
     * \param x Object to copy from.
     * \returns A reference to this object.
     *
     ***************************************************************************/
    const CSVLine& copyFrom(const CSVLine& x);

    QString toString(bool brief=true) const;
    QStringList toStringList() const;

signals:

public slots:

private:
    QString* m_originalLine;
    QList<CSVColumn> m_columns;
};

inline bool CSVLine::hasOriginalLine() const
{
    return m_originalLine != nullptr;
}

inline QString CSVLine::getOrignalLine() const
{
    return hasOriginalLine() ? *m_originalLine : QString();
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
