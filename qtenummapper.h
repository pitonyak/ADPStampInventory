#ifndef QTENUMMAPPER_H
#define QTENUMMAPPER_H

#include <QMap>
#include <QString>

#include <QRegExp>
#include <QList>

//**************************************************************************
/*! \class QtEnumMapper
 * \brief Provide a fast method to convert QT Enums to/from strings.
 *
 * Many of QT's enums are not set to include meta-data so I require custom methods to obtain this information.
 * I became tired of writing the same code over and over, so I created this class.
 *
 * Efficiency was ignored while designing this class, assuming that it will be used in low-volume situations.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2014
 ***************************************************************************/
class QtEnumMapper
{
public:
    //**************************************************************************
    /*! \brief Default constructor. Initialize constants.
     ***************************************************************************/
  QtEnumMapper();

  //**************************************************************************
  /*! \brief Destructor
   ***************************************************************************/
  ~QtEnumMapper();

  //**************************************************************************
  /*! \brief Get a list of the enum names for a specific enum type.
   *  \param [in] typeName Enum type name.
   *  \return List of the enum names.
   ***************************************************************************/
  QList<QString> getList(const QString& typeName) const;

  //**************************************************************************
  /*! \brief Convert a enum name to the corresponding enum value (as an integer).
   *  \param [in] typeName Enum type name.
   *  \param [in] name Specific enum string such as CaseSensitive.
   *  \param [in] defaultValue Value to use if the name is not found (defaults to 0).
   *  \return Enum value as an integer for the given type and name.
   ***************************************************************************/
  int  stringToEnum(const QString& typeName, const QString& name, const int defaultValue = 0) const;

  //**************************************************************************
  /*! \brief Convert a enum to the corresponding name (string) value.
   *  \param [in] typeName Enum type name.
   *  \param [in] value Specific enum value (such as Qt::CaseInsensitive
   *  \param [in] defaultName Value to use if the value is not found (defaults to "").
   *  \return Enum name as a string for the given type and value.
   ***************************************************************************/
  QString enumToString(const QString& typeName, const int value, const QString& defaultName = "") const;

  //**************************************************************************
  /*! \brief Courtesy method for calling stringToEnum for regular expression syntax types.
   *  \param [in] name Expression pattern name.
   *  \param [in] defaultValue Value to use if the name is not found (defaults to RegExp2).
   *  \return Regular Expression pattern syntax for the given name.
   ***************************************************************************/
  QRegExp::PatternSyntax stringToPatternSyntax(const QString& name, const QRegExp::PatternSyntax defaultValue = QRegExp::RegExp2) const;

  //**************************************************************************
  /*! \brief Courtesy method for calling enumToString for regular expression syntax types.
   *  \param [in] value Enum value of interest.
   *  \param [in] defaultName Enum name to use if it is not found. Defaults to "Greedy Perl-Like"
   *  \return String representation of the enum value.
   ***************************************************************************/
  QString PatternSyntaxToString(const int value, const QString& defaultName = "Greedy Perl-Like") const;

private:
  //**************************************************************************
  /*! \brief Get the map from enum value to enum Name.
   *  \param [in] typeName Type name of interest. Case is not important.
   *  \return Map from enum value to enum Name, or, null if the type name is not known.
   ***************************************************************************/
  const QMap<int, QString> *getEnumData(const QString& typeName) const;

  //**************************************************************************
  /*! \brief Fill in the enum values of interest.
   ***************************************************************************/
  void initialize();

  //**************************************************************************
  /*! \brief Map a Type name to a map of enum values to string.
   *
   * Use the type name to get the first map, then, use the enum value to find the "printable string" name for that enum value.
   *
   ***************************************************************************/
  QMap<QString, QMap<int, QString>* > m_valueToName;
};

inline QRegExp::PatternSyntax QtEnumMapper::stringToPatternSyntax(const QString& name, const QRegExp::PatternSyntax defaultValue) const {
  return static_cast<QRegExp::PatternSyntax>(stringToEnum("PatternSyntax", name, defaultValue));
}

inline QString QtEnumMapper::PatternSyntaxToString(const int value, const QString& defaultName) const {
  return enumToString("PatternSyntax", value, defaultName);
}


#endif // QTENUMMAPPER_H
