CC = gcc
CFLAGS = -c -Wextra -Wall -Wvla -std=c99

# All

all: ex3
	$(CC) $(CFLAGS) heat_eqn.c calculator.c reader.c


# Executables

ex3: reader.o heat_eqn.o calculator.o
	$(CC) reader.o heat_eqn.o calculator.o -o ex3

# Objects

heat_eqn.o: heat_eqn.h heat_eqn.c
	$(CC) $(CFLAGS) heat_eqn.c

calculator.o: calculator.h calculator.c
	$(CC) $(CFLAGS) calculator.c

reader.o: reader.c calculator.o heat_eqn.o
	$(CC) $(CFLAGS) reader.c

# Other

tar:
	tar cvf ex3.tar Makefile extension.pdf reader.c heat_eqn.h heat_eqn.c calculator.h calculator.c

clean:
	-rm -f *.o ex3
