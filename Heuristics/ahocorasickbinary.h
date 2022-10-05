#ifndef AHO_CORASICK_BINARY_H
#define AHO_CORASICK_BINARY_H

#include <cstdint>
#include <map>
#include <set>
#include <vector>

#include "bitsetdynamic.h"

/**

The Aho Corasick algorithm was developed to search for text / strings. 
The algorithm allows for multiple strings to be efficiently searched. 

A primary consideration is whether or not to have an array of nodes or if I should
implement an actual tree using pointers. 

Let K be the number of words to find.

Let n be the length of the text that you search; as in the text that may contain the words you want to find.

Let m be the sum of the length of all of the words that you want to find. So m is the sum of the length of each word you want to find.

The Aho-Corasick algorithm finds all words in O(n+m+z) where z is the total number of occurrences of words in the text. 

First the algorithm builds a Keyword Tree (Trie) that stores the words to find in a tree structure.




The input parameters are defined as: 


Given an input text and an array of k words, arr[], find all occurrences of all words in the input text. Let n be the length of text and m be the total number characters in all words, i.e. m = length(arr[0]) + length(arr[1]) + … + length(arr[k-1]). Here k is total numbers of input words.

https://www.geeksforgeeks.org/aho-corasick-algorithm-pattern-searching/

https://gist.github.com/ashpriom/b8231c806edeef50afe1

**/

// Normally the alphabet size is calcluated assuming you are using simple strings
// such as a..z so the alphabet size would be 26. This searches entire bytes
// so each character can have 256 distinct values (0-255).
#define ALPHABET_SIZE 256

class AhoCorasickBinary {
public:
    /*! Constructor */
    AhoCorasickBinary();

    /*! Destructor, clears all structures, not virtual because assume not subclassed. */
    ~AhoCorasickBinary();

    /*! Constructor */
    AhoCorasickBinary(const AhoCorasickBinary& x);

    /*! Assignment operator */
    const AhoCorasickBinary& operator=(const AhoCorasickBinary& x);

    //**************************************************************************
    //! Build the matching machine.
    /*!
     * \param [in]  words Vector of keywords. The index of each keyword is important.
     * 
     * \param [in]  word_lengths Vector of work lengths for the keywords.
     * 
     * \returns Number of states that the machine has. This is 255 because there are 0 - 255 possible values (searching bytes.)
     *
     ***************************************************************************/
    int buildMatchingMachine(const std::vector<uint8_t *> &words, const std::vector<int> &word_lengths);

    //**************************************************************************
    //! Find the first match (no order). Use to see if there is at least one match.
    /*!
     * \param [in]  data Pointer to the data. Will search this data to see if it contains any keyword.
     * 
     * \param [in]  len Length of the data to search; think number of characters.
     * 
     * \returns An arbitray match or -1 if no match is found.
     *
     ***************************************************************************/
    int findFirstMatch(const uint8_t *data, uint32_t len) const;

    //**************************************************************************
    //! All matches. The key to the map indicates the index of the keyword. The set contains the ending location of the match.
    /*!
     * \param [in]  data Pointer to the data. Will search this data to see if it contains any keyword.
     * 
     * \param [in]  len Length of the data to search; think number of characters.
     * 
     * \returns All matches with the keyword index as the map key and the ending location in data as the set.
     *
     ***************************************************************************/
    std::map<int, std::set<int> > findAllMatches(const uint8_t *data, uint32_t len) const;

    int getNumWords() const { return m_num_words; }
    int getAlphabetSize() const { return m_alphabet_size; }
    int getMaxStates() const { return m_max_states; }

    void orEquals(std::vector<bool>& lhs, const std::vector<bool>& rhs) const;

private:
    //**************************************************************************
    //! Find the next state for a transition.
    /*!
     * \param [in]  currentState - The current state of the machine. Must be between 0 and the number of states - 1, inclusive (so 0 and 255).
     * 
     * \param [in]  nextInput - The next character that enters into the machine. 
     * 
     * \returns Number of states that the machine has. This is 255 because there are 0 - 255 possible values (searching bytes.)
     *
     ***************************************************************************/
    int findNextState(int currentState, uint8_t nextInput) const;

    //**************************************************************************
    //! The goto array is a flat array but we treat it like a two dimensional array. This is a short-cut to get the index inline.
    /*!
     * We want to treat the m_goto as a 2-dimension array 
     * m_goto[m_max_states][m_alphabet_size]
     * 
     * This method provides the index assuming there really is a two dimensional array. 
     * 
     * \param [in]  state_idx - The current state of the machine. Must be between 0 and the number of states - 1.
     * 
     * \param [in]  value_idx - The next character that enters into the machine. 
     * 
     * \returns state_idx * m_alphabet_size + value_idx;
     *
     ***************************************************************************/
	int getGotoIndex(const int state_idx, const int value_idx) const;

	/*! Number of words. */
	int m_num_words;

	/*! Number of valid values. If searching characters, might be 26, one for each leter. This code searches 8-bit bytes so 256 possibilities. */
	int m_alphabet_size;

    /*! Maximum number of states. This consists of the sum of the length of all "words" that will be searched. In other words, maximum number of nodes. */
	int m_max_states;

	/*! Array of size m_max_states. This is the failure function used internally. */
	int* m_failure;

	/*! Array of size [m_max_states][m_alphabet_size]. This is the "Goto" function or -1 if fail. */
	int* m_goto;

    /*! Array of size m_max_states. Output for each state as a mask if the keyword with index i appears when the machine enters this state. */
    BitsetDynamic* m_bits_out_state;
};

inline int AhoCorasickBinary::getGotoIndex(const int state_idx, const int value_idx) const {
	// Add error checking if needed / desired.
	return state_idx * m_alphabet_size + value_idx;
}


#endif