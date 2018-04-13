#ifndef _LAZY_FREQUENT_HH_
#define _LAZY_FREQUENT_HH_

#include <iostream>
#include <sstream>

#include <list>
#include <map>
#include <vector>

#include <algorithm>

#include <stdexcept>

#include <cmath>

#include "ordered_set.hh"
#include "sequence.hh"
#include "sequential_pattern_miner.hh"

template <typename Value1, typename Value2>
class lazy_frequent : public sequential_pattern_miner<Value1,Value2> {
	public:
		using sequential_pattern_miner<Value1,Value2>::mine;
		
		#ifndef NDEBUG
		using sequential_pattern_miner<Value1,Value2>::update_graph;
		#endif
		
	protected:
		
		virtual void mine_patterns(std::vector<const sequence<Value1> *> &database, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<Value1> > > &return_patterns) const {
			#ifndef NDEBUG
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
			#endif
			
			mine_patterns(database.begin(), database.end(), items, min_support, return_patterns);
		}
		
	private:
		void mine_patterns(typename std::vector<const sequence<Value2> *>::iterator db_begin, typename std::vector<const sequence<Value2> *>::iterator db_end, sequence<Value2> &seq, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<Value2> > > &return_patterns) const {
			//std::cout << std::distance(db_begin, db_end) << " " << seq << std::endl;
			maintain_and_test(seq, std::distance(db_begin, db_end), return_patterns);
			
			std::vector<Value2> frequent_items(items);
			
			typename std::vector<const sequence<Value2> *>::iterator end_iter;
			typename std::vector<Value2>::iterator frequent_items_iter = frequent_items.begin();
			while(frequent_items_iter != frequent_items.end()) {
				seq.push_element(*frequent_items_iter);
				
				//end_iter = std::partition(db_begin, db_end, is_subsequence<Value2>(seq));
				end_iter = project_database(db_begin, db_end, is_subsequence<Value2>(seq), min_support);
				
				if(std::distance(db_begin, end_iter) >= min_support) {
					#ifndef NDEBUG
					update_graph(seq, frequent_items);
					#endif
					
					mine_patterns(db_begin, end_iter, seq, frequent_items, min_support, return_patterns);
					++frequent_items_iter;
				}
				else {
					std::swap(*frequent_items_iter, frequent_items.back());
					frequent_items.pop_back();
				}
				
				seq.pop_element();
			}
		}
		
		void mine_patterns(typename std::vector<const sequence<Value2> *>::iterator db_begin, typename std::vector<const sequence<Value2> *>::iterator db_end, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<Value2> > > &return_patterns) const {
			sequence<Value2> seq;
			
			typename std::vector<const sequence<Value2> *>::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			while(items_iter != items.end()) {
				seq.push_element(*items_iter);
				
				//end_iter = std::partition(db_begin, db_end, is_subsequence<Value2>(seq));
				end_iter = project_database(db_begin, db_end, is_subsequence<Value2>(seq), min_support);
				
				#ifndef NDEBUG
				update_graph(seq, items);
				#endif
				
				mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
				++items_iter;
				
				seq.pop_element();
			}
		}
		
		void mine_patterns(typename std::vector<const sequence<ordered_set<Value2> > *>::iterator db_begin, typename std::vector<const sequence<ordered_set<Value2> > *>::iterator db_end, sequence<ordered_set<Value2> > &seq, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<ordered_set<Value2> > > > &return_patterns) const {
			//std::cout << std::distance(db_begin, db_end) << " " << seq << std::endl;
			maintain_and_test(seq, std::distance(db_begin, db_end), return_patterns);
			
			typename std::vector<const sequence<ordered_set<Value2> > *>::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				if(seq.push_item(*items_iter)) {
					//end_iter = std::partition(db_begin, db_end, is_subsequence<Value1>(seq));
					end_iter = project_database(db_begin, db_end, is_subsequence<Value1>(seq), min_support);
					
					if(std::distance(db_begin, end_iter) >= min_support) {
						#ifndef NDEBUG
						update_graph(seq, items);
						#endif
						
						mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
					}
					
					seq.pop_item();
				}
			}
			
			std::vector<Value2> frequent_items(items);
			
			seq.push_element(ordered_set<Value2>());
			typename std::vector<Value2>::iterator frequent_items_iter = frequent_items.begin();
			while(frequent_items_iter != frequent_items.end()) {
				seq.push_item(*frequent_items_iter);
				
				//end_iter = std::partition(db_begin, db_end, is_subsequence<Value1>(seq));
				end_iter = project_database(db_begin, db_end, is_subsequence<Value1>(seq), min_support);
				
				if(std::distance(db_begin, end_iter) >= min_support) {
					#ifndef NDEBUG
					update_graph(seq, frequent_items);
					#endif
					
					mine_patterns(db_begin, end_iter, seq, frequent_items, min_support, return_patterns);
					++frequent_items_iter;
				}
				else {
					std::swap(*frequent_items_iter, frequent_items.back());
					frequent_items.pop_back();
				}
				
				seq.pop_item();
			}
			seq.pop_element();
		}
		
		void mine_patterns(typename std::vector<const sequence<ordered_set<Value2> > *>::iterator db_begin, typename std::vector<const sequence<ordered_set<Value2> > *>::iterator db_end, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<ordered_set<Value2> > > > &return_patterns) const {
			sequence<ordered_set<Value2> > seq;
			seq.push_element(ordered_set<Value2>());
			
			typename std::vector<const sequence<ordered_set<Value2> > *>::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			while(items_iter != items.end()) {
				seq.push_item(*items_iter);
				
				//end_iter = std::partition(db_begin, db_end, is_subsequence<Value1>(seq));
				end_iter = project_database(db_begin, db_end, is_subsequence<Value1>(seq), min_support);
				
				#ifndef NDEBUG
				update_graph(seq, items);
				#endif
				
				mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
				++items_iter;
				
				seq.pop_item();
			}
			
			seq.pop_element();
		}
		
};

#endif
