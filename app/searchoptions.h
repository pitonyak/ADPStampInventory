#ifndef SEARCHOPTIONS_H
#define SEARCHOPTIONS_H

#include <QString>
#include "stringutil.h"

class SearchOptions
{
public:
  SearchOptions();
  SearchOptions(const SearchOptions& options);
  const SearchOptions& operator=(const SearchOptions& options);
  const SearchOptions& copy(const SearchOptions& options);
  void setToDefault();

  Qt::CaseSensitivity getCaseSensitivity() const;
  bool isCaseSensitive() const;

  QString getFindValue() const;
  void setFindValue(const QString& s);

  QString getReplaceValue() const;
  void setReplaceValue(const QString& s);

  bool isMatchEntireString() const;
  bool isContains() const;
  bool isStartsWith() const;
  bool isEndsWith() const;

  bool isMatchAsString() const;
  bool isRegularExpression() const;
  bool isWildCard() const;

  bool isWrap() const;
  bool isBackwards() const;
  bool isAllColumns() const;

  bool isReplace() const;
  bool isFind() const;
  bool isReplaceAll() const;

  void setIsReplaceAll(const bool b = true);

  void setIsReplace(const bool b = true);

  void setCaseSensitive(const bool b=true);
  void setWrap(const bool b=true);
  void setBackwards(const bool b=true);
  void setAllColumns(const bool b=true);

  void setMatchEntireString();
  void setContains();
  void setStartsWith();
  void setEndsWith();

  void setMatchAsString();
  void setRegularExpression();
  void setWildCard();

  Qt::MatchFlags getMatchFlags() const;
  void setMatchFlags(const Qt::MatchFlags flags);

  QString serializeSettings() const;
  void deserializeSettings(const QString s);

  QRegularExpression getRegularExpression() const;

private:
  bool m_isCaseSensitive;
  bool m_isMatchEntireString;
  bool m_isContains;
  bool m_isStartsWith;
  bool m_isEndsWith;

  bool m_isMatchAsString;
  bool m_isRegularExpression;
  bool m_isWildCard;

  bool m_isWrap;
  bool m_isBackwards;
  bool m_isAllColumns;

  bool m_isReplace;
  bool m_replaceAll;

  QString m_find;
  QString m_replace;
};

inline bool SearchOptions::isMatchEntireString() const {return m_isMatchEntireString; }
inline bool SearchOptions::isContains() const { return m_isContains; }
inline bool SearchOptions::isStartsWith() const { return m_isStartsWith; }
inline bool SearchOptions::isEndsWith() const { return m_isEndsWith; }
inline bool SearchOptions::isMatchAsString() const { return m_isMatchAsString; }
inline bool SearchOptions::isRegularExpression() const { return m_isRegularExpression; }
inline bool SearchOptions::isWildCard() const { return m_isWildCard; }
inline bool SearchOptions::isWrap() const { return m_isWrap; }
inline bool SearchOptions::isBackwards() const { return m_isBackwards; }
inline bool SearchOptions::isAllColumns() const { return m_isAllColumns; }
inline bool SearchOptions::isCaseSensitive() const { return m_isCaseSensitive; }
inline Qt::CaseSensitivity SearchOptions::getCaseSensitivity() const { return m_isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive; }


inline QString SearchOptions::getFindValue() const { return m_find; }
inline void SearchOptions::setFindValue(const QString& s) { m_find = s; }

inline QString SearchOptions::getReplaceValue() const { return m_replace; }
inline void SearchOptions::setReplaceValue(const QString& s) { m_replace = s; }

inline void SearchOptions::setCaseSensitive(const bool b) { m_isCaseSensitive = b; }
inline void SearchOptions::setWrap(const bool b) { m_isWrap = b; }
inline void SearchOptions::setBackwards(const bool b) { m_isBackwards = b; }
inline void SearchOptions::setAllColumns(const bool b) { m_isAllColumns = b; }

inline bool SearchOptions::isReplace() const { return m_isReplace; }
inline bool SearchOptions::isFind() const { return !m_isReplace; }

inline void SearchOptions::setIsReplace(const bool b) { m_isReplace = b; }

inline bool SearchOptions::isReplaceAll() const { return m_replaceAll; }

inline void SearchOptions::setIsReplaceAll(const bool b) { m_replaceAll = b; }


#endif // SEARCHOPTIONS_H
