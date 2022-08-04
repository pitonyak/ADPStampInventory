
#include "iptype.h"

IPType::IPType() : 	m_iPType(false), m_valid(false), m_iPv6(false), m_dupIP(false), m_dupMAC(false)
{
}

IPType::IPType(const IPType& ipt)
{
	if (this != &ipt)
    {
    	m_iPType = ipt.m_iPType;
		m_valid = ipt.m_valid;
		m_iPv6 = ipt.m_iPv6;
		m_dupIP = ipt.m_dupIP;
		m_dupMAC = ipt.m_dupMAC;
		m_description = ipt.m_description;
    }
}