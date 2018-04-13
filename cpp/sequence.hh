#ifndef _SEQUENCE_HH_
#define _SEQUENCE_HH_

#include <iostream>
#include <ios>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <fstream>

#include <locale>

#include <utility>
#include <set>
#include <string>
#include <vector>

#include <algorithm>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <typeinfo>

#include "infix_iterator.hh"
#include "ordered_set.hh"

template <typename T>
class sequence {
	public:
		typedef typename std::vector<T>::size_type size_type;
		
		typedef typename std::vector<T>::iterator iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;
		
		typedef typename std::vector<T>::reverse_iterator reverse_iterator;
		typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;
		
		sequence() : seq() {
			
		}
		
		sequence(const T &element) : seq(1, element) {
			
		}
		
		sequence(const sequence &other) {
			seq = other.seq;
		}
		
		template <typename K>
		sequence(const sequence<K> &other) {
			std::cout << "DONT USE" << std::endl;
			exit(-1);
		}
		
		virtual ~sequence() {
		
		}
		
		sequence & operator=(const sequence &other) {
			seq = other.seq;
			return *this;
		}
		
		iterator begin() {
			return seq.begin();
		}
		
		iterator end() {
			return seq.end();
		}
		
		const_iterator begin() const {
			return seq.begin();
		}
		
		const_iterator end() const {
			return seq.end();
		}
		
		reverse_iterator rbegin() {
			return seq.rbegin();
		}
		
		reverse_iterator rend() {
			return seq.rend();
		}
		
		const_reverse_iterator rbegin() const {
			return seq.rbegin();
		}
		
		const_reverse_iterator rend() const {
			return seq.rend();
		}
		
		size_type length() const {
			return seq.size();
		}
		
		bool empty() const {
			return seq.empty();
		}
		
		void push_element(const T &element) {
			seq.push_back(element);
		}
		
		T pop_element() {
			T element = seq.back();
			seq.pop_back();
			return element;
		}

		void insert(const T &element, size_t loc) {
			seq.insert(seq.begin()+loc, element);			
		}

		T remove(size_t loc) {
			T element = seq[loc];
			seq.erase(seq.begin()+loc);
			return element;
		}
		
		T & last() {
			return seq.back();
		}
		
		const T & last() const {
			return seq.back();
		}
		
		T & operator[](size_type index) {
			return seq[index];
		}
		
		const T & operator[](size_type index) const {
			return seq[index];
		}
		
		void clear() {
			seq.clear();
		}
		
		template <typename Value>
		bool contains(const Value &element) const {
			const_iterator cur_iter = begin();
			const_iterator end_iter = end();
			while(cur_iter != end_iter) {
				if(element == *cur_iter) {
					return true;
				}
				++cur_iter;
			}
			
			return false;
		}
		
		const_iterator prefix_project(const sequence &subseq) const {
			const_iterator this_iter = begin();
			const_iterator subseq_iter = subseq.begin();
			while(this_iter != end() && subseq_iter != subseq.end()) {
				if(*this_iter == *subseq_iter) {
					++subseq_iter;
				}
				
				++this_iter;
			}
			
			return this_iter;
		}
		
		void prefix_project(const sequence &subseq, sequence &result) const {
			//result.seq.assign(prefix_project(subseq), end());
			
			const_iterator iter = prefix_project(subseq);
			result.seq.resize(std::distance(iter, end()));
			std::copy(iter, end(), result.seq.begin());
		}
		
		const_reverse_iterator suffix_project(const sequence &subseq) const {
			const_reverse_iterator this_iter = rbegin();
			const_reverse_iterator subseq_iter = subseq.rbegin();
			while(this_iter != rend() && subseq_iter != subseq.rend()) {
				if(*this_iter == *subseq_iter) {
					++subseq_iter;
				}
				
				++this_iter;
			}
			
			return this_iter;
		}
		
		void suffix_project(const sequence &subseq, sequence &result) const {
			//result.seq.assign(begin(), suffix_project(subseq).base());
			
			const_iterator iter = suffix_project(subseq).base();
			result.seq.resize(std::distance(begin(), iter));
			std::copy(begin(), iter, result.seq.begin());
		}
		
		void extend(const sequence &other) {
			seq.insert(seq.end(), other.begin(), other.end());
		}
		
