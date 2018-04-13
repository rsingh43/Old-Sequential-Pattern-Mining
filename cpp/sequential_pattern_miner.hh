#ifndef _SEQUENTIAL_PATTERN_MINER_HH_
#define _SEQUENTIAL_PATTERN_MINER_HH_

#include <iostream>
#include <sstream>

#include <list>
#include <map>
#include <set>

#include <algorithm>

#include <stdexcept>

#include <cmath>

#include "timer.hh"
#include "ordered_set.hh"
#include "sequence.hh"

#include "maintain_and_test.hh"

template <typename Iter, typename Test>
Iter project_database(Iter first, Iter last, Test pred, unsigned int min_support) {
	unsigned int max_support = std::distance(first, last);
	while(first != last) {
		while(pred(*first)) {
			if(++first == last) {
				return first;
			}
		}
		--max_support;
		
		if(first == --last) {
			return first;
		}
		
		while(!pred(*last)) {
			if((--max_support < min_support) || (first == --last)) {
				return first;
			}
		}
		
		std::swap(*first,*last);
		++first;
	}
	return first;
}

template <typename Value1, typename Value2>
class sequential_pattern_miner {
	public:
		#ifndef NDEBUG
		mutable std::vector<std::vector<std::string> > graph;
		
		void update_graph(sequence<Value1> &seq, const std::vector<Value2> &items) const {
			graph.resize(graph.size()+1);
			std::vector<std::string> &edge = graph.back();
			
			std::ostringstream oss;
			
			Value1 element = seq.pop_element();
			
			oss << seq;
			edge.push_back(oss.str());
			oss.clear();
			oss.str("");
			
			seq.push_element(element);
			
			oss << items.size() << " = {";
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			while(items_iter != items.end()) {
			
				oss << *items_iter;
				if(++items_iter != items.end()) {
					oss << ",";
				}
			}
			oss << "}";
			edge.push_back(oss.str());
			oss.clear();
			oss.str("");
			
			oss << seq;
			edge.push_back(oss.str());
			oss.clear();
			oss.str("");
			
		}
		
		void write_graph(const std::string &output_graph_name) {
			std::string output_graph_name(database_file);
			output_graph_name += ".gv";
		
			std::ofstream output_graph_file(output_graph_name.c_str());
			if(output_graph_file) {
				output_graph_file << "digraph G {" << std::endl;
				
				std::vector<std::vector<std::string> >::iterator graph_iter = graph.begin();
				for(; graph_iter != graph.end(); ++graph_iter) {
					output_graph_file << "\t\"" << graph_iter->at(0) << "\" -> \"" << graph_iter->at(2) << "\" [label=\"" << graph_iter->at(1) << "\"]" << std::endl;
				}
				output_graph_file << "}" << std::endl;
				
				if(output_graph_file.bad()) {
					std::ostringstream oss;
					oss <<  output_graph_name << ": " << strerror(errno);
					
					throw std::runtime_error(oss.str());
				}
			}
			else {
				std::ostringstream oss;
				oss <<  output_graph_name << ": " << strerror(errno);
				
				throw std::runtime_error(oss.str());
			}
		}
		
