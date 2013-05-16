#ifndef CSVCONTROLLER_H
#define CSVCONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QSet>
#include <QList>
#include "csvline.h"

//**************************************************************************
//! General options for controlling CSV import and export.
/*!
 *
 **************************************************************************/

class CSVController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QChar textQualifier READ getTextQualifier WRITE setTextQualifier NOTIFY textQualifierChanged RESET setTextQualifier STORED false)
    Q_PROPERTY(QChar columnDelimiter READ getColumnDelimiter WRITE setColumnDelimiter NOTIFY columnDelimiterChanged RESET setColumnDelimiter STORED false)
    Q_PROPERTY(QChar recordDelimiter READ getRecordDelimiter WRITE setRecordDelimiter NOTIFY recordDelimiterChanged RESET setRecordDelimiter STORED false)
    Q_PROPERTY(QChar commentQualifier READ getCommentQualifier WRITE setCommentQualifier NOTIFY commentQualifierChanged RESET setCommentQualifier STORED false)
    Q_PROPERTY(QChar qualifiedEscapeCharacter READ getQualifiedEscapeCharacter WRITE setQualifiedEscapeCharacter NOTIFY qualifiedEscapeCharacterChanged RESET setQualifiedEscapeCharacter STORED false)
    Q_PROPERTY(bool  recordDelimiterIsDefault READ getRecordDelimiterIsDefault WRITE setRecordDelimiterIsDefault NOTIFY recordDelimiterIsDefaultChanged RESET setRecordDelimiterIsDefault STORED false)
    Q_PROPERTY(bool useTextQualifier READ getUseTextQualifier WRITE setUseTextQualifier NOTIFY useTextQualifierChanged RESET setUseTextQualifier STORED false)
    Q_PROPERTY(bool useComments READ getUseComments WRITE setUseComments NOTIFY useCommentsChanged RESET setUseComments STORED false)
    Q_PROPERTY(bool trimSpaces READ getTrimSpaces WRITE setTrimSpaces NOTIFY trimSpacesChanged RESET setTrimSpaces STORED false)
    Q_PROPERTY(bool compactSpaces READ getCompactSpaces WRITE setCompactSpaces NOTIFY compactSpacesChanged RESET setCompactSpaces STORED false)
    Q_PROPERTY(bool skipEmptyLines READ getSkipEmptyLines WRITE setSkipEmptyLines NOTIFY skipEmptyLinesChanged RESET setSkipEmptyLines STORED false)
    Q_PROPERTY(bool useBackslashEscape READ getUseBackslashEscape WRITE setUseBackslashEscape NOTIFY useBackslashEscapeChanged RESET setUseBackslashEscape STORED false)
    Q_PROPERTY(bool mergeDelimiters READ getMergeDelimiters WRITE setMergeDelimiters NOTIFY mergeDelimitersChanged RESET setMergeDelimiters STORED false)

public:
    explicit CSVController(QObject *parent = 0);
    virtual ~CSVController();

    QChar getTextQualifier() const;
    QChar getColumnDelimiter() const;
    QChar getRecordDelimiter() const;
    QChar getCommentQualifier() const;
    QChar getQualifiedEscapeCharacter() const;
    bool  getRecordDelimiterIsDefault() const;
    bool getUseTextQualifier() const;
    bool getUseComments() const;
    bool getTrimSpaces() const;
    bool getCompactSpaces() const;
    bool getSkipEmptyLines() const;
    bool getUseBackslashEscape() const;
    bool getMergeDelimiters() const;

    QList<QChar> getColumnDelimiters() const;

    //**************************************************************************
    //! Checks to see if the parameter is the text qualifier (such as a double quote).
    /*!
     * \param c Character to compare.
     * \returns true if c is the qualifier character.
     *
     ***************************************************************************/
    bool isTextQualifier(QChar c) const;
    bool isColumnDelimiter(QChar c) const;
    QString makeSafe(const QString& s) const;
    QString reduceSpaces(const QString& s) const;

    //**************************************************************************
    //! Checks to see if the parameter is a record delimiter. This is either the character set by the user, or a CR or a LF if not set.
    /*!
     * \param c Character to compare.
     * \returns true if c is a record delimitter.
     *
     ***************************************************************************/
    bool isRecordDelimiter(QChar c) const;

    //**************************************************************************
    //! Checks to see if the parameter is the comment character and comments are enabled.
    /*!
     * \param c Character to compare.
     * \returns true if c is the comment character and comments are enabled.
     *
     ***************************************************************************/
    bool isComment(QChar c) const;

    //**************************************************************************
    //! Checks to see if the parameter is white space that can be skipped.
    /*!
     * This does not check if white space can be skipped. A tab is considered white space unless it has been set as a record or column delimiter.
     * \param c Character to compare.
     * \returns true if c white space that can be skipped.
     *
     ***************************************************************************/
    bool isSkipWS(QChar c) const;

    bool isEscapeCharacter(QChar c) const;

    bool isOctDigit(const QChar& c) const;
    bool isHexDigit(const QChar& c) const;
    uint hexToUnicode(const QChar& c) const;

signals:
    void textQualifierChanged(QChar);
    void columnDelimiterChanged(QChar);
    void recordDelimiterChanged(QChar);
    void commentQualifierChanged(QChar);
    void qualifiedEscapeCharacterChanged(QChar);
    void recordDelimiterIsDefaultChanged(bool);
    void useTextQualifierChanged(bool);
    void useCommentsChanged(bool);
    void trimSpacesChanged(bool);
    void compactSpacesChanged(bool);
    void skipEmptyLinesChanged(bool);
    void useBackslashEscapeChanged(bool);
    void mergeDelimitersChanged(bool);

