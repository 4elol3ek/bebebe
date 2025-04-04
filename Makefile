ifeq ($(OS),Windows_NT)
    RM = del /F /Q
    EXE = .exe
else
    RM = rm -f
    EXE =
endif

CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude
target = main

all: libmysimplecomputer.a libmyterm.a libmybigchars.a

libmysimplecomputer.a: mySimpleComputer/sc_commandEncoder.o mySimpleComputer/sc_memory.o mySimpleComputer/sc_regist.o mySimpleComputer/sc_variables.o
	@ ar rcs include/libmysimplecomputer.a mySimpleComputer/sc_commandEncoder.o mySimpleComputer/sc_memory.o mySimpleComputer/sc_regist.o mySimpleComputer/sc_variables.o

libmyterm.a: myTerm/myTerm.o
	@ ar rcs include/libmyterm.a myTerm/myTerm.o

libmybigchars.a: myBigChars/myBigChars.o
	@ ar rcs include/libmybigchars.a myBigChars/myBigChars.o

%.o: %.c
	@ gcc $(CFLAGS) -c $< -o $@

run: libmysimplecomputer.a libmyterm.a libmybigchars.a console/$(target).o
	@ gcc $(CFLAGS) console/$(target).o -Linclude -Wl,--start-group -lmysimplecomputer -lmyterm -lmybigchars -Wl,--end-group -o console/$(target)$(EXE)
	@ ./console/$(target)$(EXE)

console/$(target).o: console/$(target).c
	@ gcc $(CFLAGS) -c console/$(target).c -o console/$(target).o

clean:
	@ cd console && $(RM) $(target).o $(target)$(EXE) && cd ../include && $(RM) libmysimplecomputer.a libmyterm.a libmybigchars.a
	@ cd mySimpleComputer && make clean
	@ cd myTerm && make clean
	@ cd myBigChars && make clean
