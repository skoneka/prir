CC=mpicc
CFLAGS=-I. -ggdb
DEPS = #hellomake.h
OBJ_main = main.o

OBJ_main2 = main2.o
OBJ_next = next.o
OBJ_mpi_reduce = mpi_reduce.o
OBJ_mpi_integration1 = mpi_integration1.o
LIBS=#-lpthread

all: main main2 next mpi_reduce 
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ_main)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
OBJ_scatter = scatter.o
scatter: $(OBJ_scatter)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
main2: $(OBJ_main2)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
next: $(OBJ_next)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
mpi_reduce: $(OBJ_mpi_reduce)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean: 
	rm -f main $(OBJ_main) mpi_reduce mpi_reduce.o
	rm -f next $(OBJ_next)
	rm -f main2 $(OBJ_main2)
	rm -f scatter $(OBJ_scatter)

.PHONY: all clean
