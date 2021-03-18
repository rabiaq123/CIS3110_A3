# FULL NAME: Rabia Qureshi
# STUDENT ID: 1046427
# DATE: March 19, 2021
# ASSIGNMENT 3

# MAKEFILE GENERAL SYNTAX
# target: prerequisites
#	commands

# $^ is the names of all the prerequisites with spaces between them
# $< is the name of the first prerequisite
# $@ is the name of the target

CC = gcc
CFLAGS = -Wpedantic -std=gnu99 -pthread

dine: dine.o
	$(CC) $(CFLAGS) $^ -o $@

dine.o: dine.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o dine
