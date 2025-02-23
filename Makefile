ifeq ($(OS),Windows_NT)
    RM = del /F /Q
    EXE = .exe
else
    RM = rm -f
    EXE =
endif

CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude

all: libmysimplecomputer.a

libmysimplecomputer.a: mySimpleComputer/main.o mySimpleComputer/sc_commandEncoder.o mySimpleComputer/sc_memory.o mySimpleComputer/sc_regist.o mySimpleComputer/sc_variables.o
	@ ar rcs console/libmysimplecomputer.a mySimpleComputer/main.o mySimpleComputer/sc_commandEncoder.o mySimpleComputer/sc_memory.o mySimpleComputer/sc_regist.o mySimpleComputer/sc_variables.o

%.o: %.c
	@ gcc $(CFLAGS) -c $< -o $@

test: libmysimplecomputer.a console/main.o
	@ gcc $(CFLAGS) console/main.o -L. -lmysimplecomputer -o console/main$(EXE)
	@ ./console/main$(EXE)

console/main.o: console/main.c
	@ gcc $(CFLAGS) -c console/main.c -o console/main.o

clean:
	@ $(RM) libmysimplecomputer.a test.exe && cd console && $(RM) main.o
	@ cd mySimpleComputer && make clean
