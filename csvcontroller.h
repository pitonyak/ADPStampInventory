#ifndef CSVCONTROLLER_H
#define CSVCONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QSet>
#include <QList>
#include "csvline.h"

//**************************************************************************
/*! \class CSVController
 *  \brief General options for controlling CSV import and export.
 *
 * Too many parameters shared between reader and writer so encapsulate
 * into a single class.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2020
 *
 **************************************************************************/


class CSVController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QChar textDelimiter READ getTextDelimiter WRITE setTextDelimiter NOTIFY textDelimiterChanged RESET setTextDelimiter STORED false)
  Q_PROPERTY(QChar columnDelimiter READ getColumnDelimiter WRITE setColumnDelimiter NOTIFY columnDelimiterChanged RESET setColumnDelimiter STORED false)
  Q_PROPERTY(QChar recordDelimiter READ getRecordDelimiter WRITE setRecordDelimiter NOTIFY recordDelimiterChanged RESET setRecordDelimiter STORED false)
  Q_PROPERTY(QChar commentCharacter READ getCommentCharacter WRITE setCommentCharacter NOTIFY commentCharacterChanged RESET setCommentCharacter STORED false)
  Q_PROPERTY(QChar escapeCharacter READ getEscapeCharacter WRITE setEscapeCharacter NOTIFY escapeCharacterChanged RESET setEscapeCharacter STORED false)
  Q_PROPERTY(bool  recordDelimiterIsDefault READ getRecordDelimiterIsDefault WRITE setRecordDelimiterIsDefault NOTIFY recordDelimiterIsDefaultChanged RESET setRecordDelimiterIsDefault STORED false)
  Q_PROPERTY(bool useComments READ getUseComments WRITE setUseComments NOTIFY useCommentsChanged RESET setUseComments STORED false)
  Q_PROPERTY(bool trimSpaces READ getTrimSpaces WRITE setTrimSpaces NOTIFY trimSpacesChanged RESET setTrimSpaces STORED false)
  Q_PROPERTY(bool compactSpaces READ getCompactSpaces WRITE setCompactSpaces NOTIFY compactSpacesChanged RESET setCompactSpaces STORED false)
  Q_PROPERTY(bool skipEmptyLines READ getSkipEmptyLines WRITE setSkipEmptyLines NOTIFY skipEmptyLinesChanged RESET setSkipEmptyLines STORED false)
  Q_PROPERTY(bool mergeDelimiters READ getMergeDelimiters WRITE setMergeDelimiters NOTIFY mergeDelimitersChanged RESET setMergeDelimiters STORED false)

