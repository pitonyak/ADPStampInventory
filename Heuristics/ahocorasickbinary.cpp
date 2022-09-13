

#include "ahocorasickbinary.h"

#include <cstring>
#include <iostream>
#include <queue>

// This code is adapted from
// https://gist.github.com/ashpriom/b8231c806edeef50afe1
// No copyright or license is stated.


AhoCorasickBinary::AhoCorasickBinary() : m_num_words(0), m_alphabet_size(ALPHABET_SIZE), m_max_states(0), 
										 m_out_state(nullptr), m_failure(nullptr), m_goto(nullptr) {
}

AhoCorasickBinary::~AhoCorasickBinary() {
	// Wasted cycltes setting the pointers back to null, 
	// but sometimes, it helps when there are other bugs. 
	if (m_out_state != nullptr) {
		delete[] m_out_state;
		m_out_state = nullptr;
	}
	if (m_failure != nullptr) {
		delete[] m_failure;
		m_failure = nullptr;
	}
	if (m_goto != nullptr) {
		delete[] m_goto;
		m_goto = nullptr;
	}
}

AhoCorasickBinary::AhoCorasickBinary(const AhoCorasickBinary& x) {
	m_num_words = x.m_num_words;
	m_alphabet_size = x.m_alphabet_size;
	m_max_states = x.m_max_states;
	if (m_out_state != nullptr) {
		delete[] m_out_state;
		m_out_state = nullptr;
	}
	if (m_failure != nullptr) {
		delete[] m_failure;
		m_failure = nullptr;
	}
	if (m_goto != nullptr) {
		delete[] m_goto;
		m_goto = nullptr;
	}
	if (x.m_out_state != nullptr) {
		m_out_state = new std::vector<bool>[m_max_states];
		m_failure = new int[m_max_states];
		m_goto = new int[m_alphabet_size * m_max_states];

		for (int i = 0; i<m_alphabet_size * m_max_states; ++i) {
			m_goto[i] = x.m_goto[i];
		}

		for (int i=0; i<m_max_states; ++i) {
			m_failure[i] = x.m_failure[i];
		}

		for (int i=0; i<m_max_states; ++i) {
			m_out_state[i].reserve(m_num_words);
			for (auto const & bits: x.m_out_state[i]) {
				m_out_state[i].push_back(bits);
			}
		}
	}
}

const AhoCorasickBinary& AhoCorasickBinary::operator=(const AhoCorasickBinary& x) {
	if (this != &x) {
		m_num_words = x.m_num_words;
		m_alphabet_size = x.m_alphabet_size;
		m_max_states = x.m_max_states;

		if (m_out_state != nullptr) {
			delete[] m_out_state;
			m_out_state = nullptr;
		}
		if (m_failure != nullptr) {
			delete[] m_failure;
			m_failure = nullptr;
		}
		if (m_goto != nullptr) {
			delete[] m_goto;
			m_goto = nullptr;
		}
		if (x.m_out_state != nullptr) {
			m_out_state = new std::vector<bool>[m_max_states];
			m_failure = new int[m_max_states];
			m_goto = new int[m_alphabet_size * m_max_states];

			for (int i = 0; i<m_alphabet_size * m_max_states; ++i) {
				m_goto[i] = x.m_goto[i];
			}

			for (int i=0; i<m_max_states; ++i) {
				m_failure[i] = x.m_failure[i];
			}

			for (int i=0; i<m_max_states; ++i) {
				m_out_state[i].reserve(m_num_words);
				for (auto const & bits: x.m_out_state[i]) {
					m_out_state[i].push_back(bits);
				}
			}
		}
	}
	return *this;
}

