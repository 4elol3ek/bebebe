ifeq ($(OS),Windows_NT)
    RM = del /F /Q
    EXE = .exe
	SL = \\
else
    RM = rm -f
    EXE =
	SL = /
endif

CFLAGS = -Wall -Wextra -O3 -Iinclude
target = main
LIBDIR = include
OBJDIR = console

# .a и их .o файлы
LIBS = mySimpleComputer myTerm myBigChars myReadKey out

mysimplecomputer_OBJS = mySimpleComputer/sc_commandEncoder.o mySimpleComputer/sc_memory.o mySimpleComputer/sc_regist.o mySimpleComputer/sc_variables.o mySimpleComputer/CPU.o
myterm_OBJS = myTerm/myTerm.o
mybigchars_OBJS = myBigChars/myBigChars.o
myreadkey_OBJS = myReadKey/myReadKey.o
out_OBJS = console/out.o

.PHONY: all clean run

all: $(addprefix $(LIBDIR)/lib,$(addsuffix .a,$(LIBS)))

%.o: %.c
	@ gcc $(CFLAGS) -c $< -o $@

$(LIBDIR)/libmySimpleComputer.a: $(mysimplecomputer_OBJS)
	@ ar rcs $@ $^

$(LIBDIR)/libmyTerm.a: $(myterm_OBJS)
	@ ar rcs $@ $^

$(LIBDIR)/libmyBigChars.a: $(mybigchars_OBJS)
	@ ar rcs $@ $^

$(LIBDIR)/libmyReadKey.a: $(myreadkey_OBJS)
	@ ar rcs $@ $^

$(LIBDIR)/libout.a: $(out_OBJS)
	@ ar rcs $@ $^

$(OBJDIR)/$(target).o: $(OBJDIR)/$(target).c
	@ gcc $(CFLAGS) -c $< -o $@

run: all $(OBJDIR)/$(target).o
	@ gcc $(CFLAGS) $(OBJDIR)/$(target).o -L$(LIBDIR) -Wl,--start-group $(addprefix -l,$(LIBS)) -Wl,--end-group -o $(OBJDIR)/$(target)$(EXE)
	@ ./$(OBJDIR)/$(target)$(EXE)


clean:
	-@ $(RM) $(OBJDIR)$(SL)$(target).o $(OBJDIR)$(SL)$(target)$(EXE) $(OBJDIR)$(SL)out.o
	-@ $(RM) $(addprefix $(LIBDIR)$(SL)lib,$(addsuffix .a,$(LIBS)))
	@ $(foreach dir, $(LIBS), \
		$(if $(wildcard $(dir)$(SL)Makefile), \
			$(MAKE) -C $(dir) clean;))
