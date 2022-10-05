

#include "ahocorasickbinary.h"

#include <cstring>
#include <iostream>
#include <queue>

// This code is adapted from
// https://gist.github.com/ashpriom/b8231c806edeef50afe1
// No copyright or license is stated.


AhoCorasickBinary::AhoCorasickBinary() : m_num_words(0), m_alphabet_size(ALPHABET_SIZE), m_max_states(0), 
										 m_failure(nullptr), m_goto(nullptr), m_bits_out_state(nullptr) {
}

AhoCorasickBinary::~AhoCorasickBinary() {
	// Wasted cycltes setting the pointers back to null, 
	// but sometimes, it helps when there are other bugs. 
	if (m_failure != nullptr) {
		delete[] m_failure;
		m_failure = nullptr;
	}
	if (m_goto != nullptr) {
		delete[] m_goto;
		m_goto = nullptr;
	}
	if (m_bits_out_state) {
		delete[] m_bits_out_state;
		m_bits_out_state = nullptr;
	}
}

AhoCorasickBinary::AhoCorasickBinary(const AhoCorasickBinary& x) : AhoCorasickBinary() {
	m_num_words = x.m_num_words;
	m_alphabet_size = x.m_alphabet_size;
	m_max_states = x.m_max_states;
	if (x.m_bits_out_state != nullptr) {
		m_goto = new int[m_alphabet_size * m_max_states];
		for (int i = 0; i<m_alphabet_size * m_max_states; ++i) {
			m_goto[i] = x.m_goto[i];
		}

		m_failure = new int[m_max_states];
		for (int i=0; i<m_max_states; ++i) {
			m_failure[i] = x.m_failure[i];
		}

		m_bits_out_state = new BitsetDynamic[m_max_states];
		for (int i=0; i<m_max_states; ++i) {
			m_bits_out_state[i] = x.m_bits_out_state[i];
		}
	}
}

const AhoCorasickBinary& AhoCorasickBinary::operator=(const AhoCorasickBinary& x) {
	if (this != &x) {
		m_num_words = x.m_num_words;
		m_alphabet_size = x.m_alphabet_size;
		m_max_states = x.m_max_states;

		if (m_failure != nullptr) {
			delete[] m_failure;
			m_failure = nullptr;
		}
		if (m_goto != nullptr) {
			delete[] m_goto;
			m_goto = nullptr;
		}
		if (m_bits_out_state != nullptr) {
			delete[] m_bits_out_state;
			m_bits_out_state = nullptr;
		}
		if (x.m_bits_out_state != nullptr) {

			m_goto = new int[m_alphabet_size * m_max_states];
			for (int i = 0; i<m_alphabet_size * m_max_states; ++i) {
				m_goto[i] = x.m_goto[i];
			}

			m_failure = new int[m_max_states];
			for (int i=0; i<m_max_states; ++i) {
				m_failure[i] = x.m_failure[i];
			}

			m_bits_out_state = new BitsetDynamic[m_max_states];
			for (int i=0; i<m_max_states; ++i) {
				m_bits_out_state[i] = x.m_bits_out_state[i];
			}
		}
	}
	return *this;
}

