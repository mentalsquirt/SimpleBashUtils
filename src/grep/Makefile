NAME=s21_grep

CC=gcc
DEBUG=-g -DDEBUG
CFLAGS=-Wall -Werror -Wextra -std=c11 -D_GNU_SOURCE -pedantic

SRCS=s21_grep.c 
OBJS=$(SRCS:.c=.o)

EXIT_PATH=./

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(EXIT_PATH)$@

%.o: %.c %.h 
	$(CC) $(CFLAGS) -c $< -o $@

test: $(NAME)
	bash test_grep.sh

clean:
	rm -f $(OBJS)

fclean:
	make clean
	rm -rf $(NAME) $(NAME).dSYM

re:
	make fclean
	make

.PHONY: $(NAME) all test clean fclean re