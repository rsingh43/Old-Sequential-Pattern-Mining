#include "pattern_mining.hh"
#include "projected_list_lazy_frequent_miner.hh"

int main(int argc, char *argv[]) {
	return run<projected_list_lazy_frequent_miner>(argc, argv);
}
