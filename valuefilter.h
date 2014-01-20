#ifndef VALUEFILTER_H
#define VALUEFILTER_H

#include <QObject>
#include <QVariant>
#include <QDate>
#include <QRegExp>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class QFileInfo;
class QRegExp;

//**************************************************************************
/*! \class ValueFilter
 * \brief Filter to decide if a specific file or directory matches some criteria.
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2013
 ***************************************************************************/

class ValueFilter : public QObject
{
  Q_OBJECT
  Q_ENUMS(CompareType)
  Q_ENUMS(CompareField)

  Q_PROPERTY(CompareType compareType READ getCompareType WRITE setCompareType NOTIFY compareTypeChanged RESET setCompareTypeDefault STORED true)
  Q_PROPERTY(CompareField compareField READ getCompareField WRITE setCompareField NOTIFY compareFieldChanged RESET setCompareFieldDefault STORED true)
  Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ getCaseSensitivity WRITE setCaseSensitivity NOTIFY caseSensitivityChanged RESET setCaseSensitivityDefault STORED true)
  Q_PROPERTY(bool invertFilterResult READ isInvertFilterResult WRITE setInvertFilterResult NOTIFY invertFilterResultChanged RESET setInvertFilterResultDefault STORED true)
  Q_PROPERTY(bool filterMeansAccept READ isFilterMeansAccept WRITE setFilterMeansAccept NOTIFY filterMeansAcceptChanged RESET setFilterMeansAcceptDefault STORED true)
  Q_PROPERTY(bool filterFiles READ isFilterFiles WRITE setFilterFiles NOTIFY filterFilesChanged RESET setFilterFilesDefault STORED true)
  Q_PROPERTY(bool filterDirs READ isFilterDirs WRITE setFilterDirs NOTIFY filterDirsChanged RESET setFilterDirsDefault STORED true)
  Q_PROPERTY(QVariant value READ getValue WRITE setValue NOTIFY valueChanged RESET setValueDefault STORED true)