public:
  //**************************************************************************
  /*! \brief Constructor sets to default values.
   *
   * \param [in] s String to modify.
   * \returns String with proper escaping and quoting.
   *
   * \sa CSVController::setControllerDefaults()
   ***************************************************************************/
  explicit CSVController(QObject *parent = 0);

  /*! Descructor */
  virtual ~CSVController();

  /*! \returns Character used to delimit text, usually a double quote (") */
  QChar getTextDelimiter() const;

  //**************************************************************************
  /*! \brief Return a column delimiter.
   *
   * More than one column delimiter may be active at a time. If the comma (,)
   * is a delimiter or if no delimiter is defined, comma is returned; otherwise,
   * the first column delimiter is returned.
   *
   * Note that if there are no column delimiters, comma will not pass as a column delimiter,
   * even though it is returned as the column delimiter.
   *
   * \returns A column delimiter.
   *
   * \sa isColumnDelimiter()
   *
   ***************************************************************************/
  QChar getColumnDelimiter() const;

  //**************************************************************************
  /*! \brief Return the character set a the record delimiter.
   *
   * This character is only used if getRecordDelimiterIsDefault() returns false.
   *
   * \returns Character set a the record delimiter.
   *
   * \sa isRecordDelimiter()
   * \sa getRecordDelimiterIsDefault()
   *
   ***************************************************************************/
  QChar getRecordDelimiter() const;

  //**************************************************************************
  /*! \brief Return the character used to start a line as a comment; defaults to "#".
   *
   * \returns Character used to start a line as a comment; defaults to "#".
   ***************************************************************************/
  QChar getCommentCharacter() const;

  /*! \returns Character used to escape other characters, usually "\" */
  QChar getEscapeCharacter() const;

  //**************************************************************************
  /*! \brief Determine if the default (CR or LF) are accepted as the record delimiter.
   *
   * By default, 0x0D and 0x0A (CR and LF) are both accepted as a record delimiter.
   * If this is set to False, then the single character set as the record delimiter
   * is used instead.
   *
   * \returns True if the default record delimiter is used.
   ***************************************************************************/
  bool getRecordDelimiterIsDefault() const;

  /*! \returns True if comments are allowed. */
  bool getUseComments() const;

  /*! \returns If True, leading and trailing white space is removed. */
  bool getTrimSpaces() const;

  /*! \returns If True, all internal sequences of white space are replaced by a single space. Leading and trailing white space is removed. */
  bool getCompactSpaces() const;

  /*! \returns If True, empty lines are ignored. */
  bool getSkipEmptyLines() const;

  //**************************************************************************
  /*! \brief Not implemented, but, if True, multiple delimiters should be treated as a single delimiter. For example, if tab is the delimiter, four tabs is treated the same as one tab.
   *
   * \returns State of the merge delimiters flag.
   ***************************************************************************/
  bool getMergeDelimiters() const;

  QList<QChar> getColumnDelimiters() const;

  //**************************************************************************
  /*! \brief Checks to see if the parameter is the text delimiter (such as a double quote).
   *
   * \param [in] c Character to compare.
   * \returns true if c is the delimiter character.
   ***************************************************************************/
  bool isTextDelimiter(const QChar c) const;

  //**************************************************************************
  /*! \brief Checks to see if the parameter is a column delimiter (such as a comma or tab).
   *
   * If no column delimiter is set, then no character will ever cause true to be returned
   * even though getColumnDelimiter() will return comma as a valid comma delimiter.
   *
   * \param [in] c Character to compare.
   * \returns true if c is a columnn delimiter.
   ***************************************************************************/
  bool isColumnDelimiter(const QChar c) const;

  //**************************************************************************
  /*! \brief Renders a string safe by escaping escaping the escape character and double quoting quotes.
   *
   * Quotes are always doubled, should I escape them instead?
   *
   * \param [in] s String to modify.
   * \returns String with proper escaping and quoting.
   ***************************************************************************/
  QString makeSafe(const QString& s) const;

  //**************************************************************************
  /*! \brief Trim and compact spaces based on the current configuration.
   *
   * \param [in] s String from which spaces are removed.
   * \returns String with extra spaces eliminated.
   *
   * \sa CSVController::getTrimSpaces()
   * \sa CSVController::getCompactSpaces()
   ***************************************************************************/
  QString reduceSpaces(const QString& s) const;

  //**************************************************************************
  /*! \brief Checks to see if the parameter is a record delimiter. This is either the character set by the user, or a CR or a LF if not set.
   *
   * Current implementation allows a single character record delimiter. If CR LF is used,
   * this could introduce an empty record, but empty records are ignored, so this works.
   *
   * \param [in] c Character to compare.
   * \returns true if c is a record delimitter.
   *
   ***************************************************************************/
  bool isRecordDelimiter(const QChar c) const;

  //**************************************************************************
  /*! Checks to see if the parameter is the comment character and comments are enabled.
   *
   * \param [in] c Character to compare.
   * \returns true if c is the comment character and comments are enabled.
   *
   ***************************************************************************/
  bool isComment(const QChar c) const;

  //**************************************************************************
  /*! \brief Checks to see if the parameter is white space that can be skipped.
   *
   * This does not check if white space can be skipped. A tab is considered white space unless it has been set as a record or column delimiter.
   * \param [in] c Character to compare.
   * \returns true if c white space that can be skipped.
   *
   ***************************************************************************/
  bool isSkipWS(const QChar c) const;

  //**************************************************************************
  /*! \brief Checks to see if the parameter is the escape character.
   *
   * \param [in] c Character to compare.
   * \returns true if c is the escape character.
   *
   ***************************************************************************/
  bool isEscapeCharacter(QChar c) const;

  //**************************************************************************
  /*! \brief Checks to see if the parameter is a valid octal digit of 0, 1, 2, 3, 4, 5, 6, or 7.
   *
   * \param [in] c Character to compare.
   * \returns true if c is a valid octal digit.
   *
   ***************************************************************************/
  bool isOctDigit(const QChar& c) const;

  //**************************************************************************
  /*! \brief Checks to see if the parameter is a valid hex digit of 0-9, a-f, or A-F.
   *
   * \param [in] c Character to compare.
   * \returns true if c is a valid hex digit.
   *
   ***************************************************************************/
  bool isHexDigit(const QChar& c) const;

  //**************************************************************************
  /*! \brief Converts a hex digit (0-9, a-f, or A-F) to the corresponding numeric values of 0-15.
   *
   * \param [in] c Character to convert.
   * \returns Corresponding numeric value from the hex digit.
   *
   ***************************************************************************/
  uint hexDigitToDecimalValue(const QChar& c) const;

signals:
  void textDelimiterChanged(QChar);
  void columnDelimiterChanged(QChar);
  void recordDelimiterChanged(QChar);
  void commentCharacterChanged(QChar);
  void escapeCharacterChanged(QChar);
  void recordDelimiterIsDefaultChanged(bool);
  void useCommentsChanged(bool);
  void trimSpacesChanged(bool);
  void compactSpacesChanged(bool);
  void skipEmptyLinesChanged(bool);
  void mergeDelimitersChanged(bool);

