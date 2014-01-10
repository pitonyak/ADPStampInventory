#include "csvreader.h"
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include <QTextDecoder>
#include <QTextCodec>
#include <QList>
#include <QVariant>
#include "csvline.h"

#if defined(__GNUC__)
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)
#include "nullptr.h"
#endif
#endif



CSVReader::CSVReader(QObject *parent) :
    CSVController(parent),
    m_conversionPreferences(TypeMapper::PreferSigned | TypeMapper::PreferInt),
    m_columnTypes(nullptr),
    m_hasLastChar(false),
    m_lastChar(0)
{
    setReaderDefaults();
}

CSVReader::CSVReader(const TypeMapper::ColumnConversionPreferences preferences, QObject *parent) :
    CSVController(parent),
    m_conversionPreferences(preferences),
    m_columnTypes(nullptr),
    m_hasLastChar(false),
    m_lastChar(0)
{
    setReaderDefaults();
}

CSVReader::~CSVReader()
{
    cleanup();
}

void CSVReader::setReaderDefaults()
{
    m_file = nullptr;
    m_inStream = nullptr;
    readerInitialization();
    recordInitialization();
}

void CSVReader::cleanup()
{
    if (m_inStream != nullptr)
    {
        delete m_inStream;
        m_inStream = nullptr;
    }
    if (m_file != nullptr)
    {
        if (m_file->isOpen())
        {
            m_file->close();
        }
        delete m_file;
        m_file = nullptr;
    }
    if (m_columnTypes != nullptr)
    {
      delete m_columnTypes;
      m_columnTypes = nullptr;
    }
}

bool CSVReader::setStreamFromString(const QString& s)
{
    m_tempForTextStream = s;
    cleanup();
    m_inStream = new QTextStream(&m_tempForTextStream, QIODevice::ReadOnly);
    return canReadFromStream();
}

bool CSVReader::parseFromDevice(QIODevice* device)
{
    return parseFromDevice(device, QTextCodec::codecForLocale());
}

bool CSVReader::parseFromDevice(QIODevice* device, const char* codecName)
{
    return parseFromDevice(device, QTextCodec::codecForName(codecName));
}

bool CSVReader::parseFromDevice(QIODevice* device, QTextCodec *codec)
{
    if (device != nullptr && device->isReadable())
    {
        if (codec == nullptr)
        {
            codec = QTextCodec::codecForLocale();
        }
        if (codec != nullptr)
        {
            QTextDecoder textDecoder(codec);
            return setStreamFromString(textDecoder.toUnicode(device->readAll()));
        }
    }
    return false;
}

void CSVReader::readerInitialization()
{
    m_entireRecordRead = false;
    m_numberOfRecordsRead = 0;
    m_positionInBuffer = 0;
    m_hasChar = false;
    m_hasLastChar = false;
    m_lastChar = 0;

    // The buffer is 10MB in size.
    m_maxBufferRead = 10485760;
}

void CSVReader::recordInitialization()
{
    m_entireRecordRead = false;
    m_numberOfRecordsRead = 0;
    if (m_lines.count() > 0)
    {
        m_lines.last().clear();
    }
}

void CSVReader::columnInitialization()
{
    // TODO: ?? Remove if not used.
}

void CSVReader::endOfRecordReached()
{
    if (m_entireRecordRead)
    {
        // This is an error!
    }
    else
    {
        m_entireRecordRead = true;
        ++m_numberOfRecordsRead;
        if (getRecordDelimiterIsDefault())
        {
            readToNextLine();
        }
        else
        {
            moveToNextChar();
        }
    }
}

void CSVReader::endOfColumnReached(const QString& columnValue, bool wasDelimited)
{
    QString temp = getTrimSpaces() ? columnValue.trimmed() : columnValue;
    QString value = getCompactSpaces() ? temp.simplified() : temp;
    QMetaType::Type guessedType = wasDelimited ? QMetaType::QString : CSVColumn::guessType(value, m_conversionPreferences);
    m_lines.last().append(CSVColumn(value, wasDelimited, guessedType));
}

QChar CSVReader::currentChar()
{
    return m_readBufferString.at(m_positionInBuffer);
}

bool CSVReader::moveToNextChar()
{
    if (m_hasChar)
    {
        m_lastChar = currentChar();
        m_hasLastChar = true;
    }

    if (m_positionInBuffer < m_readBufferString.length())
    {
        ++m_positionInBuffer;
    }
    else
    {
        m_readBufferString = read(m_maxBufferRead);
        m_positionInBuffer = 0;
    }
    m_hasChar = m_positionInBuffer < m_readBufferString.length();
    return m_hasChar;
}

