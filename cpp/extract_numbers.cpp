#include <iostream>
#include <fstream>
#include <sstream>

#include <set>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <iterator>

#include <stdexcept>

#include <cmath>
#include <cerrno>
#include <cstring>

#include "sequence.hh"

void read_database(const std::string &filename, std::vector<sequence<int> > &database) {
	std::ifstream file(filename.c_str());
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

void read_labeled_database(const std::string &filename, std::vector<std::pair<bool,sequence<int> > > &database) {
	std::ifstream file(filename.c_str());
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

int main(int argc, char *argv[]) {
	std::vector<sequence<int> > results;
	read_database("/tmp/work/synthetic.patterns.dat", results);

	std::vector<std::pair<bool,sequence<int> > > database;
	read_labeled_database("/tmp/work/synthetic.class.dat", database);

	std::vector<sequence<int> >::iterator results_iter = results.begin();
	for(; results_iter != results.end(); ++results_iter) {
		unsigned int TP = 0;
		unsigned int FP = 0;
		unsigned int TN = 0;
		unsigned int FN = 0;

		std::vector<std::pair<bool,sequence<int> > >::iterator database_iter = database.begin();
		for(; database_iter != database.end(); ++database_iter) {
			if(results_iter->subsequence(database_iter->second)) {
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

		std::cout << *results_iter << " " << TP << " " << FP << " " << TN << " " << FN << std::endl;
	}

	
	return 0;
}
