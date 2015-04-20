# Makefile for Rom24. Works fine on my Debian system.
# You may need to use 'gmake' on BSD systems.

CC = gcc
RM = rm
EXE = ../area/rom
PROF = -O -g3

# Use -Wno-unused-but-set-variable to filter out those warnings where macro's might be using variables
# that appear to be unused but are in fact used.
#C_FLAGS = $(PROF) -Wall -Wno-unused-but-set-variable
C_FLAGS = $(PROF) -Wall
L_FLAGS = $(PROF) -lcrypt

# Source Files
SRC_FILES := $(wildcard *.c)

# Object Files
OBJ_DIR = obj
OBJ_FILES := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Make the Object directory if it doesn't exist
$(shell   mkdir -p $(OBJ_DIR))

rom: $(OBJ_FILES)
	$(RM) -f $(EXE)
	$(CC) -o $(EXE) $(OBJ_FILES) $(L_FLAGS)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(C_FLAGS) -c -o $@ $<

clean:
	$(RM) -f $(OBJ_FILES) $(EXE) *~ *.bak *.orig *.rej

distro:
	make clean
	cd ../..
	$(RM) -f log/*.log
	tar zvcf crimsonskiesmud-`date -I`.tar.gz CrimsonSkiesMud
