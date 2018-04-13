#ifndef _MAIN_HH_
#define _MAIN_HH_

#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

#include <list>
#include <map>
#include <string>

#include <exception>
#include <stdexcept>

#include <limits>

#include <cstring>

#include "ordered_set.hh"
#include "sequence.hh"

#include "timer.hh"

#ifndef NDEBUG
#include "leaks.hh"
#endif

void usage(const char *file_name) {
	std::cout << "Sequential Pattern Mining" << std::endl << std::endl;
	
	std::cout << "Usage:  " << file_name << " [-n/--numeric] [-i/--itemset] [-m/--min-support n] [-s/--strip-sequences] [--suffixfile file] [--rina]  [-o/--outfile file] infile" << std::endl;
	std::cout << "  --itemset           treat items as sets " << std::endl;
	std::cout << "  --numeric           sequence items are treated as nonnegative integers" << std::endl;
	std::cout << "  --min-support       minimum support, number between 0.0 and 1.0, default 0.25" << std::endl;
	std::cout << "  --strip-sequences   strip non-frequent items from database, default false" << std::endl;
	std::cout << "  --suffixfile        file of suffixes for prefix-closed mining" << std::endl;
	std::cout << "  --rina              " << std::endl;
	std::cout << "  --outfile           write results to the specified file" << std::endl;
	std::cout << "  infile              input sequence database" << std::endl;
}

bool parse_args(int argc, char *argv[], bool &numeric, bool &itemset, double &min_support, bool &strip_sequences, std::string &suffixfile, bool &rina, std::string &outfile, std::string &infile) {
	numeric = false;
	itemset = false;
	min_support = 0.25;
	strip_sequences = false;
	suffixfile.clear();
	rina = false;
	outfile.clear();
	infile.clear();
	
	if(argc == 1) {
		usage(argv[0]);
		return false;
	}
	else {
		int ii=1;
		
		while(ii < argc) {
			if(strcmp(argv[ii], "-h") == 0 || strcmp(argv[ii], "--help") == 0) {
				usage(argv[0]);
				
				return false;
			}
			else if(strcmp(argv[ii], "-n") == 0 || strcmp(argv[ii], "--numeric") == 0) {
				numeric = true;
			}
			else if(strcmp(argv[ii], "-i") == 0 || strcmp(argv[ii], "--itemset") == 0) {
				itemset = true;
			}
			else if(strcmp(argv[ii], "-m") == 0 || strcmp(argv[ii], "--min-support") == 0) {
				ii++;
				if(ii >= argc) {
					std::cerr << "Option " << argv[ii-1] << " requires an argument." << std::endl << std::endl;
					usage(argv[0]);
					
					return false;
				}
				
				std::istringstream iss(argv[ii]);
				iss >> min_support;
				
				if(iss.bad() || iss.fail() || !iss.eof()) {
					if(min_support == std::numeric_limits<double>::max()) {
						std::cerr << "Minimum support specified exceeds maximum allowed value." << std::endl << std::endl;
						usage(argv[0]);
					
						return false;
					}
					else if(min_support == std::numeric_limits<double>::min()) {
						std::cerr << "Minimum support specified less than minimum allowed value." << std::endl << std::endl;
						usage(argv[0]);
					
						return false;
					}
					else {
						std::cerr << "Option " << argv[ii-1] << " requires an integer value." << std::endl << std::endl;
						usage(argv[0]);
						
						return false;
					}
				}
				else if(min_support <= 0.0 || min_support > 1.0) {
					std::cerr << "Minimum support must be a value between 0.0 and 1.0." << std::endl << std::endl;
					usage(argv[0]);
				
					return false;
				}
			}
			else if(strcmp(argv[ii], "-s") == 0 || strcmp(argv[ii], "--strip-sequences") == 0) {
				strip_sequences = true;
			}
			else if(strcmp(argv[ii], "--suffixfile") == 0) {
				ii++;
				if(ii >= argc) {
					std::cerr << "Option " << argv[ii-1] << " requires an argument." << std::endl << std::endl;
					usage(argv[0]);
					
					return false;
				}
				
				suffixfile.assign(argv[ii]);
			}
			else if(strcmp(argv[ii], "--rina") == 0) {
				rina = true;
			}
			else if(strcmp(argv[ii], "-o") == 0 || strcmp(argv[ii], "--outfile") == 0) {
				ii++;
				if(ii >= argc) {
					std::cerr << "Option " << argv[ii-1] << " requires an argument." << std::endl << std::endl;
					usage(argv[0]);
					
					return false;
				}
				
				outfile.assign(argv[ii]);
			}
			else if(infile.empty()) {
				infile.assign(argv[ii]);
			}
			else {
				std::cerr << "Unrecognized input option " << argv[ii] << std::endl << std::endl;
				usage(argv[0]);
				
				return false;
			}
			
			ii++;
		}
		
		if(infile.empty()) {
			std::cerr << "Input file not specified." << std::endl << std::endl;
			usage(argv[0]);
			
			return false;
		}
		
		return true;
	}
}

