ifeq ($(OS),Windows_NT)
    RM = del /F /Q
    EXE = .exe
else
    RM = rm -f
    EXE =
endif

all: libmyterm.a

libmyterm.a: myTerm.o
	@ ar rcs libmyterm.a myTerm.o

%.o: %.c
	@ gcc -Wall -Wextra -std=c11 -O2 -I../include -c $< -o $@

test: libmyterm.a main.o
	@ gcc -Wall -Wextra -std=c11 -O2 main.o -L. -lmyterm -o test$(EXE)
	@ ./test$(EXE)

main.o: main.c
	@ gcc -Wall -Wextra -std=c11 -O2 -I../include -c main.c -o main.o

clean:
	@ $(RM) myTerm.o \
	         libmyterm.a main.o test$(EXE)
