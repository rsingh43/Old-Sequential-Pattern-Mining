#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

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
#include "ordered_set.hh"

void progress(double progress, unsigned int width=50, char label='#') {
	unsigned int ii;
	unsigned int count = progress * width;

	std::cerr << "\r[";
	for(ii=0; ii < count; ii++) {
		std::cerr << label;
	}
	for(; ii < width; ii++) {
		std::cerr << ' ';
	}
	std::cerr << "] ";

	std::cerr << std::setfill(' ') << std::setw(7) << std::fixed << std::setprecision(3) << (100*progress) << "%";
	if(progress == 1.0) {
		std::cerr << std::endl;
	}

	std::cerr.flush();
}

void read_database(const std::string &filename, std::vector<sequence<ordered_set<int> > > &database) {
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

void read_labeled_database(const std::string &filename, std::vector<std::pair<bool,sequence<ordered_set<int> > > > &database) {
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
	std::string database_filename("/home/tntech.edu/jagraves21/Sequential-Pattern-Mining/sample_data/proc_ords/lactic-acid.class.dat");
	std::vector<std::pair<bool,sequence<ordered_set<int> > > > database;
	read_labeled_database(database_filename, database);
	std::cerr << database.size() << std::endl;

	std::string results_filename("/home/tntech.edu/jagraves21/Sequential-Pattern-Mining/sample_data/proc_ords/lactic-acid.838.projected.020.patterns");
	std::ifstream file(results_filename.c_str());
	if(file) {
		unsigned int line_num;
		std::string line;
		std::streampos total, current;
		std::istringstream iss;
		sequence<ordered_set<int> > seq;
		sequence<ordered_set<int> > prefix;

		file.seekg(0, std::ios_base::end);
		total = file.tellg();
		file.clear();
		file.seekg(0, std::ios_base::beg);

		current = file.tellg();
		line_num = 1;
		while(getline(file, line)) {
			current = file.tellg();

			if(line_num % 1000 == 0 || current == total) {
				progress(current/(double)total);
			}

			line_num++;

			iss.str(line);
			iss.clear();
			iss >> seq;
			if(iss.bad()) {
				std::ostringstream oss;
				oss << results_filename << ": " << strerror(errno);

				throw std::runtime_error(oss.str());
			}
			if(iss.fail() && !iss.eof()) {
				std::ostringstream oss;
				oss << results_filename << ": conversion error: " << line;
				
				throw std::runtime_error(oss.str());
			}
			else {
				unsigned int TP = 0;
				unsigned int FP = 0;

				prefix = seq;
				prefix.pop_element();
				
				#pragma omp parallel for reduction(+:TP,FP)
				for(std::vector<std::pair<bool,sequence<ordered_set<int> > > >::size_type jj=0; jj < database.size(); jj++) {
					if(prefix.subsequence(database[jj].second)) {
						if(seq.subsequence(database[jj].second)) {
							TP++;
						}
						else {
							FP++;
						}
					}
				}

				unsigned int size = 0;
				sequence<ordered_set<int> >::iterator iter = seq.begin();
				for(; iter != seq.end(); iter++) {
					size += iter->size();
				}

				std::cout << TP << " " << FP << " " << (TP/(FP+1.0)) << " " << seq.length() << " " << size << " " << seq << std::endl;
			}
		}
	}
	else {
		std::ostringstream oss;
		oss << results_filename << ": " << strerror(errno);

		throw std::runtime_error(oss.str());
	}

	/*for(std::vector<sequence<ordered_set<int> > >::size_type ii=0; ii < results.size(); ii++) {
	  }*/


	return 0;
}
