CC=mpicc
CFLAGS=-I. -ggdb
DEPS = #hellomake.h
OBJ_main = mpi2.o
OBJ_mpi_my1 = mpi_my1.o
LIBS=#-lpthread

all: main mpi_my1

%.o: %.c $(DEPS)
	@$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ_main)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

mpi_my1: $(OBJ_mpi_my1)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: all
