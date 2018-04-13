# C PreProcessor
CPPFLAGS = -DNDEBUG
#CPPFLAGS = -DNDEBUG

# C Compiler
CC = gcc

# C Compiler Flags
CFLAGS = -Wall -ansi -pedantic -O3

# C++ Compiler
CXX = g++

# C++ Compiler Flags
CXXFLAGS = -Wall -ansi -pedantic -O3

# Extra flags to give to compilers when they are supposed to invoke the linker, 'ld', such as -L. Libraries (-lfoo) should be added to the LDLIBS variable instead.
LDFLAGS = 

# Library flags or names given to compilers when they are supposed to invoke the linker, 'ld'. LOADLIBES is a deprecated (but still supported) alternative to LDLIBS. Non-library linker flags, such as -L, should go in the LDFLAGS variable.
LDLIBS = -lstdc++ -lm






CPP_FILES = binomial_mixture_sequences.cpp binomial_sequences.cpp cleanup_sequences.cpp diversity.cpp evaluate_sequences.cpp extract_numbers.cpp lazy_frequent_miner.cpp projected_list_lazy_frequent_miner.cpp projected_list_miner.cpp random_sequences.cpp seq-stats.cpp sequence.cpp sequential_pattern_miner.cpp subgroup_miner.cpp work.cpp
OBJ_FILES := $(CPP_FILES:.cpp=.o)
HDR_FILES = counter.hh infix_iterator.hh lazy_frequent_miner.hh leaks.hh maintain_and_test.hh ordered_set.hh pattern_mining.hh prefix_pattern_mining.hh projected_list_lazy_frequent_miner.hh projected_list_miner.hh sequence.hh sequential_pattern_miner.hh subgroup_discovery.hh subgroup_miner.hh timer.hh

PROG = binomial_mixture_sequences binomial_sequences cleanup_sequences evaluate_sequences extract_numbers lazy_frequent_miner projected_list_lazy_frequent_miner projected_list_miner random_sequences seq-stats sequence sequential_pattern_miner subgroup_miner work

binomial_sequences: CXXFLAGS += -std=c++11
binomial_mixture_sequences: CXXFLAGS += -std=c++11
diversity: CXXFLAGS += -std=c++11 -I../gzstream
diversity: LDLIBS += -lgzstream -lz 
diversity: LDFLAGS += -L../gzstream
evaluate_sequences: CXXFLAGS += -std=c++11 -I../gzstream
evaluate_sequences: LDLIBS += -lgzstream -lz 
evaluate_sequences: LDFLAGS += -L../gzstream
random_sequences: CXXFLAGS += -std=c++11
work: CXXFLAGS += -fopenmp
work: LDFLAGS += -fopenmp -lpthread


.PHONY : all
all : $(PROG)

binomial_mixture_sequences.o: 
binomial_sequences.o:
cleanup_sequences.o: counter.hh leaks.hh ordered_set.hh sequence.hh
evaluate_sequences.o: counter.hh leaks.hh ordered_set.hh sequence.hh
extract_numbers.o: sequence.hh
lazy_frequent_miner.o: lazy_frequent_miner.hh pattern_mining.hh
projected_list_lazy_frequent_miner.o: pattern_mining.hh projected_list_lazy_frequent_miner.hh
projected_list_miner: pattern_mining.hh projected_list_miner.hh
random_sequences.o:
seq-stats.o: counter.hh leaks.hh ordered_set.hh sequence.hh
sequence.o: counter.hh leaks.hh ordered_set.hh sequence.hh
sequential_pattern_miner.o: pattern_mining.hh sequential_pattern_miner.hh
subgroup_miner.o: subgroup_discovery.hh subgroup_miner.hh
work.o: ordered_set.hh sequence.hh

.PHONY : clean
clean:
	$(RM) $(OBJ_FILES)

.PHONY : veryclean
veryclean:
	$(RM) $(PROG) $(OBJ_FILES)

.PHONY : tar
tar:
	tar -czvf src.tar.gz $(HDR_FILES) $(CPP_FILES) Makefile

