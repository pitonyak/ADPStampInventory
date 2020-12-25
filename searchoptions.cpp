#include "searchoptions.h"

#include <QStringList>

SearchOptions::SearchOptions()
{
  setToDefault();
}

SearchOptions::SearchOptions(const SearchOptions& options)
{
  copy(options);
}

const SearchOptions& SearchOptions::operator=(const SearchOptions& options)
{
  return copy(options);
}

const SearchOptions& SearchOptions::copy(const SearchOptions& options)
{
  if (this != &options)
  {
    m_isCaseSensitive = options.m_isCaseSensitive;
    m_isMatchEntireString = options.m_isMatchEntireString;
    m_isContains = options.m_isContains;
    m_isStartsWith = options.m_isStartsWith;
    m_isEndsWith = options.m_isEndsWith;

    m_isMatchAsString = options.m_isMatchAsString;
    m_isRegularExpression = options.m_isRegularExpression;
    m_isWildCard = options.m_isWildCard;

    m_isWrap = options.m_isWrap;
    m_isBackwards = options.m_isBackwards;
    m_isAllColumns = options.m_isAllColumns;

    m_find = options.m_find;
    m_replace = options.m_replace;
  }
  return *this;
}


void SearchOptions::setToDefault()
{
  setContains();
  setMatchAsString();
  setCaseSensitive(false);
  setWrap(true);
  setBackwards(false);
  setAllColumns(false);
  m_isReplace = false;
  setIsReplaceAll(false);
}

void SearchOptions::setMatchEntireString()
{
  m_isMatchEntireString = true;
  m_isContains = false;
  m_isStartsWith = false;
  m_isEndsWith = false;
}

void SearchOptions::setContains()
{
  m_isMatchEntireString = false;
  m_isContains = true;
  m_isStartsWith = false;
  m_isEndsWith = false;
}


void SearchOptions::setStartsWith()
{
  m_isMatchEntireString = false;
  m_isContains = false;
  m_isStartsWith = true;
  m_isEndsWith = false;
}


void SearchOptions::setEndsWith()
{
  m_isMatchEntireString = false;
  m_isContains = false;
  m_isStartsWith = false;
  m_isEndsWith = true;
}



void SearchOptions::setMatchAsString()
{
  m_isMatchAsString = true;
  m_isRegularExpression = false;
  m_isWildCard = false;
}

void SearchOptions::setRegularExpression()
{
  m_isMatchAsString = false;
  m_isRegularExpression = true;
  m_isWildCard = false;
}


void SearchOptions::setWildCard()
{
  m_isMatchAsString = false;
  m_isRegularExpression = false;
  m_isWildCard = true;
}


Qt::MatchFlags SearchOptions::getMatchFlags() const
{

    // *** match flags ***
    // MatchExactly = 0, Performs QVariant-based matching.
    // MatchContains = 1,
    // MatchStartsWith = 2,
    // MatchEndsWith = 3,
    // MatchRegExp = 4,
    // MatchWildcard = 5,
    // MatchFixedString = 8, Performs string-based matching.
    // MatchCaseSensitive = 16,
    // MatchWrap = 32,
    // MatchRecursive = 64 Searches the entire hierarchy, but I do not have heirarchical data.

  Qt::MatchFlags flags = Qt::MatchExactly;
  if (isMatchEntireString()) flags |= Qt::MatchFixedString;
  if (isContains()) flags |= Qt::MatchContains;
  if (isStartsWith()) flags |= Qt::MatchStartsWith;
  if (isEndsWith()) flags |= Qt::MatchEndsWith;
  //if (isMatchAsString()) flags |= Qt::MatchFixedString;
  if (isRegularExpression()) flags |= Qt::MatchRegularExpression;
  if (isWildCard()) flags |= Qt::MatchWildcard;
  if (isCaseSensitive()) flags |= Qt::MatchCaseSensitive;
  if (isWrap()) flags |= Qt::MatchWrap;
  return flags;
}

