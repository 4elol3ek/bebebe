CFLAGS = -Wall -c
TARGET = test

ifeq ($(OS),Windows_NT)
    RM = del
    EXE = .exe
else
    RM = rm -f
    EXE =
endif

all: $(TARGET)

$(TARGET): $(TARGET).o
	@ gcc -o $(TARGET)$(EXE) $(TARGET).o

$(TARGET).o: console/$(TARGET).c
	@ gcc $(CFLAGS) -o $(TARGET).o console/$(TARGET).c

run:
	@./$(TARGET)$(EXE)

clean:
	@$(RM) $(TARGET)$(EXE) $(TARGET).o
