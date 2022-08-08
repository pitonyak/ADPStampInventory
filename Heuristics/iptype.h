#ifndef IPTYPE_H
#define IPTYPE_H

#include <unordered_map>
#include <string>

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
	int m_iPType;  // Integer type
	bool m_valid;  // Do we consider this valid?
	bool m_iPv6;   // Is this for IPv6?
	bool m_dupIP;  // Does this allow or expect a duplicate IP in the payload?
	bool m_dupMAC; // Does this allow or expect a duplicate MAC in the payload?
	std::string m_description;

    /*! Simply method to print this object as a line with no context. */
	std::ostream& print(std::ostream& x) const;
};


class IPTypes {
public:
	/*! Constructor */
	IPTypes();

    /*! Desctructor, clears all structures, not virtual because assume not subclassed. */
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
     * \param [in] iptype Numeric IP type. 
     * 
     * \returns Pointer to the contained type or nullptr if it is not contained. 
     *
     ***************************************************************************/
	const IPType* getIPType(int iptype) const;

    //**************************************************************************
    //! Is this numeric type known. Does not mean it is considered valid for our purposes.
    /*!
     * \param [in] iptype Numeric IP type. 
     * 
     * \returns True if the type is contained in the object. 
     *
     ***************************************************************************/
	bool hasType(int iptype) const;

    //**************************************************************************
    //! Is this numeric type considered a valid type to process.
    /*!
     * \param [in] iptype Numeric IP type. 
     * 
     * \returns True if the type should not be rejected.
     *
     ***************************************************************************/
	bool isValid(int iptype) const;

    //**************************************************************************
    //! Is this numeric type specific to IPv6.
    /*!
     * \param [in] iptype Numeric IP type. 
     * 
     * \returns True if this numeric type is specific to IPv6
     *
     ***************************************************************************/
    bool isIPv6(int iptype) const;

    //**************************************************************************
    //! Does this numeric type allow for the IP to be repeated in the payload?
    /*!
     * \param [in] iptype Numeric IP type. 
     * 
     * \returns True if this this numeric type allows for the IP to be repeated in the payload
     *
     ***************************************************************************/
	bool isDupIP(int iptype) const;

    //**************************************************************************
    //! Does this numeric type allow for the MAC to be repeated in the payload?
    /*!
     * \param [in] iptype Numeric IP type. 
     * 
     * \returns True if this this numeric type allows for the MAC to be repeated in the payload
     *
     ***************************************************************************/
	bool isDupMAC(int iptype) const;


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
     * \returns True if no errors are encountered. 
     *
     ***************************************************************************/
    bool read(const std::string& filename);

    std::ostream& print(std::ostream& x) const;

private:
	std::unordered_map<int, IPType *> m_ipTypes;
};


inline const IPType* IPTypes::getIPType(int iptype) const
{
	std::unordered_map<int, IPType *>::const_iterator it = m_ipTypes.find(iptype);
	return (it == m_ipTypes.end()) ? nullptr : it->second;
}

inline bool IPTypes::hasType(int iptype) const {
	return getIPType(iptype) != nullptr;
}

inline bool IPTypes::isValid(int iptype) const {
	const IPType* ipt = getIPType(iptype);
	return (ipt != nullptr) ? ipt->m_valid : false;
}

inline bool IPTypes::isIPv6(int iptype) const {
	const IPType* ipt = getIPType(iptype);
	return (ipt != nullptr) ? ipt->m_iPv6 : false;
}

inline bool IPTypes::isDupIP(int iptype) const {
	const IPType* ipt = getIPType(iptype);
	return (ipt != nullptr) ? ipt->m_dupIP : false;
}

inline bool IPTypes::isDupMAC(int iptype) const {
	const IPType* ipt = getIPType(iptype);
	return (ipt != nullptr) ? ipt->m_dupMAC : false;
}


inline std::ostream& operator <<(std::ostream& x, const IPType& y) {
	return y.print(x);
}

inline std::ostream& operator <<(std::ostream& x, const IPTypes& y) {
	return y.print(x);
}

#endif // IPTYPE_H