		bool subsequence(const sequence &that) const {
			/*const_iterator this_iter = begin();
			const_iterator that_iter = that.begin();
			while(this_iter != end()) {
				
				while(true) {
					if(that_iter == that.end()) {
						return false;
					}
					
					if(*this_iter == *that_iter) {
						break;
					}
					
					++that_iter;
				}
				
				++this_iter;
				++that_iter;
			}
			
			return true;*/
			
			const_iterator this_iter = begin();
			const_iterator that_iter = that.begin();
			while(this_iter != end() && that_iter != that.end()) {
				if(*this_iter == *that_iter) {
					++this_iter;
				}
				
				++that_iter;
			}
			
			return this_iter == end();
		}
		
		virtual void swap(sequence &other) {
			seq.swap(other.seq);
		}
		
		
		
		
		virtual std::ostream & serialize(std::ostream &output) const {
			output << "<";
			std::copy(begin(), end(), infix_ostream_iterator<T>(output, ","));
			output << ">";
			
			return output;
		}
		
		friend std::ostream & operator<<(std::ostream &output, const sequence &seq) {
			return seq.serialize(output);
		}
		
		virtual std::istream & deserialize(std::istream &input) {
			std::istream::sentry sentry(input);
			if(sentry) {
				if(input.peek() != '<') {
					#ifndef NDEBUG
					std::ostringstream oss;
					oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << (char)input.peek() << "' when expencting '<'" << std::endl;
					std::cerr << oss.str();
					#endif
					
					input.setstate(std::ios_base::failbit);
					return input;
				}
				
				char delim;
				input >> delim;
				
				if(input.fail()) {
					#ifndef NDEBUG
					std::ostringstream oss;
					oss << __FILE__ << ":" << __LINE__ << " error reading delimiter '<'" << std::endl;
					std::cerr << oss.str();
					#endif
						
					return input;
				}
				else if(delim != '<') {
					#ifndef NDEBUG
					std::ostringstream oss;
					oss << __FILE__ << ":" << __LINE__ << " error reading delimiter '<'" << std::endl;
					std::cerr << oss.str();
					#endif
					
					input.setstate(std::ios_base::failbit);
					return input;
				}
					
				std::vector<T> tmp_seq;
					
				T element;
				while(delim != '>') { 
					input >> element;
					if(input.fail()) {
						#ifndef NDEBUG
						std::ostringstream oss;
						oss << "error converting sequence item (" << typeid(T).name() << ")" << std::endl;
						std::cerr << oss.str();
						#endif
					
						return input;
					}
					
					input >> delim;
					if(input.fail()) {
						#ifndef NDEBUG
						std::ostringstream oss;
						oss << __FILE__ << ":" << __LINE__ << " failed to read delimiter" << std::endl;
						std::cerr << oss.str();
						#endif
						
						return input;
					}
					else if(delim != ',' && delim != '>') {
						#ifndef NDEBUG
						std::ostringstream oss;
						oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << delim << "' when expencting ',' or '>'" << std::endl;
						std::cerr << oss.str();
						#endif
						
						input.setstate(std::ios_base::failbit);
						return input;
					}
					
					tmp_seq.push_back(element);
				}
				
				seq.swap(tmp_seq);
			}
				
			return input;
		}
				
		friend std::istream & operator>>(std::istream &input, sequence &seq) {
			return seq.deserialize(input);
		}
		
		friend bool operator<(const sequence &lhs, const sequence &rhs) {
			return lhs.length() < rhs.length() || (lhs.length() == rhs.length() && lhs.seq < rhs.seq);
		}
		friend bool operator>(const sequence &lhs, const sequence &rhs) {
			return rhs < lhs;
		}
		friend bool operator<=(const sequence &lhs, const sequence &rhs) {
			return !(lhs > rhs);
		}
		friend bool operator>=(const sequence &lhs, const sequence &rhs) {
			return !(lhs < rhs);
		}
		
		friend bool operator==(const sequence &lhs, const sequence &rhs) {
			return lhs.seq == rhs.seq;
		}
		friend bool operator!=(const sequence &lhs, const sequence &rhs) {
			return !(lhs == rhs);
		}
		
	protected:
		std::vector<T> seq;
};

