// g++ -Wall -ansi -pedantic  -std=c++11 -O3 -fopenmp coin_filp.cpp -DTRIALS=1000 -DFLIPS=200

#include <iostream>
#include <sstream>
#include <ostream>
#include <fstream>

#include <string>
#include <set>

#include <random>
#include <algorithm>

#include <limits>

#include <cstring>
#include <cerrno>

using namespace std;

template <typename RNG>
void generate_sequences(unsigned int num_sequences, unsigned int min_seq_len, unsigned int max_seq_len, unsigned int num_symbols, RNG &generator, ostream &output) {
	unsigned int seq_len;
	for(unsigned int ii=0; ii < num_sequences; ii++) {
		seq_len = generator() % (max_seq_len - min_seq_len + 1) + min_seq_len;
		
		output << "<";
		for(unsigned int jj=0; jj < seq_len; jj++) {
			output << generator() % num_symbols;
			
			if(jj+1 < seq_len) {
				output << ",";
			}
		}
		output << ">" << endl;
	}
}

template <typename RNG>
void generate_sequences(unsigned int num_sequences, unsigned int min_seq_len, unsigned int max_seq_len, unsigned int num_symbols, unsigned int min_set_size, unsigned int max_set_size, RNG &generator, ostream &output) {
	unsigned int seq_len, set_size;
	set<unsigned int> itemset;
	for(unsigned int ii=0; ii < num_sequences; ii++) {
		seq_len = generator() % (max_seq_len - min_seq_len + 1) + min_seq_len;
		output << "<";
		for(unsigned int jj=0; jj < seq_len; jj++) {
			set_size = generator() % (max_set_size - min_set_size + 1) + min_set_size;
			
			itemset.clear();
			while(itemset.size() < set_size) {
				itemset.insert(generator() % num_symbols);
			}
			
			output << "(";
			set<unsigned int>::iterator itemset_iter = itemset.begin();
			while(true) {
				output << *itemset_iter;
				
				++itemset_iter;
				if(itemset_iter == itemset.end()) {
					break;
				}
				
				output << ",";
			}
			output << ")";
			
			
			if(jj+1 < seq_len) {
				output << ",";
			}
		}
		output << ">" << endl;
	}
}

void generate_sequences(unsigned int num_sequences, unsigned int min_seq_len, unsigned int max_seq_len, unsigned int num_symbols, bool itemset, unsigned int min_set_size, unsigned int max_set_size, ostream &output) {
	random_device random;
	default_random_engine generator(random());
	
	if(itemset) {
		generate_sequences(num_sequences, min_seq_len, max_seq_len, num_symbols, min_set_size, max_set_size, generator, output);
	}
	else {
		generate_sequences(num_sequences, min_seq_len, max_seq_len, num_symbols, generator, output);
	}
}

void usage(const char *file_name) {
	cout << "Geneate sequences with a uniform symbol distrobution." << endl << endl;
	
	cout << "Usage:  " << file_name << " [--num-sequences n] [--min-seq-len n] [--max-seq-len n] [--num-symbols n] [--itemset] [--min-set-size n] [--max-set-size n] [--output file]" << endl << endl;
	cout << "  --num-sequences  number of sequences, default 100" << endl;
	cout << "  --min-seq-len    minimum sequence length, default 10" << endl;
	cout << "  --max-seq-len    maximum sequence length, default 20" << endl;
	cout << "  --num-symbols    number of unique symbols, default 10" << endl;
	cout << "  --itemset        flag to generate sequences of itemsets" << endl;
	cout << "  --min-set-size   minimum set length, default 3" << endl;
	cout << "  --max-set-size   maximum set length, default 7" << endl;
	cout << "  --output         output file name, default standard out" << endl;
}

