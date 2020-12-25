#ifndef VARIANTCOMPARER_H
#define VARIANTCOMPARER_H

#include <QObject>
#include <QVariant>

//**************************************************************************
/*! \class VariantComparer
 * \brief Use to compare two variant variables.
 *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2014-2020
 ***************************************************************************/
class VariantComparer : public QObject
{
  Q_OBJECT
  Q_ENUMS(CompareType)

public:
  //**************************************************************************
  /*! \brief Enumerate the supported comparisons such as Less and Less Equal.*/
  //**************************************************************************
  enum CompareType {Less, LessEqual, Equal, GreaterEqual, Greater, NotEqual, RegularExpression, FileSpec, Contains, RegExpPartial, RegExpFull, StartsWith, EndsWith};

  explicit VariantComparer(QObject *parent = nullptr);

  //**************************************************************************
  /*! \brief Convert a variant to a string. The primary use is to format times and dates as desired.
   *
   *  \param [in] v Variant to convert.
   *  \return String representation of the variant.
   ***************************************************************************/
  static QString variantToString(const QVariant& v);

  //**************************************************************************
  /*! \brief Compare two variants allow v1 and v2 to be different types.
   *
   *  \param [in] v1
   *  \param [in] v2
   *  \param [in] cs Case sensitivity
   *  \return -1 if v1 < v2, 0 if v1==v2 and 1 if v1 > v2
   ***************************************************************************/
  static int compare(const QVariant& v1, const QVariant& v2, const Qt::CaseSensitivity cs = Qt::CaseInsensitive);

  //**************************************************************************
  /*! \brief Compare two variants using the type assocaited with v1 assuming that both are the same type.
   *
   *  \param [in] v1
   *  \param [in] v2
   *  \param [in] cs Case sensitivity
   *  \return -1 if v1 < v2, 0 if v1==v2 and 1 if v1 > v2
   ***************************************************************************/
  static int compareSameType(const QVariant& v1, const QVariant& v2, const Qt::CaseSensitivity cs = Qt::CaseInsensitive);

  //**************************************************************************
  /*! \brief Compares v1 and v2 (or v1 and the regular expression) based on the compare type.
   *
   *  \param [in] v1
   *  \param [in] v2
   *  \param [in] compareType Compare type to use
   *  \param [in] cs Case sensitivity
   *  \param [in] expression Regular expression for an appropriate compare type.
   *  \return True if there is a match based on the compare type, false otherwise.
   ***************************************************************************/
  static bool matches(const QVariant& v1, const QVariant& v2, const CompareType compareType, const Qt::CaseSensitivity cs = Qt::CaseInsensitive, const QRegularExpression* expression=nullptr);

signals:

public slots:

};

#endif // VARIANTCOMPARER_H
