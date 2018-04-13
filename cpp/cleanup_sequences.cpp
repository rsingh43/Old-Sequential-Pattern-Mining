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

void usage(const char *filename) {
	cout << "Cleanup Seqeunces" << endl << endl;
	
	cout << "Usage:  " << filename << " [-i/--itemset] [-n/--numeric n] infile [infile ...]" << endl;
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
void load_database(const string &filename, vector<sequence<T> > &database) {
	ifstream file(filename.c_str());
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
void process_files(const vector<string> &filenames) {
	vector<sequence<T> > database;
	
	typename vector<string>::const_iterator filenames_iter = filenames.begin();
	for(; filenames_iter != filenames.end(); ++filenames_iter) {
		try {
			database.clear();
			load_database(*filenames_iter, database);

			std::string filename(*filenames_iter);
			filename += ".fix";
			std::ofstream file(filename.c_str());
			if(file) {
				file << database << std::endl;
				if(file.bad()) {
					ostringstream oss;
					oss <<  filename << ": " << strerror(errno);

					throw runtime_error(oss.str());
				}
				else if(file.fail()) {
					ostringstream oss;
					oss <<  filename << ": error writng file";

					throw runtime_error(oss.str());
				}
			}
			else {
				ostringstream oss;
				oss <<  filename << ": " << strerror(errno);

				throw runtime_error(oss.str());
			}
		}
		catch(exception &e) {
			cerr << e.what() << endl;
		}
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
