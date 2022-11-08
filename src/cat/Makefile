NAME=s21_cat

CC=gcc
DEBUG=-g -DDEBUG
CFLAGS=-Wall -Werror -Wextra -std=c11

SRCS=s21_cat.c 
OBJS=$(SRCS:.c=.o)

EXIT_PATH=./

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(EXIT_PATH)$@

%.o: %.c %.h 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean:
	make clean
	rm -f $(NAME)

.PHONY: all clean fclean