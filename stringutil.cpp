#include "stringutil.h"

StringUtil::StringUtil()
{
}

QString StringUtil::wildCardToRegExpString(const QString& wildCard)
{
  // Escape periods and other special characters.
  // Convert * to .*
  // convert ? to to .
  return QRegularExpression::escape(wildCard).replace("\\*", ".*").replace('?', ".");
}

QRegularExpression StringUtil::wildCardToRegExp(const QString &wildCard)
{
  return QRegularExpression(wildCardToRegExpString(wildCard));
}

