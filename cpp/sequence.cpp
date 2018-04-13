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
	cout << "Sequence Information" << endl << endl;
	
	cout << "Usage:  " << file_name << " [-i/--itemset] [-n/--numeric n] infile [infile ...]" << endl;
	cout << "  --itemset      treate items as sets " << endl;
	cout << "  --numeric      sequence items are treated as nonnegative integers" << endl;
	cout << "  infile         input sequence database" << endl;
}

bool parse_args(int argc, char *argv[], bool &itemset, bool &numeric, vector<string> &infile) {
	itemset = false;
	numeric = false;
	
	infile.clear();
	infile.reserve(argc);
	
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
			else {
				infile.push_back(string(argv[ii]));
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
void display_stats(vector<sequence<T> > &database) {
	cout << setw(9) << database.size() << " Sequences" << endl;
	
	std::sort(database.begin(), database.end());
	unsigned int unique = database.size();
	for(unsigned int ii=1; ii < database.size(); ii++) {
		if(database[ii] == database[ii-1]) {
			unique--;
		}
	}
	
	cout << setw(9) << unique << " Unique Sequences" << endl;
	cout << fixed << setprecision(2) << setw(9) << 100.0 * unique / database.size() << " % Unique" << endl;
	
	unsigned int max_seq_len = 0;
	unsigned min_seq_len = std::numeric_limits<unsigned int>::max();
	counter<T> occurrence_counter;
	counter<T> support_counter;
	set<T> item_set;
	
	typename vector<sequence<T> >::iterator database_iter = database.begin();
	for(; database_iter != database.end(); ++database_iter) {
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
	
	cout << setw(9) << max_seq_len << " Maximum Sequence Length" << endl;
	cout << setw(9) << min_seq_len << " Minimum Sequence Length" << endl;
	cout << setw(9) << occurrence_counter.size() << " Symbols" << endl << endl;
	
	typename counter<T>::iterator occurrence_counter_iter = occurrence_counter.begin();
	for(; occurrence_counter_iter != occurrence_counter.end(); ++occurrence_counter_iter) {
		item_set.insert(occurrence_counter_iter->second);
	}
	
	ostringstream oss;
	unsigned int max_item_len = 4;
	typename set<T>::iterator item_set_iter = item_set.begin();
	for(; item_set_iter != item_set.end(); ++item_set_iter) {
		oss.clear();
		oss.str("");
		oss << *item_set_iter;
		
		if(max_item_len < oss.str().size()) {
			max_item_len = oss.str().size();
		}
	}
	
	cout << left << setw(max_item_len) << "Item" << " " << left << setw(12) << " Occurrences";
	cout << "\t\t";
	cout << left << setw(max_item_len) << "Item" << "  Relative / Absolute Support" << endl;
	
	item_set_iter = item_set.begin();
	typename counter<T>::iterator support_counter_iter = support_counter.begin();
	for(typename set<T>::size_type ii=0; ii < item_set.size(); ii++) {
		cout << left << setw(max_item_len) << *item_set_iter << " " << right << setw(12) << occurrence_counter[*item_set_iter];
		cout << "\t\t";
		cout << left << setw(max_item_len) << support_counter_iter->second << "  " << right << setprecision(2) << setw(7) << (100.0 * support_counter_iter->first / database.size()) << "%   " << setw(8) << support_counter_iter->first << endl;
		
		++item_set_iter;
		++support_counter_iter;
	}
}

template <typename T>
void display_stats(vector<sequence<ordered_set<T> > > &database) {
	cout << setw(9) << database.size() << " Sequences" << endl;
	
	std::sort(database.begin(), database.end());
	unsigned int unique = database.size();
	for(unsigned int ii=1; ii < database.size(); ii++) {
		if(database[ii] == database[ii-1]) {
			unique--;
		}
	}
	
	cout << setw(9) << unique << " Unique Sequences" << endl;
	cout << fixed << setprecision(2) << setw(9) << 100.0 * unique / database.size() << " % Unique" << endl;
	
	unsigned int max_seq_len = 0;
	unsigned min_seq_len = std::numeric_limits<unsigned int>::max();
	unsigned int max_set_len = 0;
	unsigned min_set_len = std::numeric_limits<unsigned int>::max();
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
	
	cout << setw(9) << max_seq_len << " Maximum Sequence Length" << endl;
	cout << setw(9) << min_seq_len << " Minimum Sequence Length" << endl;
	cout << setw(9) << max_set_len << " Maximum Set Length" << endl;
	cout << setw(9) << min_set_len << " Minimum Set Length" << endl;
	cout << setw(9) << occurrence_counter.size() << " Symbols" << endl << endl;
	
	typename counter<T>::iterator occurrence_counter_iter = occurrence_counter.begin();
	for(; occurrence_counter_iter != occurrence_counter.end(); ++occurrence_counter_iter) {
		item_set.insert(occurrence_counter_iter->second);
	}
	
	ostringstream oss;
	unsigned int max_item_len = 4;
	typename set<T>::iterator item_set_iter = item_set.begin();
	for(; item_set_iter != item_set.end(); ++item_set_iter) {
		oss.clear();
		oss.str("");
		oss << *item_set_iter;
		
		if(max_item_len < oss.str().size()) {
			max_item_len = oss.str().size();
		}
	}
	
	cout << left << setw(max_item_len) << "Item" << " " << left << setw(12) << " Occurrences";
	cout << "\t\t";
	cout << left << setw(max_item_len) << "Item" << "  Relative / Absolute Support" << endl;
	
	item_set_iter = item_set.begin();
	typename counter<T>::iterator support_counter_iter = support_counter.begin();
	for(typename set<T>::size_type ii=0; ii < item_set.size(); ii++) {
		cout << left << setw(max_item_len) << *item_set_iter << " " << right << setw(12) << occurrence_counter[*item_set_iter];
		cout << "\t\t";
		cout << left << setw(max_item_len) << support_counter_iter->second << "  " << right << setprecision(2) << setw(7) << (100.0 * support_counter_iter->first / database.size()) << "%   " << setw(8) << support_counter_iter->first << endl;
		
		++item_set_iter;
		++support_counter_iter;
	}
}

template <typename T>
void display_stats(const string &file_name) {
	try {
		cout << file_name << endl;
		vector<sequence<T> > database;
		load_database(file_name, database);
		
		display_stats(database);
	}
	catch(exception &e) {
		cerr << e.what() << endl;
	}
}

template <typename T>
void display_stats(const string &file_name, vector<sequence<T> > &database) {
	cout << setw(9) << database.size() << "  "; cout.flush();
		
	std::sort(database.begin(), database.end());
	unsigned int unique = database.size();
	for(unsigned int ii=1; ii < database.size(); ii++) {
		if(database[ii] == database[ii-1]) {
			unique--;
		}
	}
	
	cout << fixed << setprecision(2) << setw(6) << 100.0 * unique / database.size() << "%  "; cout.flush();
	
	unsigned int max_seq_len = 0;
	unsigned min_seq_len = std::numeric_limits<unsigned int>::max();
	counter<T> occurrence_counter;
	
	typename vector<sequence<T> >::iterator database_iter = database.begin();
	for(; database_iter != database.end(); ++database_iter) {
		if(max_seq_len < database_iter->length()) {
			max_seq_len = database_iter->length();
		}
		if(min_seq_len > database_iter->length()) {
			min_seq_len = database_iter->length();
		}
		
		occurrence_counter.update(database_iter->begin(), database_iter->end());
	}
	
	cout << setw(9) << max_seq_len << "  ";
	cout << setw(9) << min_seq_len << "  ";
	cout << setw(9) << occurrence_counter.size() << "  ";
	cout << file_name << endl;
}

template <typename T>
void display_stats(const string &file_name, vector<sequence<ordered_set<T> > > &database) {
	cout << setw(9) << database.size() << "  "; cout.flush();
	
	std::sort(database.begin(), database.end());
	unsigned int unique = database.size();
	for(unsigned int ii=1; ii < database.size(); ii++) {
		if(database[ii] == database[ii-1]) {
			unique--;
		}
	}
	
	cout << fixed << setprecision(2) << setw(6) << 100.0 * unique / database.size() << "%  "; cout.flush();
	
	unsigned int max_seq_len = 0;
	unsigned min_seq_len = std::numeric_limits<unsigned int>::max();
	counter<T> occurrence_counter;
	
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
			occurrence_counter.update(seq_iter->begin(), seq_iter->end());
		}
	}
	
	cout << setw(9) << max_seq_len << "  ";
	cout << setw(9) << min_seq_len << "  ";
	cout << setw(9) << occurrence_counter.size() << "  ";
	cout << file_name << endl;
}

template <typename T>
void display_stats(const vector<string> &file_names) {
	vector<sequence<T> > database;
	
	cout << setw(9) << "Sequences" << "  ";
	cout << setw(7) << "Unique" << "  ";
	cout << setw(9) << "Max Len" << "  ";
	cout << setw(9) << "Min Len" << "  ";
	cout << setw(9) << "Symbols" << "  ";
	cout << "File" << endl;
	
	typename vector<string>::const_iterator file_names_iter = file_names.begin();
	for(; file_names_iter != file_names.end(); ++file_names_iter) {
		try {
			database.clear();
			load_database(*file_names_iter, database);
			display_stats(*file_names_iter, database);
		}
		catch(exception &e) {
			cerr << e.what() << endl;
		}
	}
}

template <typename T>
void process_files(const vector<string> &file_names) {
	if(file_names.size() == 1) {
		display_stats<T>(file_names.front());
	}
	else {
		display_stats<T>(file_names);
	}
}

int main(int argc, char *argv[]) {
	bool itemset;
	bool numeric;
	vector<string> input_files;
	
	try {
		if(parse_args(argc, argv, itemset, numeric, input_files)) {
			if(itemset) {
				if(numeric) {
					process_files<ordered_set<int> >(input_files);
				}
				else {
					process_files<ordered_set<string> >(input_files);
				}
			}
			else {
				if(numeric) {
					process_files<int>(input_files);
				}
				else {
					process_files<string>(input_files);
				}
			}
		}
	}
	catch(exception &e) {
		cerr << e.what() << endl;
	}
			
	return 0;
}
