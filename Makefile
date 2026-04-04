NAME = codexion

SRCS = $(wildcard coders/*.c)
INCLUDES = coders/codexion.h

OBJS = $(SRCS:.c=.o)

CC = cc
# CFLAGS = -Wall -Wextra -Werror -pthread -g -fsanitize=thread
CFLAGS = -Wall -Wextra -Werror -pthread 

ARGS = 6 900 200 200 200 10 60 fifo

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(NAME)
	./$(NAME) $(ARGS)

test-val: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) $(ARGS)
	
test-hel: $(NAME)
	valgrind --tool=helgrind ./$(NAME) $(ARGS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
