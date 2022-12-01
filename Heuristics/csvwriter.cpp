
#include "csvwriter.h"


CSVWriter::CSVWriter() : 
		m_filename(""), m_separator(","), m_quote("\""), 
		m_quoteX2("\"\""), m_column(0), 
		m_ofstream(nullptr), m_ostream(nullptr)
{
}

CSVWriter::CSVWriter(const std::string filename, const std::string separator, const std::string quote) : CSVWriter() {
	m_separator = separator;
	m_quote = quote;
	m_filename = filename;
	if (!filename.empty()) {
		m_ofstream = new std::ofstream();
		// m_ofstream->exceptions(std::ios::failbit | std::ios::badbit);
		// then can use catch(std::ofstream::failure &e) 
		// then use e.what() to see what happened.
        m_ofstream->open(filename);
        m_ostream = m_ofstream;
	}
}

CSVWriter::CSVWriter(std::ostream* o_stream, const std::string separator, const std::string quote) : CSVWriter() {
	m_separator = separator;
	m_quote = quote;
	m_ostream = o_stream;
}

CSVWriter::~CSVWriter() {
	flush();
	if (m_ofstream != nullptr) {
		m_ofstream->close();
		delete m_ofstream;
		m_ofstream = nullptr;
	}
}

void CSVWriter::newCol() {
	if (m_column > 0 && m_ostream != nullptr)
		*m_ostream << m_separator;
	++m_column;
}

CSVWriter& CSVWriter::operator << (const std::vector<std::string>& v) {
	for (const std::string& s : v) {
		*this << s;
	}
	return *this;
}