		#endif
		
		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<sequence<Value1>, Alloc> &database, double relative_support, bool strip_sequences, std::map<unsigned int,std::list<sequence<Value1> > > &return_patterns) const {
			if(relative_support < 0.0 || relative_support > 1.0) {
				std::ostringstream oss;
				oss << "domain error: invalid support (" << relative_support << "): support must be between 0.0 and 1.0";
				throw std::domain_error(oss.str());
			}
			
			mine(database, (unsigned int) ceil(database.size() * relative_support), strip_sequences, return_patterns);
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<sequence<Value1>, Alloc> &database, unsigned int absolute_support, bool strip_sequences, std::map<unsigned int,std::list<sequence<Value1> > > &return_patterns) const {
			std::set<Value2> itemset;
			
			std::cout << "Extract Items: ";
			std::cout.flush();
			
			time.tic();
			extract_items(database, itemset);
			std::cerr << time.toc() << ",";
			std::cerr.flush();
			
			std::cout << std::endl;
			
			mine(database, itemset, absolute_support, strip_sequences, return_patterns);
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<sequence<Value1>, Alloc> &database, const std::set<Value2> &items, double relative_support, bool strip_sequences, std::map<unsigned int,std::list<sequence<Value1> > > &return_patterns) const {
			mine(database, items, (unsigned int) ceil(database.size() * relative_support), strip_sequences, return_patterns);
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<sequence<Value1>, Alloc> &database, const std::set<Value2> &items, unsigned int absolute_support, bool strip_sequences, std::map<unsigned int,std::list<sequence<Value1> > > &return_patterns) const {
			std::vector<Value2> frequent_items;
			
			std::cout << "Extract Frequent Items: ";
			std::cout.flush();
			
			time.tic();
			extract_frequent_items(database, items, absolute_support, frequent_items);
			std::cerr << time.toc() << ",";
			std::cerr.flush();
			
			std::cout << std::endl;
			
			mine(database, frequent_items, absolute_support, strip_sequences, return_patterns);
		}
	protected:
		template <template <typename, typename> class Container, typename Value, typename Alloc>
		void extract_items(const Container<sequence<Value>, Alloc> &database, std::set<Value> &itemset) const {
			typename Container<sequence<Value>, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				itemset.insert(database_iter->begin(), database_iter->end());
			}
		}
		
		template <template <typename, typename> class Container, typename Value, typename Alloc>
		void extract_items(const Container<sequence<ordered_set<Value> >, Alloc> &database, std::set<Value> &itemset) const {
			typename Container<sequence<ordered_set<Value> >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				const sequence<ordered_set<Value> > &seq = *database_iter;
				
				typename sequence<ordered_set<Value> >::const_iterator seq_iter = seq.begin();
				for(; seq_iter != seq.end(); ++seq_iter) {
					itemset.insert(seq_iter->begin(), seq_iter->end());
				}
			}
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void extract_frequent_items(const Container<sequence<Value2>, Alloc> &database, const std::set<Value2> &items, unsigned int min_support, std::vector<Value2> &frequent_items) const {
			std::map<Value2,unsigned int> counts;
			
			typename Container<sequence<Value1>, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				std::set<Value2> seq_items(database_iter->begin(), database_iter->end());
				
				typename std::set<Value2>::iterator seq_items_iter = seq_items.begin();
				for(; seq_items_iter != seq_items.end(); ++seq_items_iter) {
					counts[*seq_items_iter]++;
				}
			}
			
			frequent_items.reserve(items.size());
			typename std::set<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				if(counts[*items_iter] >= min_support) {
					frequent_items.push_back(*items_iter);
				}
			}
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void extract_frequent_items(const Container<sequence<ordered_set<Value2> >, Alloc> &database, const std::set<Value2> &items, unsigned int min_support, std::vector<Value2> &frequent_items) const {
			std::map<Value2,unsigned int> counts;
			
			typename Container<sequence<ordered_set<Value2> >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				std::set<Value2> seq_items;
				
				const sequence<ordered_set<Value2> > &tmp_seq = *database_iter;
				
				typename sequence<ordered_set<Value2> >::const_iterator tmp_seq_iter = tmp_seq.begin();
				for(; tmp_seq_iter != tmp_seq.end(); ++tmp_seq_iter) {
					seq_items.insert(tmp_seq_iter->begin(), tmp_seq_iter->end());
				}
				
				typename std::set<Value2>::iterator seq_items_iter = seq_items.begin();
				for(; seq_items_iter != seq_items.end(); ++seq_items_iter) {
					counts[*seq_items_iter]++;
				}
			}
			
			frequent_items.reserve(items.size());
			typename std::set<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				if(counts[*items_iter] >= min_support) {
					frequent_items.push_back(*items_iter);
				}
			}
		}
		
	protected:
		mutable timer<long double> time;
		
		struct get_pointer {
			template <typename T>
			T * operator()(T &item) {
				return &item;
			}
		};
		
		template <typename Value>
		struct strip_sequence {
			const std::vector<Value> &items;
			strip_sequence(const std::vector<Value> &items) : items(items) {
			
			}
			
			sequence<Value> operator()(const sequence<Value> &seq) const {
				sequence<Value> new_seq;
				typename sequence<Value>::const_iterator seq_iter = seq.begin();
				for(; seq_iter != seq.end(); ++seq_iter) {
					if(std::binary_search(items.begin(), items.end(), *seq_iter)) {
						new_seq.push_element(*seq_iter);
					}
				}
				
				return new_seq;
			}
			
			sequence<ordered_set<Value> > operator()(const sequence<ordered_set<Value> > &seq) const {
				sequence<ordered_set<Value> > new_seq;
				new_seq.push_element(ordered_set<Value>());
				
				typename sequence<ordered_set<Value> >::const_iterator seq_iter = seq.begin();
				for(; seq_iter != seq.end(); ++seq_iter) {
					const ordered_set<Value> &tmp_set = *seq_iter;
					
					ordered_set<Value> &new_set = new_seq.last();
					
					typename ordered_set<Value>::const_iterator tmp_set_iter = tmp_set.begin();
					for(; tmp_set_iter != tmp_set.end(); ++tmp_set_iter) {
						if(std::binary_search(items.begin(), items.end(), *tmp_set_iter)) {
							new_set.add_element(*tmp_set_iter);
						}
					}
					
					if(!new_set.empty()) {
						new_seq.push_element(ordered_set<Value>());
					}
				}
				
				new_seq.pop_element();
				
				return new_seq;
			}
		};
		
		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<sequence<Value1>, Alloc> &database, std::vector<Value2> &frequent_items, unsigned int min_support, bool strip_sequences, std::map<unsigned int,std::list<sequence<Value1> > > &return_patterns) const {
			std::vector<sequence<Value1> > stripped_database;
			std::vector<const sequence<Value1> *> ptr_database(database.size());
			if(strip_sequences) {
				stripped_database.resize(database.size());
				
				std::cout << "Strip Sequences: ";
				std::cout.flush();
				
				time.tic();
				std::transform(database.begin(), database.end(), stripped_database.begin(), strip_sequence<Value2>(frequent_items));
				std::cerr << time.toc() << ",";
				std::cerr.flush();
				
				std::cout << std::endl;
				
				std::transform(stripped_database.begin(), stripped_database.end(), ptr_database.begin(), get_pointer());
			}
			else {
				std::transform(database.begin(), database.end(), ptr_database.begin(), get_pointer());
			}
			
			std::cout << "Mining: ";
			std::cout.flush();
			
			time.tic();
			mine_patterns(ptr_database, frequent_items, min_support, return_patterns);
			std::cerr << time.toc() << ",";
			std::cerr.flush();
			
			std::cout << std::endl;
			
			/*print_patterns(patterns);
			
			*/
		}
		
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
			
			typename std::vector<const sequence<Value2> *>::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				seq.push_element(*items_iter);
				
				//end_iter = std::partition(db_begin, db_end, is_subsequence<Value2>(seq));
				end_iter = project_database(db_begin, db_end, is_subsequence<Value2>(seq), min_support);
				
				if(std::distance(db_begin, end_iter) >= min_support) {
					#ifndef NDEBUG
					update_graph(seq, items);
					#endif
					
					mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
				}
				
				seq.pop_element();
			}
		}
		
		void mine_patterns(typename std::vector<const sequence<Value2> *>::iterator db_begin, typename std::vector<const sequence<Value2> *>::iterator db_end, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<Value2> > > &return_patterns) const {
			sequence<Value2> seq;
			
			typename std::vector<const sequence<Value2> *>::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				seq.push_element(*items_iter);
				
				//end_iter = std::partition(db_begin, db_end, is_subsequence<Value2>(seq));
				end_iter = project_database(db_begin, db_end, is_subsequence<Value2>(seq), min_support);
				
				#ifndef NDEBUG
				update_graph(seq, items);
				#endif
				
				mine_patterns(db_begin, end_iter, seq, items, min_support, return_patterns);
				
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
			
			seq.push_element(ordered_set<Value2>());
			items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				seq.push_item(*items_iter);
				
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
			seq.pop_element();
		}
		
		void mine_patterns(typename std::vector<const sequence<ordered_set<Value2> > *>::iterator db_begin, typename std::vector<const sequence<ordered_set<Value2> > *>::iterator db_end, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<ordered_set<Value2> > > > &return_patterns) const {
			sequence<ordered_set<Value2> > seq;
			seq.push_element(ordered_set<Value2>());
			
			typename std::vector<const sequence<ordered_set<Value2> > *>::iterator end_iter;
			typename std::vector<Value2>::const_iterator items_iter = items.begin();
			for(; items_iter != items.end(); ++items_iter) {
				seq.push_item(*items_iter);
				
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
			
			seq.pop_element();
		}
		
};

#endif