bool CSVReader::hasChar()
{
    return m_hasChar;
}

bool CSVReader::readToNextLine()
{
    bool lineSkipped = false;
    while (!lineSkipped && hasChar())
    {
        if (currentChar() == 13)
        {
            lineSkipped = true;
            if (moveToNextChar() && currentChar() == 10)
            {
                moveToNextChar();
            }
        }
        else if (currentChar() == 10)
        {
            lineSkipped = true;
            if (moveToNextChar() && currentChar() == 13)
            {
                moveToNextChar();
            }
        }
        else
        {
            moveToNextChar();
        }
    }
    return lineSkipped;
}

bool CSVReader::readHeader(bool clearBeforeReading)
{
    clearHeader();
    if (!readNextRecord(clearBeforeReading))
    {
        return false;
    }
    setHeader(m_lines.last());
    m_lines.removeLast();
    return true;
}

QString CSVReader::toString(bool brief) const
{
    QString s = "";
    s = m_header.toString(brief);
    for (int i=0; i<m_lines.count(); ++i)
    {
        s = s + "\n" + m_lines[i].toString(brief);
    }
    return s;
}

bool CSVReader::readNLines(int n, bool clearBeforeReading)
{
    bool noError = true;
    if (countHeaderColumns() == 0)
    {
        noError = readHeader(clearBeforeReading);
    }
    // Finish without error if there is nothing more to read.
    for (int i=0; noError && i<n && hasChar(); ++i)
    {
        if (i==0)
        {
            noError = readNextRecord(clearBeforeReading);
        }
        else
        {
            noError = readNextRecord(false);
        }
        QString s = QString("Number of lines is %1").arg(m_lines.count());
        qDebug(qPrintable(s));
    }
    return noError;
}

