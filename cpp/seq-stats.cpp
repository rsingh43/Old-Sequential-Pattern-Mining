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

#include "counter.hh"

#ifndef NDEBUG
#include "leaks.hh"
#endif

using namespace std;

void usage(const char *file_name) {
	cout << "Sequence Statistics" << endl << endl;
	
	cout << "Usage:  " << file_name << " [-i/--itemset] [-n/--numeric n] [-o/--occurrences outfile] [-s/--support outfile] infile" << endl;
	cout << "  --itemset       treate items as sets " << endl;
	cout << "  --numeric       sequence items are treated as nonnegative integers" << endl;
	cout << "  --occurrences   file to write occurrences data, occurrences.csv by default" << endl;
	cout << "  --support       file to write support data, support.csv by default" << endl;
	cout << "  infile          input sequential database" << endl;
}

bool parse_args(int argc, char *argv[], bool &itemset, bool &numeric, string &occurrences, string &support, string &infile) {
	itemset = false;
	numeric = false;
	infile.clear();
	occurrences.assign("occurrences.csv");
	support.assign("support.csv");
	
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
			else if(strcmp(argv[ii], "-o") == 0 || strcmp(argv[ii], "--occurrences") == 0) {
				ii++;
				if(ii >= argc) {
					cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
					usage(argv[0]);
					
					return false;
				}
			
				occurrences.assign(argv[ii]);
			}
			else if(strcmp(argv[ii], "-s") == 0 || strcmp(argv[ii], "--support") == 0) {
				ii++;
				if(ii >= argc) {
					cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
					usage(argv[0]);
					
					return false;
				}
			
				support.assign(argv[ii]);
			}
			else if(infile.empty()) {
				infile.assign(string(argv[ii]));
			}
			else {
				cerr << "Unrecognized input option " << argv[ii] << endl << endl;
				usage(argv[0]);
				
				return false;
			}
			
			ii++;
		}
		
		if(infile.empty()) {
			cerr << "No sequential databse file provided." << endl << endl;
			usage(argv[0]);
			
			return false;
		}
		
		return true;
	}
}

template <typename T>
void load_database(const string &file_name, vector<sequence<T> > &database) {
	ifstream file(file_name.c_str());
	if(file) {
		file >> database;
		if(file.bad()) {
			ostringstream oss;
			oss <<  file_name << ": " << strerror(errno);
			
			throw runtime_error(oss.str());
		}
		else if(file.fail() && !file.eof()) {
			ostringstream oss;
			oss <<  file_name << ": error reading file";
			
			throw runtime_error(oss.str());
		}
	}
	else {
		ostringstream oss;
		oss <<  file_name << ": " << strerror(errno);
		
		throw runtime_error(oss.str());
	}
}

template <typename T>
void display_stats(vector<sequence<T> > &database, ofstream &occurrences_file, ofstream &support_file) {
	sort(database.begin(), database.end());
	unsigned int unique = database.size();
	for(unsigned int ii=1; ii < database.size(); ii++) {
		if(database[ii] == database[ii-1]) {
			unique--;
		}
	}
	
	unsigned int max_seq_len = 0;
	unsigned min_seq_len = numeric_limits<unsigned int>::max();
	typename sequence<T>::size_type item_count = 0;
	counter<T> occurrence_counter;
	counter<T> support_counter;
	set<T> item_set;
	
	
	typename vector<sequence<T> >::iterator database_iter = database.begin();
	for(; database_iter != database.end(); ++database_iter) {
		item_count += database_iter->length();
		
		if(max_seq_len < database_iter->length()) {
			max_seq_len = database_iter->length();
		}
		if(min_seq_len > database_iter->length()) {
			min_seq_len = database_iter->length();
		}
		
		occurrence_counter.update(database_iter->begin(), database_iter->end());
		
		item_set.insert(database_iter->begin(), database_iter->end());
		support_counter.update(item_set.begin(), item_set.end());
		
		item_set.clear();
	}
	
	typename counter<T>::iterator occurrence_counter_iter = occurrence_counter.begin();
	for(; occurrence_counter_iter != occurrence_counter.end(); ++occurrence_counter_iter) {
		item_set.insert(occurrence_counter_iter->second);
	}
	
	typename set<T>::iterator item_set_iter = item_set.begin();
	occurrences_file << "Item,occurrences" << endl;
	for(; item_set_iter != item_set.end(); ++item_set_iter) {
		typename counter<T>::size_type val = occurrence_counter[*item_set_iter];
		occurrences_file << *item_set_iter << "," << val << "," << val/(double)item_count << endl;
	}
	
	item_set_iter = item_set.begin();
	support_file << "Item,Support" << endl;
	for(; item_set_iter != item_set.end(); ++item_set_iter) {
		typename counter<T>::size_type val = support_counter[*item_set_iter];
		support_file << *item_set_iter << "," << val << "," << val/(double)database.size() << endl;
	}
}

