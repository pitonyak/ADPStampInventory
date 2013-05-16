#ifndef CSVWRITER_H
#define CSVWRITER_H

#include "csvcontroller.h"

class QFile;
class QTextStream;

class CSVWriter : public CSVController
{
    Q_OBJECT
public:
    explicit CSVWriter(QObject *parent = 0);
    virtual ~CSVWriter();
    void cleanup();

    void write(const QString& s);
    void write(const QChar& c);
    void write(const CSVLine& csvLine, bool includeRecordSeparator = true);
    void writeColumnSeparator();
    void writeRecordSeparator();

    //**************************************************************************
    //! Convert a column value to a string for output.
    /*!
     * \param columnValue
     * \returns A string representation as it should be written to the CSV file.
     *
     ***************************************************************************/
    QString prepForWriting(const QVariant& columnValue);

    bool setStreamFromPath(const QString& fullPath);
    bool setStreamWriteToString(QString* s);
    void writeHeader();
    void writeLines(int firstIndex=0, int num=-1);
    void write(const CSVColumn& column);
    bool canWriteToStream() const;

signals:
    
public slots:

private:
    QFile* m_file;
    QTextStream* m_outStream;
};

#endif // CSVWRITER_H
