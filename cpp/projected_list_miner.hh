#ifndef _PROJECTED_LIST_MINER_HH_
#define _PROJECTED_LIST_MINER_HH_

#include <iostream>
#include <sstream>

#include <vector>

#include <utility>

#include <algorithm>

#include <stdexcept>

#include <cmath>

#include "ordered_set.hh"
#include "sequence.hh"
#include "sequential_pattern_miner.hh"

template <typename T>
class projected_sequence {
	public:
		projected_sequence() : seq(NULL) {
		
		}
		
		projected_sequence(const sequence<T> &seq) : seq(&seq), projections() {
			const_iterator iter = projected_sequence::seq->begin();
			projections.push_back(iter);
		}
		
		projected_sequence(const sequence<T> *seq) : seq(seq), projections() {
			const_iterator iter = projected_sequence::seq->begin();
			projections.push_back(iter);
		}
		
		projected_sequence & operator=(const projected_sequence &other) {
			seq = other.seq;
			projections = other.projections;
			
			return *this;
		}
		
		bool project(const T &item) {
			const_iterator iter = projections.back();
			while(iter != seq->end()) {
				if(*iter == item) {
					++iter;
					projections.push_back(iter);
					return true;
				}
				
				++iter;
			}
			
			return false;
		}
		
		void pop_projection() {
			projections.pop_back();
		}
		
		void swap(projected_sequence &other) {
			std::swap(seq, other.seq);
			std::swap(projections, other.projections);
		}
		
	protected:
		typedef typename sequence<T>::const_iterator const_iterator;
		
		const sequence<T> *seq;
		std::vector<const_iterator> projections;
};

template <typename Value1, typename Value2>
struct project_sequence {
	const Value2 &item;
	project_sequence(const Value2 &item) : item(item) {
	
	}
	bool operator()(projected_sequence<Value1> &seq) const {
		return seq.project(item);
	}
};

template <typename T>
class projected_sequence<ordered_set<T> > {
	public:
		projected_sequence() : seq(NULL) {
		
		}
		
		projected_sequence(const sequence<ordered_set<T> > &seq) : seq(&seq), projections() {
			std::pair<sequence_iterator,set_iterator> iter;
			iter.first = projected_sequence::seq->begin();
			if(iter.first != projected_sequence::seq->end()) {
				iter.second = iter.first.begin();
			}
			
			projections.push_back(iter);
		}
		
		projected_sequence(const sequence<ordered_set<T> > *seq) : seq(seq), projections() {
			std::pair<sequence_iterator,set_iterator> iter;
			iter.first = projected_sequence::seq->begin();
			if(projected_sequence::seq->begin() != projected_sequence::seq->end()) {
				iter.second = iter.first->begin();
			}
			
			projections.push_back(iter);
		}
		
		projected_sequence & operator=(const projected_sequence &other) {
			seq = other.seq;
			projections = other.projections;
			
			return *this;
		}
		
		bool project_item(const T &item) {
			std::pair<sequence_iterator,set_iterator> iter = projections.back();
			while(iter.second != iter.first->end()) {
				// TRY? iter.second = lower_bound(iter.second, iter.first->end(), item)
				if(*(iter.second) == item) {
					++iter.second;
					projections.push_back(iter);
					
					return true;
				}
				else if(*(iter.second) > item) {
					return false;
				}
				
				++iter.second;
			}
			
			return false;
		}
		
		bool project_set(const T &item) {
			std::pair<sequence_iterator,set_iterator> iter = projections.back();
			if(iter.first != seq->end()) {
				++iter.first;
				
				if(iter.first != seq->end()) {
					iter.second = iter.first->begin();
				}
			}
			
			while(iter.first != seq->end()) {
				while(iter.second != iter.first->end()) {
					// TRY? iter.second = lower_bound(iter.second, iter.first->end(), item)
					if(*(iter.second) == item) {
						++iter.second;
						projections.push_back(iter);
						
						return true;
					}
					else if(*(iter.second) > item) {
						return false;
					}
					
					++iter.second;
				}
				
				
				iter.second = (++iter.first)->begin();
			}
			
			return false;
		}
		
		void pop_projection() {
			projections.pop_back();
		}
		
		void swap(projected_sequence &other) {
			std::swap(seq, other.seq);
			std::swap(projections, other.projections);
		}
		
	protected:
		typedef typename sequence<ordered_set<T> >::const_iterator sequence_iterator;
		typedef typename ordered_set<T>::const_iterator set_iterator;
		const sequence<ordered_set<T> > *seq;
		std::vector<std::pair<sequence_iterator,set_iterator> > projections;
};

template <typename Value1, typename Value2>
struct project_item {
	const Value2 &item;
	project_item(const Value2 &item) : item(item) {
	
	}
	bool operator()(projected_sequence<Value1> &seq) const {
		return seq.project_item(item);
	}
};

template <typename Value1, typename Value2>
struct project_set {
	const Value2 &item;
	project_set(const Value2 &item) : item(item) {
	
	}
	bool operator()(projected_sequence<Value1> &seq) const {
		return seq.project_set(item);
	}
};

template <typename T>
void swap(projected_sequence<T> &x, projected_sequence<T> &y) {
	x.swap(y);
}

