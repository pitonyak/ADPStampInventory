#include "csvline.h"

CSVLine::CSVLine(QObject *parent) :
    QObject(parent),
    m_originalLine(nullptr)
{
}

CSVLine::CSVLine(const CSVLine& x, QObject *parent) :
    QObject(parent),
    m_originalLine(nullptr)
{
    copyFrom(x);
}

CSVLine::CSVLine(const QString& originalLine, QObject *parent) :
    QObject(parent),
    m_originalLine(new QString(originalLine))
{
}

CSVLine::~CSVLine()
{
    clearOriginalLine();
}

void CSVLine::setOriginalLine(const QString& originalLine)
{
    if (hasOriginalLine())
    {
        *m_originalLine = originalLine;
    }
    else
    {
        m_originalLine = new QString(originalLine);
    }
}

int CSVLine::count() const
{
    return m_columns.count();
}

int CSVLine::indexOf(const QString& value, int from) const
{
    for (int i=from; i<m_columns.count(); ++i)
    {
        if (value.compare(m_columns[i].getValue(), Qt::CaseInsensitive) == 0)
        {
            return i;
        }
    }
    return -1;
}

void CSVLine::clearOriginalLine()
{
    if (m_originalLine != nullptr)
    {
        delete m_originalLine;
        m_originalLine = nullptr;
    }
}

const CSVLine& CSVLine::operator=(const CSVLine& x)
{
    return copyFrom(x);
}

const CSVLine& CSVLine::copyFrom(const CSVLine& x)
{
    if (this != &x)
    {
        if (x.hasOriginalLine())
        {
            setOriginalLine(x.getOrignalLine());
        }
        else
        {
            clearOriginalLine();
        }
        m_columns.clear();
        m_columns.append(x.m_columns);
    }
    return *this;
}

QString CSVLine::toString(bool brief) const
{
    QString s = "";
    for (int i=0; i<m_columns.count(); ++i)
    {
        if (i > 0)
        {
            s = s + ", ";
        }
        s = s + m_columns[i].toString(brief);
    }
    return s;
}

QStringList CSVLine::toStringList() const
{
    QStringList list;
    for (int i=0; i<m_columns.count(); ++i)
    {
        list.append(m_columns[i].getValue());
    }
    return list;
}