template <typename T>
void display_stats(vector<sequence<ordered_set<T> > > &database, ofstream &occurrences_file, ofstream &support_file) {
	sort(database.begin(), database.end());
	unsigned int unique = database.size();
	for(unsigned int ii=1; ii < database.size(); ii++) {
		if(database[ii] == database[ii-1]) {
			unique--;
		}
	}
	
	unsigned int max_seq_len = 0;
	unsigned min_seq_len = numeric_limits<unsigned int>::max();
	unsigned int max_set_len = 0;
	unsigned min_set_len = numeric_limits<unsigned int>::max();
	typename sequence<T>::size_type item_count = 0;
	counter<T> occurrence_counter;
	counter<T> support_counter;
	set<T> item_set;
	
	typename vector<sequence<ordered_set<T> > >::iterator database_iter = database.begin();
	for(; database_iter != database.end(); ++database_iter) {
		if(max_seq_len < database_iter->length()) {
			max_seq_len = database_iter->length();
		}
		if(min_seq_len > database_iter->length()) {
			min_seq_len = database_iter->length();
		}
		
		sequence<ordered_set<T> > &seq = *database_iter;
		typename sequence<ordered_set<T> >::iterator seq_iter = seq.begin();
		for(; seq_iter != seq.end(); ++seq_iter) {
			item_count += seq_iter->size();
			
			if(max_set_len < seq_iter->size()) {
				max_set_len = seq_iter->size();
			}
			if(min_set_len > seq_iter->size()) {
				min_set_len = seq_iter->size();
			}
			
			occurrence_counter.update(seq_iter->begin(), seq_iter->end());
			item_set.insert(seq_iter->begin(), seq_iter->end());
		}
		
		support_counter.update(item_set.begin(), item_set.end());
		
		item_set.clear();
	}
	
	typename counter<T>::iterator occurrence_counter_iter = occurrence_counter.begin();
	for(; occurrence_counter_iter != occurrence_counter.end(); ++occurrence_counter_iter) {
		item_set.insert(occurrence_counter_iter->second);
	}
	
	typename set<T>::iterator item_set_iter = item_set.begin();
	occurrences_file << "Item,occurrences" << endl;
	for(; item_set_iter != item_set.end(); ++item_set_iter) {
		typename counter<T>::size_type val = occurrence_counter[*item_set_iter];
		occurrences_file << *item_set_iter << "," << val << "," << val/(double)item_count << endl;
	}
	
	item_set_iter = item_set.begin();
	support_file << "Item,Support" << endl;
	for(; item_set_iter != item_set.end(); ++item_set_iter) {
		typename counter<T>::size_type val = support_counter[*item_set_iter];
		support_file << *item_set_iter << "," << val << "," << val/(double)database.size() << endl;
	}
}

template <typename T>
void display_stats(const string &file_name, ofstream &occurrences_file, ofstream &support_file) {
	try {
		vector<sequence<T> > database;
		load_database(file_name, database);
		
		display_stats(database, occurrences_file, support_file);
	}
	catch(exception &e) {
		cerr << e.what() << endl;
	}
}

int main(int argc, char *argv[]) {
	bool itemset;
	bool numeric;
	string occurrences;
	string support;
	string infile;
	
	try {
		if(parse_args(argc, argv, itemset, numeric, occurrences, support, infile)) {
			ofstream occurrences_file(occurrences.c_str());
			ofstream support_file(support.c_str());
			
			if(itemset) {
				if(numeric) {
					display_stats<ordered_set<int> >(infile, occurrences_file, support_file);
				}
				else {
					display_stats<ordered_set<string> >(infile, occurrences_file, support_file);
				}
			}
			else {
				if(numeric) {
					display_stats<int>(infile, occurrences_file, support_file);
				}
				else {
					display_stats<string>(infile, occurrences_file, support_file);
				}
			}
		}
	}
	catch(exception &e) {
		cerr << e.what() << endl;
	}
			
	return 0;
}
