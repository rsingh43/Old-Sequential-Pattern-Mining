#include "ordered_set.hh"
#include "sequence.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <set>
#include <string>
#include <vector>

#include <algorithm>
#include <iterator>

#include <limits>

#include <stdexcept>

#include <cerrno>
#include <cstring>

#include "gzstream.h"

#include "counter.hh"

#ifndef NDEBUG
#include "leaks.hh"
#endif

using namespace std;

enum diversity_measure {Im, Wm, AWm, IWm, IAWm};

void usage(const char *filename) {
	cout << "Diversity"	<< endl << endl;
	
	cout << "Usage:  " << filename << " [-i/--itemset] [-n/--numeric n] [-a/--alpha float] [-m/--measure str] seq_file db_file" << endl;
	cout << "  --itemset   treate items as sets " << endl;
	cout << "  --numeric   sequence items are treated as nonnegative integers" << endl;
	cout << "  --alpha     alpha value (0.1, 1.0]" << endl;
	cout << "  --measure   one of Im, Wm, AWm, IWm, or IAWm" << endl;
	cout << "              Im - interestingness" << endl;
	cout << "              Wm - weighted coverage" << endl;
	cout << "              AWm - average weighted coverage" << endl;
	cout << "              IWm - interestingness times weighted coverage" << endl;
	cout << "              IAWm - interestingness times average weighted coverage" << endl;
	cout << "  seq_file    input sequence list" << endl;
	cout << "  db_file     input labeled database" << endl;
}

bool parse_args(int argc, char *argv[], bool &itemset, bool &numeric, string &seq_file, string &db_file, double &alpha, diversity_measure &measure) {
	itemset = false;
	numeric = false;
	alpha = 1.0;
	measure = Im;
	
	seq_file.clear();
	db_file.clear();
	
	if(argc == 1) {
		usage(argv[0]);
		return false;
	}
	else {
		int ii=1;
		
		while(ii < argc) {
			if(strcmp(argv[ii], "-i") == 0 || strcmp(argv[ii], "--itemset") == 0) {
				itemset = true;
			}
			else if(strcmp(argv[ii], "-n") == 0 || strcmp(argv[ii], "--numeric") == 0) {
				numeric = true;
			}
			else if(strcmp(argv[ii], "-h") == 0 || strcmp(argv[ii], "--help") == 0) {
				usage(argv[0]);
				
				return false;
			}
			else if(seq_file.empty()) {
				seq_file.assign(argv[ii]);
			}
			else if(db_file.empty()) {
				db_file.assign(argv[ii]);
			}
			else if(strcmp(argv[ii], "-a") == 0 || strcmp(argv[ii], "--alpha") == 0) {
				ii++;
				if(ii >= argc) {
					cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
					usage(argv[0]);

					return false;
				}   

				istringstream iss(argv[ii]);
				iss >> alpha;

				if(iss.bad() || iss.fail() || !iss.eof()) {
					if(alpha == std::numeric_limits<unsigned int>::max()) {
						cerr << "alpha value exceeds maximum allowed value." << endl << endl;
						usage(argv[0]);

						return false;
					} 
					else {
						cerr << "alpha value less than maximum allowed value." << endl << endl;
						usage(argv[0]);

						return false;
					}   
				}
			}
			else if(strcmp(argv[ii], "-m") == 0 || strcmp(argv[ii], "--measure") == 0) {
				ii++;
				if(ii >= argc) {
					cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
					usage(argv[0]);

					return false;
				}

				std::string measure_string(argv[ii]);
				//Im, Wm, AWm, IWm, IAWm
				if(measure_string == "Im") {
					measure = Im;
				}
				else if(measure_string == "Wm") {
					measure = Wm;
				}
				else if(measure_string == "AWm") {
					measure = AWm;
				}
				else if(measure_string == "IWm") {
					measure = IWm;
				}
				else if(measure_string == "IAWm") {
					measure = IAWm;
				}
				else {
						cerr << "Unrecognized measure value:" << measure_string << endl << endl;
						usage(argv[0]);

						return false;
				}
			}
			else {
				std::cerr << "Unrecognized input option " << argv[ii] << std::endl << std::endl;
				usage(argv[0]);

				return false;
			} 

			ii++;
		}

		if(seq_file.empty() || db_file.empty()) {
			cerr << "No sequential databse file provided." << endl << endl;
			usage(argv[0]);

			return false;
		}

		return true;
	}
}

