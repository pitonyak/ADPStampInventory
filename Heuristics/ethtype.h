#ifndef ETHTYPE_H
#define ETHTYPE_H

#include <unordered_map>
#include <string>

class EthernetType {
public:
	/*! Constructor */
	EthernetType();

	/*! Constructor */
	EthernetType(const EthernetType& ipt);

	/*! Assignment operator */
	const EthernetType& operator=(const EthernetType& ipt);

    /*! Desctructor, clears all structures, not virtual because assume not subclassed. */
    ~EthernetType();

    // Normally I would encapulate this with getters and setters, but not sure it is worth it at this point.
    // Perhaps I should just make this a struct.
	int m_EthernetType;  // Integer type
	bool m_valid;  // Do we consider this valid?
	bool m_iPv6;   // Is this for IPv6?
	bool m_dupIP;  // Does this allow or expect a duplicate IP in the payload?
	bool m_dupMAC; // Does this allow or expect a duplicate MAC in the payload?
	std::string m_description;

    /*! Simply method to print this object as a line with no context. */
	std::ostream& print(std::ostream& x) const;
};


class EthernetTypes {
public:
	/*! Constructor */
	EthernetTypes();

    /*! Desctructor, clears all structures, not virtual because assume not subclassed. */
    ~EthernetTypes();

    //**************************************************************************
    //! Add the IP Type
    /*!
     * \param [in] ipt Constant reference to the object. If this type is already there, it is replaced.
     *
     ***************************************************************************/
  	void addType(const EthernetType& ipt);

    //**************************************************************************
    //! Add the IP Type
    /*!
     * \param [in] ipt Constant reference to the object. If this type is already there, it is replaced.
     * 
     * \param [in] ownit If true, the pointer is now owned and deleted by this class. if false, a copy is made.
     *
     ***************************************************************************/
	void addType(EthernetType* ipt, bool ownit = true);

    //**************************************************************************
    //! Get a pointer to the EthernetType object based on the numeric type.
    /*!
     * \param [in] iType Numeric IP type. 
     * 
     * \returns Pointer to the contained type or nullptr if it is not contained. 
     *
     ***************************************************************************/
	const EthernetType* getEthernetType(int iType) const;

    //**************************************************************************
    //! Is this numeric type known. Does not mean it is considered valid for our purposes.
    /*!
     * \param [in] iType Numeric IP type. 
     * 
     * \returns True if the type is contained in the object. 
     *
     ***************************************************************************/
	bool hasType(int iType) const;

    //**************************************************************************
    //! Is this numeric type considered a valid type to process.
    /*!
     * \param [in] iType Numeric IP type. 
     * 
     * \returns True if the type should not be rejected.
     *
     ***************************************************************************/
	bool isValid(int iType) const;

    //**************************************************************************
    //! Is this numeric type specific to IPv6.
    /*!
     * \param [in] iType Numeric IP type. 
     * 
     * \returns True if this numeric type is specific to IPv6
     *
     ***************************************************************************/
    bool isIPv6(int iType) const;

    //**************************************************************************
    //! Does this numeric type allow for the IP to be repeated in the payload?
    /*!
     * \param [in] iType Numeric IP type. 
     * 
     * \returns True if this this numeric type allows for the IP to be repeated in the payload
     *
     ***************************************************************************/
	bool isDupIP(int iType) const;

    //**************************************************************************
    //! Does this numeric type allow for the MAC to be repeated in the payload?
    /*!
     * \param [in] iType Numeric IP type. 
     * 
     * \returns True if this this numeric type allows for the MAC to be repeated in the payload
     *
     ***************************************************************************/
	bool isDupMAC(int iType) const;

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
     * Notice that by default this argument is 16, not 0. ONLY the first column uses this parameter. 
     * All other columns use 10.
     * 
     * \returns True if no errors are encountered. 
     *
     ***************************************************************************/
    bool read(const std::string& filename, int base = 16);

    std::ostream& print(std::ostream& x) const;

    bool sameExceptType(const EthernetType& lhs, const EthernetType& rhs) const;

private:
    /*! copy Constructor that is NOT implemented so a compile time error if try to call it. */
    EthernetTypes(const EthernetTypes&);

	std::unordered_map<int, EthernetType *> m_EthernetTypes;
};

inline const EthernetType* EthernetTypes::getEthernetType(int iType) const
{
	std::unordered_map<int, EthernetType *>::const_iterator it = m_EthernetTypes.find(iType);
	return (it == m_EthernetTypes.end()) ? nullptr : it->second;
}

inline bool EthernetTypes::hasType(int iType) const {
	return getEthernetType(iType) != nullptr;
}

inline bool EthernetTypes::isValid(int iType) const {
	const EthernetType* ipt = getEthernetType(iType);
	return (ipt != nullptr) ? ipt->m_valid : false;
}

inline bool EthernetTypes::isIPv6(int iType) const {
	const EthernetType* ipt = getEthernetType(iType);
	return (ipt != nullptr) ? ipt->m_iPv6 : false;
}

inline bool EthernetTypes::isDupIP(int iType) const {
	const EthernetType* ipt = getEthernetType(iType);
	return (ipt != nullptr) ? ipt->m_dupIP : false;
}

inline bool EthernetTypes::isDupMAC(int iType) const {
	const EthernetType* ipt = getEthernetType(iType);
	return (ipt != nullptr) ? ipt->m_dupMAC : false;
}

inline std::ostream& operator <<(std::ostream& x, const EthernetType& y) {
	return y.print(x);
}

inline std::ostream& operator <<(std::ostream& x, const EthernetTypes& y) {
	return y.print(x);
}

#endif // ETHTYPE_H