template <>
std::istream & sequence<std::string>::deserialize(std::istream &input) {
	std::istream::sentry sentry(input);
	if(sentry) {
		if(input.peek() != '<') {
			#ifndef NDEBUG
			std::ostringstream oss;
			oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << (char)input.peek() << "' when expencting '<'" << std::endl;
			std::cerr << oss.str();
			#endif
			
			input.setstate(std::ios_base::failbit);
			return input;
		}
		
		char delim;
		input >> delim;
		
		if(input.fail()) {
			#ifndef NDEBUG
			std::ostringstream oss;
			oss << __FILE__ << ":" << __LINE__ << " error reading delimiter '<'" << std::endl;
			std::cerr << oss.str();
			#endif
				
			return input;
		}
		else if(delim != '<') {
			#ifndef NDEBUG
			std::ostringstream oss;
			oss << __FILE__ << ":" << __LINE__ << " error reading delimiter '<'" << std::endl;
			std::cerr << oss.str();
			#endif
			
			input.setstate(std::ios_base::failbit);
			return input;
		}
			
		std::vector<std::string> tmp_seq;
		
		std::string element;
		while(delim != '>') {
			element.clear();
			
			char next_char = input.peek();
			while(next_char != ',' && next_char != '>') {
				element.push_back(next_char);
				input.ignore();
				next_char = input.peek();
			}
			
			if(input.fail()) {
				#ifndef NDEBUG
				std::ostringstream oss;
				oss << "error converting sequence item (" << typeid(std::string).name() << ")" << std::endl;
				std::cerr << oss.str();
				#endif
			
				return input;
			}
			
			input >> delim;
			if(input.fail()) {
				#ifndef NDEBUG
				std::ostringstream oss;
				oss << __FILE__ << ":" << __LINE__ << " failed to read delimiter" << std::endl;
				std::cerr << oss.str();
				#endif
				
				return input;
			}
			else if(delim != ',' && delim != '>') {
				#ifndef NDEBUG
				std::ostringstream oss;
				oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << delim << "' when expencting ',' or '>'" << std::endl;
				std::cerr << oss.str();
				#endif
				
				input.setstate(std::ios_base::failbit);
				return input;
			}
			
			tmp_seq.push_back(element);
		}
		
		seq.swap(tmp_seq);
	}
		
	return input;
}

template <typename T>
class sequence<ordered_set<T> > {
	public:
		typedef typename std::vector<ordered_set<T> >::size_type size_type;
		
		typedef typename std::vector<ordered_set<T> >::iterator iterator;
		typedef typename std::vector<ordered_set<T> >::const_iterator const_iterator;
		
		typedef typename std::vector<ordered_set<T> >::reverse_iterator reverse_iterator;
		typedef typename std::vector<ordered_set<T> >::const_reverse_iterator const_reverse_iterator;
		
		sequence() : seq() {
			
		}
		
		sequence(const T &element) : seq(1, ordered_set<T>(element)) {
			
		}
		
		sequence(const sequence &other) {
			seq = other.seq;
		}
		
		virtual ~sequence() {
		
		}
		
		sequence & operator=(const sequence &other) {
			seq = other.seq;
			return *this;
		}
		
		iterator begin() {
			return seq.begin();
		}
		
		iterator end() {
			return seq.end();
		}
		
		const_iterator begin() const {
			return seq.begin();
		}
		
		const_iterator end() const {
			return seq.end();
		}
		
		reverse_iterator rbegin() {
			return seq.rbegin();
		}
		
		reverse_iterator rend() {
			return seq.rend();
		}
		
		const_reverse_iterator rbegin() const {
			return seq.rbegin();
		}
		
		const_reverse_iterator rend() const {
			return seq.rend();
		}
		
		size_type length() const {
			return seq.size();
		}
		
		bool empty() const {
			return seq.empty();
		}
		
		bool push_item(const T &item) {
			return seq.back().add_element(item);
		}
		
		void push_element(const ordered_set<T> &element) {
			seq.push_back(element);
		}
		
		T pop_item() {
			ordered_set<T> &element = seq.back();
			T item = element.remove_element();
			return item;
		}
		
		ordered_set<T> pop_element() {
			ordered_set<T> element = seq.back();
			seq.pop_back();
			return element;
		}
		
		ordered_set<T> & last() {
			return seq.back();
		}
		