template <typename T>
void load_database(istream &file, const string &filename, vector<sequence<T> > &database) {
	if(file) {
		file >> database;
		if(file.bad()) {
			ostringstream oss;
			oss <<  filename << ": " << strerror(errno);
			
			throw runtime_error(oss.str());
		}
		else if(file.fail() && !file.eof()) {
			ostringstream oss;
			oss <<  filename << ": error reading file";
			
			throw runtime_error(oss.str());
		}
	}
	else {
		ostringstream oss;
		oss <<  filename << ": " << strerror(errno);
		
		throw runtime_error(oss.str());
	}
}

template <typename T>
void load_labeled_database(istream &file, const std::string &filename, std::vector<std::pair<bool,sequence<T> > > &database) {
	if(file) {
		file >> database;
		if(file.bad()) {
			std::ostringstream oss;
			oss <<  filename << ": " << strerror(errno);

			throw std::runtime_error(oss.str());
		}   
		else if(file.fail() && !file.eof()) {
			std::ostringstream oss;
			oss <<  filename << ": error reading file";

			throw std::runtime_error(oss.str());
		}   
	}   
	else {
		std::ostringstream oss;
		oss <<  filename << ": " << strerror(errno);

		throw std::runtime_error(oss.str());

	}   
}

inline bool ends_with(const std::string &str, const std::string &suffix) {
	if(suffix.size() > str.size()) {
		return false;
	}

	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

template <typename T>
void load_database(const string &filename, vector<sequence<T> > &database) {
	if(ends_with(filename, ".gz")) {
		igzstream file(filename.c_str());
		load_database(file, filename, database);
	}
	else {
		ifstream file(filename.c_str());
		load_database(file, filename, database);
	}
}

template <typename T>
void load_labeled_database(const string &filename, std::vector<std::pair<bool,sequence<T> > > &database) {
	if(ends_with(filename, ".gz")) {
		igzstream file(filename.c_str());
		load_labeled_database(file, filename, database);
	}
	else {
		ifstream file(filename.c_str());
		load_labeled_database(file, filename, database);
	}
}


template <typename T>
bool is_negative( pair<bool,sequence<T> > &seq) {
	return !seq.first;
}
//if((average_weighted_coverage * interestingness[*sequences_iter]) > (best_average_weighted_coverage * interestingness[*best_iter])) {
//if((weighted_coverage * interestingness[*sequences_iter]) > (best_weighted_coverage * interestingness[*best_iter])) {
//if(average_weighted_coverage > best_average_weighted_coverage) {
//if(weighted_coverage > best_weighted_coverage) {
//if(interestingness[*sequences_iter] > interestingness[*best_iter]) {

template <typename T>
void process_file(const string &seq_filename, const string &db_filename, double alpha=1.0, diversity_measure measure=Im) {
	vector<sequence<T> > sequences;
	vector<pair<bool,sequence<T> > > database;

	load_labeled_database(db_filename, database);
	std::cout << database.size() << " sequences" << std::endl;
	
	load_database(seq_filename, sequences);
	std::cout << sequences.size() << " patterns" << std::endl;

	//database.erase( remove_if(database.begin(), database.end(), is_negative<T>), database.end() );
	typename vector<pair<bool,sequence<T> > >::iterator positive_end = remove_if(database.begin(), database.end(), is_negative<T>);
	std::cout << std::distance(database.begin(), positive_end) << " positive sequences" << std::endl;
	
	map<sequence<T> *,unsigned int> weights;
	map<sequence<T>,double> interestingness;
	map<sequence<T>,set<sequence<T> *> > TP_map;
	map<sequence<T>,set<sequence<T> *> > FP_map;

	sequence<T> prefix;
	typename vector<sequence<T> >::iterator sequences_iter = sequences.begin();
	for(; sequences_iter != sequences.end(); ++sequences_iter) {
		prefix = *sequences_iter;
		prefix.pop_element();

		typename vector<pair<bool,sequence<T> > >::iterator database_iter = database.begin();
		
		for(; database_iter != positive_end; ++database_iter) {
			if(prefix.subsequence(database_iter->second)) {
				if(sequences_iter->subsequence(database_iter->second)) {
					weights[&(database_iter->second)] = 0;
					TP_map[*sequences_iter].insert(&(database_iter->second));
					//TP++;
				}
				else {
					FP_map[*sequences_iter].insert(&(database_iter->second));
					//FP++;
				}
			}
		}

		for(; database_iter != database.end(); ++database_iter) {
			if(prefix.subsequence(database_iter->second)) {
				FP_map[*sequences_iter].insert(&(database_iter->second));
				//FP++;
			}
		}

		interestingness[*sequences_iter] = TP_map[*sequences_iter].size()/(FP_map[*sequences_iter].size()+1.0);
		//TP_map[*sequences_iter] = TP;
		//FP_map[*sequences_iter] = FP;
	}

	std::cout << weights.size() << " coverable sequences" << std::endl;
	std::cout << "pattern,TP,FP,interestingness";
	std::cout << ",weighted coverage,interestingnes times weighted coverage";
	std::cout << ",average weighted coverage,interestingnes times average weighted coverage";
	std::cout << ",covered,percentage,uncovered,percentage";
	std::cout << ",newly covered,percentage of remaining,uncovered,uncovered percentage" << std::endl;

	int asdf = 0;
	unsigned int covered = 0;
	while(covered < weights.size()) {
		unsigned int newly_covered = 0;
		double best_weighted_coverage = 0.0;
		double best_average_weighted_coverage = 0.0;
		typename vector<sequence<T> >::iterator best_iter = sequences.begin();
		typename vector<sequence<T> >::iterator sequences_iter = sequences.begin();
		for(; sequences_iter != sequences.end(); ++sequences_iter) {
			unsigned int count = 0;
			double weighted_coverage = 0.0;
			double average_weighted_coverage = 0.0;
			
			typename map<sequence<T> *,unsigned int>::iterator weights_iter = weights.begin();
			for(; weights_iter != weights.end(); ++weights_iter) {
				std::set<sequence<T> *> &TP_sequences = TP_map[*sequences_iter];
			
				//if(sequences_iter->subsequence(*(weights_iter->first))) {
				if(TP_sequences.find(weights_iter->first) != TP_sequences.end()) {
					weighted_coverage += pow(alpha,weights_iter->second);//1.0/weights_iter->second;
					count++;
				}
			}
			
			average_weighted_coverage = weighted_coverage / count;

			bool flag = false;
			switch(measure) {
				//Im, Wm, AWm, IWm, IAWm
				case Im:
					std::cout << "      " << interestingness[*sequences_iter] << " " << interestingness[*best_iter] << std::endl;
					flag = interestingness[*sequences_iter] > interestingness[*best_iter];
					break;
				case Wm:
					flag = (weighted_coverage > best_weighted_coverage) || ((weighted_coverage == best_weighted_coverage) && (interestingness[*sequences_iter] > interestingness[*best_iter]));
					break;
				case AWm:
					flag = (average_weighted_coverage > best_average_weighted_coverage) || ((average_weighted_coverage == best_average_weighted_coverage) && (interestingness[*sequences_iter] > interestingness[*best_iter]));
					break;
				case IWm:
					flag = ((weighted_coverage * interestingness[*sequences_iter]) > (best_weighted_coverage * interestingness[*best_iter])) || (((weighted_coverage * interestingness[*sequences_iter]) == (best_weighted_coverage * interestingness[*best_iter])) && (interestingness[*sequences_iter] > interestingness[*best_iter]));
					break;
				case IAWm:
					flag = ((average_weighted_coverage * interestingness[*sequences_iter]) > (best_average_weighted_coverage * interestingness[*best_iter])) || (((average_weighted_coverage * interestingness[*sequences_iter]) > (best_average_weighted_coverage * interestingness[*best_iter])) && (interestingness[*sequences_iter] > interestingness[*best_iter]));
					break;
			}
			
			if(flag) {
				best_weighted_coverage = weighted_coverage;
				best_average_weighted_coverage = average_weighted_coverage;
				best_iter = sequences_iter;
				std::cout << "   " << *best_iter << " " << interestingness[*best_iter] << std::endl;
			}

			//if((average_weighted_coverage * interestingness[*sequences_iter]) > (best_average_weighted_coverage * interestingness[*best_iter])) {
			//if((weighted_coverage * interestingness[*sequences_iter]) > (best_weighted_coverage * interestingness[*best_iter])) {
			//if(average_weighted_coverage > best_average_weighted_coverage) {
			//if(weighted_coverage > best_weighted_coverage) {
			//if(interestingness[*sequences_iter] > interestingness[*best_iter]) {
		}
		
		typename map<sequence<T> *,unsigned int>::iterator weights_iter = weights.begin();
		for(; weights_iter != weights.end(); ++weights_iter) {
			std::set<sequence<T> *> &TP_sequences = TP_map[*best_iter];
			
			//if(best_iter->subsequence(*(weights_iter->first))) {
			if(TP_sequences.find(weights_iter->first) != TP_sequences.end()) {
				if(weights_iter->second == 0) {
					covered++;
					newly_covered++;
				}
				weights_iter->second++;
			}
		}

		std::cout << "\"" << *best_iter << "\"," << TP_map[*best_iter].size() << "," << FP_map[*best_iter].size() << "," << interestingness[*best_iter];
		std::cout << "," << best_weighted_coverage << "," << (best_weighted_coverage*interestingness[*best_iter]);
		std::cout << "," << best_average_weighted_coverage << "," << (best_average_weighted_coverage*interestingness[*best_iter]);
		std::cout << "," << covered << "," << (covered/(double)weights.size()) << "," << (weights.size()-covered) << "," << ((weights.size()-covered)/(double)weights.size());
		std::cout << "," << newly_covered << "," << (newly_covered/(double)(weights.size() - (covered-newly_covered)));	
		std::cout << "," << (weights.size() - covered) << "," << ((weights.size() - covered)/(double)(weights.size() - (covered-newly_covered))) << std::endl;	
		//std::cout << best_fitness << " " << *best_iter << " " << covered << " / " << weights.size() << " " << (covered/(double)weights.size()) << std::endl;
		
		if(++asdf > 10) {
			exit(-1);
		}
		
		std::swap(*best_iter, sequences.back());
		sequences.pop_back();
	}
}

int main(int argc, char *argv[]) {
	bool itemset;
	bool numeric;
	string seq_filename;
	string db_filename;
	double alpha;
	diversity_measure measure;

	try {
		if(parse_args(argc, argv, itemset, numeric, seq_filename, db_filename, alpha, measure)) {
			if(itemset) {
				if(numeric) {
					process_file<ordered_set<int> >(seq_filename, db_filename, alpha, measure);
				}
				else {
					process_file<ordered_set<string> >(seq_filename, db_filename, alpha, measure);
				}
			}
			else {
				if(numeric) {
					process_file<int>(seq_filename, db_filename, alpha, measure);
				}
				else {
					process_file<string>(seq_filename, db_filename, alpha, measure);
				}
			}
		}
	}
	catch(exception &e) {
		cerr << e.what() << endl;
	}

	return 0;
}
