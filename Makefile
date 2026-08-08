NAME = codexion

SRC_DIR = src
INC_DIR = includes

SRC = $(SRC_DIR)/main.c \
      $(SRC_DIR)/logging.c \
      $(SRC_DIR)/time_utils.c \
      $(SRC_DIR)/monitor.c \
      $(SRC_DIR)/parsing/parsing.c \
      $(SRC_DIR)/parsing/parsing_number.c \
      $(SRC_DIR)/parsing/parsing_checks.c \
      $(SRC_DIR)/heap/heap.c \
      $(SRC_DIR)/heap/heap_cmp.c \
      $(SRC_DIR)/heap/heap_push.c \
      $(SRC_DIR)/heap/heap_pop.c \
      $(SRC_DIR)/dongle/dongle.c \
      $(SRC_DIR)/dongle/dongle_wait.c \
      $(SRC_DIR)/simulation/simulation.c \
      $(SRC_DIR)/simulation/simulation_init.c \
      $(SRC_DIR)/simulation/simulation_coders.c \
      $(SRC_DIR)/coder/coder.c \
      $(SRC_DIR)/coder/coder_status.c \
      $(SRC_DIR)/coder/coder_dongles.c

OBJ = $(SRC:.c=.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -I$(INC_DIR)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

tsan: $(SRC)
	$(CC) $(CFLAGS) -fsanitize=thread $(SRC) -o codexion_tsan

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)
	rm -f codexion_tsan

re: fclean all

.PHONY: all clean fclean re tsan