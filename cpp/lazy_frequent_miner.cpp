#include "pattern_mining.hh"
#include "lazy_frequent_miner.hh"

int main(int argc, char *argv[]) {
	return run<lazy_frequent>(argc, argv);
}