int AhoCorasickBinary::buildMatchingMachine(const std::vector<uint8_t*> &words, const std::vector<int> &word_lengths) {
	if (m_out_state != nullptr) {
		delete[] m_out_state;
		m_out_state = nullptr;
	}
	if (m_failure != nullptr) {
		delete[] m_failure;
		m_failure = nullptr;
	}
	if (m_goto != nullptr) {
		delete[] m_goto;
		m_goto = nullptr;
	}
	m_max_states = 0;
	m_num_words = words.size();
	for (auto const &len: word_lengths) {
		m_max_states += len;
	}
	if (m_num_words == 0 || m_max_states == 0) {
		return m_max_states;
	}

	m_out_state = new std::vector<bool>[m_max_states];
	m_failure = new int[m_max_states];
	int goto_size = m_alphabet_size * m_max_states;

	// m_goto[m_max_states][m_alphabet_size]
	m_goto = new int[goto_size];
	for (int i = 0; i<goto_size; ++i) {
		m_goto[i] = -1;
	}

	for (int i=0; i<m_max_states; ++i) {
		m_failure[i] = -1;
		m_out_state[i].reserve(m_num_words);
		for (int j=0; j<m_num_words; ++j) {
			m_out_state[i].push_back(false);
		}
	}

	int states = 1; // Initially, we just have the 0 state

	for (int idx_words = 0; idx_words < words.size(); ++idx_words) {
		const uint8_t* keyword = words[idx_words];
        int a_word_size = word_lengths[idx_words];

        // Think of current state as how deep into the tree.
        int currentState = 0;
        for (int letter_idx = 0; letter_idx < a_word_size; ++letter_idx) {
            // If there was a "lower bound", it would be keyword[letter_idx] - lower_bound;
            int idx = getGotoIndex(currentState, keyword[letter_idx]);
            //std::cout << "Word: " << idx_words << " Letter:" << letter_idx << " is " << keyword[letter_idx] << " currentState = " << currentState << " idx = " << idx << std::endl;
            if (m_goto[idx] == -1) { // Allocate a new node
                m_goto[idx] = states++;
                //std::cout << " allocated node at level " << currentState << " char " << (int) keyword[letter_idx] << " idx:" << idx << std::endl;
            }
            currentState = m_goto[idx];
        }
        m_out_state[currentState].at(idx_words) = true; // There's a match of keywords[idx_words] at node currentState.
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
                // m_out_state[m_goto[idx]] |= m_out_state[failure]; 
                orEquals(m_out_state[m_goto[idx]], m_out_state[failure]);
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
	if (m_out_state == nullptr) {
		std::cerr << "Aho Corasick algorithm has not been initialized." << std::endl;
		return -1;
	}

	int currentState = 0;
    for (int i = 0; i < len; ++i) {
        currentState = findNextState(currentState, data[i]);
        if (noBits(m_out_state[currentState])) continue; // Nothing new, let's move on to the next character.
        for (int idx_words = 0; idx_words < m_num_words; ++idx_words) {
           	if (m_out_state[currentState].at(idx_words)) {
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
	if (m_out_state == nullptr) {
		std::cerr << "Aho Corasick algorithm has not been initialized." << std::endl;
		return matches;
	}

	int currentState = 0;
    for (int i = 0; i < len; ++i) {
        currentState = findNextState(currentState, data[i]);
        if (noBits(m_out_state[currentState])) continue; // Nothing new, let's move on to the next character.
        for (int idx_words = 0; idx_words < m_num_words; ++idx_words) {
           	if (m_out_state[currentState].at(idx_words)) {
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
       }
   	}

	return matches;
}


void AhoCorasickBinary::orEquals(std::vector<bool>& lhs, const std::vector<bool>& rhs) const {
	for (int i=0; i<lhs.size(); ++i) {
		if (rhs.at(i))
			lhs.at(i) = true;
	}
}

bool AhoCorasickBinary::noBits(const std::vector<bool>& v) const {

	for (const bool& x : v) {
		if (x) {
			return false;
		}
	}
	return true;
}

bool AhoCorasickBinary::allBits(const std::vector<bool>& v) const {

	for (const bool& x : v) {
		if (!x) {
			return false;
		}
	}
	return true;
}

bool AhoCorasickBinary::anyBits(const std::vector<bool>& v) const {

	for (const bool& x : v) {
		if (x) {
			return true;
		}
	}
	return false;
}
