#ifndef _SEQUENTIAL_PATTERN_MINER_HH_
#define _SEQUENTIAL_PATTERN_MINER_HH_

#include <iostream>
#include <sstream>

#include <list>
#include <map>
#include <queue>
#include <set>
#include <utility>

#include <algorithm>
#include <functional>

#include <stdexcept>

#include <cmath>

#include "timer.hh"
#include "ordered_set.hh"
#include "sequence.hh"

#include "maintain_and_test.hh"

template <typename T>
bool pointer_compare(T* first, T* second) {
	return *first < *second;
}

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
class subgroup_miner {
	public:
		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, double relative_support, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			if(relative_support < 0.0 || relative_support > 1.0) {
				std::ostringstream oss;
				oss << "domain error: invalid support (" << relative_support << "): support must be between 0.0 and 1.0";
				throw std::domain_error(oss.str());
			}

			unsigned int positive_count=0;
			typename Container<std::pair<bool,sequence<Value1> >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				if(database_iter->first) {
					positive_count++;
				}
			}
			
			mine(database, (unsigned int) ceil(positive_count * relative_support), generalization, strip_sequences, return_patterns);
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void beam_mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, double relative_support, unsigned int beam_width, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			if(relative_support < 0.0 || relative_support > 1.0) {
				std::ostringstream oss;
				oss << "domain error: invalid support (" << relative_support << "): support must be between 0.0 and 1.0";
				throw std::domain_error(oss.str());
			}

			unsigned int positive_count=0;
			typename Container<std::pair<bool,sequence<Value1> >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				if(database_iter->first) {
					positive_count++;
				}
			}
			
			beam_mine(database, (unsigned int) ceil(positive_count * relative_support), beam_width, generalization, strip_sequences, return_patterns);
		}
		

		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, unsigned int absolute_support, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			std::set<Value2> itemset;

			std::cout << "Extract Items: ";
			std::cout.flush();

			time.tic();
			extract_items(database, itemset);
			std::cerr << time.toc() << ",";
			std::cerr.flush();

			std::cout << std::endl;

			mine(database, itemset, absolute_support, generalization, strip_sequences, return_patterns);
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void beam_mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, unsigned int absolute_support, unsigned int beam_width, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			std::set<Value2> itemset;

			std::cout << "Extract Items: ";
			std::cout.flush();

			time.tic();
			extract_items(database, itemset);
			std::cerr << time.toc() << ",";
			std::cerr.flush();

			std::cout << std::endl;

			beam_mine(database, itemset, absolute_support, beam_width, generalization, strip_sequences, return_patterns);
		}

		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, const std::set<Value2> &items, double relative_support, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			unsigned int positive_count=0;
			typename Container<std::pair<bool,sequence<Value1> >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				if(database_iter->first) {
					positive_count++;
				}
			}

			mine(database, items, (unsigned int) ceil(positive_count * relative_support), generalization, strip_sequences, return_patterns);
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void beam_mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, const std::set<Value2> &items, double relative_support, unsigned int beam_width, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			unsigned int positive_count=0;
			typename Container<std::pair<bool,sequence<Value1> >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				if(database_iter->first) {
					positive_count++;
				}
			}

			beam_mine(database, items, (unsigned int) ceil(positive_count * relative_support), beam_width, generalization, strip_sequences, return_patterns);
		}

		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, const std::set<Value2> &items, unsigned int absolute_support, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			std::vector<Value2> frequent_items;

			std::cout << "Extract Frequent Items: ";
			std::cout.flush();

			time.tic();
			extract_frequent_items(database, items, absolute_support, frequent_items);
			std::cerr << time.toc() << ",";
			std::cerr.flush();

			std::cout << std::endl;

			mine(database, frequent_items, absolute_support, generalization, strip_sequences, return_patterns);
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void beam_mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, const std::set<Value2> &items, unsigned int absolute_support, unsigned int beam_width, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			std::vector<Value2> frequent_items;

			std::cout << "Extract Frequent Items: ";
			std::cout.flush();

			time.tic();
			extract_frequent_items(database, items, absolute_support, frequent_items);
			std::cerr << time.toc() << ",";
			std::cerr.flush();

			std::cout << std::endl;

			beam_mine(database, frequent_items, absolute_support, beam_width, generalization, strip_sequences, return_patterns);
		}

	protected:
		mutable timer<long double> time;
		
		template <template <typename, typename> class Container, typename Value, typename Alloc>
		void extract_items(const Container<std::pair<bool,sequence<Value> >, Alloc> &database, std::set<Value> &itemset) const {
			typename Container<std::pair<bool,sequence<Value> >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				if(database_iter->first) {
					itemset.insert(database_iter->second.begin(), database_iter->second.end());
				}
			}
		}

		template <template <typename, typename> class Container, typename Value, typename Alloc>
		void extract_items(const Container<std::pair<bool,sequence<ordered_set<Value> > >, Alloc> &database, std::set<Value> &itemset) const {
			typename Container<std::pair<bool,sequence<ordered_set<Value> > >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				if(database_iter->first) {
					const sequence<ordered_set<Value> > &seq = database_iter->second;

					typename sequence<ordered_set<Value> >::const_iterator seq_iter = seq.begin();
					for(; seq_iter != seq.end(); ++seq_iter) {
						itemset.insert(seq_iter->begin(), seq_iter->end());
					}
				}
			}
		}

		template <template <typename, typename> class Container, typename Alloc>
		void extract_frequent_items(const Container<std::pair<bool,sequence<Value2> >, Alloc> &database, const std::set<Value2> &items, unsigned int min_support, std::vector<Value2> &frequent_items) const {
			std::map<Value2,unsigned int> counts;

			typename Container<std::pair<bool,sequence<Value1> >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				if(database_iter->first) {
					std::set<Value2> seq_items(database_iter->second.begin(), database_iter->second.end());

					typename std::set<Value2>::iterator seq_items_iter = seq_items.begin();
					for(; seq_items_iter != seq_items.end(); ++seq_items_iter) {
						counts[*seq_items_iter]++;
					}
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
		void extract_frequent_items(const Container<std::pair<bool,sequence<ordered_set<Value2> > >, Alloc> &database, const std::set<Value2> &items, unsigned int min_support, std::vector<Value2> &frequent_items) const {
			std::map<Value2,unsigned int> counts;

			typename Container<std::pair<bool,sequence<ordered_set<Value2> > >, Alloc>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				if(database_iter->first) {
					std::set<Value2> seq_items;

					const sequence<ordered_set<Value2> > &tmp_seq = database_iter->second;

					typename sequence<ordered_set<Value2> >::const_iterator tmp_seq_iter = tmp_seq.begin();
					for(; tmp_seq_iter != tmp_seq.end(); ++tmp_seq_iter) {
						seq_items.insert(tmp_seq_iter->begin(), tmp_seq_iter->end());
					}

					typename std::set<Value2>::iterator seq_items_iter = seq_items.begin();
					for(; seq_items_iter != seq_items.end(); ++seq_items_iter) {
						counts[*seq_items_iter]++;
					}
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

			std::pair<bool,sequence<Value> > operator()(const std::pair<bool,sequence<Value> > &item) const {
				std::pair<bool,sequence<Value> > ret_item;
				ret_item.first = item.first;

				sequence<Value> &new_seq = ret_item.second;
				const sequence<Value> &seq = item.second;
				typename sequence<Value>::const_iterator seq_iter = seq.begin();
				for(; seq_iter != seq.end(); ++seq_iter) {
					if(std::binary_search(items.begin(), items.end(), *seq_iter)) {
						new_seq.push_element(*seq_iter);
					}
				}

				return ret_item;
			}

			std::pair<bool,sequence<ordered_set<Value> > > operator()(const std::pair<bool,sequence<ordered_set<Value> > > &item) const {
				std::pair<bool,sequence<ordered_set<Value> > > ret_item;
				ret_item.first = item.first;

				sequence<ordered_set<Value> > &new_seq = ret_item.second;
				new_seq.push_element(ordered_set<Value>());
				
				const sequence<ordered_set<Value> > &seq = item.second;

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

				return ret_item;
			}
		};

		template <template <typename, typename> class Container, typename Alloc>
		void mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, std::vector<Value2> &frequent_items, unsigned int min_support, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			std::vector<std::pair<bool,sequence<Value1> > > stripped_database;
			std::vector<const std::pair<bool,sequence<Value1> > *> ptr_database(database.size());
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

			std::sort(ptr_database.begin(), ptr_database.end(), pointer_compare<const std::pair<bool,sequence<Value1> > >);

			std::cout << "Mining: ";
			std::cout.flush();

			time.tic();
			//mine_patterns(ptr_database, frequent_items, min_support, generalization, return_patterns);
			std::cerr << time.toc() << ",";
			std::cerr.flush();

			std::cout << std::endl;
		}
		
		template <template <typename, typename> class Container, typename Alloc>
		void beam_mine(const Container<std::pair<bool,sequence<Value1> >, Alloc> &database, std::vector<Value2> &frequent_items, unsigned int min_support, unsigned int beam_width, double generalization, bool strip_sequences, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
			std::vector<std::pair<bool,sequence<Value1> > > stripped_database;
			std::vector<const std::pair<bool,sequence<Value1> > *> ptr_database(database.size());
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

			std::sort(ptr_database.begin(), ptr_database.end(), pointer_compare<const std::pair<bool,sequence<Value1> > >);

			std::cout << "Mining: ";
			std::cout.flush();

			time.tic();
			mine_patterns(ptr_database, frequent_items, min_support, beam_width, generalization, return_patterns);
			std::cerr << time.toc() << ",";
			std::cerr.flush();

			std::cout << std::endl;
		}


		virtual void mine_patterns(std::vector<const std::pair<bool,sequence<Value1> > *> &database, const std::vector<Value2> &items, unsigned int min_support, unsigned int beam_width, double generalization, std::vector<std::pair<double,sequence<Value1> > > &return_patterns) const {
#ifndef NDEBUG
			std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
#endif
			beam_search(database, items, min_support, beam_width, generalization, return_patterns);
		}

	private:

		void fitness(const std::vector<const std::pair<bool,sequence<Value1> > *> &database, const sequence<Value1> &seq, unsigned int &TP, unsigned int &FP) const {
			TP = 0;
			FP = 0;
			typename std::vector<const std::pair<bool,sequence<Value1> > *>::const_iterator database_iter = database.begin();
			for(; database_iter != database.end(); ++database_iter) {
				if(seq.subsequence((*database_iter)->second)) {
					if((*database_iter)->first) {
						TP++;
					}
					else {
						FP++;
					}
				}
			}
		}
		
		void beam_search(std::vector<const std::pair<bool,sequence<Value2> > *> &database, const std::vector<Value2> &items, unsigned int min_support, unsigned int beam_width, double generalization, std::vector<std::pair<double,sequence<Value2> > > &results) const {
			unsigned int TP, FP;
			std::pair<double,sequence<Value2> > element;
			std::vector<std::pair<double,sequence<Value2> > > beam;
			std::vector<std::pair<double,sequence<Value2> > > next_beam;
			
			beam.push_back(element);
			while( !beam.empty() ) {
				next_beam.clear();

				typename std::vector<std::pair<double,sequence<Value2> > >::iterator beam_iter = beam.begin();
				for(; beam_iter != beam.end(); ++beam_iter) {
					
					typename sequence<Value2>::size_type len = beam_iter->second.length();	
					for(typename sequence<Value2>::size_type ii=0; ii < len; ii++) {
						typename std::vector<Value2>::const_iterator items_iter = items.begin();
						for(; items_iter != items.end(); ++items_iter) {
							beam_iter->second.insert(*items_iter, ii);

							fitness(database, beam_iter->second, TP, FP);
							if(TP >= min_support) {
								beam_iter->first = TP/(FP+generalization);
								next_beam.push_back(*beam_iter);
							}   

							beam_iter->second.remove(ii);
						}
					}
					
					typename std::vector<Value2>::const_iterator items_iter = items.begin();
					for(; items_iter != items.end(); ++items_iter) {
						beam_iter->second.push_element(*items_iter);

						fitness(database, beam_iter->second, TP, FP);
						if(TP >= min_support) {
							beam_iter->first = TP/(FP+generalization);
							next_beam.push_back(*beam_iter);
						}   

						beam_iter->second.pop_element();
					}
				}

				std::make_heap(next_beam.begin(), next_beam.end());
				
				beam.clear();
				while(beam.size() < beam_width && next_beam.size() > 0) {
					element = next_beam.front();
					std::pop_heap(next_beam.begin(), next_beam.end()); 
					next_beam.pop_back();
					
					beam.push_back(element);
					if(results.size() < beam_width) {
						results.push_back(element);
						std::push_heap(results.begin(), results.end(), std::greater<std::pair<double,sequence<Value2> > >());
					}
					else if(element.first > results.front().first) {
						std::pop_heap(results.begin(), results.end(), std::greater<std::pair<double,sequence<Value2> > >());
						results.back() = element;
						std::push_heap(results.begin(), results.end(), std::greater<std::pair<double,sequence<Value2> > >());
					}
				}

			}
		}
		
		void beam_search(std::vector<const std::pair<bool,sequence<ordered_set<Value2 > > > *> &database, const std::vector<Value2> &items, unsigned int min_support, unsigned int beam_width, double generalization, std::vector<std::pair<double,sequence<ordered_set<Value2> > > > &results) const {
				throw std::runtime_error("not yet implemented");
		}
		/*void mine_patterns(typename std::vector<const std::pair<bool,sequence<Value2> *> >::iterator db_begin, typename std::vector<const std::pair<bool,sequence<Value2> *> >::iterator db_end, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<Value2> > > &return_patterns) const {
		
		}


		void mine_patterns(typename std::vector<const std::pair<bool,sequence<ordered_set<Value2> > > *>::iterator db_begin, typename std::vector<const std::pair<bool,sequence<ordered_set<Value2> > > *>::iterator db_end, const std::vector<Value2> &items, unsigned int min_support, std::map<unsigned int,std::list<sequence<ordered_set<Value2> > > > &return_patterns) const {
		
		}*/

};

#endif

