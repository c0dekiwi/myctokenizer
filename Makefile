.DEFAULT_GOAL=main

CC=gcc
PROG=myctokenizer
CFLAGS=-Wall -Wextra -Werror
CFLAGS += -std=c99
SRCS=\
	tokenizer.c \
	utils.c

main:
	$(CC) $(CFLAGS) $(SRCS) -o $(PROG)
