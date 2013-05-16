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
    if (x.contains(getTextQualifier()))
    {
        x = x.replace(getTextQualifier(), QString(2, getTextQualifier()));
    }
    if (x.contains(getQualifiedEscapeCharacter()))
    {
        x = x.replace(getQualifiedEscapeCharacter(), QString(2, getQualifiedEscapeCharacter()));
    }
    return x;
}

QChar CSVController::getTextQualifier() const
{
    return m_textQualifier;
}

QChar CSVController::getRecordDelimiter() const
{
    return m_recordDelimiter;
}

QChar CSVController::getCommentQualifier() const
{
    return m_comment;
}

QChar CSVController::getQualifiedEscapeCharacter() const
{
    return m_qualifiedEscapeCharacter;
}

bool CSVController::getRecordDelimiterIsDefault() const
{
    return m_recordDelimiterIsDefault;
}

bool CSVController::getUseTextQualifier() const
{
    return m_useTextQualifier;
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

bool CSVController::getUseBackslashEscape() const
{
    return m_useBackslashEscape;
}

void CSVController::setRecordDelimiter(QChar delimiter)
{
    if (m_recordDelimiter != delimiter)
    {
        m_recordDelimiter = delimiter;
        emit recordDelimiterChanged(m_recordDelimiter);
    }
}

void CSVController::setTextQualifier(QChar delimiter)
{
    if (m_textQualifier != delimiter)
    {
        m_textQualifier = delimiter;
        emit textQualifierChanged(m_textQualifier);
    }
}

void CSVController::setUseTextQualifier(bool useTextQualifier)
{
    if (m_useTextQualifier != useTextQualifier)
    {
        m_useTextQualifier = useTextQualifier;
        emit useTextQualifierChanged(m_useTextQualifier);
    }
}

void CSVController::setCommentQualifier(QChar comment)
{
    if (m_comment != comment)
    {
        m_comment = comment;
        emit commentQualifierChanged(m_comment);
    }
}

void CSVController::setQualifiedEscapeCharacter(QChar escapeCharacter)
{
    if (m_qualifiedEscapeCharacter != escapeCharacter)
    {
        m_qualifiedEscapeCharacter = escapeCharacter;
        emit qualifiedEscapeCharacterChanged(m_qualifiedEscapeCharacter);
    }
}

void CSVController::setRecordDelimiterIsDefault(bool recordDelimiterIsDefault)
{
    if (m_recordDelimiterIsDefault != recordDelimiterIsDefault)
    {
        m_recordDelimiterIsDefault = recordDelimiterIsDefault;
        emit recordDelimiterIsDefaultChanged(m_recordDelimiterIsDefault);
    }
}

void CSVController::setUseComments(bool useComments)
{
    if (m_useComments != useComments)
    {
        m_useComments = useComments;
        emit useCommentsChanged(m_useComments);
    }
}

void CSVController::setTrimSpaces(bool trimSpaces)
{
    if (m_trimSpaces != trimSpaces)
    {
        m_trimSpaces = trimSpaces;
        emit trimSpacesChanged(m_trimSpaces);
    }
}

void CSVController::setCompactSpaces(bool compactSpaces)
{
    if (m_compactSpaces != compactSpaces)
    {
        m_compactSpaces = compactSpaces;
        emit compactSpacesChanged(m_compactSpaces);
    }
}

void CSVController::setSkipEmptyLines(bool skipEmptyLines)
{
    if (m_skipEmptyLines != skipEmptyLines)
    {
        m_skipEmptyLines = skipEmptyLines;
        emit skipEmptyLinesChanged(m_skipEmptyLines);
    }
}

void CSVController::setUseBackslashEscape(bool useBackslashEscape)
{
    if (m_useBackslashEscape != useBackslashEscape)
    {
        m_useBackslashEscape = useBackslashEscape;
        emit useBackslashEscapeChanged(m_useBackslashEscape);
    }
}

void CSVController::setMergeDelimiters(bool mergeDelimiters)
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

uint CSVController::hexToUnicode(const QChar& c) const
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

void CSVController::setColumnDelimiter(QChar delimiter)
{
    if (!isColumnDelimiter(delimiter))
    {
        m_columnDelimiters.clear();
        m_columnDelimiters.insert(delimiter);
        emit columnDelimiterChanged(delimiter);
    }
}

void CSVController::addColumnDelimiter(QChar delimiter)
{
    if (!isColumnDelimiter(delimiter))
    {
        m_columnDelimiters.insert(delimiter);
        emit columnDelimiterChanged(delimiter);
    }
}

void CSVController::removeColumnDelimiter(QChar delimiter)
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
    m_textQualifier = '"';
    setColumnDelimiter(',');
    m_recordDelimiter = '\n';
    m_comment = '#';
    m_qualifiedEscapeCharacter = '\\';
    m_recordDelimiterIsDefault = true;
    m_useTextQualifier = true;
    m_useComments = true;
    m_compactSpaces = true;
    m_trimSpaces = true;
    m_skipEmptyLines = true;
    m_useBackslashEscape = false;
    m_mergeDelimiters = false;
}

int CSVController::countColumns(int index) const
{
    return (index >= 0 && index < m_lines.count()) ? m_lines[index].count() : 0;
}

void CSVController::setHeader(const CSVLine& header)
{
    m_header = header;
}

const CSVLine& CSVController::getLine(int index) const
{
    if (0 <= index && index < m_lines.count())
    {
        return m_lines[index];
    }
    //TODO: Deal with errors
    return m_lines[index];
}


/**
void CSVController::setHeaderNames(const QStringList& headers)
{
    m_headerNames.clear();
    m_headerNames.append(headers);
}

void CSVController::setHeaderTypes(const QList<QVariant::Type>& types)
{
    m_headerTypes.clear();
    m_headerTypes.append(types);
}

void CSVController::addHeader(const QString& headerName)
{
    m_headerNames.append(headerName);
}

void CSVController::addHeader(const QString& headerName, const QVariant::Type& headerType)
{
    addHeader(headerType);
    addHeader(headerName);
}

void CSVController::addHeader(const QVariant::Type& headerType)
{
    m_headerTypes.append(headerType);
}

QString CSVController::getHeaderName(int i) const
{
    return 0<=i && i<m_headerNames.count() ? m_headerNames[i] : QString();
}

QVariant::Type CSVController::getHeaderType(int i) const
{
    return 0<=i && i<m_headerTypes.count() ? m_headerTypes[i] : QVariant::Invalid;
}

void CSVController::clearHeaderNames()
{
    m_headerNames.clear();
}

void CSVController::clearHeaderTypes()
{
    m_headerTypes.clear();
}
**/
