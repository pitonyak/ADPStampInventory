#ifndef IPTYPE_H
#define IPTYPE_H

#include <string>

class IPType {
public:
	/*! Constructor */
	IPType();
	IPType(const IPType& ipt);

    /*! Desctructor, clears all structures, not virtual because assume not subclassed. */
    ~IPType();

    // Normally I would encapulate this with getters and setters, but not sure it is worth it at this point.
    // Perhaps I should just make this a struct.
	int m_iPType;
	bool m_valid;
	bool m_iPv6;
	bool m_dupIP;
	bool m_dupMAC;
	std::string m_description;
};

#endif // IPTYPE_H
