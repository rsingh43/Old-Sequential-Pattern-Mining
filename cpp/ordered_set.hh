#ifndef _ORDERED_SET_HH_
#define _ORDERED_SET_HH_

#include <iostream>
#include <ios>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <fstream>

#include <locale>

#include <set>
#include <string>
#include <vector>

#include <functional>
#include <algorithm>

#include <typeinfo>

#include <stdexcept>

#include <cerrno>
#include <cstring>

#include "infix_iterator.hh"

template <typename T>
class ordered_set {
	public:
		typedef typename std::vector<T>::size_type size_type; 
		typedef typename std::vector<T>::iterator iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;
		
		ordered_set() : elements() {
			
		}
		
		ordered_set(const T &element) : elements(1, element) {
			
		}
		
		ordered_set(const ordered_set &other) {
			elements = other.elements;
		}
		
		virtual ~ordered_set() {
		
		}
		
		ordered_set & operator=(const ordered_set &other) {
			elements = other.elements;
			return *this;
		}
		
		iterator begin() {
			return elements.begin();
		}
		
		iterator end() {
			return elements.end();
		}
		
		const_iterator begin() const {
			return elements.begin();
		}
		
		const_iterator end() const {
			return elements.end();
		}
		
		size_type size() const {
			return elements.size();
		}
		
		bool empty() const {
			return elements.empty();
		}
		
		bool add_element(const T &element) {
			if(!elements.empty() && (elements.back() >= element)) {
				return false;
			}
			
			elements.push_back(element);
			return true;
		}
		
		T remove_element() {
			T element = elements.back();
			elements.pop_back();
			return element;
		}
		
		const T & get_element() const {
			return elements.back();
		}
		
		const T & operator[](size_type index) const {
			return elements[index];
		}
		
		void clear() {
			elements.clear();
		}
		
		bool contains(const T &element) const {
			return std::binary_search(begin(), end(), element);
		}
		
		bool subset(const ordered_set &that) const {
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
				if(*this_iter < *that_iter) {
					return false;
				}
				else if(*that_iter < *this_iter) {
					++that_iter;
				}
				else {
					++this_iter;
					++that_iter;
				}
			}
			
			return this_iter == end();
		}
		
		virtual void swap(ordered_set &other) {
			elements.swap(other.elements);
		}
		
		
		
		
		virtual std::ostream & serialize(std::ostream &output) const {
			output << "(";
			std::copy(begin(), end(), infix_ostream_iterator<T>(output, ","));
			output << ")";
			
			return output;
		}
		
		friend std::ostream & operator<<(std::ostream &output, const ordered_set &set) {
			return set.serialize(output);
		}
		
		virtual std::istream & deserialize(std::istream &input) {
			std::istream::sentry sentry(input);
			if(sentry) {
				if(input.peek() != '(') {
					#ifndef NDEBUG
					std::ostringstream oss;
					oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << (char)input.peek() << "' when expencting '('" << std::endl;
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
					oss << __FILE__ << ":" << __LINE__ << " error reading delimiter ')'" << std::endl;
					std::cerr << oss.str();
					#endif
						
					return input;
				}
				else if(delim != '(') {
					#ifndef NDEBUG
					std::ostringstream oss;
					oss << __FILE__ << ":" << __LINE__ << " error reading delimiter '('" << std::endl;
					std::cerr << oss.str();
					#endif
					
					input.setstate(std::ios_base::failbit);
					return input;
				}
					
				std::set<T> tmp_elements;
					
				T element;
				while(delim != ')') { 
					input >> element;
					if(input.fail()) {
						#ifndef NDEBUG
						std::ostringstream oss;
						oss << "error converting ordered_set item (" << typeid(T).name() << ")" << std::endl;
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
					else if(delim != ',' && delim != ')') {
						#ifndef NDEBUG
						std::ostringstream oss;
						
						oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << delim << "' when expencting ',' or ')'" << std::endl;
						std::cerr << oss.str();
						#endif
						
						input.setstate(std::ios_base::failbit);
						return input;
					}
					
					tmp_elements.insert(element);
				}
				
				std::vector<T> tmp_vector(tmp_elements.begin(), tmp_elements.end());
				elements.swap(tmp_vector);
			}
				
			return input;
		}
				
		friend std::istream & operator>>(std::istream &input, ordered_set &set) {
			return set.deserialize(input);
		}
		
		friend bool operator<(const ordered_set &lhs, const ordered_set &rhs) {
			return lhs.elements.size() < rhs.elements.size() || (lhs.elements.size() == rhs.elements.size() && lhs.elements < rhs.elements);
		}
		friend bool operator>(const ordered_set &lhs, const ordered_set &rhs) {
			return rhs < lhs;
		}
		friend bool operator<=(const ordered_set &lhs, const ordered_set &rhs) {
			return !(lhs > rhs);
		}
		friend bool operator>=(const ordered_set &lhs, const ordered_set &rhs) {
			return !(lhs < rhs);
		}
		
		friend bool operator==(const ordered_set &lhs, const ordered_set &rhs) {
			return lhs.elements == rhs.elements;	
		}
		friend bool operator!=(const ordered_set &lhs, const ordered_set &rhs) {
			return !(lhs == rhs);
		}
		
	protected:
		std::vector<T> elements;
};

template <>
std::istream & ordered_set<std::string>::deserialize(std::istream &input) {
	std::istream::sentry sentry(input);
	if(sentry) {
		if(input.peek() != '(') {
			#ifndef NDEBUG
			std::ostringstream oss;
			oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << (char)input.peek() << "' when expencting '('" << std::endl;
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
			oss << __FILE__ << ":" << __LINE__ << " error reading delimiter '('" << std::endl;
			std::cerr << oss.str();
			#endif
				
			return input;
		}
		else if(delim != '(') {
			#ifndef NDEBUG
			std::ostringstream oss;
			oss << __FILE__ << ":" << __LINE__ << " error reading delimiter '('" << std::endl;
			std::cerr << oss.str();
			#endif
			
			input.setstate(std::ios_base::failbit);
			return input;
		}
			
		std::set<std::string,std::less<std::string> > tmp_elements;
		
		std::string element;
		while(delim != ')') {
			element.clear();
			
			char next_char = input.peek();
			while(next_char != ',' && next_char != ')') {
				element.push_back(next_char);
				input.ignore();
				next_char = input.peek();
			}
			
			if(input.fail()) {
				#ifndef NDEBUG
				std::ostringstream oss;
				oss << "error converting ordered_set item (" << typeid(std::string).name() << ")" << std::endl;
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
			else if(delim != ',' && delim != ')') {
				#ifndef NDEBUG
				std::ostringstream oss;
				oss << __FILE__ << ":" << __LINE__ << " unexpected delimiter, found '" << delim << "' when expencting ',' or ')'" << std::endl;
				std::cerr << oss.str();
				#endif
				
				input.setstate(std::ios_base::failbit);
				return input;
			}
			
			tmp_elements.insert(element);
		}
		
		std::vector<std::string> tmp_vector(tmp_elements.begin(), tmp_elements.end());
		elements.swap(tmp_vector);
	}
		
	return input;
}

template <typename T>
void swap (ordered_set<T> &x, ordered_set<T> &y) {
	x.swap(y);
}

#endif
