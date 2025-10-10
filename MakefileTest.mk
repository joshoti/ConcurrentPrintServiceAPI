# Compiler, flags, and libraries
CC = gcc

CFLAGS = -g -Wall -Iinclude -Iinclude/common

# --- Configuration for Executables ---
TARGETS = test_linked_list test_preprocessing

# --- Rules ---
all: $(TARGETS)

test_linked_list: test_linked_list.o linked_list.o test_utils.o
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $^ 

test_preprocessing: test_preprocessing.o preprocessing.o test_utils.o
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -o $@ $^

test_preprocessing.o: tests/test_preprocessing.c include/preprocessing.h include/common/common.h
	$(CC) $(CFLAGS) -c tests/test_preprocessing.c

preprocessing.o: src/preprocessing.c include/preprocessing.h include/common/common.h
	$(CC) $(CFLAGS) -c src/preprocessing.c

test_linked_list.o: tests/test_linked_list.c include/linked_list.h include/common/common.h
	$(CC) $(CFLAGS) -c tests/test_linked_list.c

linked_list.o: src/linked_list.c include/linked_list.h include/common/common.h
	$(CC) $(CFLAGS) -c src/linked_list.c

test_utils.o: tests/test_utils.c include/test_utils.h
	$(CC) $(CFLAGS) -c tests/test_utils.c

clean:
	rm -rf $(TARGETS) *.o

# Declare targets that are not actual files
.PHONY: all clean