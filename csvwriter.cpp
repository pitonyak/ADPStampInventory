#include "csvwriter.h"

#include <QFile>
#include <QTextStream>

CSVWriter::CSVWriter(QObject *parent) :
    CSVController(parent),
    m_file(nullptr),
    m_outStream(nullptr)
{
}

CSVWriter::~CSVWriter()
{
    cleanup();
}

QString CSVWriter::prepForWriting(const QVariant& columnValue)
{
    QVariant x(columnValue);
    if (!x.convert(QMetaType(QMetaType::QString)))
    {
        // TODO: Error

    }
    else
    {
        return makeSafe(reduceSpaces(x.toString()));
    }
    return QString();
}

void CSVWriter::cleanup()
{
    if (m_file != nullptr)
    {
        if (m_file->isOpen())
        {
            m_file->close();
        }
        delete m_file;
        m_file = nullptr;
    }
    if (m_outStream != nullptr)
    {
        delete m_outStream;
        m_outStream = nullptr;
    }
}

bool CSVWriter::setStreamWriteToString(QString* s)
{
    cleanup();
    if (s != nullptr)
    {
        m_outStream = new QTextStream(s, QIODevice::WriteOnly);
    }
    return canWriteToStream();
}

bool CSVWriter::canWriteToStream() const
{
    return m_outStream != nullptr && m_outStream->status() ==  QTextStream::Ok;
}

bool CSVWriter::setStreamFromPath(const QString& fullPath)
{
    cleanup();
    m_file = new QFile(fullPath);
    // It is assumed that the user has already verified that
    // replacing an existing file is OK.
    if (!m_file->open(QIODevice::WriteOnly))
    {
        delete m_file;
        m_file = nullptr;
        return false;
    }
    m_outStream = new QTextStream(m_file);
    return true;
}

void CSVWriter::write(const QString& s)
{
    if (canWriteToStream())
    {
        *m_outStream << s;
    }
}

void CSVWriter::write(const QChar& c)
{
    if (canWriteToStream())
    {
        *m_outStream << c;
    }
}

void CSVWriter::writeColumnSeparator()
{
    write(getColumnDelimiter());
}

void CSVWriter::writeRecordSeparator()
{
    if (getRecordDelimiterIsDefault())
    {
        write("\n");
    }
    else
    {
        write(getRecordDelimiter());
    }
}

void CSVWriter::write(const CSVColumn& column)
{
    QString s = reduceSpaces(column.getValue());
    if (s.length() > 0)
    {
        if (column.isQualified())
        {
            write(getTextDelimiter());
            write(makeSafe(s));
            write(getTextDelimiter());
        }
        else
        {
            write(s);
        }
    }
}

void CSVWriter::write(const CSVLine& csvLine, bool includeRecordSeparator)
{
    if (csvLine.size() > 0)
    {
        for (int i=0; i<csvLine.size() && canWriteToStream(); ++i)
        {
            if (i>0)
            {
                writeColumnSeparator();
            }
            write(csvLine[i]);
        }
        if (includeRecordSeparator)
        {
            writeRecordSeparator();
        }
    }
}

void CSVWriter::writeHeader()
{
    write(m_header);
}

void CSVWriter::writeLines(int firstIndex, int num)
{
    if (firstIndex < 0)
    {
        firstIndex = 0;
    }
    if (num < 0)
    {
        num = m_lines.size();
    }
    for (int i=firstIndex; i<m_lines.size() && num > 0; --i, --num)
    {
        write(m_lines[i]);
    }
}