public:
    //**************************************************************************
    /*! \brief Enumerate the supported comparisons such as Less and Less Equal.*/
    //**************************************************************************
    enum CompareType {Less, LessEqual, Equal, GreaterEqual, Greater, NotEqual, RegularExpression, FileSpec, Contains, RegExpPartial, RegExpFull};

    //**************************************************************************
    /*! \brief Enumerate the parts of a file that can be compared. */
    //**************************************************************************
    enum CompareField {Date, DateTime, FullPath, Name, Size, Time, PathOnly};

    //**************************************************************************
    /*! \brief Default constructor with the specified parent.
     *
     *  All objects owned by the parent are destroyed in the destructor.
     *  \param [in] parent The object's owner.
     ***************************************************************************/
    explicit ValueFilter(QObject *parent = nullptr);

    //**************************************************************************
    /*! \brief Constructor copied from the specified filter object.
     *
     *  \param [in] filter This filter object is copied into this object.
     ***************************************************************************/
    ValueFilter(const ValueFilter& filter);

    //**************************************************************************
    /*! \brief Constructor copied from the specified filter object.
     *
     *  \param [in] filter This filter object is copied into this object.
     *  \param [in] parent The object's owner.
     ***************************************************************************/
    explicit ValueFilter(const ValueFilter& filter, QObject *parent);

    //**************************************************************************
    /*! \brief Destructor in case I need any special clean-up. */
    //**************************************************************************
    ~ValueFilter();

    //**************************************************************************
    /*! \brief Copy anoter filter into this filter.
     *
     *  This object's parent object will not change.
     *  \param [in] filter Filter to copy into this one.
     *  \return Reference to this object.
     ***************************************************************************/
    ValueFilter& operator=(const ValueFilter& filter);

    //**************************************************************************
    /*! \brief Create a copy of this object that you then own and must delete.
     *
     *  \param [in] parent The cloned object's new owner; because that is not copied while cloning.
     *  \return Cloned copy of this object that you own.
     ***************************************************************************/
    ValueFilter* clone(QObject *parent = 0) const;

    //**************************************************************************
    /*! \brief Determine if the fileInfo object passes the filter.
     *
     *  \param [in] fileInfo Information about the file to compare.
     *  \return True if the object passes the filter. It is the callers problem to then understand that the filter is a "reject" filter, which means that the file is rejected.
     ***************************************************************************/
    bool passes(const QFileInfo& fileInfo) const;
    bool passes(const int value) const;
    //bool passes(const bool value) const;
    bool passes(const double value) const;
    bool passes(const qlonglong value) const;
    bool passes(const qulonglong value) const;
    bool passes(const QTime& value) const;
    bool passes(const QDate& value) const;
    bool passes(const QDateTime& value) const;
    bool passes(const QString& value) const;
    bool passes(const QChar& value) const;
    bool passes(const QVariant& value) const;
    bool passes(const QStringList& value) const;


    //**************************************************************************
    /*! \brief Determine if this filter is applicable to the fileInfo type. In other words, some filters apply to directories, and some to filenames.
     *
     *  Applicable ignores the inverted flag. Inverted only applies to the comparison.
     *  \param [in] fileInfo Information about the file to compare.
     *  \return True if the filter applies to files and QFileInfo represents a file, similarly for directories.
     ***************************************************************************/
    bool applicable(const QFileInfo& fileInfo) const;

    //**************************************************************************
    /*! \brief Get the compare type; equal, less than, greater than, etc...
     ***************************************************************************/
    CompareType getCompareType() const;
    void setCompareType(CompareType compareType=Equal);
    void setCompareTypeDefault();
    QString getCompareTypeAsString() const;

    //**************************************************************************
    /*! \brief Get the field that is compared; file name, full path, date, etc...
     ***************************************************************************/
    CompareField getCompareField() const;
    void setCompareField(CompareField compareField=Name);
    void setCompareFieldDefault();
    QString getCompareFieldAsString() const;

    //**************************************************************************
    /*! \brief Are comparisons case sensitive?
     ***************************************************************************/
    Qt::CaseSensitivity getCaseSensitivity() const;
    void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity=Qt::CaseInsensitive);
    void setCaseSensitivityDefault();

    //**************************************************************************
    /*! \brief Determine if the filter criteria is inverted before determining acceptance.
     *
     * Inverting a filter provides an easy way to expand the comparison criteria.
     * This allows me to say things such as the filter passes if a regular expression does not match.
     *
     *  \return True if the filter criteria is inverted before determining acceptance.
     ***************************************************************************/
    bool isInvertFilterResult() const;
    void setInvertFilterResult(bool invertFilterResult=false);
    void setInvertFilterResultDefault();

    //**************************************************************************
    /*! \brief Does passing the filter mean that the value should be accepted / backed up. If not, then passing means the file or directory is skipped.
     ***************************************************************************/
    bool isFilterMeansAccept() const;
    void setFilterMeansAccept(bool filterMeansAccept=true);
    void setFilterMeansAcceptDefault();

    //**************************************************************************
    /*! \brief Does this filter apply to directories?
     ***************************************************************************/
    bool isFilterDirs() const;
    void setFilterDirs(bool filterDirs=true);
    void setFilterDirsDefault();

    //**************************************************************************
    /*! \brief Does this filter apply to files?
     ***************************************************************************/
    bool isFilterFiles() const;
    void setFilterFiles(bool filterFiles=true);
    void setFilterFilesDefault();

    //**************************************************************************
    /*! \brief Is the "value" a comma delimited set of values where "passing" means match any one value.
     ***************************************************************************/
    bool isMultiValued() const;
    void setMultiValued(bool multiValued=false);
    void setMultiValuedDefault();

    //**************************************************************************
    /*! \brief Value against which the compare field is compared.
     ***************************************************************************/
    const QVariant& getValue() const;
    void setValue(const QVariant& value);
    void setValueDefault();

    //**************************************************************************
    /*! \brief Same as calling each set function with the default value.
     *
     * - Default Compare Type is Equal
     * - Default compare field is the name (which includes the file extension).
     * - Default for case comparisons is case insensitive compares.
     * - Default Filter acceptance is inverted is False
     * - Default Filter means accept is True
     * - Default Filter applies to directories is True
     * - Default Filter applies to files is True
     * - Default Multi-Valued is False
     ***************************************************************************/
    void setAllDefault();


    void setValue(const qlonglong);
    void setValue(const QDate&);
    void setValue(const QTime&);
    void setValue(const QDateTime&);
    void setValue(const QString&);
    void setValue(const QRegExp&);
    void setValue(const double);
    void setValue(const int);

    QString getMainValueAsString() const;

    QXmlStreamWriter& operator<<(QXmlStreamWriter& writer) const;
    QXmlStreamReader& operator>>(QXmlStreamReader& reader);


private:
    QXmlStreamReader& readFilter(QXmlStreamReader& reader);
    void readInternals(QXmlStreamReader& reader, const QString& version);
    void createRegularExpression(const QVariant& value);


signals:
    void compareTypeChanged(CompareType);
    void compareFieldChanged(CompareField);
    void caseSensitivityChanged(bool);
    void invertFilterResultChanged(bool);
    void filterMeansAcceptChanged(bool);
    void filterFilesChanged(bool);
    void filterDirsChanged(bool);
    void valueChanged(QVariant);
    void multiValuedChanged(bool);

public slots:

private:
    //**************************************************************************
    /*! \brief Inverts the result if the invert filter result flag is set.
     *
     *  \param [in] value
     *  \return Returns the value if the invert filter result flag is clear, and the inverse if it is set.
     ***************************************************************************/
    bool invertIfNeeded(const bool value) const;
    bool compareValues(const qlonglong value) const;
    bool compareValues(const qulonglong value) const;
    bool compareValues(const double value) const;
    bool compareValues(const QTime& aTime) const;
    bool compareValues(const QDate& aDate) const;
    bool compareValues(const QDateTime& aDateTime) const;
    bool compareValues(const QString& value) const;

    /*! \brief Empty the lists, but do not delete them. */
    void clearLists(bool deleteLists, bool createIfDoNotExist);
    void createLists();
    void createRegularExpressions();

    /*! \brief Enumerate the supported comparisons such as Less and Less Equal. */
    enum CompareType m_compareType;

    /*! \brief Enumerate the parts of a file that can be compared. */
    enum CompareField m_compareField;

    /*! \brief Should compares be done in a case sensitive way. */
    Qt::CaseSensitivity m_caseSensitivity;

    /*! \brief If true, the comparison is performed, and then the filter is inverted. This is useful to easily say 'NOT(a OR b OR c)'. */
    bool m_invertFilterResult;

    /*! \brief Extra aid for an evaluator to determine if passing means a particular behavior is performed. This could arguablly be considered a duplicate of the invert filter result flag. */
    bool m_filterMeansAccept;

    /*! \brief For use while filtering paths and directories, should the filter be applied to files. */
    bool m_filterFiles;

    /*! \brief For use while filtering paths and directories, should the filter be applied to directories. */
    bool m_filterDirs;

    /*! \brief If true, the "value" is treated as a comma delimited list of values. The filter is tested against each value until a match is found or there are no more values to check. */
    bool m_multiValued;

    /*! \brief Single value representation of this filter. */
    QVariant m_value;

    /*! \brief If the multi-valued flag is set, the value is split by commas and each value is stored here. */
    QList<QVariant>* m_values;

    /*! \brief If the value type is a regular expression, the regular expressions are stored here. */
    QList<QRegExp*>* m_expressions;
};


inline ValueFilter::CompareType ValueFilter::getCompareType() const
{
  return m_compareType;
}

inline void ValueFilter::setCompareTypeDefault()
{
  setCompareType();
}

inline ValueFilter::CompareField ValueFilter::getCompareField() const
{
  return m_compareField;
}

inline void ValueFilter::setCompareFieldDefault()
{
  setCompareField();
}

inline Qt::CaseSensitivity ValueFilter::getCaseSensitivity() const
{
  return m_caseSensitivity;
}

inline void ValueFilter::setCaseSensitivityDefault()
{
  setCaseSensitivity();
}

inline bool ValueFilter::isInvertFilterResult() const
{
  return m_invertFilterResult;
}

inline void ValueFilter::setInvertFilterResultDefault()
{
  setInvertFilterResult();
}

inline bool ValueFilter::isFilterMeansAccept() const
{
  return m_filterMeansAccept;
}

inline void ValueFilter::setFilterMeansAcceptDefault()
{
  setFilterMeansAccept();
}

inline void ValueFilter::setValue(const double x)
{
    setValue(QVariant(x));
}

inline void ValueFilter::setValue(const int x)
{
    setValue(QVariant(x));
}

inline void ValueFilter::setValue(const qint64 x)
{
  setValue(QVariant(x));
}

inline void ValueFilter::setValue(const QDate& x)
{
  setValue(QVariant(x));
}

inline void ValueFilter::setValue(const QTime& x)
{
  setValue(QVariant(x));
}

inline void ValueFilter::setValue(const QDateTime& x)
{
  setValue(QVariant(x));
}

inline void ValueFilter::setValue(const QString& x)
{
  setValue(QVariant(x));
}

inline void ValueFilter::setValue(const QRegExp& x)
{
  setValue(QVariant(x));
}

inline QString ValueFilter::getMainValueAsString() const
{
  if (m_value.canConvert(QVariant::String))
  {
    return m_value.toString();
  } else {
    return "";
  }
}

inline const QVariant& ValueFilter::getValue() const
{
  return m_value;
}

inline QXmlStreamWriter& operator<<(QXmlStreamWriter& os, const ValueFilter& filter)
{
    return filter.operator<<(os);
}

inline QXmlStreamReader& operator>>(QXmlStreamReader& is, ValueFilter& filter)
{
    return filter.operator>>(is);
}


inline bool ValueFilter::invertIfNeeded(const bool value) const
{
    return isInvertFilterResult() ? !value : value;
}

inline bool ValueFilter::passes(const int value) const
{
    return invertIfNeeded(compareValues((qlonglong)value));
}

inline bool ValueFilter::passes(const double value) const
{
    return invertIfNeeded(compareValues(value));
}

inline bool ValueFilter::passes(const qlonglong value) const
{
    return invertIfNeeded(compareValues(value));
}

inline bool ValueFilter::passes(const qulonglong value) const
{
    return invertIfNeeded(compareValues(value));
}

inline bool ValueFilter::passes(const QTime& value) const
{
    return invertIfNeeded(compareValues(value));
}

inline bool ValueFilter::passes(const QDate& value) const
{
    return invertIfNeeded(compareValues(value));
}

inline bool ValueFilter::passes(const QDateTime& value) const
{
    return invertIfNeeded(compareValues(value));
}

inline bool ValueFilter::passes(const QString& value) const
{
    return invertIfNeeded(compareValues(value));
}

inline bool ValueFilter::passes(const QChar& value) const
{
    return invertIfNeeded(compareValues(value));
}




#endif // VALUEFILTER_H
