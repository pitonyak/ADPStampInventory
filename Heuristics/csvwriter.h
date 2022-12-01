#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "utilities.h"

//**************************************************************************
//! Assist in writing CSV files.
/*!
  *
  * There are some caveats with this class and some cheats were performed
  * to simply the coding.
  * 
  * There is minimal to no error checking. Exceptions are not intentional thrown.
  * Files are not checked to see if they exist or write access exists in directories.
  * This is a minimal class with almost the least work possible to obtain a working solution.
  * 
  * You can specify the delimiter and the quote but they default to 
  * a comma and double quote.
  * 
  * ALL strings are quoted rather than checking if the string must be quoted.
  * Strings do not need to be quoted unless they contain 
  * a comma, a double quote, or embedded line breaks.
  * Note that many CSV implementations do not support reading data with 
  * embedded line breaks.
  * 
  * RFC 4180 forbids triming strings, and spaces outside of the quoted strings
  * are not allowed, so extra spaces are not added.
  * 
  * Regardless of the quote character, which is allows by this code to be an
  * arbitrary string, all strings are preceded and followed by the quote string.
  * Also, All instances of the quote character / string is replaced by two
  * instances. So, the text:
  * 
  * a"b
  * 
  * is writting as
  * 
  * "a""b"
  * 
  * In locales where the comma is used as a decimal separator, a semicolon, 
  * TAB, or other character is used instead of a comma to separate fields. 
  * This code does not attempt to understand this and always uses a comma
  * unless it is set differently when the object is created.
  * 
  * Every time something is "streamed" to the CSV file it is assumed
  * to be a new field / column. A comma is automatically inserted between
  * each field as needed. You can insert an empty column by calling newCol().
  * 
  * 
  * 
 ***************************************************************************/
class CSVWriter {
public:
    /*! Constructor */
    CSVWriter();

    //**************************************************************************
    //! Constructor
    /*!
     * A file is created and opened immediately. The file is closed in the destructor.
     * 
     * \param [in] filename - A new file is created in the constructor with this name.
     * 
     * \param [in] separator - defaults to a comma (,)
     * 
     * \param [in] quote Defaults to a double quote (")
     * 
     ***************************************************************************/
    CSVWriter(const std::string filename, const std::string separator = ",", const std::string quote = "\"");

    //**************************************************************************
    //! Constructor
    /*!
     * This allows for the creation and use of your own stream that is owned and
     * destroyed by the caller. This exists so that things such as a string string
     * can be used. For example:
     * 
     * @code{.cpp}
     * std::ostringstream strStream;
     *   std::vector<std::string> heading;
     *  heading.push_back("#");
     *  heading.push_back("heading 1");
     *  heading.push_back("heading 2");
     *  std::ostringstream strStream;
     *  CSVWriter csv(&strStream);
     *  csv << heading;
     *  csv.endRow();
     *  csv << 0 << "zero" << "ZERO";
     *  csv.endRow();
     *  csv << 1 << "one" << "ONE";
     *  csv.flush();
     *  std::string s2 = strStream.str();
     * @endcode
     * 
     * \param [in] o_stream - An output stream that is still owned by the caller.
     * 
     * \param [in] separator - defaults to a comma (,)
     * 
     * \param [in] quote Defaults to a double quote (")
     * 
     ***************************************************************************/
    CSVWriter(std::ostream* o_stream, const std::string separator = ",", const std::string quote = "\"");

    /*! Destructor. Calls flush(). Closes the file if it exists. */
    ~CSVWriter();

    /*! Calls flush on the stream. This is useful with a string stream. */
    void flush();

    /*! Prints a new line in the file and reset the column counter to zero. */
    void endRow();

    //**************************************************************************
    //! Increment column counter and stream a column delimiter if this is NOT the first column.
    /*!
     * Use newCol() to add an empty field without quotes. Streaming an empty string
     * in this implementation inserts "".
     * 
     ***************************************************************************/
    void newCol();

    /*! Replaces all instances of the quote string with two instances of the quote string. */
    std::string quoteString(const std::string& s) const;

    //**************************************************************************
    //! Adds a string field that is properly quoted.
    /*!
     * Every new field has a leading delimeter (,) added as needed.
     * 
     * Strings are always surrounded by quotes, even when the field is empty 
     * so an empty string is always written as "". Use newCol() to include an unquoted empty column.
     * 
     * All internal quotes are converted to be two so 'A"B' is converted to be 'A""B'
     * 
     * \param [in] x - String to stream.
     * 
     * \returns Reference to this object so that multiple things can be streamed in a single statement.
     * 
     ***************************************************************************/
    CSVWriter& operator << (const std::string& x);

    /*! Stream each entry as a field. This is useful to stream headers. This does not call endRow(). */
    CSVWriter& operator << (const std::vector<std::string>& v);

    CSVWriter& operator << (float x);
    CSVWriter& operator << (double x);
    CSVWriter& operator << (char x);
    CSVWriter& operator << (int x);
    CSVWriter& operator << (short x);
    CSVWriter& operator << (long x);
    CSVWriter& operator << (long long x);
    CSVWriter& operator << (unsigned char x);
    CSVWriter& operator << (unsigned int x);
    CSVWriter& operator << (unsigned short x);
    CSVWriter& operator << (unsigned long x);
    CSVWriter& operator << (unsigned long long x);


private:
    //**************************************************************************
    //! Copy Constructor.
    /*!
     * This object is not implemented. I have no particular use for one and
     * this is an attempt to make it more difficult to call. 
     * 
     * Calling from outside the class is a compile error. 
     * 
     * Calling from inside the class is a link error. 
     * 
     * Copying streams is messy and I do not want to implement this. 
     * If this object is called, the user probaby really wanted to 
     * pass a reference to the object and not an object so this will
     * probably catch a bug in the code anyway.
     * 
     * \param [in] x - Object to copy.
     * 
     ***************************************************************************/
    CSVWriter(const CSVWriter& x);

    /*! Filename if this is a file. */
    std::string m_filename;

    /*! This is probably a comma (,) */
    std::string m_separator;

    /*! This is probably a double quote (") */
    std::string m_quote;

    /*! This is the quote twice, so probably ("") */
    std::string m_quoteX2;

    /*! Next column number that will be streamed. Starts at zero (so ready for the first column). */
    int m_column;

    //**************************************************************************
    //! Created if this is a file stream. Owned by this class.
    /*!
     * This object is created in the constructor and owned by the class.
     * The ofstream is a subclass of ostream so m_ostream is immediately 
     * set to reference this object. The file stream object is used to
     * close the file as needed but otherwise m_ostream is used to do most things.
     * 
     ***************************************************************************/
	std::ofstream* m_ofstream;

    /*! The primary stream used for operations. */
	std::ostream* m_ostream;
};

inline void CSVWriter::flush() {
	if (m_ostream != nullptr)
		m_ostream->flush(); 
}

inline void CSVWriter::endRow()  { 
	if (m_ostream != nullptr)
		*m_ostream << std::endl; 
	m_column = 0; 
}

inline std::string CSVWriter::quoteString(const std::string& s) const { 
	return replaceAll(s, m_quote, m_quoteX2); 
}

inline CSVWriter& CSVWriter::operator << (const std::string& x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << m_quote << quoteString(x) << m_quote;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (float x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (double x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (char x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (short x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (int x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (long x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (long long x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (unsigned char x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (unsigned short x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (unsigned int x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (unsigned long x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

inline CSVWriter& CSVWriter::operator << (unsigned long long x) { 
	newCol(); 
	if (m_ostream != nullptr)
		*m_ostream << x;
	return *this;
}

#endif