ifeq ($(OS),Windows_NT)
    RM = del /F /Q
    EXE = .exe
else
    RM = rm -f
    EXE =
endif

CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude

all: libmysimplecomputer.a myterm.a

libmysimplecomputer.a: mySimpleComputer/main.o mySimpleComputer/sc_commandEncoder.o mySimpleComputer/sc_memory.o mySimpleComputer/sc_regist.o mySimpleComputer/sc_variables.o
	@ ar rcs include/libmysimplecomputer.a mySimpleComputer/main.o mySimpleComputer/sc_commandEncoder.o mySimpleComputer/sc_memory.o mySimpleComputer/sc_regist.o mySimpleComputer/sc_variables.o

%.o: %.c
	@ gcc $(CFLAGS) -c $< -o $@

myterm.a: myTerm/myTerm.o
	@ ar rcs include/libmyterm.a myTerm/myTerm.o

run: libmysimplecomputer.a myterm.a console/main.o
	@ gcc $(CFLAGS) console/main.o -Linclude -Wl,--start-group -lmysimplecomputer -lmyterm -Wl,--end-group -o console/main$(EXE)
	@ ./console/main$(EXE)

console/main.o: console/main.c
	@ gcc $(CFLAGS) -c console/main.c -o console/main.o

clean:
	@ cd console && $(RM) main.o main$(EXE) && cd ../include && $(RM) libmysimplecomputer.a
	@ cd mySimpleComputer && make clean
