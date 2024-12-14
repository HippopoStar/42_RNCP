
# https://directory.fsf.org/wiki/Make
# https://www.gnu.org/software/make/manual/html_node/index.html

CFLAGS   = -Wall -Wextra -Werror
CPPFLAGS = -I./include/
LDFLAGS  =
LDLIBS   =

CC = gcc

NAME = ft_ping



HEADER = \
	./include/ft_ping.h \
	./include/ft_ping_options.h \
	./include/ft_ping_args.h \
	./include/ft_ping_setup_teardown.h \
	./include/ft_ping_run.h

SRC_DIR = ./src

SRC_NAME = \
	ft_ping.c \
	ft_ping_options.c \
	ft_ping_args.c \
	ft_ping_setup_teardown.c \
	ft_ping_run.c

SRC = $(addprefix $(SRC_DIR)/,$(SRC_NAME))

OBJ_DIR = ./obj

OBJ_NAME = $(SRC_NAME:.c=.o)

OBJ = $(addprefix $(OBJ_DIR)/,$(OBJ_NAME))



all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ)
#	$(CC) -o $(NAME) $(LDFLAGS) $(OBJ) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER) | $(OBJ_DIR)
	$(CC) -c -o $@ $(CFLAGS) $(CPPFLAGS) $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	rm -f $(OBJ)
	rm -df $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