int AhoCorasickBinary::buildMatchingMachine(const std::vector<uint8_t*> &words, const std::vector<int> &word_lengths) {

	if (m_failure != nullptr) {
		delete[] m_failure;
		m_failure = nullptr;
	}
	if (m_goto != nullptr) {
		delete[] m_goto;
		m_goto = nullptr;
	}
	if (m_bits_out_state != nullptr) {
		delete[] m_bits_out_state;
		m_bits_out_state = nullptr;
	}
	m_max_states = 0;
	m_num_words = words.size();
	for (auto const &len: word_lengths) {
		m_max_states += len;
	}
	if (m_num_words == 0 || m_max_states == 0) {
		return m_max_states;
	}

	m_bits_out_state = new BitsetDynamic[m_max_states];
	m_failure = new int[m_max_states];

	// m_goto[m_max_states][m_alphabet_size]
	int goto_size = m_alphabet_size * m_max_states;
	m_goto = new int[goto_size];
	for (int i = 0; i<goto_size; ++i) {
		m_goto[i] = -1;
	}

	for (int i=0; i<m_max_states; ++i) {
		m_failure[i] = -1;
		// Clears everything and sets all bits to zero.
		m_bits_out_state[i].resetSize(m_num_words);
	}

	int states = 1; // Initially, we just have the 0 state

	for (size_t idx_words = 0; idx_words < words.size(); ++idx_words) {
		const uint8_t* keyword = words[idx_words];
        int a_word_size = word_lengths[idx_words];

        // Think of current state as how deep into the tree.
        int currentState = 0;
        for (int letter_idx = 0; letter_idx < a_word_size; ++letter_idx) {
        	// Each keyword is a series of bytes.
        	// Normally it would be a string perhaps of lower case characters.
        	// To save space, we normally map the lowest expected letter (such as 'a')
        	// to be zero. Because we are searching binary data we do not do that. 
        	// c = letter - lowestChar.
        	// Instead we just use keyword[letter_idx] and take the entire byte.
            int idx = getGotoIndex(currentState, keyword[letter_idx]);
            //std::cout << "Word: " << idx_words << " Letter:" << letter_idx << " is " << (int) keyword[letter_idx] << " currentState = " << currentState << " idx = " << idx << std::endl;
            // m_goto is initialized to -1 for every entry.
            if (m_goto[idx] == -1) { // Allocate a new node
                m_goto[idx] = states++;
                //std::cout << " allocated node at level " << currentState << " char " << (int) keyword[letter_idx] << " idx:" << idx << std::endl;
            }
            currentState = m_goto[idx];
        }

        // There's a match of keywords[idx_words] at node currentState.
        m_bits_out_state[currentState].setBit(idx_words, true); 
    }

    // State 0 should have an outgoing edge for all characters.
    for (int i = 0; i < m_alphabet_size; ++i) {
    	int idx = getGotoIndex(0, i);
    	if (m_goto[idx] == -1) {
                m_goto[idx] = 0;
        }
    }

    // Build the failure function
    std::queue<int> q;
    // Iterate over every possible input
    for (int c = 0; c < m_alphabet_size; ++c) {
        // All nodes s of depth 1 have m_failure[s] = 0
        int idx = getGotoIndex(0, c);
        if (m_goto[idx] != -1 && m_goto[idx] != 0) {
            m_failure[m_goto[idx]] = 0;
            q.push(m_goto[idx]);
        }
    }

    while (q.size()) {
        int state = q.front();
        q.pop();
        for (int c = 0; c < m_alphabet_size; ++c) {
        	int idx = getGotoIndex(state, c);
            if (m_goto[idx] != -1) {
                int failure = m_failure[state];
                while (m_goto[getGotoIndex(failure, c)] == -1) {
                    failure = m_failure[failure];
                }
                failure = m_goto[getGotoIndex(failure, c)];
                m_failure[m_goto[idx]] = failure;

                // Merge out values
                m_bits_out_state[m_goto[idx]] |= m_bits_out_state[failure];
                q.push(m_goto[idx]);
            }
        }
    }

    return states;
}

int AhoCorasickBinary::findNextState(int currentState, uint8_t nextInput) const {
    int answer = currentState;
    while (m_goto[getGotoIndex(answer, nextInput)] == -1) {
    	answer = m_failure[answer];
    }
    return m_goto[getGotoIndex(answer, nextInput)];
}

int AhoCorasickBinary::findFirstMatch(const uint8_t *data, uint32_t len) const {
	if (data == nullptr || m_num_words == 0) {
		return -1;
	}

	if (m_bits_out_state == nullptr) {
		std::cerr << "Aho Corasick algorithm has not been initialized." << std::endl;
		return -1;
	}

	int currentState = 0;
    for (uint32_t i = 0; i < len; ++i) {
        currentState = findNextState(currentState, data[i]);

        if (m_bits_out_state[currentState].none()) continue; // Nothing new, let's move on to the next character.
        for (int idx_words = 0; idx_words < m_num_words; ++idx_words) {
        	if (m_bits_out_state[currentState].at(idx_words)) {
        		return idx_words;
        	}
       }
   	}

	return -1;
}

std::map<int, std::set<int> > AhoCorasickBinary::findAllMatches(const uint8_t *data, uint32_t len) const {

	std::map<int, std::set<int> > matches;
	std::map<int, std::set<int> >::iterator it;

	if (data == nullptr) {
		return matches;
	}

	if (m_bits_out_state == nullptr) {
		std::cerr << "Aho Corasick algorithm has not been initialized." << std::endl;
		return matches;
	}

	int currentState = 0;
    for (uint32_t i = 0; i < len; ++i) {
        currentState = findNextState(currentState, data[i]);
        if (m_bits_out_state[currentState].none()) continue; // Nothing new, let's move on to the next character.
        for (int idx_words = 0; idx_words < m_num_words; ++idx_words) {
        	if (m_bits_out_state[currentState].at(idx_words)) {
           		// Match from: i - keywords[idx_words].size() + 1 to i
	            //std::cout << "Keyword " << idx_words << " ends at location " << i << std::endl;
	            it = matches.find(idx_words);
	            if (it == matches.end()) {
	            	std::set<int> aSet;
	            	aSet.insert(i);
	            	matches.insert( std::make_pair(idx_words, aSet) );
	            } else {
	           		it->second.insert(i);
	            }
        	}
//           	if (m_bits_out_state[currentState].at(idx_words)) {
//           		// Match from: i - keywords[idx_words].size() + 1 to i
//	            //std::cout << "Keyword " << idx_words << " ends at location " << i << std::endl;
//	            it = matches.find(idx_words);
//	            if (it == matches.end()) {
//	            	std::set<int> aSet;
//	            	aSet.insert(i);
//	            	matches.insert( std::make_pair(idx_words, aSet) );
//	            } else {
//	           		it->second.insert(i);
//	            }
//            }
       }
   	}

	return matches;
}
