#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <QRegularExpression>

class StringUtil
{
public:
  StringUtil();

  static QString wildCardToRegExpString(const QString& wildCard);
  static QRegularExpression wildCardToRegExp(const QString& wildCard);

};

#endif // STRINGUTIL_H
