C=gcc
CFLAGS=-I. -ggdb
DEPS = #hellomake.h
OBJ = main.o
LIBS=-lpthread

%.o: %.c $(DEPS)
	@$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	@gcc -o $@ $^ $(CFLAGS) $(LIBS)
