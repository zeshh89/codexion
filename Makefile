NAME = codexion

SRC = main.c parsing.c heap.c dongle.c time_utils.c simulation.c coder.c monitor.c logging.c
OBJ = $(SRC:.c=.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re