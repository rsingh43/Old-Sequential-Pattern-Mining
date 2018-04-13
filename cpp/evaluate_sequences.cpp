#include "ordered_set.hh"
#include "sequence.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <set>
#include <string>
#include <vector>

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

void usage(const char *filename) {
	cout << "Sequence Evaluation"	<< endl << endl;
	
	cout << "Usage:  " << filename << " [-i/--itemset] [-n/--numeric n] seq_file db_file" << endl;
	cout << "  --itemset      treate items as sets " << endl;
	cout << "  --numeric      sequence items are treated as nonnegative integers" << endl;
	cout << "  infile         input sequence database" << endl;
}

bool parse_args(int argc, char *argv[], bool &itemset, bool &numeric, string &seq_file, string &db_file) {
	itemset = false;
	numeric = false;
	
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
void process_file(const string &seq_filename, const string &db_filename) {
	vector<sequence<T> > sequences;
	vector<pair<bool,sequence<T> > > database;

	load_labeled_database(db_filename, database);
	load_database(seq_filename, sequences);

	typename vector<sequence<T> >::iterator sequences_iter = sequences.begin();
	for(; sequences_iter != sequences.end(); ++sequences_iter) {
		unsigned int TP = 0;
		unsigned int TN = 0;
		unsigned int FP = 0;
		unsigned int FN = 0;
		
		typename vector<pair<bool,sequence<T> > >::iterator database_iter = database.begin();
		for(; database_iter != database.end(); ++database_iter) {
			if(sequences_iter->subsequence(database_iter->second)) {
				if(database_iter->first) {
					TP++;
				}
				else {
					FP++;
				}
			}
			else {
				if(database_iter->first) {
					FN++;
				}
				else {
					TN++;
				}
			}
		}

		std::cout << *sequences_iter << " " << TP << " " << FP << " " << TN << " " << FN << std::endl;
	}
}

int main(int argc, char *argv[]) {
	bool itemset;
	bool numeric;
	string seq_filename;
	string db_filename;

	try {
		if(parse_args(argc, argv, itemset, numeric, seq_filename, db_filename)) {
			if(itemset) {
				if(numeric) {
					process_file<ordered_set<int> >(seq_filename, db_filename);
				}
				else {
					process_file<ordered_set<string> >(seq_filename, db_filename);
				}
			}
			else {
				if(numeric) {
					process_file<int>(seq_filename, db_filename);
				}
				else {
					process_file<string>(seq_filename, db_filename);
				}
			}
		}
	}
	catch(exception &e) {
		cerr << e.what() << endl;
	}

	return 0;
}
