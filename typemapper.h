#ifndef TYPEMAPPER_H
#define TYPEMAPPER_H

#include <QObject>
#include <QMetaType>
#include <QMap>
#include <QVariant>

class TypeMapper : public QObject
{
    Q_OBJECT
public:

  /*! \brief
   *
   *
   *
   *  \param [in]
   *  \return
   */
    explicit TypeMapper(QObject *parent = 0);

  /*! \brief
   *
   *
   *
   *  \param [in]
   *  \return
   */
    QVariant::Type metaToVariantType(QMetaType::Type metaType) const;

    /*! \brief
     *
     *
     *
     *  \param [in]
     *  \return
     */
    bool hasMetaTypeEnry(QMetaType::Type metaType) const;

    /*! \brief
     *
     *
     *
     *  \param [in]
     *  \return
     */
    QMetaType::Type guessType(const QString& s);

    QMetaType::Type mostGenericType(const QMetaType::Type metaType1, const QMetaType::Type metaType2) const;

signals:

public slots:

private:

    /*! \brief
     *
     *
     *
     *  \param [in]
     *  \return
     */
    void initialize();
    QMap<QMetaType::Type, QVariant::Type> m_typeMap;
    QList<QMetaType::Type> m_NumList;
    QList<QMetaType::Type> m_UNumList;
};

inline QVariant::Type TypeMapper::metaToVariantType(QMetaType::Type metaType) const
{
    return hasMetaTypeEnry(metaType) ? m_typeMap[metaType] : QVariant::Invalid;
}

inline bool TypeMapper::hasMetaTypeEnry(QMetaType::Type metaType) const
{
    return m_typeMap.contains(metaType);
}

#endif // TYPEMAPPER_H
