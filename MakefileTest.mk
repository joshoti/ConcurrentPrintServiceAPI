# Compiler, flags, and libraries
CC = gcc

CFLAGS = -g -Wall -Iinclude -Iinclude/common -Iexternal

# --- Configuration for Executables ---
TARGETS = test_linked_list test_preprocessing test_job_receiver

# --- Rules ---
all: $(TARGETS)

test_linked_list: test_linked_list.o linked_list.o test_utils.o
	$(CC) $(CFLAGS) -o $@ $^ 

test_preprocessing: test_preprocessing.o preprocessing.o test_utils.o
	$(CC) $(CFLAGS) -o $@ $^

test_job_receiver: test_job_receiver.o job_receiver.o test_utils.o preprocessing.o linked_list.o timeutils.o simulation_stats.o logger.o
	$(CC) $(CFLAGS) -o $@ $^

test_preprocessing.o: tests/test_preprocessing.c include/preprocessing.h include/common/common.h
	$(CC) $(CFLAGS) -c tests/test_preprocessing.c

test_job_receiver.o: tests/test_job_receiver.c include/job_receiver.h
	$(CC) $(CFLAGS) -c tests/test_job_receiver.c

timeutils.o: src/common/timeutils.c include/common/timeutils.h include/common/common.h
	$(CC) $(CFLAGS) -c src/common/timeutils.c

simulation_stats.o: src/simulation_stats.c include/simulation_stats.h include/common/common.h
	$(CC) $(CFLAGS) -c src/simulation_stats.c

job_receiver.o: src/job_receiver.c include/job_receiver.h include/linked_list.h include/common/common.h include/simulation_stats.h include/common/timeutils.h include/preprocessing.h include/logger.h
	$(CC) $(CFLAGS) -c src/job_receiver.c

logger.o: src/logger.c include/logger.h include/common/common.h
	$(CC) $(CFLAGS) -c src/logger.c

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