public slots:
  //**************************************************************************
  /*! \brief Set the column delimiter.
   *
   * \param [in] delimiter New value for the column delimiter.
   *
   ***************************************************************************/
  void setColumnDelimiter(const QChar delimiter = ',');
  void addColumnDelimiter(const QChar delimiter = ',');
  void removeColumnDelimiter(const QChar delimiter);
  void clearColumnDelimiters();

  //**************************************************************************
  /*! \brief Set the record delimiter.
   *
   * \param [in] delimiter New value for the record delimiter.
   *
   ***************************************************************************/
  void setRecordDelimiter(const QChar delimiter = '\n');

  //**************************************************************************
  /*! \brief Set the text delimiter used to surround text elements; usually a double quote.
   *
   * \param [in] delimiter New value for the record delimiter.
   *
   ***************************************************************************/
  void setTextDelimiter(const QChar delimiter = '"');

  //**************************************************************************
  /*! \brief Set the character that indicates that a line is a comment. Ignored unless use comments is true. Default is '#'
   *
   * \param [in] comment is the character that identifies a comment.
   *
   ***************************************************************************/
  void setCommentCharacter(const QChar comment = '#');

  //**************************************************************************
  /*! \brief Qualified text is almost always escaped with a backslash.
   *
   * \param [in] escapeCharacter New character used to escape special characters.
   *
   ***************************************************************************/
  void setEscapeCharacter(const QChar escapeCharacter = '\\');

  //**************************************************************************
  /*! \brief
   *
   * \param [in] recordDelimiterIsDefault
   *
   ***************************************************************************/
  void setRecordDelimiterIsDefault(const bool recordDelimiterIsDefault = true);

  //**************************************************************************
  /*! \brief Set to true if the CSV file contains comments that are ignored.
   *
   * \param [in] useComments
   *
   ***************************************************************************/
  void setUseComments(const bool useComments=true);

  //**************************************************************************
  /*! \brief Set if leading and trailing white space is removed.
   *
   * \param [in] trimSpaces
   *
   ***************************************************************************/
  void setTrimSpaces(const bool trimSpaces = true);

  //**************************************************************************
  /*! \brief Set if runs of white space are compacted.
   *
   * \param [in] compactSpaces
   *
   ***************************************************************************/
  void setCompactSpaces(const bool compactSpaces = true);


  void setSkipEmptyLines(const bool skipEmptyLines = true);
  void setMergeDelimiters(const bool mergeDelimiters = false);

  void addHeader(const QString& headerName, const QMetaType::Type headerType);
  QString getHeaderName(int i) const;
  QMetaType::Type getHeaderType(int i) const;

  void setHeader(const CSVLine& header);

  /*! \returns The header element as set. */
  const CSVLine& getHeader() const;

  /*! Clear the columns from the header. */
  void clearHeader();

  /*! Clear the lines read so far */
  void clearLines();

  /*! \returns Number of columns in the header */
  int countHeaderColumns() const;

  /*! \returns Number of lines that have been read */
  int countLines() const;

  //**************************************************************************
  /*! \brief Count the columns in the specified line.
   *
   * \param [in] index Index of the line for which columns are counted.
   * \returns the numer of columns in the specified line.
   ***************************************************************************/
  int countColumns(int index=0) const;

  //**************************************************************************
  /*! \brief Return the specified line.
   *
   * \param [in] index Index of the line to return.
   * \returns Specified line.
   ***************************************************************************/
  const CSVLine& getLine(int index=0) const;

private:
  void setControllerDefaults();

  QChar m_textDelimiter;
  QChar m_recordDelimiter;
  QChar m_comment;
  QChar m_escapeCharacter;

  //**************************************************************************
  /*! \brief Track this so that I know if I must look for both CR and LF.
   *
   *
   ***************************************************************************/
  bool  m_recordDelimiterIsDefault;

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


inline bool CSVController::isTextDelimiter(const QChar c) const
{
  return m_textDelimiter == c;
}

inline bool CSVController::isColumnDelimiter(const QChar c) const
{
  return m_columnDelimiters.contains(c);
}

inline bool CSVController::isRecordDelimiter(const QChar c) const
{
  if (m_recordDelimiterIsDefault)
  {
    return (c.toLatin1() == 13 || c.toLatin1() == 10);
  }
  return (m_textDelimiter == c);
}

inline bool CSVController::isComment(const QChar c) const
{
  return  m_useComments && m_comment == c;
}

inline bool CSVController::isEscapeCharacter(QChar c) const
{
  return m_escapeCharacter == c;
}

inline bool CSVController::isSkipWS(const QChar c) const
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
  return m_header.size();
}

inline int CSVController::countLines() const
{
  return m_lines.size();
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
