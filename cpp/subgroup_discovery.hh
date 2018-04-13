#ifndef _MAIN_HH_
#define _MAIN_HH_

#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

#include <list>
#include <map>
#include <queue>
#include <utility>
#include <string>

#include <algorithm>
#include <functional>

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
	std::cout << "Subgroup Discovery" << std::endl << std::endl;
	
	std::cout << "Usage:  " << file_name << " [-n/--numeric] [-i/--itemset] [-s/--strip-sequences]" << std::endl;
	std::cout << "          [-m/--min-support n] [-b/--beam-width n ] [-g/--generalization n]" << std::endl;
	std::cout << "          [-o/--outfile file] infile" << std::endl;
	std::cout << "  --itemset           treat items as sets " << std::endl;
	std::cout << "  --numeric           sequence items are treated as nonnegative integers" << std::endl;
	std::cout << "  --strip-sequences   strip non-frequent items from database, default false" << std::endl;
	std::cout << "  --min-support       minimum support, number between 0.0 and 1.0, default 0.25" << std::endl;
	std::cout << "  --beam-width        number of solutions keept on each iteration, default 20" << std::endl;
	std::cout << "  --generalization    generalization parameter, default 1.0" << std::endl;
	std::cout << "  --outfile           write results to the specified file" << std::endl;
	std::cout << "  infile              input sequence database" << std::endl;
}

