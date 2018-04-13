#ifndef _MAINTAIN_AND_TEST_HH_
#define _MAINTAIN_AND_TEST_HH_

#include "ordered_set.hh"
#include "sequence.hh"

#include <map>
#include <list>

template <typename T>
void maintain_and_test(const sequence<T> &seq, unsigned int support, std::map<unsigned int,std::list<sequence<T> > > &patterns) {
	std::list<sequence<T> > &sequences = patterns[support];
	
	typename std::list<sequence<T> >::iterator sequences_iter = sequences.begin();
	for(; sequences_iter != sequences.end(); ++sequences_iter) {
		if(sequences_iter->length() <= seq.length()) {
			break;
		}
		else if(seq.subsequence(*sequences_iter)) {
			return;
		}
	}
	
	typename std::list<sequence<T> >::iterator upper = sequences_iter;
	for(; sequences_iter != sequences.end(); ++sequences_iter) {
		if(sequences_iter->length() < seq.length()) {
			break;
		}
		else if(seq.subsequence(*sequences_iter)) {
			return;
		}
	}
	
	upper = sequences.insert(upper, seq);
	++upper;
	
	while(upper != sequences.end()) {
		if(upper->subsequence(seq)) {
			upper= sequences.erase(upper);
		}
		else {
			++upper;
		}
	}
}

#endif
