# Makefile for CS-Mud - may need to apt-get cshell to use.
# You may need to use 'gmake' on BSD systems.

CC = gcc
RM = rm
EXE = ../area/cs-mud
CRYPTEXE = ../area/crypt
PROF = -O -g3

# Use -Wno-unused-but-set-variable to filter out those warnings where macro's might be using variables
# that appear to be unused but are in fact used.
# -psthread and -ldl were added in order to get sqlite to compile under Ubuntu
#C_FLAGS = $(PROF) -Wall -Wno-unused-but-set-variable
C_FLAGS = $(PROF) -Wall
L_FLAGS = $(PROF) -lcrypt -pthread -ldl

# SQLite files
SQLITE_SRC = sqlite/sqlite3.c

# Source Files
SRC_FILES := $(wildcard *.c) $(SQLITE_SRC)

# Object Files
OBJ_DIR = obj
OBJ_FILES := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
OBJ_FILES := $(filter-out $(OBJ_DIR)/crypt.o, $(OBJ_FILES))
CRYPTOBJ_FILES := $(OBJ_DIR)/sha256.o $(OBJ_DIR)/crypt.o

# Make the Object directory if it doesn't exist
$(shell   mkdir -p $(OBJ_DIR))
$(shell   mkdir -p $(OBJ_DIR)/sqlite)

cs-mud: $(OBJ_FILES)
	$(RM) -f $(EXE)
	$(CC) -o $(EXE) $(OBJ_FILES) $(L_FLAGS)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(C_FLAGS) -c -o $@ $<

crypt: $(CRYPTOBJ_FILES)
	$(RM) -f $(CRYPTEXE)
	$(CC) -o $(CRYPTEXE) $(CRYPTOBJ_FILES) $(L_FLAGS)

clean:
	$(RM) -f $(OBJ_FILES) $(EXE) *~ *.bak *.orig *.rej

distro:
	make clean
	cd ../..
	$(RM) -f log/*.log
	tar zvcf crimsonskiesmud-`date -I`.tar.gz CrimsonSkiesMud