template <typename Value1, typename Value2>
class projected_list_miner : public sequential_pattern_miner<Value1,Value2> {
	public:
		using sequential_pattern_miner<Value1,Value2>::mine;
		
	protected:
		
		virtual void mine_patterns(std::vector<const sequence<Value1> *> &database, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<Value1> > > &return_patterns) const {
			#ifndef NDEBUG
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			#endif
			
			std::vector<projected_sequence<Value1> > projected_database;
			projected_database.reserve(database.size());
			
			typename std::vector<const sequence<Value1> *>::iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				projected_database.push_back(projected_sequence<Value1>(*database_iter));
			}
			
			mine_patterns(projected_database.begin(), projected_database.end(), items, min_support, return_patterns);
		}
		
	private:
		void mine_patterns(typename std::vector<projected_sequence<Value2> >::iterator db_begin, typename std::vector<projected_sequence<Value2> >::iterator db_end, sequence<Value2> &seq, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<Value2> > > &return_patterns) const {
			//std::cout << std::distance(db_begin, db_end) << " " << seq << std::endl;
			maintain_and_test(seq, std::distance(db_begin, db_end), return_patterns);
			
			typename std::vector<projected_sequence<Value2> >::iterator tmp_iter;
			typename std::vector<projected_sequence<Value2> >::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				seq.push_element(*items_iter);
				
				end_iter = std::partition(db_begin, db_end, project_sequence<Value1,Value2>(*items_iter));
				
				if(std::distance(db_begin, end_iter) >= min_support) {
					mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
				}
				
				tmp_iter = db_begin;
				for(tmp_iter = db_begin; tmp_iter != end_iter; ++tmp_iter) {
					tmp_iter->pop_projection();
				}
				
				seq.pop_element();
			}
		}
		
		void mine_patterns(typename std::vector<projected_sequence<Value2> >::iterator db_begin, typename std::vector<projected_sequence<Value2> >::iterator db_end, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<Value2> > > &return_patterns) const {
			sequence<Value2> seq;
			
			typename std::vector<projected_sequence<Value2> >::iterator tmp_iter;
			typename std::vector<projected_sequence<Value2> >::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				seq.push_element(*items_iter);
				
				end_iter = std::partition(db_begin, db_end, project_sequence<Value1,Value2>(*items_iter));
				mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
				
				tmp_iter = db_begin;
				for(tmp_iter = db_begin; tmp_iter != end_iter; ++tmp_iter) {
					tmp_iter->pop_projection();
				}
				
				seq.pop_element();
			}
		}
		
		void mine_patterns(typename std::vector<projected_sequence<ordered_set<Value2> > >::iterator db_begin, typename std::vector<projected_sequence<ordered_set<Value2> > >::iterator db_end, sequence<ordered_set<Value2> > &seq, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<ordered_set<Value2> > > > &return_patterns) const {
			//std::cout << std::distance(db_begin, db_end) << " " << seq << std::endl;
			maintain_and_test(seq, std::distance(db_begin, db_end), return_patterns);
			
			typename std::vector<projected_sequence<ordered_set<Value2> > >::iterator tmp_iter;
			typename std::vector<projected_sequence<ordered_set<Value2> > >::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				if(seq.push_item(*items_iter)) {
					end_iter = std::partition(db_begin, db_end, project_item<Value1,Value2>(*items_iter));
					
					if(std::distance(db_begin, end_iter) >= min_support) {
						mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
					}
					
					tmp_iter = db_begin;
					for(tmp_iter = db_begin; tmp_iter != end_iter; ++tmp_iter) {
						tmp_iter->pop_projection();
					}
					
					seq.pop_item();
				}
			}
			
			seq.push_element(ordered_set<Value2>());
			items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				seq.push_item(*items_iter);
				
				end_iter = std::partition(db_begin, db_end, project_set<Value1,Value2>(*items_iter));
				
				if(std::distance(db_begin, end_iter) >= min_support) {
					mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
				}
				
				tmp_iter = db_begin;
				for(tmp_iter = db_begin; tmp_iter != end_iter; ++tmp_iter) {
					tmp_iter->pop_projection();
				}
				
				seq.pop_item();
			}
			seq.pop_element();
		}
		
		void mine_patterns(typename std::vector<projected_sequence<ordered_set<Value2> > >::iterator db_begin, typename std::vector<projected_sequence<ordered_set<Value2> > >::iterator db_end, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<ordered_set<Value2> > > > &return_patterns) const {
			sequence<ordered_set<Value2> > seq;
			seq.push_element(ordered_set<Value2>());
			
			typename std::vector<projected_sequence<ordered_set<Value2> > >::iterator tmp_iter;
			typename std::vector<projected_sequence<ordered_set<Value2> > >::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				seq.push_item(*items_iter);
				
				end_iter = std::partition(db_begin, db_end, project_set<Value1,Value2>(*items_iter));
				
				mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
				
				tmp_iter = db_begin;
				for(tmp_iter = db_begin; tmp_iter != end_iter; ++tmp_iter) {
					tmp_iter->pop_projection();
				}
				
				seq.pop_item();
			}
			seq.pop_element();
		}
		
};

#endif