		const ordered_set<T> & last() const {
			return seq.back();
		}
		
		ordered_set<T> & operator[](size_type index) {
			return seq[index];
		}
		
		const ordered_set<T> & operator[](size_type index) const {
			return seq[index];
		}
		
		void clear() {
			seq.clear();
		}
		
		bool contains(const T &element) const {
			const_iterator cur_iter = begin();
			const_iterator end_iter = end();
			while(cur_iter != end_iter) {
				if(cur_iter->contains(element)) {
					return true;
				}
				++cur_iter;
			}
			
			return false;
		}
		
		const_iterator prefix_project(const sequence &subseq) const {
			const_iterator this_iter = begin();
			const_iterator subseq_iter = subseq.begin();
			while(this_iter != end() && subseq_iter != subseq.end()) {
				if(subseq_iter->subset(*this_iter)) {
					++subseq_iter;
				}
				
				++this_iter;
			}
			
			return this_iter;
		}
		
		void prefix_project(const sequence &subseq, sequence &result) const {
			//result.seq.assign(prefix_project(subseq), end());
			
			const_iterator iter = prefix_project(subseq);
			result.seq.resize(std::distance(iter, end()));
			std::copy(iter, end(), result.seq.begin());
		}
		
		const_reverse_iterator suffix_project(const sequence &subseq) const {
			const_reverse_iterator this_iter = rbegin();
			const_reverse_iterator subseq_iter = subseq.rbegin();
			while(this_iter != rend() && subseq_iter != subseq.rend()) {
				if(subseq_iter->subset(*this_iter)) {
					++subseq_iter;
				}
				
				++this_iter;
			}
			
			return this_iter;
		}
		
		void suffix_project(const sequence &subseq, sequence &result) const {
			//result.seq.assign(begin(), suffix_project(subseq).base());
			
			const_iterator iter = suffix_project(subseq).base();
			result.seq.resize(std::distance(begin(), iter));
			std::copy(begin(), iter, result.seq.begin());
		}
		
		void extend(const sequence &other) {
			seq.insert(seq.end(), other.begin(), other.end());
		}
		
		bool subsequence(const sequence &that) const {
			/*const_iterator this_iter = begin();
			const_iterator that_iter = that.begin();
			while(this_iter != end()) {
				
				while(true) {
					if(that_iter == that.end()) {
						return false;
					}
					
					if(this_iter->subset(*that_iter)) {
						break;
					}
					
					++that_iter;
				}
				
				++this_iter;
				++that_iter;
			}
			
			return true;*/
			
			const_iterator this_iter = begin();
			const_iterator that_iter = that.begin();
			while(this_iter != end() && that_iter != that.end()) {
				if(this_iter->subset(*that_iter)) {
					++this_iter;
				}
				
				++that_iter;
			}
			
			return this_iter == end();
		}
		
		virtual void swap(sequence &other) {
			seq.swap(other.seq);
		}
		
		
		
		
		virtual std::ostream & serialize(std::ostream &output) const {
			output << "<";
			std::copy(begin(), end(), infix_ostream_iterator<ordered_set<T> >(output, ","));
			output << ">";
			
			return output;
		}
		
		friend std::ostream & operator<<(std::ostream &output, const sequence &seq) {
			return seq.serialize(output);
		}
		
