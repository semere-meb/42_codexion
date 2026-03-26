NAME = codexion

SRCS = $(wildcard coders/*.c)
INCLUDES = coders/codexion.h

OBJS = $(SRCS:.c=.o)

CC = cc
# CFLAGS = -Wall -Wextra -Werror -pthread -g
CFLAGS = -Wall -Wextra -Werror -pthread -g -fsanitize=thread
ARGS = 2 800 200 100 10 3 0 edf

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# number_of_coders
# time_to_burnout
# time_to_compile
# time_to_debug
# time_to_refactor
# number_of_compiles_required
# dongle_cooldown
# scheduler

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