void SearchOptions::setMatchFlags(const Qt::MatchFlags flags)
{
  if ((flags & Qt::MatchWildcard) == Qt::MatchWildcard) {
      setWildCard();
  } else if ((flags & Qt::MatchRegularExpression) == Qt::MatchRegularExpression) {
      setRegularExpression();
  } else {
    setMatchAsString();
  }

  if ((flags & Qt::MatchEndsWith) == Qt::MatchEndsWith) {
      setEndsWith();
  } else if ((flags & Qt::MatchStartsWith) == Qt::MatchStartsWith) {
    setStartsWith();
  } else if ((flags & Qt::MatchContains) == Qt::MatchContains) {
    setContains();
  } else {
    setMatchEntireString();
  }

  setCaseSensitive((flags & Qt::MatchCaseSensitive) == Qt::MatchCaseSensitive);
  setWrap((flags & Qt::MatchWrap) == Qt::MatchWrap);
}

void SearchOptions::deserializeSettings(const QString allOptions)
{
  setMatchEntireString();
  setMatchAsString();
  setCaseSensitive(false);
  setBackwards(false);
  setAllColumns(false);
  setWrap(false);
  setIsReplace(false);
  setIsReplaceAll(false);
  QStringList list = allOptions.split(",");
  for (int i=0; i<list.count(); ++i) {
    QString s = list.at(i);
    if (s.compare("MatchEntireString", Qt::CaseInsensitive) == 0) {
      setMatchEntireString();
    } else if (s.compare("Contains", Qt::CaseInsensitive) == 0) {
      setContains();
    } else if (s.compare("StartsWith", Qt::CaseInsensitive) == 0) {
      setStartsWith();
    } else if (s.compare("EndsWith", Qt::CaseInsensitive) == 0) {
      setEndsWith();
    } else if (s.compare("AsString", Qt::CaseInsensitive) == 0) {
      setMatchAsString();
    } else if (s.compare("RegExp", Qt::CaseInsensitive) == 0) {
      setRegularExpression();
    } else if (s.compare("Wildcard", Qt::CaseInsensitive) == 0) {
      setWildCard();
    } else if (s.compare("CaseSensitive", Qt::CaseInsensitive) == 0) {
      setCaseSensitive();
    } else if (s.compare("CaseInSensitive", Qt::CaseInsensitive) == 0) {
      setCaseSensitive(false);
    } else if (s.compare("Wrap", Qt::CaseInsensitive) == 0) {
      setWrap(true);
    } else if (s.compare("NoWrap", Qt::CaseInsensitive) == 0) {
      setWrap(false);
    } else if (s.compare("Backward", Qt::CaseInsensitive) == 0) {
      setBackwards(true);
    } else if (s.compare("Forward", Qt::CaseInsensitive) == 0) {
      setBackwards(false);
    } else if (s.compare("AllColumns", Qt::CaseInsensitive) == 0) {
      setAllColumns(true);
    } else if (s.compare("OneColumn", Qt::CaseInsensitive) == 0) {
      setAllColumns(false);
    } else if (s.compare("Replace", Qt::CaseInsensitive) == 0) {
      setIsReplace(true);
    } else if (s.compare("ReplaceAll", Qt::CaseInsensitive) == 0) {
      setIsReplaceAll(true);
    }
  }
}

QString SearchOptions::serializeSettings() const
{
  QStringList list;
  if (isMatchEntireString()) list << "MatchEntireString";
  if (isContains()) list << "Contains";
  if (isStartsWith()) list << "StartsWith";
  if (isEndsWith()) list << "EndsWith";

  if (isMatchAsString()) list << "AsString";
  if (isRegularExpression()) list << "RegExp";
  if (isWildCard()) list << "Wildcard";

  list << (isCaseSensitive() ? "CaseSensitive" : "CaseInSensitive");
  list << (isWrap() ? "Wrap" : "NoWrap");
  list << (isBackwards() ? "Backward" : "Forward");
  list << (isAllColumns() ? "AllColumns" : "OneColumn");
  if (isReplace()) list << "Replace";
  if (isReplaceAll()) list << "ReplaceAll";
  return list.join(",");
}

QRegularExpression SearchOptions::getRegularExpression() const
{
  QRegularExpression regexp = isWildCard() ? StringUtil::wildCardToRegExp(getFindValue()) : QRegularExpression(getFindValue());
  if (!isCaseSensitive()) {
    regexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
  }
  return regexp;
}