		virtual std::istream & deserialize(std::istream &input) {
			std::istream::sentry sentry(input);
			if(sentry) {
				if(input.peek() != '<') {
					#ifndef NDEBUG
					std::ostringstream oss;
					oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << (char)input.peek() << "' when expencting '<'" << std::endl;
					std::cerr << oss.str();
					#endif
					
					input.setstate(std::ios_base::failbit);
					return input;
				}
				
				char delim;
				input >> delim;
				
				if(input.fail()) {
					#ifndef NDEBUG
					std::ostringstream oss;
					oss << __FILE__ << ":" << __LINE__ << " error reading delimiter '<'" << std::endl;
					std::cerr << oss.str();
					#endif
						
					return input;
				}
				else if(delim != '<') {
					#ifndef NDEBUG
					std::ostringstream oss;
					oss << __FILE__ << ":" << __LINE__ << " error reading delimiter '<'" << std::endl;
					std::cerr << oss.str();
					#endif
					
					input.setstate(std::ios_base::failbit);
					return input;
				}
					
				std::vector<ordered_set<T> > tmp_seq;
					
				ordered_set<T> element;
				while(delim != '>') { 
					input >> element;
					if(input.fail()) {
						#ifndef NDEBUG
						std::ostringstream oss;
						oss << "error converting sequence item (" << typeid(T).name() << ")" << std::endl;
						std::cerr << oss.str();
						#endif
					
						return input;
					}
					
					input >> delim;
					if(input.fail()) {
						#ifndef NDEBUG
						std::ostringstream oss;
						oss << __FILE__ << ":" << __LINE__ << " failed to read delimiter" << std::endl;
						std::cerr << oss.str();
						#endif
						
						return input;
					}
					else if(delim != ',' && delim != '>') {
						#ifndef NDEBUG
						std::ostringstream oss;
						oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << delim << "' when expencting ',' or '>'" << std::endl;
						std::cerr << oss.str();
						#endif
						
						input.setstate(std::ios_base::failbit);
						return input;
					}
					
					tmp_seq.push_back(element);
				}
				
				seq.swap(tmp_seq);
			}
				
			return input;
		}
				
		friend std::istream & operator>>(std::istream &input, sequence &seq) {
			return seq.deserialize(input);
		}
		
		friend bool operator<(const sequence &lhs, const sequence &rhs) {
			return lhs.length() < rhs.length() || (lhs.length() == rhs.length() && lhs.seq < rhs.seq);
		}
		friend bool operator>(const sequence &lhs, const sequence &rhs) {
			return rhs < lhs;
		}
		friend bool operator<=(const sequence &lhs, const sequence &rhs) {
			return !(rhs < lhs);
		}
		friend bool operator>=(const sequence &lhs, const sequence &rhs) {
			return !(lhs < rhs);
		}
		
		friend bool operator==(const sequence &lhs, const sequence &rhs) {
			return lhs.seq == rhs.seq;
		}
		friend bool operator!=(const sequence &lhs, const sequence &rhs) {
			return !(lhs == rhs);
		}
		
		sequence & operator+=(const sequence &rhs) {
			extend(rhs);
			
			return *this;
		}
		
		friend sequence operator+(sequence lhs, const sequence &rhs) {
			lhs += rhs;
			return lhs;
		}
		
	protected:
		std::vector<ordered_set<T> > seq;
};

template <typename T>
void swap(sequence<T> &x, sequence<T> &y) {
	x.swap(y);
}

template <typename T>
struct is_subsequence {
	const sequence<T> &seq;
	is_subsequence(const sequence<T> &seq) : seq(seq) {
	
	}
	
	bool operator()(const sequence<T> *test_seq) const {
		return seq.subsequence(*test_seq);
	}
};

template <template <typename, typename> class Container, typename Value, typename Allocator>
std::ostream & operator<<(std::ostream &output, const Container<sequence<Value>, Allocator> &container) {
	std::copy(container.begin(), container.end(), infix_ostream_iterator<sequence<Value> >(output, "\n"));
	return output;
}

template <template <typename, typename, typename> class Container, typename Value, typename Compare, typename Allocator>
std::ostream & operator<<(std::ostream &output, const Container<sequence<Value>, Compare, Allocator> &container) {
	std::copy(container.begin(), container.end(), infix_ostream_iterator<sequence<Value> >(output, "\n"));
	return output;
}

/* read sequences into a container */
template <template <typename, typename> class Container, typename Value, typename Allocator>
std::istream & operator>>(std::istream &input, Container<sequence<Value>, Allocator> &container) {
	std::istream::sentry sentry(input, false);
	if(sentry) {
		sequence<Value> seq;
		Container<sequence<Value>, Allocator> tmp_container;
		while(input >> seq) {
			tmp_container.insert(tmp_container.end(), seq);
		}
		
		if(input.bad()) {
			// IO error
		}
		else if(input.fail() && !input.eof()) {
			// conversion error
		}
		else {
			input.clear(input.rdstate() & ~std::ios_base::failbit);
			container.swap(tmp_container);	
		}
	}
	
	return input;
}