template <typename Value>
void read_database(const std::string &file_name, std::list<sequence<Value> > &database) {
	timer<long double> time;
	
	std::ifstream file(file_name.c_str());
	if(file) {
		std::cout << "Read Time: ";
		std::cout.flush();
		
		database.clear();
		
		time.tic();
		file >> database;
		std::cerr << time.toc() << ",";
		std::cerr.flush();
		
		std::cout << std::endl;
		
		if(file.bad()) {
			std::ostringstream oss;
			oss <<  file_name << ": " << strerror(errno);
			
			throw std::runtime_error(oss.str());
		}
		else if(file.fail() && !file.eof()) {
			std::ostringstream oss;
			oss <<  file_name << ": error reading file";
			
			throw std::runtime_error(oss.str());
		}
	}
	else {
		std::ostringstream oss;
		oss <<  file_name << ": " << strerror(errno);
		
		throw std::runtime_error(oss.str());
	}
}

template <typename T>
std::ostream & operator<<(std::ostream &output, const std::map<unsigned int,std::list<sequence<T> > > &patterns) {
	typename std::map<unsigned int,std::list<sequence<T> > >::const_iterator patterns_iter = patterns.begin();
	for(; patterns_iter != patterns.end(); ++patterns_iter) {
		const std::list<sequence<T> > &patterns = patterns_iter->second;
		
		output << patterns_iter->first << std::endl;
		
		std::vector<sequence<T> > tmp_patterns(patterns.begin(), patterns.end());
		std::sort(tmp_patterns.begin(), tmp_patterns.end());
		typename std::vector<sequence<T> >::iterator tmp_patterns_iter = tmp_patterns.begin();
		for(; tmp_patterns_iter != tmp_patterns.end(); ++tmp_patterns_iter) {
			output << *tmp_patterns_iter << std::endl;
		}
	}
	
	return output;
}

/*template <typename T>
std::ostream & operator<<(std::ostream &output, const std::map<unsigned int,std::set<sequence<T> > > &patterns) {
	typename std::map<unsigned int,std::set<sequence<T> > >::const_iterator patterns_iter = patterns.begin();
	for(; patterns_iter != patterns.end(); ++patterns_iter) {
		const std::set<sequence<T> > &patterns = patterns_iter->second;
		
		output << patterns_iter->first << std::endl;
		
		std::vector<sequence<T> > tmp_patterns(patterns.begin(), patterns.end());
		typename std::vector<sequence<T> >::iterator tmp_patterns_iter = tmp_patterns.begin();
		for(; tmp_patterns_iter != tmp_patterns.end(); ++tmp_patterns_iter) {
			output << *tmp_patterns_iter << std::endl;
		}
	}
	
	return output;
}*/

