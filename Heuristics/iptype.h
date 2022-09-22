#ifndef IPTYPE_H
#define IPTYPE_H

#include <unordered_map>
#include <string>

//**************************************************************************
//! Encapsulate an IP Type
/*!
 * This is complicated by fact that we may want or need a specific port with the 
 * type to make a determination. So, specific behavior will be defined here. 
 * 
 * For a specific type, if there is a port of -1, then that is used if the port
 * is not found. If there is not port of -1 then the port must match. 
 *
 ***************************************************************************/


class IPType {
public:
	/*! Constructor */
	IPType();

	/*! Constructor */
	IPType(const IPType& ipt);

	/*! Assignment operator */
	const IPType& operator=(const IPType& ipt);

    /*! Desctructor, clears all structures, not virtual because assume not subclassed. */
    ~IPType();

    // Normally I would encapulate this with getters and setters, but not sure it is worth it at this point.
    // Perhaps I should just make this a struct.
	int  m_iPType; // Integer type or Protocol.
	bool m_valid;  // Do we consider this valid?
	bool m_dupIP;  // Does this allow or expect a duplicate IP in the payload?
	bool m_dupMAC; // Does this allow or expect a duplicate MAC in the payload?
	int  m_port;   // Port or -1 for port does not matter
	std::string m_description;

    /*! Simply method to print this object as a line with no context. */
	std::ostream& print(std::ostream& x) const;
};


class IPTypes {
public:
	/*! Constructor */
	IPTypes();

    /*! Destructor, clears all structures, not virtual because assume not subclassed. */
    ~IPTypes();

    //**************************************************************************
    //! Add the IP Type
    /*!
     * \param [in] ipt Constant reference to the object. If this type is already there, it is replaced.
     *
     ***************************************************************************/
  	void addType(const IPType& ipt);

    //**************************************************************************
    //! Add the IP Type
    /*!
     * \param [in] ipt Constant reference to the object. If this type is already there, it is replaced.
     * 
     * \param [in] ownit If true, the pointer is now owned and deleted by this class. if false, a copy is made.
     *
     ***************************************************************************/
	void addType(IPType* ipt, bool ownit = true);

    //**************************************************************************
    //! Get a pointer to the IPType object based on the numeric type.
    /*!
     * \param [in] protocol Numeric IP type. 
     * 
     * \returns Pointer to the contained type or nullptr if it is not contained. 
     *
     ***************************************************************************/
	const IPType* getIPType(int protocol, int port) const;

    //**************************************************************************
    //! Is this numeric type known. Does not mean it is considered valid for our purposes.
    /*!
     * \param [in] protocol Numeric IP type. 
     * 
     * \returns True if the type is contained in the object. 
     *
     ***************************************************************************/
	bool hasType(int protocol, int port) const;

    //**************************************************************************
    //! Is this numeric type considered a valid type to process.
    /*!
     * \param [in] protocol Numeric IP type. 
     * 
     * \returns True if the type should not be rejected.
     *
     ***************************************************************************/
	bool isValid(int protocol, int port) const;

    //**************************************************************************
    //! Does this numeric type allow for the IP to be repeated in the payload?
    /*!
     * \param [in] protocol Numeric IP type. 
     * 
     * \returns True if this this numeric type allows for the IP to be repeated in the payload
     *
     ***************************************************************************/
	bool isDupIP(int protocol, int port) const;

    //**************************************************************************
    //! Does this numeric type allow for the MAC to be repeated in the payload?
    /*!
     * \param [in] protocol Numeric IP type. 
     * 
     * \returns True if this this numeric type allows for the MAC to be repeated in the payload
     *
     ***************************************************************************/
	bool isDupMAC(int protocol, int port) const;


    //**************************************************************************
    //! Read a list of IP types from a file.
    /*!
     * 
     * Valid IP types found here: 
     * https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
     * 
     * All columns should be present:
     * Numeric Type
     * Valid 0/1 - will the file be rejected (0) or processed (1)
     * IPv6 0/1 - 1 means only for IPv6
     * Duplicate IP possible or expected 0/1 in the payload.
     * Dupliate MAC possible or expected 0/1 in the payload.
     * Comment or description of the type.
     * 
     * The numeric type can be a range such as (5-7), but no spaces are allowed in a range.
     * 
     * All whitespace types are convereted to a space and multiple spaces are converted to a single space.
     * 
     * \param [in] filename Full path to the input file.
     * 
     * \param [in] base Numerical base (radix) that determines the valid characters and their interpretation.
     * If this is 0, the base used is determined by the format in the sequence. 
     * Notice that by default this argument is 10, not 0. ONLY the first column uses this parameter. 
     * All other columns use 10.
     * 
     * \returns True if no errors are encountered. 
     *
     ***************************************************************************/
    bool read(const std::string& filename, int base = 10);

    bool readProtocols(const std::string& filename, bool hasPort, int base = 10);
    bool readProtocolPorts(const std::string& filename);

    std::ostream& print(std::ostream& x) const;

private:
	/*! Copy Constructor and I do not want it to be called, that will be bad bad bad. */
	IPTypes(const IPTypes&);

	std::unordered_map<int, std::unordered_map<int, IPType *>* > m_ipTypes;
};

inline bool IPTypes::isValid(int protocol, int port) const {
	const IPType* ipt = getIPType(protocol, port);
	return (ipt != nullptr) ? ipt->m_valid : false;
}

inline bool IPTypes::isDupIP(int protocol, int port) const {
	const IPType* ipt = getIPType(protocol, port);
	return (ipt != nullptr) ? ipt->m_dupIP : false;
}

inline bool IPTypes::isDupMAC(int protocol, int port) const {
	const IPType* ipt = getIPType(protocol, port);
	return (ipt != nullptr) ? ipt->m_dupMAC : false;
}


inline std::ostream& operator <<(std::ostream& x, const IPType& y) {
	return y.print(x);
}

inline std::ostream& operator <<(std::ostream& x, const IPTypes& y) {
	return y.print(x);
}

#endif // IPTYPE_H