bool CSVReader::readNextRecord(bool clearBeforeReading)
{
    if (!clearBeforeReading || m_lines.count() == 0)
    {
        m_lines.append(CSVLine());
    }
    else if (m_lines.count() == 1)
    {
        // There is only one, so use the existing one.
        m_lines[0].clear();
    }
    else
    {
        m_lines.clear();
        m_lines.append(CSVLine());
    }

    CSVReadMode mode = CSVReadMode::MODE_RECORD_START;

    recordInitialization();
    columnInitialization();
    bool textDelimiterFound = false;
    QString currentColumn = "";

    while (hasChar() && !m_entireRecordRead && mode != CSVReadMode::MODE_ERROR && mode)
    {
        switch (mode)
        {
        case CSVReader::MODE_RECORD_START:
            // Skip comments
            while (hasChar() && (isComment(currentChar())))
            {
                readToNextLine();
            }
            mode = CSVReader::MODE_COLUMN_START;
            break;

        case CSVReader::MODE_COLUMN_START:
            textDelimiterFound = false;
            currentColumn = "";
            if (isTextDelimiter(currentChar()))
            {
                textDelimiterFound = true;
                mode = CSVReader::MODE_COLUMN_READ;
                moveToNextChar();
            }
            else if (isColumnDelimiter(currentChar()))
            {
                // Set to a null string because was totally empty.
                endOfColumnReached(QString(), textDelimiterFound);
                mode = CSVReader::MODE_COLUMN_START;
                moveToNextChar();
            }
            else if (isRecordDelimiter(currentChar()))
            {
                // Set to a null string because was totally empty.
                endOfColumnReached(QString(), textDelimiterFound);
                endOfRecordReached();
            }
            else
            {
                // Do NOT move to the next character.
                // This way process for escapes can be done all in one location.
                mode = CSVReader::MODE_COLUMN_READ;
            }
            break;

        case CSVReader::MODE_COLUMN_READ:
            // We are in read mode, which means that the last character was NOT
            // the text Delimiter (ie, a double quote).
            if (isTextDelimiter(currentChar()))
            {
                moveToNextChar();
                if (textDelimiterFound)
                {
                    if (!hasChar())
                    {
                        endOfColumnReached(currentColumn, textDelimiterFound);
                        endOfRecordReached();
                    }
                    else if (isTextDelimiter(currentChar()))
                    {
                        // A second text Delimiter was found, so use
                        // the text Delimiter
                        currentColumn = currentColumn + currentChar();
                        if (!moveToNextChar())
                        {
                            // TODO: End of record with an open double quote!
                            endOfColumnReached(currentColumn, textDelimiterFound);
                        }
                    }
                    else
                    {
                        // end of column is not considered reached unless we find
                        // a new column character, or a new record character.
                        // Perhaps there is another text Delimiter.
                        // endOfColumnReached(currentColumn, textDelimiterFound);
                        mode = CSVReader::MODE_COLUMN_END;
                    }
                }
                else
                {
                    // Found a text Delimiter and one was not previously found.
                    // This should be an error. But.... Will simply ignore the preceding stuff.
                    // TODO: Warning?
                    currentColumn = "";
                    textDelimiterFound = true;
                }
            }
            else if ( isEscapeCharacter(currentChar()))
            {
                QChar c = currentChar();
                if (!moveToNextChar())
                {
                    endOfColumnReached(currentColumn + c, textDelimiterFound);
                    endOfRecordReached();
                    if (textDelimiterFound)
                    {
                        // TODO: End of record with an open double quote!
                    }
                }
                else if (isEscapeCharacter(currentChar()))
                {
                    currentColumn = currentColumn + c;
                }
                else
                {
                    if (currentChar() == 'a') {
                        currentColumn = currentColumn +  '\a';
                        moveToNextChar();
                    } else if (currentChar() == 'b') {
                        currentColumn = currentColumn +  '\b';
                        moveToNextChar();
                    } else if (currentChar() == 'e') {
                        currentColumn = currentColumn +  '\e';
                        moveToNextChar();
                    } else if (currentChar() == 'f') {
                        currentColumn = currentColumn +  '\f';
                        moveToNextChar();
                    } else if (currentChar() == 'n') {
                        currentColumn = currentColumn +  '\n';
                        moveToNextChar();
                    } else if (currentChar() == 'r') {
                        currentColumn = currentColumn +  '\r';
                        moveToNextChar();
                    } else if (currentChar() == 't') {
                        currentColumn = currentColumn +  '\t';
                        moveToNextChar();
                    } else if (currentChar() == 'v') {
                        currentColumn = currentColumn +  '\v';
                        moveToNextChar();
                    } else if (currentChar() == 'u' || currentChar() == 'U') {
                        // Process unicode character.
                        uint c = 0;
                        int cLen = 0;
                        moveToNextChar();
                        while (hasChar() && cLen < 4 && isHexDigit(currentChar()));
                        {
                            c = c * 16 + hexDigitToDecimalValue(currentChar());
                            moveToNextChar();
                            ++cLen;
                        }
                        currentColumn = currentColumn + QChar(c);
                    } else if (currentChar() == '0') {
                        // TODO: Check for numbers beginning with a 0.
                        if (moveToNextChar())
                        {
                            uint c = 0;
                            int cLen = 0;
                            if (currentChar() == 'x')
                            {
                                moveToNextChar();
                                // Hex number
                                while (hasChar() && cLen < 2 && isHexDigit(currentChar()));
                                {
                                    c = c * 16 + hexDigitToDecimalValue(currentChar());
                                    moveToNextChar();
                                    ++cLen;
                                }
                                currentColumn = currentColumn + QChar(c);
                            }
                            else if (isOctDigit(currentChar()))
                            {
                                // Octal Number
                                do
                                {
                                    c = c * 8 + currentChar().digitValue();
                                    moveToNextChar();
                                    ++cLen;
                                }
                                while (hasChar() && cLen < 3 && isOctDigit(currentChar()));
                                currentColumn = currentColumn + QChar(c);
                            }
                            else
                            {
                                // Take the "0" character as is.
                                currentColumn = currentColumn + '\0';
                            }
                        }
                    } else if (currentChar().isDigit()) {
                        // Decimal number (ascii)
                        uint c = 0;
                        int cLen = 0;
                        do
                        {
                            c = c * 10 + currentChar().digitValue();
                            moveToNextChar();
                            ++cLen;
                        }
                        while (hasChar() && cLen < 3 && currentChar().isDigit());
                        currentColumn = currentColumn + QChar(c);
                    } else {
                        currentColumn = currentColumn +  currentChar();
                        moveToNextChar();
                    }
                }
            }
            else if (textDelimiterFound)
            {
                currentColumn = currentColumn + currentChar();
                moveToNextChar();
            }
            else if (isColumnDelimiter(currentChar()))
            {
                endOfColumnReached(currentColumn, textDelimiterFound);
                mode = CSVReadMode::MODE_COLUMN_START;
                moveToNextChar();
            }
            else if (isRecordDelimiter(currentChar()))
            {
                endOfColumnReached(currentColumn, textDelimiterFound);
                endOfRecordReached();
            }
            else
            {
                currentColumn = currentColumn + currentChar();
                moveToNextChar();
            }
            break;

        case CSVReader::MODE_COLUMN_END:
            while (hasChar() && !isColumnDelimiter(currentChar()) && !isRecordDelimiter(currentChar()))
            {
                moveToNextChar();
            }
            endOfColumnReached(currentColumn, textDelimiterFound);
            if (hasChar() && isColumnDelimiter(currentChar()))
            {
                moveToNextChar();
                mode = CSVReader::MODE_COLUMN_START;
            }
            else
            {
                endOfRecordReached();
            }
            break;

         case CSVReadMode::MODE_ERROR:
            // I should NEVER get here!
            return false;
            break;
        }
    }
    // TODO: Continue from here
    return m_entireRecordRead;
}

