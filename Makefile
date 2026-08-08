NAME = codexion

SRC = main.c parsing.c heap.c dongle.c dongle_wait.c time_utils.c \
      simulation.c simulation_init.c simulation_coders.c \
      coder.c coder_status.c coder_dongles.c monitor.c logging.c

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