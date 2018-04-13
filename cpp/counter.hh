#ifndef _COUNTER_HH_
#define _COUNTER_HH_

#include <iostream>
#include <sstream>
#include <ostream>

#include <map>
#include <set>

#include <stdexcept>
#include <cassert>

template <typename T>
class counter {
	public:
		typedef long int size_type;
		typedef typename std::set<std::pair<size_type,T> >::iterator iterator;
		typedef typename std::set<std::pair<size_type,T> >::const_iterator const_iterator;
		
		counter() : count_map(), count_set(){
		
		}
		
		counter(const counter &other) : count_map(other.count_map),  count_set(count_set) {
		
		}
		
		counter & operator=(const counter &other) {
			count_map = other.count_map;
			count_set = other.count_set;
			
			return *this;
		}
		
		virtual ~counter() {
		
		}
		
		iterator begin() {
			return count_set.begin();
		}
		
		const_iterator begin() const {
			return count_set.begin();
		}
		
		iterator end() {
			return count_set.end();
		}
		
		const_iterator end() const {
			return count_set.end();
		}
		
		size_type size() const {
			return count_set.size();
		}
		
		bool empty() const {
			return count_set.empty();
		}
		
		void update(const T &item) {
			update(item, 1);
		}
		
		void update(const T &item, size_type count) {
			std::pair<typename std::map<T,size_type>::iterator,bool> count_map_ret = count_map.insert(std::pair<T,size_type>(item,count));
			if(count_map_ret.second) {
				#ifndef NDEBUG
				std::pair<iterator,bool> count_set_ret = count_set.insert(std::pair<size_type,T>(count,item));
				assert(count_set_ret.second);
				#else
				count_set.insert(std::pair<size_type,T>(count,item));
				#endif
			}
			else {
				iterator count_set_hint_iter = count_set.find(std::pair<size_type,T>(count_map_ret.first->second,item));
				assert(count_set_hint_iter != count_set.end());
				
				count_map_ret.first->second += count;
				
				iterator count_set_remove_iter = count_set_hint_iter;
				
				// in C++11, this should be the next item (not the previous)
				if(count_set_remove_iter != count_set.begin()) {
					count_set_hint_iter--;
					count_set.erase(count_set_remove_iter);
					count_set.insert(count_set_hint_iter, std::pair<size_type,T>(count_map_ret.first->second,item));
				}
				else {
					count_set.erase(count_set_remove_iter);
					count_set.insert(std::pair<size_type,T>(count_map_ret.first->second,item));
				}
				
				
			}
		}
		
		void clear() {
			count_map.clear();
			count_set.clear();
		}
		
		template <typename Iter>
		void update(Iter begin, Iter end) {
			/*std::map<T,size_type> tmp_map;
			while(begin != end) {;
				tmp_map[*begin] += 1;
				++begin;
			}
			
			typename std::map<T,size_type>::iterator tmp_map_iter = tmp_map.begin();
			for(; tmp_map_iter != tmp_map.end(); ++tmp_map_iter) {
				update(tmp_map_iter->first, tmp_map_iter->second);
			}*/
			
			while(begin != end) {
				update(*begin, (size_type)1);
				++begin;
			}
		}
		
		void update(const counter &other) {
			iterator other_counts_iter = other.begin();
			
			for(; other_counts_iter != other.end(); ++other_counts_iter) {
				update(other_counts_iter->second, other_counts_iter->first);
			}
		}
		
		const T & most_common() const {
			typename std::map<T,size_type>::const_iterator count_set_iter = count_set.begin();
			if(count_set_iter == count_set.end()) {
				std::ostringstream oss;
				oss << "counter is empty";
				
				throw std::runtime_error(oss.str());
			}
			
			return count_set->second;
		}
		
		size_type operator[](const T &item) const {
			typename std::map<T,size_type>::const_iterator count_map_iter = count_map.find(item);
			if(count_map_iter == count_map.end()) {
				return 0;
			}
			else {
				return count_map_iter->second;
			}
		}
		
		virtual std::ostream & serialize(std::ostream &output) const {
			output << "{";
			const_iterator iter = begin();
			while(iter != end()) {
				output << "(" << iter->second << ":" << iter->first << ")";
				++iter;
				
				if(iter != end()) {
					output << " ";
				}
			}
			output << "}";
			
			return output;
		}
		
		friend std::ostream & operator<<(std::ostream &output, const counter &counts) {
			counts.serialize(output);
			
			return output;
		}
		
	protected:
		struct reverse_comp {
			bool operator() (const std::pair<size_type,T> &lhs, const std::pair<size_type,T> &rhs) const {
				if(lhs.first > rhs.first) {
					return true;
				}
				else if(lhs.first == rhs.first) {
					return lhs.second <  rhs.second;
				}
				
				return false;
			}
		};
		
		std::map<T,size_type> count_map;
		std::set<std::pair<size_type,T>,reverse_comp> count_set;

};

#endif