bool parse_args(int argc, char *argv[], unsigned int &num_sequences, unsigned int &min_seq_len, unsigned int &max_seq_len, unsigned int &num_symbols, bool &itemset, unsigned int &min_set_size, unsigned int &max_set_size, string &output) {
	num_sequences = 100;
	min_seq_len = 10;
	max_seq_len = 20;
	num_symbols = 10;
	itemset = false;
	min_set_size = 3;
	max_set_size = 7;
	output.assign("");
	
	int ii=1;
	while(ii < argc) {
		if(strcmp(argv[ii], "--num-sequences") == 0) {
			ii++;
			if(ii >= argc) {
				cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
				usage(argv[0]);
				
				return false;
			}
			
			istringstream iss(argv[ii]);
			iss >> num_sequences;
			
			if(iss.bad() || iss.fail() || !iss.eof()) {
				if(num_sequences == std::numeric_limits<unsigned int>::max()) {
					cerr << "Number of sequences specified exceeds maximum allowed value." << endl << endl;
					usage(argv[0]);
				
					return false;
				}
				else {
					cerr << "Option " << argv[ii-1] << " requires a positive integer value." << endl << endl;
					usage(argv[0]);
					
					return false;
				}
			}
		}
		else if(strcmp(argv[ii], "--min-seq-len") == 0) {
			ii++;
			if(ii >= argc) {
				cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
				usage(argv[0]);
				
				return false;
			}
			
			istringstream iss(argv[ii]);
			iss >> min_seq_len;
			
			if(iss.bad() || iss.fail() || !iss.eof()) {
				if(min_seq_len == std::numeric_limits<unsigned int>::max()) {
					cerr << "Minimum sequence length exceeds maximum allowed value." << endl << endl;
					usage(argv[0]);
				
					return false;
				}
				else {
					cerr << "Option " << argv[ii-1] << " requires a positive integer value." << endl << endl;
					usage(argv[0]);
					
					return false;
				}
			}
		}
		else if(strcmp(argv[ii], "--max-seq-len") == 0) {
			ii++;
			if(ii >= argc) {
				cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
				usage(argv[0]);
				
				return false;
			}
			
			istringstream iss(argv[ii]);
			iss >> max_seq_len;
			
			if(iss.bad() || iss.fail() || !iss.eof()) {
				if(max_seq_len == std::numeric_limits<unsigned int>::max()) {
					cerr << "Maximum sequence length exceeds maximum allowed value." << endl << endl;
					usage(argv[0]);
				
					return false;
				}
				else {
					cerr << "Option " << argv[ii-1] << " requires a positive integer value." << endl << endl;
					usage(argv[0]);
					
					return false;
				}
			}
		}
		else if(strcmp(argv[ii], "--min-symbol") == 0) {
			ii++;
			if(ii >= argc) {
				cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
				usage(argv[0]);
				
				return false;
			}
			
			istringstream iss(argv[ii]);
			iss >> num_symbols;
			
			if(iss.bad() || iss.fail() || !iss.eof()) {
				if(num_symbols == std::numeric_limits<unsigned int>::max()) {
					cerr << "Maximum number of symbols specified exceeds maximum allowed value." << endl << endl;
					usage(argv[0]);
				
					return false;
				}
				else {
					cerr << "Option " << argv[ii-1] << " requires a positive integer value." << endl << endl;
					usage(argv[0]);
					
					return false;
				}
			}
		}
		else if(strcmp(argv[ii], "--num-symbols") == 0) {
			ii++;
			if(ii >= argc) {
				cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
				usage(argv[0]);
				
				return false;
			}
			
			istringstream iss(argv[ii]);
			iss >> num_symbols;
			
			if(iss.bad() || iss.fail() || !iss.eof()) {
				if(num_symbols == std::numeric_limits<unsigned int>::max()) {
					cerr << "Number of symbols specified exceeds maximum allowed value." << endl << endl;
					usage(argv[0]);
				
					return false;
				}
				else {
					cerr << "Option " << argv[ii-1] << " requires a positive integer value." << endl << endl;
					usage(argv[0]);
					
					return false;
				}
			}
		}
		else if(strcmp(argv[ii], "--itemset") == 0) {
			itemset = true;
		}
		else if(strcmp(argv[ii], "--min-set-size") == 0) {
			ii++;
			if(ii >= argc) {
				cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
				usage(argv[0]);
				
				return false;
			}
			
			istringstream iss(argv[ii]);
			iss >> min_set_size;
			
			if(iss.bad() || iss.fail() || !iss.eof()) {
				if(min_set_size == std::numeric_limits<unsigned int>::max()) {
					cerr << "Minimum set length specified exceeds maximum allowed value." << endl << endl;
					usage(argv[0]);
				
					return false;
				}
				else {
					cerr << "Option " << argv[ii-1] << " requires a positive integer value." << endl << endl;
					usage(argv[0]);
					
					return false;
				}
			}
		}
		else if(strcmp(argv[ii], "--max-set-size") == 0) {
			ii++;
			if(ii >= argc) {
				cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
				usage(argv[0]);
				
				return false;
			}
			
			istringstream iss(argv[ii]);
			iss >> max_seq_len;
			
			if(iss.bad() || iss.fail() || !iss.eof()) {
				if(max_seq_len == std::numeric_limits<unsigned int>::max()) {
					cerr << "Maximum set length specified exceeds maximum allowed value." << endl << endl;
					usage(argv[0]);
				
					return false;
				}
				else {
					cerr << "Option " << argv[ii-1] << " requires a positive integer value." << endl << endl;
					usage(argv[0]);
					
					return false;
				}
			}
		}
		else if(strcmp(argv[ii], "--output") == 0) {
			ii++;
			if(ii >= argc) {
				cerr << "Option " << argv[ii-1] << " requires an argument." << endl << endl;
				usage(argv[0]);
				
				return false;
			}
			
			output.assign(argv[ii]);
		}
		else if((strcmp(argv[ii], "-h") == 0) || (strcmp(argv[ii], "--help") == 0)) {
			usage(argv[0]);
			
			return false;
		}
		else {
			cerr << "Unrecognized input option " << argv[ii] << endl << endl;
			usage(argv[0]);
			
			return false;
		}
		
		ii++;
	}
	
	if(max_seq_len < min_seq_len) {
		cerr << "Maximum sequence length (" << max_seq_len << ") can not be less than the minimum sequence length (" << min_seq_len << ")." << endl << endl;
		usage(argv[0]);
		
		return false;
	}
	else if(max_set_size < min_set_size) {
		cerr << "Maximum set length (" << max_set_size << ") can not be less than the minimum set length (" << min_set_size << ")." << endl << endl;
		usage(argv[0]);
		
		return false;
	}
	else if(itemset && num_symbols < max_set_size) {
		cerr << "Maximum set length (" << max_set_size << ") must less than the number of symbols (" << num_symbols << ")." << endl << endl;
		usage(argv[0]);
		
		return false;
	}
	
	return true;
}

int main(int argc, char *argv[]) {
	unsigned int num_sequences, min_seq_len, max_seq_len, num_symbols, min_set_size, max_set_size;
	bool itemset;
	string file_name;
	
	if(parse_args(argc, argv, num_sequences, min_seq_len, max_seq_len, num_symbols, itemset, min_set_size, max_set_size, file_name)) {
		if(!file_name.empty()) {
			ofstream file(file_name.c_str());
			if(file) {
				generate_sequences(num_sequences, min_seq_len, max_seq_len, num_symbols, itemset, min_set_size, max_set_size, file);
			}
			else {
				cerr << file_name << " : error opening file file, " << strerror(errno);
			}
		}
		else {
			generate_sequences(num_sequences, min_seq_len, max_seq_len, num_symbols, itemset, min_set_size, max_set_size, cout);
		}
	}
	
	return 0;
}
