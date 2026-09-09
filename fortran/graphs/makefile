FC = gfortran
FFLAGS = -O2 -g -std=f2008 -Wall -Wextra

LIB_OBJ = graphs.o graph_algorithms.o
EXAMPLES = example_shortest_paths example_components_mst example_toposort example_grid_benchmark

all: $(EXAMPLES)

graphs.o: graphs.f90
	$(FC) $(FFLAGS) -c $<

graph_algorithms.o: graph_algorithms.f90 graphs.o
	$(FC) $(FFLAGS) -c $<

example_%: example_%.f90 $(LIB_OBJ)
	$(FC) $(FFLAGS) -o $@ $< $(LIB_OBJ)

run: all
	./example_shortest_paths
	./example_components_mst
	./example_toposort
	./example_grid_benchmark

test_graphs: test_graphs.f90 $(LIB_OBJ)
	$(FC) $(FFLAGS) -o $@ $< $(LIB_OBJ)

test: test_graphs
	./test_graphs

clean:
	rm -f *.o *.mod $(EXAMPLES) test_graphs

.PHONY: all run test clean
