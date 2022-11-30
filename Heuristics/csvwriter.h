#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "utilities.h"

//**************************************************************************
//! Encapsulate a collection of IP addresses.
/*!
  *
 ***************************************************************************/
class CSVWriter {
public:
    /*! Constructor */
    CSVWriter();

    //**************************************************************************
    //! Add a IP if it is NOT already stored here. NULL is ignored. A copy is made.
    /*!
     * \param [in]  ip Four or 16 bytes for the IP address.
     * 
     * \param [in] isIPv4 True if IPv4, otherwise IPv6
     * 
     * \param [in] ownIt, if true, you own this pointer and delete it. 
     * 
     * \returns True if added.
     *
     ***************************************************************************/
    CSVWriter(const std::string filename, const std::string separator = ",", const std::string quote = "\"");
    CSVWriter(std::ostream* o_stream, const std::string separator = ",", const std::string quote = "\"");

    /*! Destructor */
    ~CSVWriter();

    void flush();

    void endRow();

	//! Increment the current column and stream a column delimiter if this is NOT the first column.
    void newCol();

    std::string quoteString(const std::string& s) const;

    CSVWriter& operator << (const std::string& x);
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
    /*! Constructor */
    CSVWriter(const CSVWriter& x);

    std::string m_filename;
    std::string m_separator;
    std::string m_quote;
    bool m_quote_all_strings;
    std::string m_quoteX2;
    int m_column;
	std::ofstream* m_ofstream;
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