bool parse_args(int argc, char *argv[], bool &numeric, bool &itemset, bool &strip_sequences, double &min_support, unsigned int &beam_width, double &generalization, std::string &outfile, std::string &infile) {
	numeric = false;
	itemset = false;
	strip_sequences = false;
	min_support = 0.25;
	beam_width = 20;
	generalization = 1.0;
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
			else if(strcmp(argv[ii], "-s") == 0 || strcmp(argv[ii], "--strip-sequences") == 0) {
				strip_sequences = true;
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
						std::cerr << "Option " << argv[ii-1] << " requires a value between 0.0 and 1.0." << std::endl << std::endl;
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
			else if(strcmp(argv[ii], "-b") == 0 || strcmp(argv[ii], "--beam-width") == 0) {
				ii++;
				if(ii >= argc) {
					std::cerr << "Option " << argv[ii-1] << " requires an argument." << std::endl << std::endl;
					usage(argv[0]);
					
					return false;
				}
				
				std::istringstream iss(argv[ii]);
				iss >> beam_width;
				
				if(iss.bad() || iss.fail() || !iss.eof()) {
					if(beam_width == std::numeric_limits<unsigned int>::max()) {
						std::cerr << "Beam width specified exceeds maximum allowed value." << std::endl << std::endl;
						usage(argv[0]);
					
						return false;
					}
					else if(beam_width == std::numeric_limits<unsigned int>::min()) {
						std::cerr << "Beam width specified less than minimum allowed value." << std::endl << std::endl;
						usage(argv[0]);
					
						return false;
					}
					else {
						std::cerr << "Option " << argv[ii-1] << " requires an integer value." << std::endl << std::endl;
						usage(argv[0]);
						
						return false;
					}
				}
				else if(beam_width == 0) {
					std::cerr << "Beam width must be a positive integer." << std::endl << std::endl;
					usage(argv[0]);
				
					return false;
				}
			}
			else if(strcmp(argv[ii], "-g") == 0 || strcmp(argv[ii], "--generalization") == 0) {
				ii++;
				if(ii >= argc) {
					std::cerr << "Option " << argv[ii-1] << " requires an argument." << std::endl << std::endl;
					usage(argv[0]);
					
					return false;
				}
				
				std::istringstream iss(argv[ii]);
				iss >> generalization;
				
				if(iss.bad() || iss.fail() || !iss.eof()) {
					if(generalization == std::numeric_limits<double>::max()) {
						std::cerr << "Generalization parameter specified exceeds maximum allowed value." << std::endl << std::endl;
						usage(argv[0]);
					
						return false;
					}
					else if(generalization == std::numeric_limits<double>::min()) {
						std::cerr << "Generalization parameter specified less than minimum allowed value." << std::endl << std::endl;
						usage(argv[0]);
					
						return false;
					}
					else {
						std::cerr << "Option " << argv[ii-1] << " requires an integer value." << std::endl << std::endl;
						usage(argv[0]);
						
						return false;
					}
				}
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
void read_database(const std::string &file_name, std::list<std::pair<bool,sequence<Value> > > &database) {
	timer<long double> time;
	
	std::ifstream file(file_name.c_str());
	if(file) {
		std::cout << "Read Time: ";
		std::cout.flush();
		
		database.clear();
		
		time.tic();
		
		std::pair<bool,sequence<Value> > item;
		while(file >> item.second && file >> item.first) {
			database.push_back(item);
		}

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
std::ostream & operator<<(std::ostream &output, const std::vector<std::pair<double,sequence<T> > > &patterns) {
	typename std::vector<std::pair<double,sequence<T> > >::const_iterator patterns_iter = patterns.begin();
	for(; patterns_iter != patterns.end(); ++patterns_iter) {
		output << patterns_iter->first << " " << patterns_iter->second << std::endl;
	}
	
	return output;
}

template <template <typename, typename> class Miner, typename Value1, typename Value2>
void mine(std::list<std::pair<bool,sequence<Value1> > > &database, const std::string &outfile, double min_support, unsigned int beam_width, double generalization, bool strip_sequences, const Miner<Value1,Value2> &miner) {
	std::vector<std::pair<double,sequence<Value1> > > patterns;
	
	miner.beam_mine(database, min_support, beam_width, generalization, strip_sequences, patterns);
	
	if(!outfile.empty()) {
		std::ofstream output(outfile.c_str());
		if(output) {
			std::sort_heap(patterns.begin(), patterns.end(), std::greater<std::pair<double,sequence<Value2> > >());
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
}

template <template <typename, typename> class Miner, typename Value1, typename Value2>
void mine(const std::string &database_file, const std::string &outfile, double min_support, unsigned int beam_width, double generalization, bool strip_sequences, const Miner<Value1,Value2> &miner) {
	std::cout << "Min Support: ";
	std::cout.flush();
	std::cerr << min_support << ",";
	std::cerr.flush();
	std::cout << std::endl;
	
	std::list<std::pair<bool,sequence<Value1> > > database;
	read_database(database_file, database);
	
	mine(database, outfile, min_support, beam_width, generalization, strip_sequences, miner);	
}

template <template <typename, typename> class Miner, typename Value1, typename Value2>
void run(double min_support, unsigned int beam_width, double generalization, bool strip_sequences, const std::string &infile, const std::string &outfile, const Miner<Value1,Value2> &miner) {
	timer<long double> time;			
			
	mine(infile, outfile, min_support, beam_width, generalization, strip_sequences, miner);
	
	std::cout << "Total: ";
	std::cout.flush();
	
	std::cerr << time.toc() << std::endl;
}

template <template <typename, typename> class Miner>
int run(int argc, char *argv[]) {
	bool numeric, itemset;
	bool strip_sequences;
	double min_support, generalization;
	unsigned int beam_width;
	std::string output_name, database_name;
	
	if(parse_args(argc, argv, numeric, itemset, strip_sequences, min_support, beam_width, generalization, output_name, database_name)) {
		try {
			if(itemset) {
				if(numeric) {
					run(min_support, beam_width, generalization, strip_sequences, database_name, output_name, Miner<ordered_set<int>,int>());
				}
				else {
					run(min_support, beam_width, generalization, strip_sequences, database_name, output_name, Miner<ordered_set<std::string>,std::string>());
				}
			}
			else {
				if(numeric) {
					run(min_support, beam_width, generalization, strip_sequences, database_name, output_name, Miner<int,int>());
				}
				else {
					run(min_support, beam_width, generalization, strip_sequences, database_name, output_name, Miner<std::string,std::string>());
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
