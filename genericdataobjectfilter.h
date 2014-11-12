#ifndef GENERICDATAOBJECTFILTER_H
#define GENERICDATAOBJECTFILTER_H

#include <QObject>
#include <QVariant>
#include <QMetaType>


class QXmlStreamWriter;
class QXmlStreamReader;

//**************************************************************************
/*! \class GenericDataObjectFilter
 * \brief A filter based on a single comparison and field.
 *
 * So, it may say filter field "name" for contains "tom".
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2014-2014
 **************************************************************************/
class GenericDataObjectFilter : public QObject
{
    Q_OBJECT
    Q_ENUMS(CompareType)

public:

    //**************************************************************************
    /*! \brief Enumerate the supported comparisons such as Less and Less Equal.*/
    //**************************************************************************
    enum CompareType {Less, LessEqual, Equal, GreaterEqual, Greater, NotEqual, RegularExpression, FileSpec, Contains, RegExpPartial, RegExpFull, StartsWith, EndsWith};

    explicit GenericDataObjectFilter(QObject *parent = nullptr);

    //**************************************************************************
    /*! \brief Constructor copied from the specified filter object.
     *
     *  \param [in] filter This filter object is copied into this object.
     ***************************************************************************/
    GenericDataObjectFilter(const GenericDataObjectFilter& filter);

    //**************************************************************************
    /*! \brief Constructor copied from the specified filter object.
     *
     *  \param [in] filter This filter object is copied into this object.
     *  \param [in] parent The object's owner.
     ***************************************************************************/
    explicit GenericDataObjectFilter(const GenericDataObjectFilter& filter, QObject *parent);

    //**************************************************************************
    /*! \brief Destructor in case I need any special clean-up. */
    //**************************************************************************
    ~GenericDataObjectFilter();

    //**************************************************************************
    /*! \brief Copy anoter filter into this filter.
     *
     *  This object's parent object will not change.
     *  \param [in] filter Filter to copy into this one.
     *  \return Reference to this object.
     ***************************************************************************/
    GenericDataObjectFilter& operator=(const GenericDataObjectFilter& filter);

    //**************************************************************************
    /*! \brief Create a copy of this object that you then own and must delete.
     *
     *  \param [in] parent The cloned object's new owner; because that is not copied while cloning.
     *  \return Cloned copy of this object that you own.
     ***************************************************************************/
    GenericDataObjectFilter* clone(QObject *parent = 0) const;

    //**************************************************************************
    /*! \brief Determine if the fileInfo object passes the filter.
     *
     *  \param [in] fileInfo Information about the file to compare.
     *  \return True if the object passes the filter. It is the callers problem to then understand that the filter is a "reject" filter, which means that the file is rejected.
     ***************************************************************************/
    //??bool passes(const QFileInfo& fileInfo) const;

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
    const QString &getCompareField() const;
    QString getCompareField();

    void setCompareField(const QString& compareField);
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
    void setValueDefault(const QVariant::Type aType);
    void setValueDefault(const QMetaType::Type aType);

    void setValue(const qlonglong);
    void setValue(const QDate&);
    void setValue(const QTime&);
    void setValue(const QDateTime&);
    void setValue(const QString&);
    void setValue(const QRegExp&);

    QString getMainValueAsString() const;

    QXmlStreamWriter& operator<<(QXmlStreamWriter& writer) const;
    QXmlStreamReader& operator>>(QXmlStreamReader& reader);


private:
    QXmlStreamReader& readFilter(QXmlStreamReader& reader);
    void readInternals(QXmlStreamReader& reader, const QString& version);
    void createRegularExpression(const QVariant& value);

public slots:

private:
    bool compareValues(const qlonglong aSize) const;
    bool compareValues(const QTime& aTime) const;
    bool compareValues(const QDate& aDate) const;
    bool compareValues(const QDateTime& aDateTime) const;
    bool compareValues(const QString& filePortion) const;

    /*! \brief Empty the lists, but do not delete them. */
    void clearLists(bool deleteLists, bool createIfDoNotExist);
    void createLists();
    void createRegularExpressions();

    /*! \brief Enumerate the supported comparisons such as Less and Less Equal. */
    enum CompareType m_compareType;

    /*! \brief Field to compare. */
    QString m_compareField;

    Qt::CaseSensitivity m_caseSensitivity;
    bool m_invertFilterResult;
    bool m_filterMeansAccept;
    bool m_multiValued;

    /*! \brief Single value representation of this filter. */
    QVariant m_value;

    //QRegExp* m_regularExpression;
    QList<QVariant>* m_values;
    QList<QRegExp*>* m_expressions;

};

inline GenericDataObjectFilter::CompareType GenericDataObjectFilter::getCompareType() const
{
  return m_compareType;
}

inline void GenericDataObjectFilter::setCompareTypeDefault()
{
  setCompareType();
}

inline const QString &GenericDataObjectFilter::getCompareField() const
{
  return m_compareField;
}

inline QString GenericDataObjectFilter::getCompareField()
{
  return m_compareField;
}

inline Qt::CaseSensitivity GenericDataObjectFilter::getCaseSensitivity() const
{
  return m_caseSensitivity;
}

inline void GenericDataObjectFilter::setCaseSensitivityDefault()
{
  setCaseSensitivity();
}

inline bool GenericDataObjectFilter::isInvertFilterResult() const
{
  return m_invertFilterResult;
}

inline void GenericDataObjectFilter::setInvertFilterResultDefault()
{
  setInvertFilterResult();
}

inline bool GenericDataObjectFilter::isFilterMeansAccept() const
{
  return m_filterMeansAccept;
}

inline void GenericDataObjectFilter::setFilterMeansAcceptDefault()
{
  setFilterMeansAccept();
}

inline void GenericDataObjectFilter::setValue(const qint64 x)
{
  setValue(QVariant(x));
}

inline void GenericDataObjectFilter::setValue(const QDate& x)
{
  setValue(QVariant(x));
}

inline void GenericDataObjectFilter::setValue(const QTime& x)
{
  setValue(QVariant(x));
}

inline void GenericDataObjectFilter::setValue(const QDateTime& x)
{
  setValue(QVariant(x));
}

inline void GenericDataObjectFilter::setValue(const QString& x)
{
  setValue(QVariant(x));
}

inline void GenericDataObjectFilter::setValue(const QRegExp& x)
{
  setValue(QVariant(x));
}

inline QString GenericDataObjectFilter::getMainValueAsString() const
{
  if (m_value.canConvert(QVariant::String))
  {
    return m_value.toString();
  } else {
    return "";
  }
}

inline const QVariant& GenericDataObjectFilter::getValue() const
{
  return m_value;
}

inline QXmlStreamWriter& operator<<(QXmlStreamWriter& os, const GenericDataObjectFilter& filter)
{
    return filter.operator<<(os);
}

inline QXmlStreamReader& operator>>(QXmlStreamReader& is, GenericDataObjectFilter& filter)
{
    return filter.operator>>(is);
}


#endif // GENERICDATAOBJECTFILTER_H
