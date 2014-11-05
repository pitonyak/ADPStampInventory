#include "csvcontroller.h"

#include <QTextStream>
#include <QFile>
#include <QDebug>

#if defined(__GNUC__)
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)
#include "nullptr.h"
#endif
#endif

CSVController::CSVController(QObject *parent) : QObject(parent)
{
    setControllerDefaults();
}

CSVController::~CSVController()
{
}

QString CSVController::reduceSpaces(const QString& s) const
{
    if (getTrimSpaces())
    {
        if (getCompactSpaces())
        {
            return s.simplified();
        }
        return s.trimmed();
    }
    else if (getCompactSpaces())
    {
        return s.simplified();
    }
    return s;
}

QString CSVController::makeSafe(const QString& s) const
{
    QString x = s;
    if (x.contains(getTextDelimiter()))
    {
        x = x.replace(getTextDelimiter(), QString(2, getTextDelimiter()));
    }
    if (x.contains(getEscapeCharacter()))
    {
        x = x.replace(getEscapeCharacter(), QString(2, getEscapeCharacter()));
    }
    return x;
}

QChar CSVController::getTextDelimiter() const
{
    return m_textDelimiter;
}

QChar CSVController::getRecordDelimiter() const
{
    return m_recordDelimiter;
}

QChar CSVController::getCommentCharacter() const
{
    return m_comment;
}

QChar CSVController::getEscapeCharacter() const
{
    return m_escapeCharacter;
}

bool CSVController::getRecordDelimiterIsDefault() const
{
    return m_recordDelimiterIsDefault;
}

bool CSVController::getUseComments() const
{
    return m_useComments;
}

bool CSVController::getTrimSpaces() const
{
    return m_trimSpaces;
}

bool CSVController::getCompactSpaces() const
{
    return m_compactSpaces;
}

bool CSVController::getSkipEmptyLines() const
{
    return m_skipEmptyLines;
}

void CSVController::setRecordDelimiter(const QChar delimiter)
{
    if (m_recordDelimiter != delimiter)
    {
        m_recordDelimiter = delimiter;
        emit recordDelimiterChanged(m_recordDelimiter);
    }
}

void CSVController::setTextDelimiter(const QChar delimiter)
{
    if (m_textDelimiter != delimiter)
    {
        m_textDelimiter = delimiter;
        emit textDelimiterChanged(m_textDelimiter);
    }
}

void CSVController::setCommentCharacter(const QChar comment)
{
    if (m_comment != comment)
    {
        m_comment = comment;
        emit commentCharacterChanged(m_comment);
    }
}

void CSVController::setEscapeCharacter(const QChar escapeCharacter)
{
    if (m_escapeCharacter != escapeCharacter)
    {
        m_escapeCharacter = escapeCharacter;
        emit escapeCharacterChanged(m_escapeCharacter);
    }
}

void CSVController::setRecordDelimiterIsDefault(const bool recordDelimiterIsDefault)
{
    if (m_recordDelimiterIsDefault != recordDelimiterIsDefault)
    {
        m_recordDelimiterIsDefault = recordDelimiterIsDefault;
        emit recordDelimiterIsDefaultChanged(m_recordDelimiterIsDefault);
    }
}

void CSVController::setUseComments(const bool useComments)
{
    if (m_useComments != useComments)
    {
        m_useComments = useComments;
        emit useCommentsChanged(m_useComments);
    }
}

void CSVController::setTrimSpaces(const bool trimSpaces)
{
    if (m_trimSpaces != trimSpaces)
    {
        m_trimSpaces = trimSpaces;
        emit trimSpacesChanged(m_trimSpaces);
    }
}

void CSVController::setCompactSpaces(const bool compactSpaces)
{
    if (m_compactSpaces != compactSpaces)
    {
        m_compactSpaces = compactSpaces;
        emit compactSpacesChanged(m_compactSpaces);
    }
}

void CSVController::setSkipEmptyLines(const bool skipEmptyLines)
{
    if (m_skipEmptyLines != skipEmptyLines)
    {
        m_skipEmptyLines = skipEmptyLines;
        emit skipEmptyLinesChanged(m_skipEmptyLines);
    }
}

void CSVController::setMergeDelimiters(const bool mergeDelimiters)
{
    if (m_mergeDelimiters != mergeDelimiters)
    {
        m_mergeDelimiters = mergeDelimiters;
        emit mergeDelimitersChanged(m_mergeDelimiters);
    }
}

bool CSVController::isOctDigit(const QChar& c) const
{
    return '0' <= c && c <= '7';
}

bool CSVController::isHexDigit(const QChar& c) const
{
    return c.isDigit() || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
}

uint CSVController::hexDigitToDecimalValue(const QChar& c) const
{
    if (c.isDigit())
    {
        return c.digitValue();
    }
    else if ('a' <= c && c <= 'f')
    {
        return c.unicode() - 'a' + 10;
    }
    else if ('A' <= c && c <= 'F')
    {
        return c.unicode() - 'A' + 10;
    }
    return 0;
}

void CSVController::setColumnDelimiter(const QChar delimiter)
{
    if (!isColumnDelimiter(delimiter))
    {
        m_columnDelimiters.clear();
        m_columnDelimiters.insert(delimiter);
        emit columnDelimiterChanged(delimiter);
    }
}

void CSVController::addColumnDelimiter(const QChar delimiter)
{
    if (!isColumnDelimiter(delimiter))
    {
        m_columnDelimiters.insert(delimiter);
        emit columnDelimiterChanged(delimiter);
    }
}

void CSVController::removeColumnDelimiter(const QChar delimiter)
{
    if (isColumnDelimiter(delimiter))
    {
        m_columnDelimiters.remove(delimiter);
        emit columnDelimiterChanged(delimiter);
    }
}

QList<QChar> CSVController::getColumnDelimiters() const
{
    return m_columnDelimiters.values();
}

void CSVController::clearColumnDelimiters()
{
    foreach (const QChar &value, m_columnDelimiters)
    {
        removeColumnDelimiter(value);
    }
}


// TODO: Make this more efficient.
QChar CSVController::getColumnDelimiter() const
{
    return isColumnDelimiter(',') || m_columnDelimiters.isEmpty() ? ',' : m_columnDelimiters.values().first();
}

void CSVController::setControllerDefaults()
{
    m_textDelimiter = '"';
    setColumnDelimiter(',');
    m_recordDelimiter = '\n';
    m_comment = '#';
    m_escapeCharacter = '\\';
    m_recordDelimiterIsDefault = true;
    m_useComments = true;
    m_compactSpaces = true;
    m_trimSpaces = true;
    m_skipEmptyLines = true;
    m_mergeDelimiters = false;
}

int CSVController::countColumns(int index) const
{
    return (index >= 0 && index < m_lines.size()) ? m_lines[index].size() : 0;
}

void CSVController::setHeader(const CSVLine& header)
{
    m_header = header;
}

const CSVLine& CSVController::getLine(int index) const
{
    if (0 <= index && index < m_lines.size())
    {
        return m_lines[index];
    }
    //TODO: Deal with errors
    return m_lines[index];
}

void CSVController::addHeader(const QString& headerName, const QMetaType::Type headerType)
{
  m_header.append(CSVColumn(headerName, true, headerType));
}

QString CSVController::getHeaderName(int i) const
{
  return 0<=i && i<m_header.size() ? m_header[i].getValue() : "";
}

QMetaType::Type CSVController::getHeaderType(int i) const
{
  return 0<=i && i<m_header.size() ? m_header[i].getType() : QMetaType::Void;
}