public slots:
    //**************************************************************************
    //! Set the column delimiter.
    /*!
     * \param delimiter New value for the column delimiter.
     *
     ***************************************************************************/
    void setColumnDelimiter(QChar delimiter = ',');
    void addColumnDelimiter(QChar delimiter = ',');
    void removeColumnDelimiter(QChar delimiter);
    void clearColumnDelimiters();

    //**************************************************************************
    //! Set the record delimiter.
    /*!
     * \param delimiter New value for the record delimiter.
     *
     ***************************************************************************/
    void setRecordDelimiter(QChar delimiter = '\n');

    //**************************************************************************
    //! Set the text qualifier used to surround text elements; usually a double quote.
    /*!
     * \param delimiter New value for the record delimiter.
     *
     ***************************************************************************/
    void setTextQualifier(QChar delimiter = '"');

    //**************************************************************************
    //! If true, then text elements are exported using the text qualifier.
    /*!
     * \param useTextQualifier
     *
     ***************************************************************************/
    void setUseTextQualifier(bool useTextQualifier = true);

    //**************************************************************************
    //! Set the character that indicates that a line is a comment. Ignored unless use comments is true. Default is '#'
    /*!
     * \param comment is the character that identifies a comment.
     *
     ***************************************************************************/
    void setCommentQualifier(QChar comment = '#');

    //**************************************************************************
    //! Qualified text is almost always escaped with a backslash.
    /*!
     * \param escapeCharacter New character used to escape special characters.
     *
     ***************************************************************************/
    void setQualifiedEscapeCharacter(QChar escapeCharacter = '\\');

    //**************************************************************************
    //!
    /*!
     * \param
     *
     ***************************************************************************/
    void setRecordDelimiterIsDefault(bool recordDelimiterIsDefault = true);

    //**************************************************************************
    //! Set to true if the CSV file contains comments that are ignored.
    /*!
     * \param useComments
     *
     ***************************************************************************/
    void setUseComments(bool useComments=true);

    //**************************************************************************
    //! Set if leading and trailing white space is removed.
    /*!
     * \param trimSpaces
     *
     ***************************************************************************/
    void setTrimSpaces(bool trimSpaces = true);

    //**************************************************************************
    //! Set if runs of white space are compacted.
    /*!
     * \param compactSpaces
     *
     ***************************************************************************/
    void setCompactSpaces(bool compactSpaces = true);


    void setSkipEmptyLines(bool skipEmptyLines = true);
    void setUseBackslashEscape(bool useBackslashEscape = false);
    void setMergeDelimiters(bool mergeDelimiters = false);
/**
    void setHeaderNames(const QStringList& headers);
    void setHeaderTypes(const QList<QVariant::Type>& types);
    void addHeader(const QString& headerName);
    void addHeader(const QString& headerName, const QVariant::Type& headerType);
    void addHeader(const QVariant::Type& headerType);
    QString getHeaderName(int i) const;
    QVariant::Type getHeaderType(int i) const;
    void clearHeaderNames();
    void clearHeaderTypes();
    **/

    void setHeader(const CSVLine& header);
    const CSVLine& getHeader() const;
    void clearHeader();
    void clearLines();
    int countHeaderColumns() const;
    int countLines() const;
    int countColumns(int index=0) const;
    const CSVLine& getLine(int index=0) const;

private:
    void setControllerDefaults();

    QChar m_textQualifier;
    QChar m_recordDelimiter;
    QChar m_comment;
    QChar m_qualifiedEscapeCharacter;

    //**************************************************************************
    //! Track this so that I know if I must look for both CR and LF.
    /*!
     *
     ***************************************************************************/
    bool  m_recordDelimiterIsDefault;

    //**************************************************************************
    //! This is about exporting data. The text qualifier is always recognized while reading.
    /*!
     *
     ***************************************************************************/
    bool m_useTextQualifier;
    bool m_useBackslashEscape;
    bool m_useComments;
    bool m_trimSpaces;
    bool m_compactSpaces;
    bool m_skipEmptyLines;

    // TODO: Support merge delimiters, not currently supported
    bool m_mergeDelimiters;

protected:
    CSVLine m_header;
    QList<CSVLine> m_lines;
    QSet<QChar> m_columnDelimiters;
};


inline bool CSVController::isTextQualifier(QChar c) const
{
    return m_textQualifier == c;
}

inline bool CSVController::isColumnDelimiter(QChar c) const
{
    return m_columnDelimiters.contains(c);
}

inline bool CSVController::isRecordDelimiter(QChar c) const
{
    if (m_recordDelimiterIsDefault)
    {
        return (c == 13 || c == 10);
    }
    return (m_textQualifier == c);
}

inline bool CSVController::isComment(QChar c) const
{
    return  m_useComments && m_comment == c;
}

inline bool CSVController::isEscapeCharacter(QChar c) const
{
    return m_useBackslashEscape && m_qualifiedEscapeCharacter == c;
}

inline bool CSVController::isSkipWS(QChar c) const
{
    return !isColumnDelimiter(c) && !isRecordDelimiter(c) && (c == ' ' || c == '\t');
}

inline void CSVController::clearHeader()
{
    m_header.clear();
}

inline void CSVController::clearLines()
{
    m_lines.clear();
}

inline int CSVController::countHeaderColumns() const
{
    return m_header.count();
}

inline int CSVController::countLines() const
{
    return m_lines.count();
}

inline bool CSVController::getMergeDelimiters() const
{
    return m_mergeDelimiters;
}

inline const CSVLine& CSVController::getHeader() const
{
    return m_header;
}


#endif // CSVCONTROLLER_H
