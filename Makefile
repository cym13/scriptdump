#
# Makefile for 'scriptdump'.
#
# Type 'make' or 'make scriptdump' to create the binary.
# Type 'make clean' or 'make clear' to delete all temporaries.
# Type 'make run' to execute the binary.
# Type 'make debug' to debug the binary using gdb(1).
#

# build target specs
CC = gcc
CFLAGS = -g
OUT_DIR = release_build
LIBS =

# first target entry is the target invoked when typing 'make'
default: scriptdump

scriptdump: $(OUT_DIR) $(OUT_DIR)/autocleaner.c.o $(OUT_DIR)/screenbuffer.c.o $(OUT_DIR)/scriptdump.c.o
	@echo -n 'Linking scriptdump... '
	@$(CC) $(CFLAGS) -o scriptdump $(OUT_DIR)/autocleaner.c.o $(OUT_DIR)/screenbuffer.c.o $(OUT_DIR)/scriptdump.c.o $(LIBS)
	@echo Done.

$(OUT_DIR):
	mkdir "$(OUT_DIR)"

$(OUT_DIR)/autocleaner.c.o: autocleaner.c autocleaner.h
	@echo -n 'Compiling autocleaner.c... '
	@$(CC) $(CFLAGS) -o $(OUT_DIR)/autocleaner.c.o -c autocleaner.c
	@echo Done.

$(OUT_DIR)/screenbuffer.c.o: screenbuffer.c screenbuffer.h
	@echo -n 'Compiling screenbuffer.c... '
	@$(CC) $(CFLAGS) -o $(OUT_DIR)/screenbuffer.c.o -c screenbuffer.c
	@echo Done.

$(OUT_DIR)/scriptdump.c.o: scriptdump.c scriptdump.h screenbuffer.h \
 autocleaner.h
	@echo -n 'Compiling scriptdump.c... '
	@$(CC) $(CFLAGS) -o $(OUT_DIR)/scriptdump.c.o -c scriptdump.c
	@echo Done.

run:
	./scriptdump 

debug:
	gdb ./scriptdump

clean:
	@echo -n 'Removing all temporary binaries... '
	@rm -f scriptdump $(OUT_DIR)/*.o
	@echo Done.

clear:
	@echo -n 'Removing all temporary binaries... '
	@rm -f scriptdump $(OUT_DIR)/*.o
	@echo Done.