template <template <typename, typename> class Miner, typename Value1, typename Value2>
void mine(std::list<sequence<Value1> > &database, const std::string &outfile, double min_support, bool strip_sequences, const Miner<Value1,Value2> &miner) {
	std::map<unsigned int,std::list<sequence<Value1> > > patterns;
	
	miner.mine(database, min_support, strip_sequences, patterns);
	
	if(!outfile.empty()) {
		std::ofstream output(outfile.c_str());
		if(output) {
			output << patterns << std::endl;
			if(output.bad()) {
				std::ostringstream oss;
				oss <<  outfile << ": " << strerror(errno);
				
				throw std::runtime_error(oss.str());
			}
		}
		else {
			std::ostringstream oss;
			oss <<  outfile << ": " << strerror(errno);
			
			throw std::runtime_error(oss.str());
		}
	}
	
	#ifndef NDEBUG
		std::string output_graph_name(database_file);
		output_graph_name += ".gv";
		
		std::ofstream output_graph_file(output_graph_name.c_str());
		if(output_graph_file) {
			std::vector<std::vector<std::string> > &graph = miner.graph;
			
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
	#endif
}

template <template <typename, typename> class Miner, typename Value1, typename Value2>
void mine_prefixes(std::list<sequence<Value1> > &database, std::list<sequence<Value1> > &suffixes, const std::string &outfile, double min_support, bool strip_sequences, const Miner<Value1,Value2> &miner) {
	std::map<unsigned int,std::list<sequence<Value1> > > patterns;
	
	miner.mine(database, min_support, strip_sequences, patterns);
	
	std::map<unsigned int,std::list<sequence<Value1> > > prefix_patterns;
	
	sequence<Value1> tmp_seq;
	typename std::list<sequence<Value1> >::iterator suffixes_iter = suffixes.begin();
	for(; suffixes_iter != suffixes.end(); ++suffixes_iter) {
	
		typename std::map<unsigned int,std::list<sequence<Value1> > >::reverse_iterator patterns_iter = patterns.rbegin();
		for(; patterns_iter != patterns.rend(); ++patterns_iter) {
			std::list<sequence<Value1> > &sequences = patterns_iter->second;
			prefix_patterns[patterns_iter->first];
			
			typename std::list<sequence<Value1> >::iterator sequences_iter = sequences.begin();
			for(; sequences_iter != sequences.end(); ++sequences_iter) {
				
				sequences_iter->suffix_project(*suffixes_iter, tmp_seq);
				if(!tmp_seq.empty()) {
					tmp_seq.extend(*suffixes_iter);
					
					typename std::map<unsigned int,std::list<sequence<Value1> > >::reverse_iterator prefix_patterns_iter = prefix_patterns.rbegin();
					for(; prefix_patterns_iter != prefix_patterns.rend(); ++prefix_patterns_iter) {
						if(prefix_patterns_iter->first == patterns_iter->first) {
							maintain_and_test(tmp_seq, patterns_iter->first, prefix_patterns);
						}
						else {
							std::list<sequence<Value1> > &tmp_sequences = prefix_patterns_iter->second;
							
							typename std::list<sequence<Value1> >::iterator tmp_sequences_iter = tmp_sequences.begin();
							for(; tmp_sequences_iter != tmp_sequences.end(); ++tmp_sequences_iter) {
								if(tmp_sequences_iter->length() < tmp_seq.length()) {
									tmp_sequences_iter = tmp_sequences.end();
									break;
								}
								else if(tmp_seq.subsequence(*tmp_sequences_iter)) {
									break;
								}
							}
							
							if(tmp_sequences_iter != tmp_sequences.end()) {
								break;
							}
						}
					}
				}
			}
			
			typename std::map<unsigned int,std::list<sequence<Value1> > >::iterator prefix_patterns_iter = prefix_patterns.find(patterns_iter->first);
			if(prefix_patterns_iter->second.empty()) {
				prefix_patterns.erase(prefix_patterns_iter);
			}
		}
	}
	
	if(!outfile.empty()) {
		std::ofstream output(outfile.c_str());
		if(output) {
			output << prefix_patterns << std::endl;
			if(output.bad()) {
				std::ostringstream oss;
				oss <<  outfile << ": " << strerror(errno);
				
				throw std::runtime_error(oss.str());
			}
		}
		else {
			std::ostringstream oss;
			oss <<  outfile << ": " << strerror(errno);
			
			throw std::runtime_error(oss.str());
		}
	}
	
	/*result.clear();
	
	if(suffix.rbegin()->subset(*pattern.rbegin())) {
		pattern.rproject(suffix, result);
		if(!result.empty()) {
			result.extend(suffix);
		}
	}*/
}

template <template <typename, typename> class Miner, typename Value1, typename Value2>
void rina_mine_prefixes(std::list<sequence<Value1> > &database, std::list<sequence<Value1> > &suffixes, const std::string &outfile, double min_support, bool strip_sequences, const Miner<Value1,Value2> &miner) {
	std::list<sequence<Value1> > suffix_projected_database(database.size());
	
	std::map<unsigned int,std::list<sequence<Value1> > > prefix_patterns;
	
	typename std::list<sequence<Value1> >::iterator suffixes_iter = suffixes.begin();
	for(; suffixes_iter != suffixes.end(); ++suffixes_iter) {
		
		typename std::list<sequence<Value1> >::iterator suffix_projected_database_iter = suffix_projected_database.begin();
		typename std::list<sequence<Value1> >::iterator database_iter = database.begin();
		while(database_iter != database.end()) {
			database_iter->suffix_project(*suffixes_iter, *suffix_projected_database_iter);
			
			++database_iter;
			++suffix_projected_database_iter;
		}
		
		std::map<unsigned int,std::list<sequence<Value1> > > patterns;
		miner.mine(suffix_projected_database, min_support, strip_sequences, patterns);
		
		typename std::map<unsigned int,std::list<sequence<Value1> > >::iterator patterns_iter = patterns.begin();
		for(; patterns_iter != patterns.end(); ++patterns_iter) {
			std::list<sequence<Value1> > &sequences = patterns_iter->second;
			
			typename std::list<sequence<Value1> >::iterator sequences_iter = sequences.begin();
			for(; sequences_iter != sequences.end(); ++sequences_iter) {
				std::list<sequence<Value1> > &tmp_sequences = prefix_patterns[patterns_iter->first];
				
				tmp_sequences.push_back(*sequences_iter);
				tmp_sequences.back().extend(*suffixes_iter);
			}
		}
	}
	
	if(!outfile.empty()) {
		std::ofstream output(outfile.c_str());
		if(output) {
			output << prefix_patterns << std::endl;
			if(output.bad()) {
				std::ostringstream oss;
				oss <<  outfile << ": " << strerror(errno);
				
				throw std::runtime_error(oss.str());
			}
		}
		else {
			std::ostringstream oss;
			oss <<  outfile << ": " << strerror(errno);
			
			throw std::runtime_error(oss.str());
		}
	}
}

template <template <typename, typename> class Miner, typename Value1, typename Value2>
void mine(const std::string &database_file, const std::string &suffix_file, bool rina, const std::string &outfile, double min_support, bool strip_sequences, const Miner<Value1,Value2> &miner) {
	std::cout << "Min Support: ";
	std::cout.flush();
	std::cerr << min_support << ",";
	std::cerr.flush();
	std::cout << std::endl;
	
	std::list<sequence<Value1> > database;
	read_database(database_file, database);
	
	if(suffix_file.empty()) {
		mine(database, outfile, min_support, strip_sequences, miner);	
	}
	else {
		std::list<sequence<Value1> > suffixes;
		read_database(suffix_file, suffixes);
		
		if(suffixes.empty()) {
			std::ostringstream oss;
			oss <<  suffix_file << ": no suffixes found";
			
			throw std::runtime_error(oss.str());
		}
		
		if(rina) {
			rina_mine_prefixes(database, suffixes, outfile, min_support, strip_sequences, miner);
		}
		else {
			mine_prefixes(database, suffixes, outfile, min_support, strip_sequences, miner);
		}
	}
	
}

template <template <typename, typename> class Miner, typename Value1, typename Value2>
void run(double min_support, bool strip_sequences, const std::string &infile, const std::string &suffixfile, bool rina, const std::string &outfile, const Miner<Value1,Value2> &miner) {
	timer<long double> time;			
			
	mine(infile, suffixfile, rina, outfile, min_support, strip_sequences, miner);
	//prefix_closed_mining_rina(infile, std::string("suffixes.dat"), min_support, strip_sequences, miner);
	
	std::cout << "Total: ";
	std::cout.flush();
	
	std::cerr << time.toc() << std::endl;
}

template <template <typename, typename> class Miner>
int run(int argc, char *argv[]) {
	bool numeric, itemset;
	double min_support;
	bool strip_sequences;
	bool rina;
	std::string output_name, suffix_name, database_name;
	if(parse_args(argc, argv, numeric, itemset, min_support, strip_sequences, suffix_name, rina, output_name, database_name)) {
		try {
			if(itemset) {
				if(numeric) {
					run(min_support, strip_sequences, database_name, suffix_name, rina, output_name, Miner<ordered_set<int>,int>());
				}
				else {
					run(min_support, strip_sequences, database_name, suffix_name, rina, output_name, Miner<ordered_set<std::string>,std::string>());
				}
			}
			else {
				if(numeric) {
					run(min_support, strip_sequences, database_name, suffix_name, rina, output_name, Miner<int,int>());
				}
				else {
					run(min_support, strip_sequences, database_name, suffix_name, rina, output_name, Miner<std::string,std::string>());
				}
			}
		}
		catch(std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
	
	#ifndef NDEBUG
	call_leaks(&(argv[0][2]));
	#endif
	
	return 0;
}

#endif
