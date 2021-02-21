CC= g++
CFLAGS= -c -Wvla -Wall
CODEFILES= ex3.tar reader.c calculator.c  Makefile

#generating ex3.exe
all:   reader.o calculator.o heat_eqn.o
	$(CC) reader.o calculator.o heat_eqn.o -o ex3

#generating calculator
calculator:   calculator.c calculator.h heat_eqn.c heat_eqn.h
	$(CC) $(CFLAGS) calculator.c

#generating reader
reader:   reader.c calculator.h heat_eqn.h
	(CC) $(CFLAGS) reader.c

#generating heat_eqn
heat_eqn:   heat_eqn.h heat_eqn.c
	(CC) $(CFLAGS) heat_eqn.c

#generating .tar file
tar:
	tar -cvf $(CODEFILES)

clean:
	rm -f ex3 *.o