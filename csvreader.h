#ifndef CSVREADER_H
#define CSVREADER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include "csvcontroller.h"
#include "typemapper.h"

//**************************************************************************
/*! \class CSVReader
 *  \brief Parse CSV files that are assumed to be small enough to fit into memory.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2020
 *
 **************************************************************************/

class QIODevice;
class QFile;
class QTextStream;

class CSVReader : public CSVController
{
  Q_OBJECT
public:
  //**************************************************************************
  //! Modes for a simple state machine while parsing a record.
  /*!
   * MODE_COLUMN_START means that we are looking for a column start.
   *
   *
   **************************************************************************/
  enum CSVReadMode {MODE_ERROR, MODE_RECORD_START, MODE_COLUMN_START, MODE_COLUMN_READ, MODE_COLUMN_END};


  explicit CSVReader(QObject *parent = nullptr);
  explicit CSVReader(const TypeMapper::ColumnConversionPreferences preferences, QObject *parent = nullptr);
  virtual ~CSVReader();

  bool setStreamFromPath(const QString& fullPath);

  //**************************************************************************
  //! Begin Parsing CSV contained in a string assuming that everything is in this string.
  /*!
   * \param s The string to parse.
   * \returns True if initial parsing worked fine.
   *
   ***************************************************************************/
  bool setStreamFromString(const QString& s);

  //**************************************************************************
  //! Begin Parsing CSV contained in the open device. It is assumed that readAll reads everything available.
  /*!
   * The current locale is assumed.
   * \param device Opened device containing the CSV to parse.
   * \returns True if initial parsing worked fine.
   *
   ***************************************************************************/
  bool parseFromDevice(QIODevice* device);

  //**************************************************************************
  //! Begin Parsing CSV contained in the open device. It is assumed that readAll reads everything available.
  /*!
   * \param device Opened device containing the CSV to parse.
   * \param codecName Name of the codec to use for parsing; so that other locales can be specified.
   * \returns True if initial parsing worked fine.
   *
   ***************************************************************************/
  // TODO: ?? bool parseFromDevice(QIODevice* device, const char* codecName);

  //**************************************************************************
  //! Read the header record. Headers are always cleared before reading, so previous headers are lost.
  /*!
   * \param clearBeforeReading If true, then lines are cleared before reading.
   * \returns True if no errors are encountered.
   *
   ***************************************************************************/
  bool readHeader(bool clearBeforeReading = true);

  //**************************************************************************
  //! Read the next CSV record.
  /*!
   * \param clearBeforeReading If true, lines are cleared before reading, otherwise, it is appended to the list.
   * \returns True if no errors are encountered.
   *
   ***************************************************************************/
  bool readNextRecord(bool clearBeforeReading = true);

  //**************************************************************************
  //! Try to read the specified number of lines.
  /*!
   * If the header has not yet been read, then the header is read before lines are read.
   * Returns without error if no more characters are available for reading.
   *
   * \param n Number of lines to try to reqad.
   * \param clearBeforeReading If true, lines are cleared before reading, otherwise, it is appended to the list.
   * \returns True if no errors are encountered. This does not imply that n lines were read.
   *
   ***************************************************************************/
  bool readNLines(int n, bool clearBeforeReading = true);

  QChar currentChar();
  bool moveToNextChar();

  //**************************************************************************
  //! Check if a character is ready to be read. moveToNextChar must be called before the first character can be read.
  /*!
   * \returns True the buffer is ready to return a single character.
   *
   ***************************************************************************/
  bool hasChar();

  //**************************************************************************
  //! Check if a previous character is set. This is useful to see if two double quotes are next to each other.
  /*!
   * \returns True if the "last character" property is set.
   *
   ***************************************************************************/
  bool hasLastChar() const;

  //**************************************************************************
  //!
  /*!
   * \returns The character before the current character.
   *
   ***************************************************************************/
  QChar lastChar() const;

  //**************************************************************************
  //! Move the buffer pointer to the first character after a CR and/or LF.
  /*!
   * A new line is considered CR/LF, LF/CR, CR, or LF.
   * \returns True if a new line is found.
   *
   ***************************************************************************/
  bool readToNextLine();

  //**************************************************************************
  //! Check if the stream exists, is open, and not at the end.
  /*!
   * \returns True if the stream can be read from and is not at the end.
   *
   ***************************************************************************/
  bool canReadFromStream() const;

  //**************************************************************************
  //! Reads at most maxlen characters from the stream, and returns the data read as a QString.
  /*!
   * \param maxlen
   * \returns Data read. If canReadFromStream returns false, than a NULL string is returned.
   *
   ***************************************************************************/
  QString read( qint64 maxlen );

  QString toString(bool brief = true) const;

  void setColumnType(const int i, const QMetaType::Type t);

  QMetaType::Type guessColumnType(const int i, TypeMapper::ColumnConversionPreferences flags=TypeMapper::PreferNone);

  //**************************************************************************
  //! Look at the read data and guess the column types based on the data present.
  /*!
   * \param flags
   *
   ***************************************************************************/
  void guessColumnTypes(TypeMapper::ColumnConversionPreferences flags=TypeMapper::PreferNone);

  QMetaType::Type getColumnType(const int i) const;
  QVariant getNullVariant(const int i) const;

  int getHeaderIndexByName(const QString& name) const;
  QList<int> getHeaderIndexByName(const QStringList& names) const;

signals:

public slots:

private:
  /*! Define it but do not implement it to prevent this from being used. */
  CSVReader(const CSVReader& reader);

  /*! Define it but do not implement it to prevent this from being used. */
  CSVReader& operator=(const CSVReader& reader);

  void readerInitialization();
  void recordInitialization();
  void columnInitialization();
  void cleanup();
  void setReaderDefaults();

  //**************************************************************************
  //! Called when the end of a record is reached.
  /*!
   * Allows for common tasks to be performed in one location.
   * Number of records read is incremented and a flag is set indicating
   * that a record was read in its entirety. Note that the header is
   * considered a record in this context.
   *
   ***************************************************************************/
  void endOfRecordReached();

  //**************************************************************************
  //! Called when the end of a column is reached.
  /*!
   * Allows for common tasks to be performed in one location.
   * Number of columns read is incremented, values are set for that column,
   * any desired white space elimination is performed.
   * \param columnValue last column value read.
   *
   ***************************************************************************/
  void endOfColumnReached(const QString& columnValue, bool wasDelimited);

  TypeMapper::ColumnConversionPreferences m_conversionPreferences;

  QList<QMetaType::Type> * m_columnTypes;

  bool m_entireRecordRead;
  int m_numberOfRecordsRead;
  qint64 m_maxBufferRead;

  QString m_readBufferString;
  int m_positionInBuffer;
  bool m_hasChar;

  QFile* m_file;
  QTextStream* m_inStream;
  bool m_hasLastChar;
  QChar m_lastChar;
  QString m_tempForTextStream;
};

inline bool CSVReader::hasLastChar() const
{
  return m_hasLastChar;
}

inline QChar CSVReader::lastChar() const
{
  return m_lastChar;
}


#endif // CSVREADER_H