bool CSVReader::canReadFromStream() const
{
    return m_inStream != nullptr && !m_inStream->atEnd() && m_inStream->status() ==  QTextStream::Ok;
}

bool CSVReader::setStreamFromPath(const QString& fullPath)
{
    cleanup();
    m_file = new QFile(fullPath);
    if (!m_file->exists() || !m_file->open(QIODevice::ReadOnly))
    {
        delete m_file;
        m_file = nullptr;
    }
    else
    {
        m_inStream = new QTextStream(m_file);
        if (!moveToNextChar())
        {
            cleanup();
        }
    }
    return hasChar();
}

QString CSVReader::read( qint64 maxlen )
{
    if (maxlen < 0)
    {
        // Default to 10MB in size.
        maxlen = 10485760;
    }
    return canReadFromStream() ? m_inStream->read(maxlen) : QString();
}

void CSVReader::setColumnType(const int i, const QMetaType::Type t)
{
  if (m_columnTypes == nullptr)
  {
    guessColumnTypes();
  }
  if (m_columnTypes != nullptr && i >= 0)
  {
    while (i >= m_columnTypes->size())
    {
      m_columnTypes->append(t);
    }
    m_columnTypes->replace(i, t);
  }
}

QMetaType::Type CSVReader::guessColumnType(const int i, TypeMapper::ColumnConversionPreferences flags)
{
  if (m_columnTypes == nullptr)
  {
    guessColumnTypes(flags);
  }
  return (m_columnTypes == nullptr) || (i < 0) || (i > m_columnTypes->size()) ? QMetaType::Void : m_columnTypes->at(i);
}

void CSVReader::guessColumnTypes(TypeMapper::ColumnConversionPreferences flags)
{
  if (m_columnTypes == nullptr)
  {
    m_columnTypes = new QList<QMetaType::Type>();
  }

  TypeMapper typeMapper;

  for (int iRow = 0; iRow < m_lines.count(); ++iRow)
  {
    const CSVLine& line = m_lines[iRow];
    for (int iCol=0; iCol < line.count(); ++ iCol)
    {
      if (iCol >= m_columnTypes->count())
      {
        m_columnTypes->append(line[iCol].getType());
      }
      else
      {
        m_columnTypes->replace(iCol, typeMapper.mostGenericType(m_columnTypes->at(iCol), line[iCol].getType()));
      }
    }
  }
  QString s;
  for (int i=0; i<m_columnTypes->count(); ++i)
  {
    if (i == 0)
    {
      s = QMetaType::typeName(m_columnTypes->at(i));
    }
    else
    {
      s = s + ", " + QMetaType::typeName(m_columnTypes->at(i));
    }
  }
  qDebug(qPrintable(s));
}


QMetaType::Type CSVReader::getColumnType(const int i) const
{
  return (m_columnTypes != nullptr && 0 <= i && i<m_columnTypes->size()) ? m_columnTypes->at(i) : QMetaType::Void;
}

QVariant CSVReader::getNullVariant(const int i) const
{
  TypeMapper typeMapper;
  return typeMapper.getNullVariant(getColumnType(i));
}


int CSVReader::getHeaderIndexByName(const QString& name) const
{
  return getHeader().indexOf(name, 0, Qt::CaseInsensitive);
}

QList<int> CSVReader::getHeaderIndexByName(const QStringList& names) const
{
  QList<int> indexList;
  for (int i=0; i<names.size(); ++i)
  {
    indexList << getHeaderIndexByName(names[i]);
  }
  return indexList;
}

