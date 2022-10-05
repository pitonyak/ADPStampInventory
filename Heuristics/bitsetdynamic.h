#ifndef BIT_SET_DYNAMIC_H
#define BIT_SET_DYNAMIC_H

#include <cstdint>
#include <string>


/**


**/

#define BITS_PER_UNIT 64


class BitsetDynamic {
public:
    /*! Constructor */
    BitsetDynamic();

    BitsetDynamic(std::size_t n);

    /*! Resets the size which clears everything and sets all bits to 0 (false). */
    void resetSize(std::size_t n);

    /*! Destructor, clears all structures, not virtual because assume not subclassed. */
    ~BitsetDynamic();

    /*! Constructor */
    BitsetDynamic(const BitsetDynamic& x);

    /*! Assignment operator */
    const BitsetDynamic& operator=(const BitsetDynamic& x);

    bool operator==(const BitsetDynamic& rhs) const;
    bool operator!=(const BitsetDynamic& rhs) const;
    bool operator[](std::size_t pos) const;
    bool at(std::size_t pos) const;
    bool all() const;
    bool any() const;
    bool none() const;

    std::size_t count() const;
    std::size_t size() const;

    void clearAllBits();
    void setAllBits();


    const BitsetDynamic& operator&=(const BitsetDynamic& rhs);
    const BitsetDynamic& operator|=(const BitsetDynamic& rhs);
    //const BitsetDynamic& operator^=(const BitsetDynamic& rhs);

    void setBit(std::size_t pos, bool flag);

    std::string toString() const;

private:
    std::size_t m_numBits;
    std::size_t m_arraySize;
	/*! All of the bits */
    uint64_t* m_bits;
};

inline std::size_t BitsetDynamic::size() const { return m_numBits; }

inline bool BitsetDynamic::operator[](std::size_t pos) const {
    return at(pos);
}

inline bool BitsetDynamic::operator!=(const BitsetDynamic& rhs) const
{
    return !operator==(rhs);
}

#endif