/* read sequences into a container, storing sequences as pointers */
template <template <typename, typename> class Container, typename Value, typename Allocator>
std::istream & operator>>(std::istream &input, Container<sequence<Value> *, Allocator> &container) {
	std::istream::sentry sentry(input, false);
	if(sentry) {
		Container<sequence<Value> *, Allocator> tmp_container;
		
		try {
			sequence<Value> seq;
			
			while(input >> seq) {
				// insert a NULL first to insure the container can allocate room for the new item
				typename Container<sequence<Value> *, Allocator>::iterator tmp_container_iter = tmp_container.insert(tmp_container.end(), NULL);
				
				sequence<Value> *tmp_seq = new sequence<Value>();
				seq.swap(*tmp_seq);
				
				*tmp_container_iter = tmp_seq;
			}
			
			if(input.bad()) {
				// IO error
			}
			else if(input.fail() && !input.eof()) {
				// conversion error
			}
			else {
				input.clear(input.rdstate() & ~std::ios_base::failbit);
				container.swap(tmp_container);	
			}
		}
		catch(...) {
			typename Container<sequence<Value> *, Allocator>::iterator tmp_container_iter = tmp_container.begin();
			for(; tmp_container_iter != tmp_container.end(); ++tmp_container_iter) {
				delete *tmp_container_iter;
			}
			
			throw;
		}
	}
	
	return input;
}

/* read sequences into a container with a compare (i.e., a set) */
template <template <typename, typename, typename> class Container, typename Value, typename Compare, typename Allocator>
std::istream & operator>>(std::istream &input, Container<sequence<Value>, Compare, Allocator> &container) {
	std::istream::sentry sentry(input, false);
	if(sentry) {
		sequence<Value> seq;
		Container<sequence<Value>, Compare, Allocator> tmp_container;
		while(input >> seq) {
			tmp_container.insert(tmp_container.end(), seq);
		}
		
		if(input.bad()) {
			// IO error
		}
		else if(input.fail() && !input.eof()) {
			// conversion error
		}
		else {
			input.clear(input.rdstate() & ~std::ios_base::failbit);
			container.swap(tmp_container);	
		}
	}
	
	return input;
}


/* read sequences into a container with a compare (i.e., a set), storing sequences as pointers */
template <template <typename, typename, typename> class Container, typename Value, typename Compare, typename Allocator>
std::istream & operator>>(std::istream &input, Container<sequence<Value> *, Compare, Allocator> &container) {
	std::istream::sentry sentry(input, false);
	if(sentry) {
		Container<sequence<Value> *, Compare, Allocator> tmp_container;
		
		try {
			sequence<Value> seq;
			
			while(input >> seq) {
				sequence<Value> *tmp_seq = new sequence<Value>();
				seq.swap(*tmp_seq);
				
				try {
					tmp_container.insert(tmp_container.end(), tmp_seq);
				}
				catch(...) {
					delete tmp_seq;
					throw;
				}
			}
			
			if(input.bad()) {
				// IO error
			}
			else if(input.fail() && !input.eof()) {
				// conversion error
			}
			else {
				input.clear(input.rdstate() & ~std::ios_base::failbit);
				container.swap(tmp_container);	
			}
		}
		catch(...) {
			typename Container<sequence<Value> *, Compare, Allocator>::iterator tmp_container_iter = tmp_container.begin();
			for(; tmp_container_iter != tmp_container.end(); ++tmp_container_iter) {
				delete *tmp_container_iter;
			}
			
			throw;
		}
	}
	
	return input;
}

/* read labeled sequences into a container */
template <template <typename, typename> class Container, typename Value, typename Allocator>
std::istream & operator>>(std::istream &input, Container<std::pair<bool,sequence<Value> >, Allocator> &container) {
	std::istream::sentry sentry(input, false);
	if(sentry) {
		std::pair<bool,sequence<Value> > labeled_seq;
		Container<std::pair<bool,sequence<Value> >, Allocator> tmp_container;
		while(input >> labeled_seq.second && input >> labeled_seq.first) {
			tmp_container.insert(tmp_container.end(), labeled_seq);
		}
		
		if(input.bad()) {
			// IO error
		}
		else if(input.fail() && !input.eof()) {
			// conversion error
		}
		else {
			input.clear(input.rdstate() & ~std::ios_base::failbit);
			container.swap(tmp_container);	
		}
	}
	
	return input;
}

#endif
