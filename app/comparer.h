#ifndef COMPARER_H
#define COMPARER_H

#include "typemapper.h"

#include <QObject>
#include <QVariant>

//**************************************************************************
/*! \class Comparer
 * \brief Filter to decide if a value matches some criteria.
 * This object is used to compare two values. These values are either both of type QString or QVariant.
 * Two values are passed into this object, which then decides how these compare.
 * Flags and configuration does not control the comparisons, that is left to the object deciding what to do with the results;
 * for example, this class never checks to see if a value is multi-valued or if the result should be inverted, the class that calls this one does that.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2020
 ***************************************************************************/
class Comparer : public QObject
{
  Q_OBJECT
  Q_ENUMS(CompareType)
  Q_ENUMS(CompareFlags)
public:

  //**************************************************************************
  /*! \brief Enumerate the supported comparisons such as Less and Less Equal. */
  //**************************************************************************
  enum CompareType {Less, LessEqual, Equal, GreaterEqual, Greater, NotEqual, RegularExpression, FileSpec, Contains};

  //**************************************************************************
  /*! \brief Identify Filter behavior
   *
   *  InvertResult - If set, before returning True or False for this compare sequence,
   *  invert the result. This provides an easy method to say NOT (a OR b OR c)
   *
   *  MultiValued - If set, the single value is split and each
   *
   *  MatchingMeansReject - If set, a match means that the value is rejected by the filter.
   *
   **************************************************************************/
  enum FilterFlags {None=0, InvertResult=1, MultiValued=2, MatchMeansReject=4};

  explicit Comparer(QObject *parent = 0);

  static bool isEqual(const QString& x1, const QString& x2, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
  static bool isEqual(const QVariant& x1, const QVariant& x2, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

  static bool isLess(const QString& x1, const QString& x2, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
  static bool isLess(const QVariant& x1, const QVariant& x2, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

  static int compare(const QString& x1, const QString& x2, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
  static int compare(const QVariant& x1, const QVariant& x2, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

  //**************************************************************************
  /*! \brief Is the InvertResult flag set?
   *  \return Return true if the InvertResult flag is set, and false otherwise.
   ***************************************************************************/
  bool isInvertResult() const;

  //**************************************************************************
  /*! \brief Is the MultiValued flag set?
   *  \return Return true if the MultiValued flag is set, and false otherwise.
   ***************************************************************************/
  bool isMultiValued() const;

  //**************************************************************************
  /*! \brief Is the MatchMeansReject flag set?
   *  \return Return true if the MatchMeansReject flag is set, and false otherwise.
   ***************************************************************************/
  bool isMatchMeansReject() const;

  //**************************************************************************
  /*! \brief If set, anything that matches does not match, and vice-versa.
   *  \param [in] b If true, set the flag, if false, clear the flag.
   ***************************************************************************/
  void setInvertResult(const bool b=true);

  //**************************************************************************
  /*! \brief If set, the filter value is assumed to be a comma delimited set of values. Matching any one value causes a match.
   *
   *  This provides a quick way to say something like (name=bob or name=tom or name=fred).
   *
   *  \param [in] b If true, set the flag, if false, clear the flag.
   ***************************************************************************/
  void setMultiValued(const bool b=true);

  //**************************************************************************
  /*! \brief If Set, matching causes the value to be immediately rejected by the filter.
   *  \param [in] b If true, set the flag, if false, clear the flag.
   ***************************************************************************/
  void setMatchMeansReject(const bool b=true);

signals:

public slots:

private:
  FilterFlags m_flags;
  static TypeMapper mapper;
};


inline bool Comparer::isInvertResult() const
{
  return (m_flags & Comparer::InvertResult) == Comparer::InvertResult;
}

inline bool Comparer::isMultiValued() const
{
  return (m_flags & Comparer::MultiValued) == Comparer::MultiValued;
}

inline bool Comparer::isMatchMeansReject() const
{
  return (m_flags & Comparer::MatchMeansReject) == Comparer::MatchMeansReject;
}

#endif // COMPARER_H
